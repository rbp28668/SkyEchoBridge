#  TrafficGen
TrafficGen allows dummy GDL-90 traffic to be sent to SkyEcho2 programme as if it had come from an actual SkyEcho2 device.  TrafficGen includes the Lua scripting language so that test scenarios can be scripted with control over the "ownship" aircraft and other target aircraft.

The command line options are as follows:
``` 
--help                      - display this information.
--udp <ip_address> <port>   - send data to this IP and port
```

Note that the taget IP and target port default to  "127.0.0.1" and 4000 respectively.

Any other parameters passed on the command line are assumed to be names of Lua scripting files.

test.lua is an example file:

```
print("Ownship properties")
print(ownship:callsign())
print(ownship:latitude())
print(ownship:longitude())


ownship:setSpeedKts(75)
ownship:setAltFeet( 2000 )
ownship:setTrack(90)

t = simulation.createTarget()
print("Target ", t:callsign())
t:setEmitter(Emitter.GLIDER) 
t:setSpeedKts(75);
t:setAltFeet(2000);

lat, lon = ownship:getLocation()
t:setLatitude(lat)
t:setLongitude(lon)

print("Ownship: ", ownship:getLocation())
print("T: ", t:latitude(), t:longitude())

t:makeConflictingTo(ownship, 2, 90)

for i = 1,180,1 do
    simulation.tick();
    print(i, t:latitude(), t:longitude(), ownship:distanceTo(t))
end
```

See [Scripting Objects](./scripting.md) for details of the scripting constants, objects and methods.

