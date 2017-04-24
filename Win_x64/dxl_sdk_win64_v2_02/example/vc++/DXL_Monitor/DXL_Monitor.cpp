#include <Windows.h>
#include <iostream>
#include <conio.h>
#include "dynamixel2.h"
#include "FunctionDefine.h"

#if _WIN64
#pragma comment(lib, "dynamixel2_win64.lib")
#else
#pragma comment(lib, "dynamixel2_win32.lib")
#endif

#define MAX_IN_CHAR 128

using namespace std;

int main(void)
{
	char input[MAX_IN_CHAR];
	char *token, *context;
	char param[20][30];
	char cmd[80];
	int port_num = 0, baud_rate = 0, input_len, num_param;

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

	while(true)
	{
		printf("[CMD] ");
		gets_s(input);
		fflush(stdin);

		input_len = strlen(input);
		if(input_len == 0)
			continue;

		token = strtok_s(input, " ", &context);
		if(token == 0)
			continue;

		strcpy_s(cmd, token);
		token = strtok_s(0, " ", &context);
		num_param = 0;
		while(token != 0)
		{
			strcpy_s(param[num_param++], token);
			token = strtok_s(0, " ", &context);
		}
		if(strcmp(cmd, "help") == 0 || strcmp(cmd, "h") == 0)
		{
			Help();
		}
		else if(strcmp(cmd, "baud") == 0)
		{
			if(num_param == 1)
			{
				if(dxl_change_baudrate(atoi(param[0])) == 0)
				{
					printf( "Failed to change baudrate!\n" );
					continue;
				}
				baud_rate = atoi(param[0]);
				printf("\n Success to change baudrate! [baud num : %d]\n\n", baud_rate);
			}
			else
			{
				printf(" Invalid parameter!\n");
				continue;
			}
		}
		else if(strcmp(cmd, "exit") == 0)
		{
			dxl_terminate();
			return 0;
		}
		else if(strcmp(cmd, "scan") == 0)
		{
			Scan();
		}
		else if(strcmp(cmd, "ping") == 0)
		{
			if(num_param == 0)
			{
				printf(" Invalid parameter!\n");
				continue;
			}

			printf("\n");
			printf("Ping Using Protocol 1.0\n");
			for(int i = 0; i < num_param; i++)
			{
				dxl_ping(atoi(param[i]));
				if( dxl_get_comm_result() == COMM_RXSUCCESS)
					printf("                   ... SUCCESS \r");
				else
					printf("                   ... FAIL \r");
				printf(" CHECK ID : %d \n", atoi(param[i]));
			}
			printf("\n");

			printf("\n");
			printf("Ping Using Protocol 2.0\n");
			for(int i = 0; i < num_param; i++)
			{
				dxl2_ping(atoi(param[i]));
				if( dxl_get_comm_result() == COMM_RXSUCCESS)
					printf("                   ... SUCCESS \r");
				else
					printf("                   ... FAIL \r");
				printf(" CHECK ID : %d \n", atoi(param[i]));
			}
			printf("\n");
		}
		else if(strcmp(cmd, "bp") == 0)
		{
			BroadcastPing();
		}
		else if(strcmp(cmd, "wrb") == 0 || strcmp(cmd, "w") == 0)
		{
			if(num_param == 3)
				Write(atoi(param[0]), atoi(param[1]), atoi(param[2]), 1);
			else
			{
				printf(" Invalid parameter!\n");
				continue;
			}
		}
		else if(strcmp(cmd, "wrb2") == 0 || strcmp(cmd, "w2") == 0)
		{
			if(num_param == 3)
				Write2(atoi(param[0]), atoi(param[1]), atoi(param[2]), 1);
			else
			{
				printf(" Invalid parameter!\n");
				continue;
			}
		}
		else if(strcmp(cmd, "wrw") == 0)
		{
			if(num_param == 3)
				Write(atoi(param[0]), atoi(param[1]), atoi(param[2]), 2);
			else
			{
				printf(" Invalid parameter!\n");
				continue;
			}
		}
		else if(strcmp(cmd, "wrw2") == 0)
		{
			if(num_param == 3)
				Write2(atoi(param[0]), atoi(param[1]), atoi(param[2]), 2);
			else
			{
				printf(" Invalid parameter!\n");
				continue;
			}
		}
		else if(strcmp(cmd, "wrd2") == 0)
		{
			if(num_param == 3)
				Write2(atoi(param[0]), atoi(param[1]), atoi(param[2]), 4);
			else
			{
				printf(" Invalid parameter!\n");
				continue;
			}
		}
		else if(strcmp(cmd, "rdb") == 0)
		{
			if(num_param == 2)
				Read(atoi(param[0]), atoi(param[1]), 1);
			else
			{
				printf(" Invalid parameter!\n");
				continue;
			}
		}
		else if(strcmp(cmd, "rdb2") == 0)
		{
			if(num_param == 2)
				Read2(atoi(param[0]), atoi(param[1]), 1);
			else
			{
				printf(" Invalid parameter!\n");
				continue;
			}
		}
		else if(strcmp(cmd, "rdw") == 0)
		{
			if(num_param == 2)
				Read(atoi(param[0]), atoi(param[1]), 2);
			else
			{
				printf(" Invalid parameter!\n");
				continue;
			}
		}
		else if(strcmp(cmd, "rdw2") == 0)
		{
			if(num_param == 2)
				Read2(atoi(param[0]), atoi(param[1]), 2);
			else
			{
				printf(" Invalid parameter!\n");
				continue;
			}
		}
		else if(strcmp(cmd, "rdd2") == 0)
		{
			if(num_param == 2)
				Read2(atoi(param[0]), atoi(param[1]), 4);
			else
			{
				printf(" Invalid parameter!\n");
				continue;
			}
		}
		else if(strcmp(cmd, "r") == 0)
		{
			if(num_param == 3)
				ReadTable(atoi(param[0]), atoi(param[1]), atoi(param[2]));
			else
			{
				printf(" Invalid parameter!\n");
				continue;
			}
		}
		else if(strcmp(cmd, "r2") == 0)
		{
			if(num_param == 3)
				ReadTable2(atoi(param[0]), atoi(param[1]), atoi(param[2]));
			else
			{
				printf(" Invalid parameter!\n");
				continue;
			}
		}
		else if(strcmp(cmd, "reboot") == 0 || strcmp(cmd, "rbt") == 0)
        {
            if(num_param == 1)
			{
				dxl2_reboot(atoi(param[0]));
				if(dxl_get_comm_result() != COMM_RXSUCCESS)
					printf("Failed to reboot! (result: %d)\n", dxl_get_comm_result());
			}
            else
            {
                printf(" Invalid parameter!\n");
                continue;
            }
        }
        else if(strcmp(cmd, "reset") == 0 || strcmp(cmd, "rst") == 0)
        {
            if(num_param == 2)
			{
				dxl2_factory_reset(atoi(param[0]), atoi(param[1]));
                if(dxl_get_comm_result() != COMM_RXSUCCESS)
					printf("Failed to reset! (result: %d)\n", dxl_get_comm_result());
			}
            else
            {
                printf(" Invalid parameter!\n");
                continue;
            }
        }
		else
		{
			printf(" Bad command! Please input 'help'.\n");
		}
	}
}