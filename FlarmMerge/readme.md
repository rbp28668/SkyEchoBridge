# FLARM Data Merge
Receiving the data from the SkyEcho is only part of the problem.  The data can be sent directly
to a device (such as XCSoar) especially if that device can be configured to receive data from
multiple sources.

However, the S80/Oudie combination expects to receive data from FLARM only.  The data is also sent
over a physical RS-232 link. This part of the jigsaw then manages reception, merging and retransmission
of the FLARM data with the converted messages from SkyEchoBridge.

## Merging algorithm.

The FLARM PFLAU messages are the ones that drive the FLARM display.  The aim then is to forward these 
from the FLARM with minimal delay and only subsitute a GDL-90 derived message if the latter has a 
higher alert score or there is a target with a traffic alert.

FLARM PFLAA messages are buffered along with GDL-90 dervived PFLAA messsages then sorted by urgency.
If GLD-90 messages have the same ICAO ID as a FLARM message then they are discarded to prevent dual
target indication.

This process is triggered by a PFLAU message being received and sent.  This means that the PFLAA messages
precede the next PFLAU - in effect they're delayed by 1 second.  Note however that the critical PLFAU 
message is not delayed.

## Communications
The converted GLD-90 data is received over a named pipe.  This isolates this merging code 
(which has some near-real-time constraints to deal with serial data) from the core SkyEchoBridge code.
Using named pipes makes deploying this optional and avoids the program having to fork.

## FLARM
Note FLARM is a trademark of:

FLARM Technology Ltd

Hinterbergstrasse 15

CH-6330 Cham