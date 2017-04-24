#include <stdio.h>
#include <conio.h>
#include "dynamixel2.h"
#include "FunctionDefine.h"

void Help()
{
    printf("\n");
    printf(" ============================= DXL Monitor Help =============================\n");
	printf(" ========================== Common Command ==========================\n");
	printf(" help                      :Displays help information\n");
	printf(" baud [BAUD_RATE]          :Baudrate change to [BAUD_RATE] \n");
    printf(" exit                      :Exit this program\n");
    printf(" scan                      :Outputs the current status of all DXLs\n");
    printf(" ping [ID] [ID] ...        :Outputs the current status of [ID]s \n");
    printf(" bp                        :Broadcast ping (DXL Protocol 2.0 only)\n");
	printf(" \n");
    printf(" ======================= for DXL Protocol 1.0 =======================\n");
	printf(" wrb|w [ID] [ADDR] [VALUE] :Write byte [VALUE] to [ADDR] of [ID]\n");
    printf(" wrw [ID] [ADDR] [VALUE]   :Write word [VALUE] to [ADDR] of [ID]\n");
    printf(" rdb [ID] [ADDR]           :Read byte value from [ADDR] of [ID]\n");
    printf(" rdw [ID] [ADDR]           :Read word value from [ADDR] of [ID]\n");
    printf(" r [ID] [ADDR] [LENGTH]    :Dumps the control table of [ID]\n");
    printf("                             ([LENGTH] bytes from [ADDR])\n");
	printf(" \n");
	printf(" ======================= for DXL Protocol 2.0 =======================\n");
	printf(" wrb2|w2 [ID] [ADDR] [VALUE] :Write byte [VALUE] to [ADDR] of [ID]\n");
    printf(" wrw2 [ID] [ADDR] [VALUE]  :Write word [VALUE] to [ADDR] of [ID]\n");
    printf(" wrd2 [ID] [ADDR] [VALUE]  :Write dword [VALUE] to [ADDR] of [ID]\n");
    printf(" rdb2 [ID] [ADDR]          :Read byte value from [ADDR] of [ID]\n");
    printf(" rdw2 [ID] [ADDR]          :Read word value from [ADDR] of [ID]\n");
    printf(" rdd2 [ID] [ADDR]          :Read dword value from [ADDR] of [ID]\n");
    printf(" r2 [ID] [ADDR] [LENGTH]   :Dumps the control table of [ID]\n");
    printf("                             ([LENGTH] bytes from [ADDR])\n");
    printf(" reboot|rbt [ID]           :Reboot the Dynamixel of [ID]\n");
    printf(" reset|rst [ID] [OPTION]   :Factory reset the Dynamixel of [ID]\n");
	printf("                             OPTION: 255(All), 1(Except ID), 2(Except ID&Baud)\n");
    
    printf("\n");
}

void Scan()
{
    printf("\n");
	printf("Scan DXL Using Protocol 1.0\n");
    for(int id = 1; id < 253; id++)
    {
		dxl_ping(id);
        if( dxl_get_comm_result() == COMM_RXSUCCESS)
        {
            printf("\n                              ... OK \r");
            printf("CHECK ID: %d \n", id);
        }
        else
        {
            printf(".");
        }

        if(_kbhit())
        {
            char c = _getch();
            if(c == 0x1b)
                break;
        }
    }
    printf("\n");

    printf("\n");
	printf("Scan DXL Using Protocol 2.0\n");
    for(int id = 1; id < 253; id++)
    {
		dxl2_ping(id);
        if( dxl_get_comm_result() == COMM_RXSUCCESS)
        {
            printf("\n                              ... OK \r");
            printf("CHECK ID: %d \n", id);
        }
        else
        {
            printf(".");
        }

        if(_kbhit())
        {
            char c = _getch();
            if(c == 0x1b)
                break;
        }
    }
    printf("\n");
}

void BroadcastPing()
{
	int i = 0, count = 0;

	dxl2_broadcast_ping();

	printf("\n");
	for(int id = 1; id <= MAX_ID; id++)
	{
		int model_num = dxl2_get_ping_result(id, PING_INFO_MODEL_NUM);
		if(model_num != 0)
			printf(" [ID:%.3d] MODEL : %.5d (0x%.2X 0x%.2X) , FIRM VER : %d \n", id, model_num, 
					DXL_LOBYTE(model_num), DXL_HIBYTE(model_num), dxl2_get_ping_result(id, PING_INFO_FIRM_VER));
	}
	printf("\n");
}

void Write( int id, int addr, unsigned int value, int len)
{
    int result = COMM_TXFAIL, error = 0;

    if(len == 1)
        dxl_write_byte(id, addr, (int)value);
    else if(len == 2)
        dxl_write_word(id, addr, (int)value);

	result = dxl_get_comm_result();

    if(result != COMM_RXSUCCESS)
    {
        printf("Fail to write!\n");
        return;
    }
	else
		printf("Writing successful!\n");

	error = dxl_get_rxpacket_error_byte();

    if(error != 0)
    {
        PrintErrorCode(error);
        return;
    }
}

void Write2( int id, int addr, unsigned int value, int len)
{
    int result = COMM_TXFAIL, error = 0;

    if(len == 1)
        dxl2_write_byte(id, addr, (int)value);
    else if(len == 2)
        dxl2_write_word(id, addr, (int)value);
    else if(len == 4)
        dxl2_write_dword(id, addr, value);

	result = dxl_get_comm_result();

    if(result != COMM_RXSUCCESS)
    {
        printf("Fail to write!\n");
        return;
    }
	else
		printf("Writing successful!\n");

	error = dxl2_get_rxpacket_error_byte();

    if(error != 0)
    {
        PrintErrorCode(error);
        return;
    }
}

