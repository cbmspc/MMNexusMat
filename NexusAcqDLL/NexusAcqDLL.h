#ifndef INDLL_H
#define INDLL_H

#ifdef EXPORTING_DLL
extern __declspec(dllexport) DWORD NexusAPI_Init( DWORD numChan ) ;
#else
extern __declspec(dllimport) DWORD NexusAPI_Init( DWORD numChan ) ;
#endif

#ifdef EXPORTING_DLL
extern __declspec(dllexport) DWORD NexusAPI_Start( DWORD dwSamplerate, DWORD dwBufferSizeSeconds ) ;
#else
extern __declspec(dllimport) DWORD NexusAPI_Start( DWORD dwSamplerate, DWORD dwBufferSizeSeconds ) ;
#endif

#ifdef EXPORTING_DLL
extern __declspec(dllexport) DWORD NexusAPI_Stop() ;
#else
extern __declspec(dllimport) DWORD NexusAPI_Stop() ;
#endif

#ifdef EXPORTING_DLL
extern __declspec(dllexport) DWORD NexusAPI_GetData( DWORD numSampl, float * pBuf ) ;
#else
extern __declspec(dllimport) DWORD NexusAPI_GetData( DWORD numSampl, float * pBuf ) ;
#endif


#endif