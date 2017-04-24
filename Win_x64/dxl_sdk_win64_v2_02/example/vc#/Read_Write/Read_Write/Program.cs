using System;
using System.Collections.Generic;
using System.Text;

using ROBOTIS;

namespace Read_Write
{
    class Program
    {
        // Control table address (for DXL Pro)
        public const int P_TORQUE_ENABLE        = 562;
        public const int P_GOAL_POSITION_L      = 596;
        public const int P_PRESENT_POSITION_L   = 611;
        public const int P_MOVING               = 610;

        // Default settings
        public const int DEFAULT_PORT   = 4;    // COM4
        public const int DEFAULT_BAUD   = 57600;
        public const int DEFAULT_ID     = 1;
        
        static void Main(string[] args)
        {
            long[] GoalPos = new long[] {-10000, 10000};
            int index = 0;
            int Moving;
            ulong PresentPos;
            int CommStatus;

            // Open device
            if (dynamixel.dxl_initialize(DEFAULT_PORT, DEFAULT_BAUD) == 0)
            {
                Console.WriteLine("Failed to open USB2Dynamixel!");
                Console.WriteLine("Press any key to terminate...");
                Console.ReadKey(true);
                return;
            }
            else
                Console.WriteLine("Succeed to open USB2Dynamixel!");

            dynamixel.dxl2_write_byte(DEFAULT_ID, P_TORQUE_ENABLE, 1);
            CommStatus = dynamixel.dxl_get_comm_result();
            if (CommStatus == dynamixel.COMM_RXSUCCESS)
            {
                PrintErrorCode();
            }
            else
            {
                PrintCommStatus(CommStatus);
                Console.WriteLine("Press any key to terminate...");
                Console.ReadKey(true);
                return;
            }

            while (true)
            {
                Console.WriteLine("Press any key to continue! (Press ESC to quit)");
                if (Console.ReadKey(true).Key == ConsoleKey.Escape)
                    break;

                dynamixel.dxl2_write_dword(DEFAULT_ID, P_GOAL_POSITION_L, (UInt32)GoalPos[index]);
                do
                {
                    PresentPos = dynamixel.dxl2_read_dword(DEFAULT_ID, P_PRESENT_POSITION_L);
                    CommStatus = dynamixel.dxl_get_comm_result();
                    if (CommStatus == dynamixel.COMM_RXSUCCESS)
                    {
                        Console.WriteLine("{0:d}    {1:d}", GoalPos[index], (Int32)PresentPos);
                        PrintErrorCode();
                    }
                    else
                    {
                        PrintCommStatus(CommStatus);
                        break;
                    }

                    // Check moving done
                    Moving = dynamixel.dxl2_read_byte(DEFAULT_ID, P_MOVING);
                    CommStatus = dynamixel.dxl_get_comm_result();
                    if (CommStatus == dynamixel.COMM_RXSUCCESS)
                    {
                        if (Moving == 0)
                        {
                            // Change goal position
                            if (index == 0)
                                index = 1;
                            else
                                index = 0;
                        }
                        PrintErrorCode();
                    }
                    else
                    {
                        PrintCommStatus(CommStatus);
                        break;
                    }
                } while (Moving == 1);
            }

            dynamixel.dxl_terminate();
            Console.WriteLine("Press any key to terminate...");
            Console.ReadKey(true);
        }

        // Print communication result
        static void PrintCommStatus(int CommStatus)
        {
            switch (CommStatus)
            {
                case dynamixel.COMM_TXFAIL:
                    Console.WriteLine("COMM_TXFAIL: Failed transmit instruction packet!");
                    break;

                case dynamixel.COMM_TXERROR:
                    Console.WriteLine("COMM_TXERROR: Incorrect instruction packet!");
                    break;

                case dynamixel.COMM_RXFAIL:
                    Console.WriteLine("COMM_RXFAIL: Failed get status packet from device!");
                    break;

                case dynamixel.COMM_RXWAITING:
                    Console.WriteLine("COMM_RXWAITING: Now recieving status packet!");
                    break;

                case dynamixel.COMM_RXTIMEOUT:
                    Console.WriteLine("COMM_RXTIMEOUT: There is no status packet!");
                    break;

                case dynamixel.COMM_RXCORRUPT:
                    Console.WriteLine("COMM_RXCORRUPT: Incorrect status packet!");
                    break;

                default:
                    Console.WriteLine("This is unknown error code!");
                    break;
            }
        }

        // Print error bit of status packet
        static void PrintErrorCode()
        {
            int error = dynamixel.dxl2_get_rxpacket_error_byte();

            if ((error & dynamixel.ERRBIT_ALERT) != 0)
                Console.WriteLine("Device Error! Check the \"Device Status Check\" value. \n");

            if ((error & 0x7F) == dynamixel.ERR_RESULT_FAIL)
                Console.WriteLine("Failed to process the instruction packet! \n");
            else if ((error & 0x7F) == dynamixel.ERR_INSTRUCTION)
                Console.WriteLine("Instruction error! \n");
            else if ((error & 0x7F) == dynamixel.ERR_CRC)
                Console.WriteLine("CRC check error! \n");
            else if ((error & 0x7F) == dynamixel.ERR_DATA_RANGE)
                Console.WriteLine("Data range error! \n");
            else if ((error & 0x7F) == dynamixel.ERR_DATA_LENGTH)
                Console.WriteLine("Data length error! \n");
            else if ((error & 0x7F) == dynamixel.ERR_DATA_LIMIT)
                Console.WriteLine("Data limit error! \n");
            else if ((error & 0x7F) == dynamixel.ERR_ACCESS)
                Console.WriteLine("Access error! \n");
        }
    }
}
