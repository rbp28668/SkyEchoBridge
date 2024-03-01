# SEBridge_hc06.py
This configures a HC06 bluetooth module connected to a Raspberry Pi's 
serial port (on the GPIO pins) to 38400 baud,  pin of 1234 and 
bluetooth name of SE2BRG.

The same software should work on A PC (both Windows and Linux) 
with no more than a change of the DEVICE constant near the top of the file.

This works with both v2 and v3 of the HC06 firmware - the command
set is quite different between the two.  It also scans through
pretty much all possible baud rates until it gets an 'OK' from the
device so it shouldn't matter how it was configured initially.

Once configured ./SkyEcho2 --serial /dev/ttyAMA0 38400 should send
data out through it.


Meanwhile, on an Android serial terminal connected via bluetooth I receive:
22:18:07.966 $GPRMC,221807.000,A,5159.9991,N,00025.0333,W,75.00,90.00,01324,0.000,W,*71
22:18:08.962 $PGRMZ,2000,F,2*08
22:18:08.962 $GPGGA,221807.000,5159.9991,N,00025.0333,W,1,12,10,2000.0,F,,,,,0000*2E
22:18:08.962 $PFLAA,0,1976,-1965,0,000001,0,,38,0,A*3E
22:18:08.964 $PFLAU,1,2,0,1,-45,0,0,2786,000001*46
