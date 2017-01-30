// NexusAcqDLL.cpp : Defines the entry point for the DLL application.
//

/*   REVISION HISTORY

	2010-01-06 by soschin and ptwang
		Initial version
	
	2011-10-03 by ptwang
		Cleaned up source code.
		Remove obsolete codes.
		gData has been removed.
		All mutex have been removed.
	
	2011-10-04 by soschin
		Added mutex protection for gCurrentTimePointPos
		Planned to use Critical Section instead of mutex. Not active yet.
	
	2011-10-05 by soschin and ptwang
		Changed to using Critical Section instead of mutex

	2011-12-09 by soschin and ptwang
		Upgraded to using new Nexus DLLs

	2012-10-25 by ptwang
		1. Zero out already fetched data
		2. New function to fetch data from the oldest index

	2013-08-26 by ptwang
		Removed the Sample Rate check

	2013-08-27 by ptwang
		Will throw an exception if sample rate out of bound
 */


#include "stdafx.h"
#include <Windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <vector>

#define EXPORTING_DLL
#include "NexusGDIAcqDLL.h"
#include "NexusGDIDLL.h"


BOOL InitializeApi();
BOOL FreeApiResources();
void ProcessData(int nSamples, int nChan, float *fData);

typedef void (*DLL_ShowData)( int nSamples, int nChan, float *fData);
//typedef unsigned long (*DLL_INIT)( DLL_ShowData fFunc );
typedef unsigned long (*DLL_INIT)( DLL_ShowData fFunc, int nSearchMode, long long lDeviceSerialNumber );
typedef unsigned long (*DLL_START)( unsigned long *dwSamplerate );
typedef unsigned long (*DLL_STOP)( void );

DLL_INIT		InitNeXusDevice ;
DLL_START		StartNeXusDevice;
DLL_STOP		StopNeXusDevice;

// Minimum duration of data (in seconds) that can be stored
#define MINIMUMBUFFERSIZESECONDS	10

int		gNumChannels;
int		gSamplRate;
BOOL	gisStarted;
int		gBufferMaxNumTimes;
int		gCurrentTimePointPos = -1; // Current time point position that has data.
long	gDataPointsReceived = 0; // Total time points of data received from Nexus
long	gDataPointsFetched = 0; // Total time points of data fetched by Matlab
CRITICAL_SECTION gCritSect;

std::vector <std::vector <float> > gBuffer;


int modulus (int a, int b)
{
	while (a<0)
		a+=b;
	return (a%b);
}


BOOL APIENTRY DllMain(
HANDLE hModule,	// Handle to DLL module
	unsigned long ul_reason_for_call,	// Reason for calling function
	LPVOID lpReserved ) // Reserved
{
	BOOL res = TRUE;
	switch ( ul_reason_for_call )
	{
		case DLL_PROCESS_ATTACH:
		// A process is loading the DLL.
			res = InitializeApi(); // Load the NexusDLL.dll functions
		break;
		case DLL_THREAD_ATTACH:
		// A process is creating a new thread.
		break;
		case DLL_THREAD_DETACH:
		// A thread exits normally.
		break;
		case DLL_PROCESS_DETACH:
		// A process unloads the DLL.
			res = FreeApiResources();
		break;
	}
	return res;
}

BOOL InitializeApi()
{
//	HINSTANCE hDLL = LoadLibrary( "NeXusDLL.dll" );
#if _WIN64 || __amd64__
	HINSTANCE hDLL = LoadLibrary( "NexusGDIDLL_x64.dll" );
#else
	HINSTANCE hDLL = LoadLibrary( "NexusGDIDLL_x32.dll" );
#endif
	BOOL res;
	if (hDLL != NULL)
	{
//	InitNeXusDevice		= (DLL_INIT) GetProcAddress( hDLL, "InitNeXusDevice" );
//	StartNeXusDevice	= (DLL_START)GetProcAddress( hDLL, "StartNeXusDevice" );
//	StopNeXusDevice		= (DLL_STOP) GetProcAddress( hDLL, "StopNeXusDevice" );
	InitNeXusDevice		= (DLL_INIT) GetProcAddress( hDLL, "InitGenericDevice" );
	StartNeXusDevice	= (DLL_START)GetProcAddress( hDLL, "StartGenericDevice" );
	StopNeXusDevice		= (DLL_STOP) GetProcAddress( hDLL, "StopGenericDevice" );
	res = TRUE;
	gNumChannels = 32; // Dummy value. Will be properly set in NexusAPI_Init
	gisStarted = FALSE;
	InitializeCriticalSection (&gCritSect);


 } else {
	InitNeXusDevice = NULL;
	StartNeXusDevice = NULL;
	StopNeXusDevice = NULL;
	res = FALSE;
 }
 return res;
}

