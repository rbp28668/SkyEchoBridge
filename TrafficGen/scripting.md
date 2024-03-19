

## Globals

ownship is the ownship.

### simulation    

```
simulation.tick() -- move the simulation forward one step / wait for next tick time.
simulation.createTarget() -- creates a target aircraft
simulation.createTargetFrom( template) -- create a target aircraft using an existing one as a template.
```

### Constants

#### Address Type

Defines the different types of address available to the aircraft / vehicle / thing.

```
Address.ADSB_IACO          -- ADS-B with ICAO address
Address.ADSB_SELF_ASSIGNED -- ADS-B with Self-assigned address
Address.TISB_ICAO          -- TIS-B with ICAO address
Address.TISB_TRACK_FILE    -- TIS-B with track file ID.
Address.SURFACE_VEHICLE    -- Surface Vehicle
Address.GROUND_STATION     -- Ground Station Beacon
```
#### Emitter

Defines the type of "thing" emitting an ADSB signal. See 3.5.1.10 EMITTER CATEGORY of the GDL90 specification.

```
Emitter.UNKNOWN             -- 0 - No aircraft type information
Emitter.LIGHT               -- 1 - Light (ICAO) < 15 500 lbs
Emitter.SMALL               -- 2 - Small - 15 500 to 75 000 lbs
Emitter.LARGE               -- 3 - Large - 75 000 to 300 000 lbs
Emitter.LARGE_HV            -- 4 - High Vortex Large (e.g., aircraft such as B757)
Emitter.HEAVY               -- 5 - Heavy (ICAO) - > 300 000 lbs
Emitter.HIGHLY_MANEUVERABLE -- 6 - Highly Maneuverable > 5G acceleration and high speed
Emitter.ROTORCRAFT          -- 7 - Rotorcraft
Emitter.GLIDER              -- 9 - Glider/sailplane
Emitter.LTA                 -- 10 - Lighter than air
Emitter.PARACHUTIST         -- 11 - Parachutist/sky diver
Emitter.ULTRALIGHT          -- 12 - Ultra light/hang glider/paraglider
Emitter.UAV                 -- 14 - Unmanned aerial vehicle
Emitter.SPACE               -- 15 - Space/transatmospheric vehicle
Emitter.EMERGENCY_VEHICLE   -- 17 - Surface vehicle — emergency vehicle
Emitter.SERVICE_VEHICLE     -- 18 - Surface vehicle — service vehicle
Emitter.POINT_OBSTACLE      -- 19 - Point Obstacle (includes tethered balloons)
Emitter.CLUSTER_OBSTACLE    -- 20 - Cluster Obstacle
Emitter.LINE_OBSTACLE       -- 21 - Line Obstacle
```

#### Emergency

Defines the type of emergency (if any) that the target is experiencing.

```
Emergency.NONE                    -- no emergency
Emergency.GENERAL                 -- general emergency
Emergency.MEDICAL                 -- medical emergency
Emergency.FUEL                    -- minimum fuel
Emergency.COMMUNICATIONS          -- no communication
Emergency.UNLAWFUL_INTERFERENCE   -- unlawful interference
Emergency.DOWNED                  -- downed aircraft
```
#### Misc

```
SPEED_UNKNOWN -- The value 0xFFF is reserved to convey that no horizontal velocity information is available.
VERTICAL_SPEED_UNKNOWN -- The value 0x800 is reserved to convey that no vertical velocity information is available.
```

## Target and Ownship

In the following "t" is used for the target or ownship.

## Getters

```
t:alert() -- returns true if the alert flag is set.
t:addressType() -- returns the address type - see Address constants above.
t:address() -- returns the numeric address (usually expressed as a 6 digit hex number).
t:latitude() -- returns degrees latitude, +ve is north, -ve is south.
t:longitude() -- returns degrees longitude, +ve is east, -ve is west.
t:altFeet() -- returns the altitude in feet.
t:miscIndicators() -- returns the miscIndicators bitmap, See 3.5.1.5 MISCELLANEOUS INDICATORS of the GDL-90 spec.
t:nic() -- gets position integrety value, See 3.5.1.6 INTEGRITY (NIC) AND ACCURACY (NACP)
t:nacp() -- gets position accuracy value, See  3.5.1.6 INTEGRITY (NIC) AND ACCURACY (NACP)
t:emitter() -- gets the type of target.  See Emitter constant.
t:speedKts() what it says.
t:verticalVelocity() in feet per minute with a resolution of 64fpm. +ve is climbing.
t:track() -- track in degrees true (unless misc indicators says otherwise). Resolution is only 360/256th of a degree.
t:callsign() -- gets the callsign e.g. G-registration of the target.
t:emergency() -- gets the emergency status of the target - see Emergency constants.
```

## Setters

```
t:setAlert(alert)
t:setAddressType(addressType)
t:setLatitude(degreesLatitude)
t:setLongitude(degreesLongitude)
t:setAltFeet(altFeet)
t:setMiscIndicators(miscIndicators)
t:setNic(nic)
t:setNacp(nacp)
t:setEmitter(emitter)
t:setSpeedKts(speedKts)
t:setVerticalVelocity(verticalVelocityFps)
t:setTrack(trackDegrees)
t:setCallsign(callsign)
t:setEmergency(emergency)
```

## Misc
lat,lon = t:getLocation() -- gets location in a single call
t:move(metresNorth, metresEast) -- moves the target.  -ve values give south and west respectively.
t:makeConflictingTo(other, range, bearing) -- makes t come into conflict with other (usually ownship) starting at given range and bearing.
distMetres = t:distanceTo(other) -- gets the distance to another target.