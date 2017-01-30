#ifndef INDLL_H
#define INDLL_H

#define EXPORTED_FUNCTION
extern EXPORTED_FUNCTION unsigned long NexusAPI_Init( unsigned long numChan ) ;
EXPORTED_FUNCTION unsigned long NexusAPI_Start( unsigned long dwSamplerate, unsigned long dwBufferSizeSeconds ) ;
EXPORTED_FUNCTION unsigned long NexusAPI_Stop() ;
EXPORTED_FUNCTION unsigned long NexusAPI_GetData( unsigned long numSampl, float * pBuf ) ;
EXPORTED_FUNCTION unsigned long NexusAPI_GetDataForward( unsigned long numSampl, float * pBuf ) ;


#endif