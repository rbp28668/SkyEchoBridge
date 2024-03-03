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
This uses GCC and CMake.  You should just be able to use `cmake .` followed by `make` to get an executable.

# Output
## Console
Default output and probably the least useful.  It should however be possible to pipe stdout into another program (but see Named Pipe below).

`./SkyEcho2 --console`

## Serial
Outputs on the Pi's serial output.   Useful if wired directly into a display device or an HC06 serial to bluetooth module.

`./SkyEcho2 --serial /dev/ttyAMA0 38400`


## UDP network
This will send NMEA/FLARM data out in uDP packets.  These can be received by XCSoar IF the networking supports it - but the SkyEcho won't forward packets between connected devices.  It is however useful for testing.

`./SkyEcho2 --udp 192.168.0.42 4353`

## Named pipe
Intended for use with the FlarmMerge program and outputs to a named pipe / fifo.  The FlarmMerge program should be listening on the other end of the pipe and should merge the GDL-90 derived messages with the real Flarm messages

`./SkyEcho2 --pipe /opt/sky_echo/sky_echo_fifo`

# Options

Command line options are as follows.  Most have sensible defaults:

```
--help                            - display this information.
--listen <port>                   - listen for GDL-90 packets on this port
--console  (default)              - set output device to console (stdout).
--pipe <fifo>                     - set output device to named pipe.
--serial <device> <baud>          - set output device to serial port.
--udp <ip_address> <port>         - set output device to UDP socket
--old_target <value>              - time in seconds to keep target information without an update.
--ignore_distance  <value>        - ignore targets further away than this (metres).
--ignore_height  <value>          - ignore targets with a larger height difference than this (metres).
--advise_distance  <value>        - distance within which to trigger a traffic advisory.
--advise_height  <value>          - height difference within which to trigger a traffic advisory.
--threat_distance  <value>        - threat score threshold horizontal distance
--threat_height  <value>          - threat score threshold vertical distance
--threat_time  <value>            - threat score threshold time to closest approach
--threat_bubble_distance  <value> - treat targets closer than this as a possible threat (metres).
--threat_bubble_height  <value>   - treat targets with a closer height difference as a possible threat (metres).
```