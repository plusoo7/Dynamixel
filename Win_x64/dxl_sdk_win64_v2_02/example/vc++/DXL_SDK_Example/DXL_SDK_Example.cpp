#include <Windows.h>
#include <iostream>
#include <conio.h>
#include "dynamixel2.h"

#if _WIN64
#pragma comment(lib, "dynamixel2_win64.lib")
#else
#pragma comment(lib, "dynamixel2_win32.lib")
#endif

int giID_List[2] = {1, 2};

#define MAX_IN_CHAR 128

using namespace std;

#define P_TORQUE_ENABLE 562
#define P_GOAL_POSITION 596
#define P_PRESENT_POS	611

#define POSITION_LENGTH 4

int main(void)
{
	int port_num = 0, baud_rate = 0;

	while(true)
	{
		port_num = 0;
		printf("Input COM port number : ");
		scanf_s("%d", &port_num);
		fflush(stdin);

		baud_rate = 0;
		printf("Input baudrate : ");
		scanf_s("%d", &baud_rate);
		fflush(stdin);

		printf("\nYour input info. is\n");
		printf("COM port number : %d\n", port_num);
		printf("       Baudrate : %d\n", baud_rate);
		char _temp;
		while(true)
		{
			printf("Are you sure? (Y/N)  ");

			scanf_s("%c", &_temp);
			fflush(stdin);
			if(_temp == 'y' || _temp == 'Y')
				break;
			else if(_temp == 'n' || _temp == 'N')
				break;
			else
				printf("Incorrect Answer!\n");

		}
		if( (_temp == 'y') || (_temp == 'Y'))
			break;
		else
			printf("\n");
	}

	if(dxl_initialize(port_num, baud_rate) == 0)
	{
		printf( "Failed to open USB2Dynamixel!\n" );
		printf( "Press any key to terminate...\n" );
		_getch();
		return 0;
	}
	else
		printf( "Succeed to open USB2Dynamixel!\n\n" );


	printf("/======================= read/write test =======================/\n");
	
	printf("Torque On\n");
	for(int i = 0; i < (sizeof(giID_List)/sizeof(*giID_List)); i++)
	{
		dxl2_write_byte(giID_List[i], P_TORQUE_ENABLE, 1);
		if( dxl_get_comm_result() != COMM_RXSUCCESS)
			printf("ID %d : Torque on Faild\n", giID_List[i]);
		else
			printf("ID %d : Torque on Succeed\n", giID_List[i]);
	}

	printf("\nWrite\n");
	for(int i = 0; i < (sizeof(giID_List)/sizeof(*giID_List)); i++)
	{
		dxl2_write_dword(giID_List[i] , P_GOAL_POSITION, 10000);
		if( dxl_get_comm_result() != COMM_RXSUCCESS)
			printf("ID %d : Write Faild\n", giID_List[i]);
		else
			printf("ID %d : Write Succeed\n", giID_List[i]);
	}

	printf("\nRead\n");
	bool is_moving = false;
	while(true)
	{
		is_moving = false;
		for(int i = 0; i < (sizeof(giID_List)/sizeof(*giID_List)); i++)
		{
			long pos = (long)dxl2_read_dword(giID_List[i], P_PRESENT_POS);
			if( dxl_get_comm_result() != COMM_RXSUCCESS)
				printf(" ID %d : Read Faild\n", giID_List[i]);
			else
				printf(" [ID %d : %d]", giID_List[i], pos);

			if( pos < 10000 - 300)
				is_moving = true;
		}
		printf("\n");

		if(is_moving == false)
			break;
	}
	
	printf("/================== read/write test complete ==================/\n");
	printf("\n");
	printf("/==================== sync write/read test ====================/\n");
	_getch();
	dxl2_set_txpacket_id(BROADCAST_ID);
	dxl2_set_txpacket_instruction(INST_SYNC_WRITE);
	unsigned short idx = 0;
	dxl2_set_txpacket_parameter( idx++, DXL_LOBYTE( P_GOAL_POSITION ));
	dxl2_set_txpacket_parameter( idx++, DXL_HIBYTE( P_GOAL_POSITION ));
	dxl2_set_txpacket_parameter( idx++, DXL_LOBYTE( POSITION_LENGTH ));
	dxl2_set_txpacket_parameter( idx++, DXL_HIBYTE( POSITION_LENGTH ));
	for(int i = 0; i < (sizeof(giID_List)/sizeof(*giID_List)); i++)
	{
		dxl2_set_txpacket_parameter( idx++, (unsigned char) giID_List[i]);
		dxl2_set_txpacket_parameter( idx++, DXL_LOBYTE(DXL_LOWORD(-10000)));
		dxl2_set_txpacket_parameter( idx++, DXL_HIBYTE(DXL_LOWORD(-10000)));
		dxl2_set_txpacket_parameter( idx++, DXL_LOBYTE(DXL_HIWORD(-10000)));
		dxl2_set_txpacket_parameter( idx++, DXL_HIBYTE(DXL_HIWORD(-10000)));
	}
	dxl2_set_txpacket_length(idx + 3);
	dxl2_txrx_packet();

	
	dxl2_set_txpacket_id(BROADCAST_ID);
	dxl2_set_txpacket_instruction(INST_SYNC_READ);
	idx = 0;
	dxl2_set_txpacket_parameter(idx++, DXL_LOBYTE( P_PRESENT_POS ));
	dxl2_set_txpacket_parameter(idx++, DXL_HIBYTE( P_PRESENT_POS ));
	dxl2_set_txpacket_parameter(idx++, DXL_LOBYTE( POSITION_LENGTH ));
	dxl2_set_txpacket_parameter(idx++, DXL_HIBYTE( POSITION_LENGTH ));
	for(int i = 0; i < (sizeof(giID_List)/sizeof(*giID_List)); i++)
		dxl2_set_txpacket_parameter(idx++, (unsigned char) giID_List[i]);
	dxl2_set_txpacket_length(idx + 3);
	while(true)
	{
		dxl2_txrx_packet();

		if( dxl_get_comm_result() != COMM_RXSUCCESS)
			printf(" Sync Read Faild\n");

		is_moving = false;
		for(int i = 0; i < (sizeof(giID_List)/sizeof(*giID_List)); i++)
		{
			long pos = dxl2_get_sync_read_data_dword(giID_List[i], P_PRESENT_POS);
			printf(" [ID %d : %d]", giID_List[i], pos);

			if( pos > -10000 + 300)
				is_moving = true;
		}
		printf("\n");

		if(is_moving == false)
			break;
	}

	printf("/=============== sync write/read test complete ================/\n");
	printf("\n");

	printf("/==================== bulk write/read test ====================/\n");
	_getch();
	dxl2_set_txpacket_id(BROADCAST_ID);
	dxl2_set_txpacket_instruction(INST_BULK_WRITE);
	idx = 0;
	for(int i = 0; i < (sizeof(giID_List)/sizeof(*giID_List)); i++)
	{
		dxl2_set_txpacket_parameter( idx++, (unsigned char) giID_List[i]);
		dxl2_set_txpacket_parameter( idx++, DXL_LOBYTE( P_GOAL_POSITION ));
		dxl2_set_txpacket_parameter( idx++, DXL_HIBYTE( P_GOAL_POSITION ));
		dxl2_set_txpacket_parameter( idx++, DXL_LOBYTE( POSITION_LENGTH ));
		dxl2_set_txpacket_parameter( idx++, DXL_HIBYTE( POSITION_LENGTH ));
		dxl2_set_txpacket_parameter( idx++, DXL_LOBYTE(DXL_LOWORD(10000)));
		dxl2_set_txpacket_parameter( idx++, DXL_HIBYTE(DXL_LOWORD(10000)));
		dxl2_set_txpacket_parameter( idx++, DXL_LOBYTE(DXL_HIWORD(10000)));
		dxl2_set_txpacket_parameter( idx++, DXL_HIBYTE(DXL_HIWORD(10000)));
	}
	dxl2_set_txpacket_length(idx + 3);
	dxl2_txrx_packet();


	dxl2_set_txpacket_id(BROADCAST_ID);
	dxl2_set_txpacket_instruction(INST_BULK_READ);
	idx = 0;
	for(int i = 0; i < (sizeof(giID_List)/sizeof(*giID_List)); i++)
	{
		dxl2_set_txpacket_parameter( idx++, (unsigned char) giID_List[i]);
		dxl2_set_txpacket_parameter( idx++, DXL_LOBYTE( P_PRESENT_POS ));
		dxl2_set_txpacket_parameter( idx++, DXL_HIBYTE( P_PRESENT_POS ));
		dxl2_set_txpacket_parameter( idx++, DXL_LOBYTE( POSITION_LENGTH ));
		dxl2_set_txpacket_parameter( idx++, DXL_HIBYTE( POSITION_LENGTH ));
	}
	dxl2_set_txpacket_length(idx + 3);
	while(true)
	{
		dxl2_txrx_packet();

		if( dxl_get_comm_result() != COMM_RXSUCCESS)
			printf(" Bulk Read Faild\n");

		is_moving = false;
		for(int i = 0; i < (sizeof(giID_List)/sizeof(*giID_List)); i++)
		{
			long pos = dxl2_get_bulk_read_data_dword(giID_List[i], P_PRESENT_POS);
			printf(" [ID %d : %d]", giID_List[i], pos);

			if( pos < 10000 - 300)
				is_moving = true;
		}
		printf("\n");

		if(is_moving == false)
			break;
	}

	printf("/=============== bulk write/read test complete ================/\n");
	printf("\n");
	_getch();
}
