using System;
using System.Collections.Generic;
using System.Text;

using ROBOTIS;

namespace SyncWrite
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

        public const int POSITION_LENGTH = 4;

        static void Main(string[] args)
        {
            byte[] ID_List = new byte[] { 1, 2 };
            long[] GoalPos = new long[] { -10000, 10000 };
            int index = 0;

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

            Console.WriteLine("Torque On");
            for (int i = 0; i < ID_List.Length; i++)
            {
                dynamixel.dxl2_write_byte(ID_List[i], P_TORQUE_ENABLE, (byte)1);
                if (dynamixel.dxl_get_comm_result() != dynamixel.COMM_RXSUCCESS)
                    Console.WriteLine("ID {0:d} : Torque On failed!", ID_List[i]);
            }

            while (true)
            {
                Console.WriteLine("Press any key to SyncWrite goal position! (Press ESC to quit)");
                if (Console.ReadKey(true).Key == ConsoleKey.Escape)
                    break;

                // SyncWrite Goal Position
                dynamixel.dxl2_set_txpacket_id(dynamixel.BROADCAST_ID);
                dynamixel.dxl2_set_txpacket_instruction(dynamixel.INST_SYNC_WRITE);

                UInt16 param_idx = 0;
                dynamixel.dxl2_set_txpacket_parameter(param_idx++, dynamixel.DXL_LOBYTE(P_GOAL_POSITION_L));
                dynamixel.dxl2_set_txpacket_parameter(param_idx++, dynamixel.DXL_HIBYTE(P_GOAL_POSITION_L));
                dynamixel.dxl2_set_txpacket_parameter(param_idx++, dynamixel.DXL_LOBYTE(POSITION_LENGTH));
                dynamixel.dxl2_set_txpacket_parameter(param_idx++, dynamixel.DXL_HIBYTE(POSITION_LENGTH));
                for (int i = 0; i < ID_List.Length; i++)
                {
                    dynamixel.dxl2_set_txpacket_parameter(param_idx++, ID_List[i]);
                    dynamixel.dxl2_set_txpacket_parameter(param_idx++, dynamixel.DXL_LOBYTE(dynamixel.DXL_LOWORD((UInt32)GoalPos[index])));
                    dynamixel.dxl2_set_txpacket_parameter(param_idx++, dynamixel.DXL_HIBYTE(dynamixel.DXL_LOWORD((UInt32)GoalPos[index])));
                    dynamixel.dxl2_set_txpacket_parameter(param_idx++, dynamixel.DXL_LOBYTE(dynamixel.DXL_HIWORD((UInt32)GoalPos[index])));
                    dynamixel.dxl2_set_txpacket_parameter(param_idx++, dynamixel.DXL_HIBYTE(dynamixel.DXL_HIWORD((UInt32)GoalPos[index])));
                }
                dynamixel.dxl2_set_txpacket_length((UInt16)(param_idx + 3));
                dynamixel.dxl2_txrx_packet();

                if (index == 0) index = 1;
                else index = 0;


                Console.WriteLine("Press any key to SyncRead present position! (Press ESC to quit)");
                if (Console.ReadKey(true).Key == ConsoleKey.Escape)
                    break;

                // SyncRead Present Position
                dynamixel.dxl2_set_txpacket_id(dynamixel.BROADCAST_ID);
                dynamixel.dxl2_set_txpacket_instruction(dynamixel.INST_SYNC_READ);

                param_idx = 0;
                dynamixel.dxl2_set_txpacket_parameter(param_idx++, dynamixel.DXL_LOBYTE(P_PRESENT_POSITION_L));
                dynamixel.dxl2_set_txpacket_parameter(param_idx++, dynamixel.DXL_HIBYTE(P_PRESENT_POSITION_L));
                dynamixel.dxl2_set_txpacket_parameter(param_idx++, dynamixel.DXL_LOBYTE(POSITION_LENGTH));
                dynamixel.dxl2_set_txpacket_parameter(param_idx++, dynamixel.DXL_HIBYTE(POSITION_LENGTH));
                for (int i = 0; i < ID_List.Length; i++)
                    dynamixel.dxl2_set_txpacket_parameter(param_idx++, ID_List[i]);
                dynamixel.dxl2_set_txpacket_length((UInt16)(param_idx + 3));
                dynamixel.dxl2_txrx_packet();
                if (dynamixel.dxl_get_comm_result() == dynamixel.COMM_RXSUCCESS)
                {
                    for (int i = 0; i < ID_List.Length; i++)
                        Console.WriteLine("ID {0:d} Present Position : {1:d}", ID_List[i], (Int32)dynamixel.dxl2_get_sync_read_data_dword(ID_List[i], P_PRESENT_POSITION_L));
                }
                else
                    Console.WriteLine("SyncRead failed!");
            }

            dynamixel.dxl_terminate();
            Console.WriteLine("Press any key to terminate...");
            Console.ReadKey(true);
        }
    }
}
