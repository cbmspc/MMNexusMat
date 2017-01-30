#pragma once

#define DEVICE_MAX_CHANNELS		80
#define DEVICE_NAME_SIZE		40
#define DEVICE_SERIAL_SIZE		40
#define DEVICE_DESCRIPTION_SIZE	40
#define CONNECTION_TYPE_SIZE    40
#define CHANNEL_NAME_SIZE		40
#define MODULE_NAME				"GenericDeviceInterface"
#define PORTI_OVERFLOW_VALUE	0X80000000

typedef enum _SEARCH_MODE
{
	SM_USB_BT				= 0,	// Fiber device present? If not search for an Bluetooth device
	SM_BT 					= 1,	// Search for an Bluetooth device
	SM_USB	   				= 2,	// Search for an USB device
	SM_USB_COM				= 3	   	// USB device present? If not search for an serial device
} 
SEARCH_MODE, *PSEARCH_MODE;

//typedef enum _INTERFACE_STATUS
//{
//	IS_OK	   				= 0,	// OK
//	IS_INVALID_DEVICE		= -1,	// No valid Device failed
//	IS_FAILURE_MEMALLOC   	= -2,	// Memory allocation failure (Channel info)
//	IS_FALSE_INFORMATION	= -3,	// False information from device
//	IS_NO_START	   			= -4,	// Could not start device
//	IS_NO_DATATHREAD		= -5,	// Could not start Data collection thread
//	IS_NO_VALID_SERIAL		= -6,	// Could not start the Device with the specified serial number
//	IS_DRIVER_FAIL			= -7,	// Could not load the Generic Device driver properly
//} 
//INTERFACE_STATUS, *PINTERFACE_STATUS;

//typedef enum _INPUT_TYPE 
//{
//	IT_UNK					= 0,	// Unknown
//	IT_UNP 					= 1,	// Unipolar(uV)
//	IT_BIP 					= 2,	// Bipolar(uV)
//	IT_AUX 					= 3,	// Aux(mV)
//	IT_BIT	 				= 4		// Bit
//} 
//INPUT_TYPE, * PINPUT_TYPE;

//typedef struct _DEVICE_INFO
//{
//	char cName[DEVICE_NAME_SIZE];
//	char cSerialNumber[DEVICE_SERIAL_SIZE];		// 0-3 type, 4-5 yy, 6-9 index number from year
//	char cDescription[DEVICE_DESCRIPTION_SIZE];
//	char cConnectionType[CONNECTION_TYPE_SIZE];
//	unsigned long dwTypeID;									
//	unsigned long dwNumberOfChannels;
//	bool bAuthenticated;
//} 
//DEVICE_INFO, *PDEVICE_INFO;

//typedef struct _CHANNEL_INFO
//{ 
//	char cName[CHANNEL_NAME_SIZE];
//	unsigned long dwSampleRate;
//	_INPUT_TYPE InputType;	 
//} 
//CHANNEL_INFO, *PCHANNEL_INFO;