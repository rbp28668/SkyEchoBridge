# SkyEcho2 Bridge

This bridges the GDL-90 data sent out via UDP from a SkyEcho2 and the FLARM port protocol
understood by Oudies, S80/100 varios etc.
Written to run on a Raspberry Pi (Pi zero W in particualar) but there's nothing Pi specific
so should run on pretty much anything Linux.  

It *may* be possible to port it to a RPi PicoW which would give a very low power compact solution.

Based loosely on the Python code originally by Davis Chappins and later by Bogdan Manoiu

Note, this wasn't written specifically for XCSoar so uses my naming/coding conventions. That said, 
shouldn't be that hard to change. 

## Building
This uses GCC and CMake.  You should just be able to use cmake . followed by make to get an 
executable.

# Output
## Console
Default output and probably the least useful.  It should however be possible to pipe stdout into another program (but see Named Pipe below).

## Serial
Outputs on the Pi's serial output.

## UDP network
This will send NMEA/FLARM data out in uDP packets.  These can be received by XCSoar IF the networking supports it - but the SkyEcho won't forward packets between connected devices.  It is however useful for testing.
## Named pipe