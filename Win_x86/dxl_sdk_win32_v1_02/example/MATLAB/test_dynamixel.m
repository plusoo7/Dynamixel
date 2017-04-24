% (STEP 1) Load Library.
loadlibrary('dynamixel', 'dynamixel.h');
%(STEP 2) Check the list of library functions.
libfunctions('dynamixel');
DEFAULT_PORTNUM = 23;  %COM23
DEFAULT_BAUDNUM = 1  %1Mbps
calllib('dynamixel', 'dxl_initialize', DEFAULT_PORTNUM, DEFAULT_BAUDNUM);
