#include <Windows.h>
#include <stdio.h>
#include "dxl_hal2.h"




HANDLE ghSerial_Handle = INVALID_HANDLE_VALUE; // Serial port handle
LARGE_INTEGER freq, counter;

////////////////////////////////////////////////////////////////////////////////
/// summary : Get current time
/// return  : the current time (in milliseconds)
double  dxl_hal_get_curr_time()
{
    QueryPerformanceCounter(&counter);
    QueryPerformanceFrequency(&freq);
    return (double)counter.QuadPart / (double)freq.QuadPart * 1000.0;
}

////////////////////////////////////////////////////////////////////////////////
/// summary : Open device
/// param   : port       handle to COM port
/// param   : dev_index  device index
/// param   : baudrate   real baudrate (ex: 115200, 57600, 38400, ...)
/// return  :
///    0 - failed
///    1 - succeed
int dxl_hal_open(int dev_index, int baudrate )
{
    DCB Dcb;
    COMMTIMEOUTS Timeouts;
    DWORD dwError;
    char PortName[15];

    dxl_hal_close();

    // Make real port name
    sprintf_s(PortName, sizeof(PortName), "\\\\.\\COM%d", dev_index);
    // Open serial device
    ghSerial_Handle = CreateFile( PortName, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    if( ghSerial_Handle == INVALID_HANDLE_VALUE )
        return 0;

    // Setting communication property
    Dcb.DCBlength = sizeof(DCB);
    if( GetCommState( ghSerial_Handle, &Dcb ) == FALSE )
        goto DXL_HAL_OPEN_ERROR;
    
    // Set baudrate
    Dcb.BaudRate            = (DWORD)baudrate;    
    Dcb.ByteSize            = 8;                    // Data bit = 8bit
    Dcb.Parity              = NOPARITY;             // No parity
    Dcb.StopBits            = ONESTOPBIT;           // Stop bit = 1
    Dcb.fParity             = NOPARITY;             // No Parity check
    Dcb.fBinary             = 1;                    // Binary mode
    Dcb.fNull               = 0;                    // Get Null byte
    Dcb.fAbortOnError       = 0;
    Dcb.fErrorChar          = 0;
    // Not using XOn/XOff
    Dcb.fOutX               = 0;
    Dcb.fInX                = 0;
    // Not using H/W flow control
    Dcb.fDtrControl         = DTR_CONTROL_DISABLE;
    Dcb.fRtsControl         = RTS_CONTROL_DISABLE;
    Dcb.fDsrSensitivity     = 0;
    Dcb.fOutxDsrFlow        = 0;
    Dcb.fOutxCtsFlow        = 0;
    if( SetCommState( ghSerial_Handle, &Dcb ) == FALSE )
        goto DXL_HAL_OPEN_ERROR;

    if( SetCommMask( ghSerial_Handle, 0 ) == FALSE ) // Not using Comm event
        goto DXL_HAL_OPEN_ERROR;
    if( SetupComm( ghSerial_Handle, 4096, 4096 ) == FALSE ) // Buffer size (Rx,Tx)
        goto DXL_HAL_OPEN_ERROR;
    if( PurgeComm( ghSerial_Handle, PURGE_TXABORT|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_RXCLEAR ) == FALSE ) // Clear buffer
        goto DXL_HAL_OPEN_ERROR;
    if( ClearCommError( ghSerial_Handle, &dwError, NULL ) == FALSE )
        goto DXL_HAL_OPEN_ERROR;
    
    if( GetCommTimeouts( ghSerial_Handle, &Timeouts ) == FALSE )
        goto DXL_HAL_OPEN_ERROR;
    // Timeout (Not using timeout)
    // Immediatly return
    Timeouts.ReadIntervalTimeout = 0;
    Timeouts.ReadTotalTimeoutMultiplier = 0;
    Timeouts.ReadTotalTimeoutConstant = 1; // must not be zero.
    Timeouts.WriteTotalTimeoutMultiplier = 0;
    Timeouts.WriteTotalTimeoutConstant = 0;
    if( SetCommTimeouts( ghSerial_Handle, &Timeouts ) == FALSE )
        goto DXL_HAL_OPEN_ERROR;
    
    return 1;

DXL_HAL_OPEN_ERROR:
    dxl_hal_close();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// summary : Close device
/// param   : port       handle to COM port
void dxl_hal_close()
{
    // Closing device
    if(ghSerial_Handle != INVALID_HANDLE_VALUE)
    {
        CloseHandle( ghSerial_Handle );
        ghSerial_Handle = INVALID_HANDLE_VALUE;
    }
}


////////////////////////////////////////////////////////////////////////////////
/// summary : Change com port baudrate
/// param   : baudrate   real baudrate to set
/// return  :
///    0 - failed
///    1 - succeed
int dxl_hal_change_baudrate( float baudrate )
{
    DCB Dcb;
    GetCommState(ghSerial_Handle, &Dcb);
    Dcb.BaudRate = (DWORD) baudrate;

    if( SetCommState(ghSerial_Handle, &Dcb ) == FALSE )
    {
        dxl_hal_close();
        return 0;
    }
    else
    {
        return 1;
    }
}


////////////////////////////////////////////////////////////////////////////////
/// summary : Clear receive buffer
/// param   : port       handle to COM port
void dxl_hal_clear(void)
{
    // Clear communication buffer
    PurgeComm( ghSerial_Handle, PURGE_RXABORT|PURGE_RXCLEAR );
}

////////////////////////////////////////////////////////////////////////////////
/// summary : Sending data
/// param   : pPacket    pointer to data to write
/// param   : numPacket  number of bytes to write
/// return  : number of bytes written or -1(error)
int  dxl_hal_write( void *pPacket, unsigned int numPacket )
{
	DWORD dwWritten = 0;

    if( WriteFile( ghSerial_Handle, pPacket, (DWORD)numPacket, &dwWritten, NULL ) == FALSE )
        return -1;
    
    return (int)dwWritten;
}

////////////////////////////////////////////////////////////////////////////////
/// summary : Receiving data
/// param   : pPacket     pointer to data to read
/// param   : numPacket   number of bytes to read
/// return  : number of bytes read or -1(error)
int  dxl_hal_read( void *pPacket, unsigned int numPacket )
{
	DWORD dwRead = 0;

    if( ReadFile( ghSerial_Handle, pPacket, (DWORD)numPacket, &dwRead, NULL ) == FALSE )
        return -1;

    return (int)dwRead;
}