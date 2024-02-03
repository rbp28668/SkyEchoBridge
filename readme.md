# SkyEcho2 Bridge

This bridges the GDL-90 data sent out via UDP from a SkyEcho2 and the FLARM port protocol
understood by Oudies, S80/100 varios etc.
Written to run on a Raspberry Pi (Pi zero W in particualar) but there's nothing Pi specific
so should run on pretty much anything Linux.  

It *may* be possible to port it to a RPi PicoW which would give a very low power compact solution.

Based loosely on the Python code originally by Davis Chappins and later by Bogdan Manoiu

Note, this wasn't written specifically for XCSoar so uses my naming/coding conventions.  There's 
the odd static buffer lying about because in this application that *just works*.  Should be easy
enough to change.

## Building
This uses GCC and CMake.  You should just be able to use cmake . followed by make to get an 
executable.