void Read(int id, int addr, int len)
{
    int result = COMM_TXFAIL, error = 0;
    unsigned int value = 0;
    int ivalue = 0;

    if(len == 1)
        ivalue = dxl_read_byte(id, addr);
    else if(len == 2)
        ivalue = dxl_read_word(id, addr);

	result = dxl_get_comm_result();
	error = dxl_get_rxpacket_error_byte();

    if(result != COMM_RXSUCCESS)
    {
        printf("Fail to read!\n");
        return;
    }

    if(len == 1 || len == 2)
        value = (unsigned int)ivalue;
	printf("READ VALUE : [ SIGNED: %d ] , [ UNSIGNED: %u ] \n", value, value);

    if(error != 0)
    {
        PrintErrorCode(error);
        return;
    }
}

void Read2(int id, int addr, int len)
{
    int result = COMM_TXFAIL, error = 0;
    unsigned long value = 0;
    long lvalue = 0;

    if(len == 1)
        lvalue = dxl2_read_byte(id, addr);
    else if(len == 2)
        lvalue = dxl2_read_word(id, addr);
    else if(len == 4)
        lvalue = dxl2_read_dword(id, addr);

	result = dxl_get_comm_result();
	error = dxl2_get_rxpacket_error_byte();

    if(result != COMM_RXSUCCESS)
    {
        printf("Fail to read!\n");
        return;
    }

    if(len == 1 || len == 2 || len == 4)
        value = (unsigned long)lvalue;
	printf("READ VALUE : [ SIGNED: %d ] , [ UNSIGNED: %u ] \n", value, value);

    if(error != 0)
    {
        PrintErrorCode(error);
        return;
    }
}

void ReadTable(int id, int addr, int len)
{
	if(addr < 0 || addr > 65535)
	{
		printf("Address is not available!\n");
		return;
	}
	if(len < 0 || len > 65535)
	{
		printf("Length is not available!\n");
		return;
	}

	dxl_set_txpacket_id((unsigned char)id);
	dxl_set_txpacket_length(7);
	dxl_set_txpacket_instruction(INST_READ);
	dxl_set_txpacket_parameter(0, DXL_LOBYTE(addr));
	dxl_set_txpacket_parameter(1, DXL_HIBYTE(addr));
	dxl_set_txpacket_parameter(2, DXL_LOBYTE(len));
	dxl_set_txpacket_parameter(3, DXL_HIBYTE(len));

	dxl_txrx_packet();

	if(dxl_get_comm_result() == COMM_RXSUCCESS && id != BROADCAST_ID)
	{
		printf("\n");
		for(int i = addr; i < addr + len; i++)
			printf("ADDR %.2d [0x%.2X] :     %.3d [0x%.2X] \n", i, i, dxl_get_rxpacket_parameter(i - addr), dxl_get_rxpacket_parameter(i - addr));
		printf("\n");
	}
}

void ReadTable2(int id, int addr, int len)
{
	if(addr < 0 || addr > 65535)
	{
		printf("Address is not available!\n");
		return;
	}
	if(len < 0 || len > 65535)
	{
		printf("Length is not available!\n");
		return;
	}

	dxl2_set_txpacket_id((unsigned char)id);
	dxl2_set_txpacket_length(7);
	dxl2_set_txpacket_instruction(INST_READ);
	dxl2_set_txpacket_parameter(0, DXL_LOBYTE(addr));
	dxl2_set_txpacket_parameter(1, DXL_HIBYTE(addr));
	dxl2_set_txpacket_parameter(2, DXL_LOBYTE(len));
	dxl2_set_txpacket_parameter(3, DXL_HIBYTE(len));

	dxl2_txrx_packet();

	if(dxl_get_comm_result() == COMM_RXSUCCESS && id != BROADCAST_ID)
	{
		printf("\n");
		for(int i = addr; i < addr + len; i++)
			printf("ADDR %.2d [0x%.2X] :     %.3d [0x%.2X] \n", i, i, dxl2_get_rxpacket_parameter(i - addr), dxl2_get_rxpacket_parameter(i - addr));
		printf("\n");
	}
}

void PrintErrorCode(int ErrorCode)
{
	printf("ErrorCode : %d (0x%X)\n", ErrorCode, ErrorCode);

	if(ErrorCode & ERRBIT_ALERT)
		printf("Device Error! Check the \"Device Status Check\" value. \n");

    if((ErrorCode & 0x7F) == ERR_RESULT_FAIL)
        printf("Failed to process the instruction packet!\n");
	else if((ErrorCode & 0x7F) == ERR_INSTRUCTION)
		printf("Instruction error!\n");
	else if((ErrorCode & 0x7F) == ERR_CRC)
		printf("CRC check error!\n");
	else if((ErrorCode & 0x7F) == ERR_DATA_RANGE)
		printf("Data range error!\n");
	else if((ErrorCode & 0x7F) == ERR_DATA_LENGTH)
		printf("Data length error!\n");
	else if((ErrorCode & 0x7F) == ERR_DATA_LIMIT)
		printf("Data limit error!\n");
	else if((ErrorCode & 0x7F) == ERR_ACCESS)
		printf("Access error!\n");
}