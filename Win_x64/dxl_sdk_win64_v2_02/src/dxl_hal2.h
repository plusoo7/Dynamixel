#ifndef _DYNAMIXEL_HAL2_HEADER
#define _DYNAMIXEL_HAL2_HEADER


#ifdef __cplusplus
extern "C" {
#endif


#define DXL_MAXNUM_TXPACKET  (10000)
#define DXL_MAXNUM_RXPACKET  (10000)


int  dxl_hal_open( int devIndex, int baudrate );
void dxl_hal_close();
int  dxl_hal_change_baudrate(float baudrate);
void dxl_hal_clear();

double  dxl_hal_get_curr_time();
int  dxl_hal_write( void *pPacket, unsigned int numPacket );
int  dxl_hal_read(  void *pPacket, unsigned int numPacket );

#ifdef __cplusplus
}
#endif

#endif
