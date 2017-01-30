// NexusAcqDLL.cpp : Defines the entry point for the DLL application.
//

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

#define BUFFER_SIZE		2048*67*20  // memory can hold 20 sec of data for 67 channels at 2048 Hz
int		gNumChannels;
float * gData;
HANDLE  gMutex;
//int		gNumSamples;	// num samples in the buffer
int		gSamplRate;
BOOL	gisStarted;
int		gBufferMaxNumTimes;
int		gCurrentTimePointPos = -1; // Current time point position that has data.

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
	gMutex = CreateMutex( NULL, FALSE, "NexusAPI_MUTEX");
	if (!gMutex) res = FALSE;

	gData = (float *) calloc( BUFFER_SIZE, sizeof(float));
	if (!gData) res = FALSE;

	gNumChannels = 32;
	//gNumSamples = 0;
	gisStarted = FALSE;
 } else {

	InitNeXusDevice = NULL;
	StartNeXusDevice = NULL;
	StopNeXusDevice = NULL;
	gMutex = NULL;
	res = FALSE;
 }
 return res;
}

BOOL FreeApiResources()
{
	if (gMutex) CloseHandle( gMutex );
	if (gData) free( gData );

	return TRUE;
}


DWORD NexusAPI_Init( DWORD numChan )
{
	if (numChan < 1) 
		numChan = 32; // default is 32 channels
	else if (numChan > 67)
		numChan = 67;

	gNumChannels = numChan;
    //gNumSamples = 0;
	gSamplRate = 128;
	gisStarted = FALSE;
	return InitNeXusDevice( (DLL_ShowData)ProcessData );
}

DWORD NexusAPI_Start( DWORD dwSamplerate, DWORD dwBufferSizeSeconds )
{
	if (dwSamplerate < 128)
	{
		dwSamplerate =128;
	}
	if (dwSamplerate >2048)
	{
		dwSamplerate =2048;
	}

	//memset(gData, 0, BUFFER_SIZE*sizeof(float));

	//gNumSamples = 0;
	gSamplRate = dwSamplerate;
	gBufferMaxNumTimes = gSamplRate*((int) max(10,dwBufferSizeSeconds));

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
	int Origin = gCurrentTimePointPos;
	/*

	gCurrentTimePointPos marks the time index of the most current samples
	in gBufferMaxNumTimes.

	If consumer specifies numSampl smaller than our buffer size gBufferMaxNumTimes,
	we will transfer the tail end of gBuffer to pBuf.

	If consumer specifies numSampl larger than our buffer size,
	we will transfer the entire gBuffer to the tail-end of the pBuf.

	Since we initialized gBuffer to be all zeros, there is no need to wait for data
	to fill up.

	*/
	
	for (tp = 0; tp < gBufferMaxNumTimes; tp++)
	{
		otp = tp - gBufferMaxNumTimes + numSampl;
		if (otp >= 0)
		{
			for (ch = 0; ch < gNumChannels; ch++)
			{
				pBuf[otp*gNumChannels+ch] = gBuffer[(tp+Origin-1)%gBufferMaxNumTimes][ch];
			}
		}
	}
	/*int numReady = 0;
	int numCheck = 0;
	while (numReady < numSampl) {
		
		if ((numCheck > 1) && (numReady == 0)) return 1;

		if (numCheck > 0) // data not ready - sleep as long as needed
			Sleep((numSampl-numReady)*1000/gSamplRate);

		if (WaitForSingleObject( gMutex, 2000 ) == WAIT_OBJECT_0) {
			numReady = gNumSamples;
			ReleaseMutex( gMutex );
			++numCheck;
		} 
		else 
			return 2;
	}*/

	/*for (int i = 0; i < numSampl*gNumChannels; i++) {
		pBuf[i] = gData[i];
	}*/
/*
#if 0
	if ( WaitForSingleObject( gMutex, 2000 ) == WAIT_OBJECT_0) {
		gNumSamples = 0;
		ReleaseMutex( gMutex );
		return 0;
	}
	else return 3;
#else*/
	return 0;
//#endif
}

// Callback function that is registered by InitNeXusDevice()
// (data producer)
void ProcessData(int nSamples, int nChan, float *fData)
{
	WaitForSingleObject(gMutex, INFINITE);

	gCurrentTimePointPos = (++gCurrentTimePointPos)%gBufferMaxNumTimes;

	for (int i=0; i < gNumChannels; i++)
	{
		gBuffer[gCurrentTimePointPos][i] = fData[i];
	}

/*	if ( (gNumSamples+1)*gNumChannels < BUFFER_SIZE ) {
		int ofs = gNumSamples * gNumChannels;

		for(int i = 0; i < gNumChannels; i++)
		{
			gData[ofs + i] = fData[i];
		}
		++gNumSamples;
	}
	*/
	ReleaseMutex(gMutex);
}