BOOL FreeApiResources()
{
	DeleteCriticalSection(&gCritSect);
	return TRUE;
}


unsigned long NexusAPI_Init( unsigned long numChan )
{
	if (numChan < 1) numChan = 34; // default is 34 channels
	else if (numChan > 67) numChan = 67;
	gNumChannels = numChan;
	gSamplRate = 128; // Dummy value. Will be properly set in NexusAPI_Start
	gisStarted = FALSE;
	//return InitNeXusDevice( (DLL_ShowData)ProcessData );
	int nSearchMode = SM_USB_BT; // or SM_USB or SM_BT
	long long lDeviceSerialNumber = 0; // zero for "ALL"
	return InitNeXusDevice( (DLL_ShowData)ProcessData, nSearchMode, lDeviceSerialNumber );
}

unsigned long NexusAPI_Start( unsigned long dwSamplerate, unsigned long dwBufferSizeSeconds )
{
	// Error if the requested sample rate is out of bound
	if (dwSamplerate <  128 || dwSamplerate > 2048)
		throw std::out_of_range ("Sample Rate Fs must be within 128 <= Fs <= 2048");
	gSamplRate = dwSamplerate;
	gBufferMaxNumTimes = gSamplRate*((int) max(MINIMUMBUFFERSIZESECONDS,dwBufferSizeSeconds));
	gBuffer.assign(gBufferMaxNumTimes,std::vector <float> (gNumChannels,0));
	gisStarted = TRUE;
	return StartNeXusDevice( &dwSamplerate );
}

unsigned long NexusAPI_Stop()
{
	if (gisStarted)
	{
		unsigned long retval = StopNeXusDevice();
		if (retval == 0) gisStarted = FALSE;
		return retval; 
	}
	else return 0;
}

unsigned long NexusAPI_GetData(unsigned long numSampl, float * pBuf)
// Data consumer, called by application to read data from buffer
{
	int otp; //output time point index tracked by pBuf
	int tp; //time point index tracked by gBuffer
	int ch; //channel index tracked by both

	EnterCriticalSection (&gCritSect);
	int Origin = gCurrentTimePointPos;
	LeaveCriticalSection (&gCritSect);

	// gCurrentTimePointPos marks the time index of the most current samples
	// in gBufferMaxNumTimes. Because ProcessData could still be writing into
	// the current position, this will instead fetch up to one sample before.

	for (otp = numSampl - 1, tp = Origin - 1; otp > -1 ; otp--, tp--)
	{
		if (tp < 0) tp = gBufferMaxNumTimes - 1;
		for (ch = 0; ch < gNumChannels; ch++)
		{
			pBuf[otp*gNumChannels+ch] = gBuffer[tp][ch];
			// 2012-10-25 Zero out already-fetched data points
			gBuffer[tp][ch] = 0;
		}
	}

	return 0;
}


unsigned long NexusAPI_GetDataForward(unsigned long numSampl, float * pBuf)
// Data consumer, called by application to read data from buffer
// Gets from the front. If getting too fast, right edge will contain zeros
{
	int otp; //output time point index tracked by pBuf
	int tp; //time point index tracked by gBuffer
	int ch; //channel index tracked by both

	EnterCriticalSection (&gCritSect);
	int NewFetchStart = gDataPointsFetched % gBufferMaxNumTimes;
	LeaveCriticalSection (&gCritSect);
	for (otp = 0, tp = NewFetchStart; otp < numSampl; otp++, tp++)
	{
		if (tp >= gBufferMaxNumTimes)
		{
			tp = 0;
		}
		if (gDataPointsFetched < gDataPointsReceived)
		{
			for (ch = 0; ch < gNumChannels; ch++)
			{
				pBuf[otp*gNumChannels+ch] = gBuffer[tp][ch];
				gBuffer[tp][ch] = 0;
			}
			gDataPointsFetched++;
		}
		else
		{
			// Fetching faster than producer can fill. Zero fill.
			for (ch = 0; ch < gNumChannels; ch++)
			{
				pBuf[otp*gNumChannels+ch] = 0;
			}
		}
	}
	return 0;
}



void ProcessData(int nSamples, int nChan, float *fData)
// Callback function that is registered by InitNeXusDevice()
// (data producer)
{
	for (int k=0; k < nSamples; k++) 
	{
		EnterCriticalSection (&gCritSect);
		gCurrentTimePointPos = (++gCurrentTimePointPos)%gBufferMaxNumTimes;
		gDataPointsReceived++;
		LeaveCriticalSection (&gCritSect);

		for (int i=0; i < gNumChannels; i++)
		{
			gBuffer[gCurrentTimePointPos][i] = fData[i];
		}
	}
}
