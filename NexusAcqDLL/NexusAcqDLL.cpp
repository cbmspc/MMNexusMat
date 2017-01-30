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
 */


#include "stdafx.h"
#include <Windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <vector>

#define EXPORTING_DLL
#include "NexusAcqDLL.h"

BOOL InitializeApi();
BOOL FreeApiResources();
void ProcessData(int nSamples, int nChan, float *fData);

typedef void (*DLL_ShowData)( int nSamples, int nChan, float *fData);
typedef DWORD (*DLL_INIT)( DLL_ShowData fFunc );
typedef DWORD (*DLL_START)( DWORD *dwSamplerate );
typedef DWORD (*DLL_STOP)( void );

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
//HANDLE  gMutex;
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
	DWORD ul_reason_for_call,	// Reason for calling function
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
 HINSTANCE hDLL = LoadLibrary( "NeXusDLL.dll" );
 BOOL res;
 if ( hDLL != NULL)
 {
	InitNeXusDevice		= (DLL_INIT) GetProcAddress( hDLL, "InitNeXusDevice" );
	StartNeXusDevice	= (DLL_START)GetProcAddress( hDLL, "StartNeXusDevice" );
	StopNeXusDevice		= (DLL_STOP) GetProcAddress( hDLL, "StopNeXusDevice" );
	res = TRUE;
	gNumChannels = 32; // Dummy value. Will be properly set in NexusAPI_Init
	gisStarted = FALSE;
	//gMutex = CreateMutex( NULL, FALSE, "NexusAPI_MUTEX");
	InitializeCriticalSection (&gCritSect);

	//if (!gMutex) res = FALSE;

 } else {
	InitNeXusDevice = NULL;
	StartNeXusDevice = NULL;
	StopNeXusDevice = NULL;
	//gMutex = NULL;
	res = FALSE;
 }
 return res;
}

BOOL FreeApiResources()
{
	//if (gMutex) CloseHandle( gMutex );
	DeleteCriticalSection(&gCritSect);
	return TRUE;
}


DWORD NexusAPI_Init( DWORD numChan )
{
	if (numChan < 1) numChan = 32; // default is 32 channels
	else if (numChan > 67) numChan = 67;
	gNumChannels = numChan;
	gSamplRate = 128; // Dummy value. Will be properly set in NexusAPI_Start
	gisStarted = FALSE;
	return InitNeXusDevice( (DLL_ShowData)ProcessData );
}

DWORD NexusAPI_Start( DWORD dwSamplerate, DWORD dwBufferSizeSeconds )
{
	// These are the acceptable sampling rates by NeXus acquisition hardware
	if (dwSamplerate <  128) dwSamplerate = 128;
	if (dwSamplerate > 2048) dwSamplerate = 2048;
	gSamplRate = dwSamplerate;
	gBufferMaxNumTimes = gSamplRate*((int) max(MINIMUMBUFFERSIZESECONDS,dwBufferSizeSeconds));
	gBuffer.assign(gBufferMaxNumTimes,std::vector <float> (gNumChannels,0));
	gisStarted = TRUE;
	return StartNeXusDevice( &dwSamplerate );
}

DWORD NexusAPI_Stop()
{
	if (gisStarted)
	{
		DWORD retval = StopNeXusDevice();
		if (retval == 0) gisStarted = FALSE;
		return retval; 
	}
	else return 0;
}

// Data consumer, called by application to read data from buffer

DWORD NexusAPI_GetData(DWORD numSampl, float * pBuf)
{
	int otp; //output time point index tracked by pBuf
	int tp; //time point index tracked by gBuffer
	int ch; //channel index tracked by both

	//WaitForSingleObject(gMutex, INFINITE);
	EnterCriticalSection (&gCritSect);
	int Origin = gCurrentTimePointPos;
	//ReleaseMutex(gMutex);
	LeaveCriticalSection (&gCritSect);

	// gCurrentTimePointPos marks the time index of the most current samples
	// in gBufferMaxNumTimes. Because ProcessData could still be writing into
	// the current position, this will instead fetch up to one sample before.

	for (otp = numSampl - 1, tp = Origin - 1; otp > -1 ; otp--, tp--)
	{
		if (tp < 0) tp = gBufferMaxNumTimes - 1;
		for (ch = 0; ch < gNumChannels; ch++)
			pBuf[otp*gNumChannels+ch] = gBuffer[tp][ch];
	}

	return 0;
}

// Callback function that is registered by InitNeXusDevice()
// (data producer)
void ProcessData(int nSamples, int nChan, float *fData)
{
	//WaitForSingleObject(gMutex, INFINITE);
	EnterCriticalSection (&gCritSect);
	gCurrentTimePointPos = (++gCurrentTimePointPos)%gBufferMaxNumTimes;
	//ReleaseMutex(gMutex);
	LeaveCriticalSection (&gCritSect);

	for (int i=0; i < gNumChannels; i++)
	{
		gBuffer[gCurrentTimePointPos][i] = fData[i];
	}
}
