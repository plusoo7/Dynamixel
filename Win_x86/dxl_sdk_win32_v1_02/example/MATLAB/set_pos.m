GoalPos=[0 1023];
DEFAULT_PORTNUM = 1;  %COM16
DEFAULT_BAUDNUM = 1%1Mbps
res = calllib('dynamixel','dxl_initialize',DEFAULT_PORTNUM,DEFAULT_BAUDNUM);
calllib('dynamixel','dxl_write_word',0,30,GoalPos(1));
pause(1);
calllib('dynamixel','dxl_write_word',0,30,GoalPos(2));
calllib('dynamixel','dxl_terminate');