class DynamixelFcn
{
/// Table Control definition////
	public byte MOTOR_ID				= 3;
	public byte BAUDRATE				= 4;
	public byte TORQUE_ENABLE		= 24;
	public byte CW_MARGIN			= 26;
	public byte CCW_MARGIN			= 27;
	public byte CW_SLOPE				= 28;
	public byte CCW_SLOPE			= 29;
	public byte GOAL_POS_L			= 30;
	public byte GOAL_POS_H			= 31;
	public byte SPEED_L				= 32;
	public byte SPEED_H				= 33;
	public byte TORQUE_LIMIT_L		= 34;
	public byte TORQUE_LIMIT_H		= 35;
	public byte PRESENT_POS_L		= 36;
	public byte PRESENT_POS_H		= 37;
	public byte PRESENT_SPEED_L		= 38;
	public byte PRESENT_SPEED_H		= 39;
	public byte PRESENT_LOAD_L		= 40;
	public byte PRESENT_LOAD_H		= 41;
	public byte PRESENT_VOLTAGE		= 42;
	public byte PRESENT_VTEM			= 43;
	public byte MOVING				= 46;
	public byte PUNCH_L				= 48;
	public byte PUNCH_H				= 49;
	
///////////// set/get packet methods //////////////////////////
	public byte MAXNUM_TXPARAM = 150; 	// Maximum length of TX data 
	public byte MAXNUM_RXPARAM = 60;  	// Maximum length of RX data
	public byte BROADCAST_ID =254;		// ID for all servos connected 
	public byte INST_PING = 1; 			// No action. Used for obtaining a Status
	public byte INST_READ  = 2;			// read values in the control tables
	public byte INST_WRITE = 3;			// write values to the control table
	public byte INST_REG_WRITE = 4; 	//Similar to WRITE_DATA, but stays in standby mode until the ACTION instruction is given
	public byte INST_ACTION = 5;		// Triggers the action registered by the REG_WRITE instruction
	public byte INST_RESET = 6;			// Changes the control table values of the Dynamixel actuator to the Factory Default Valuesettings
	public byte INST_SYNC_WRITE = 131;	//Used for controlling many Dynamixel actuatorsat the same time
	
	///////////// Error Byte definition //////////////////////////
	public byte ERRBIT_VOLTAGE = 1; 	//Set to 1 if the voltage is out of the operating voltage range as defined in the control table.
	public byte ERRBIT_ANGLE = 2;		//Set as 1 if the Goal Position is set outside of the range between CW Angle Limit and CCW Angle Limit.
	public byte ERRBIT_OVERHEAT = 4; 	//Set to 1 if the internal temperature of the Dynamixel unit is above the operating 
	//temperature range as defined in the control table.
	
	public byte ERRBIT_RANGE = 8; 		//Set to 1 if the instruction sent is out of the defined range.
	public byte ERRBIT_CHECKSUM = 16;	//Set to 1 if the checksum of the instruction packet is incorrect
	public byte ERRBIT_OVERLOAD = 32;	//Set to 1 if the specified maximum torque can't control the applied load.
	public byte ERRBIT_INSTRUCTION = 64;//Set to 1 if an undefined instruction is sent or an action instruction is sent without a Reg_Write instruction.
	
	////// Definition for Transmit data///////
	//------------------------------------------------------------//
	//
	public byte ID = 2;					// position of ID byte in transmit data
	public byte LENGTH = 3;				// position of LENGTH Byte in transmit data
	public byte INSTRUCTION = 4;		// Instruction to be perform
	public byte ERRBIT = 4;				// position of Error byte in receive data
	public byte PARAMETER = 5;			// Position of PARAMETER starting byte in transmit data
	public byte gbInstructionPacket = new byte[MAXNUM_TXPARAM+10]; // Data array to be sent to controller
	public byte gbInstructionPacket1 = new byte[MAXNUM_TXPARAM+10];
	public byte action_buffer = new byte[3];
	
	public byte gbStatusPacket 		= new byte[MAXNUM_RXPARAM+10]; //// Data array to be received to controller
	public int checksum =0;
	/// Method
	public void reset_tx_data()
	{
		for (int i = ID; i<MAXNUM_TXPARAM+11;i++)
		gbInstructionPacket[i]=0;
	}
	public void my_checksum()
	{
		int i=0;
		for( i=0; i<(gbInstructionPacket[LENGTH]+1); i++ )
		checksum += gbInstructionPacket[i+2];
		gbInstructionPacket[gbInstructionPacket[LENGTH]+3] = (byte)(~checksum);
		
	}
	public void dxl_write_byte(byte servo_id, byte table_address, byte value)
	{
		gbInstructionPacket[ID] = servo_id;
		gbInstructionPacket[INSTRUCTION] = INST_REG_WRITE;
		gbInstructionPacket[PARAMETER] = table_address;
		gbInstructionPacket[PARAMETER+1] = value;
		gbInstructionPacket[LENGTH] = 4;
		my_checksum();
	}
	public void dxl_write_word(byte servo_id, byte table_address, int value)
	{
		gbInstructionPacket[ID] = servo_id;
		gbInstructionPacket[INSTRUCTION] 	= INST_REG_WRITE;
		gbInstructionPacket[PARAMETER] 		= table_address;
		gbInstructionPacket[PARAMETER+1] 	= dxl_get_lowbyte(value);
		gbInstructionPacket[PARAMETER+2] 	= dxl_get_highbyte(value);
		gbInstructionPacket[LENGTH] = 5;
		my_checksum();
	}
	byte dxl_get_low_byte(int word)
	{
		return (byte)word;
	}
	byte dxl_get_hight_byte(int word)
	{
		return (byte)(word>>8);
	}
	publicvoid  dxl_set_possition(byte motor_name, float deg)
	{
		int value = (int) (deg*1023/300);
		dxl_write_word(motor_name,GOAL_POS_L,value);
		
	}
	public static void main(String[] args)
	{
		System.out.printf("Thanh");
		
		
	}
	

	
	
}

















