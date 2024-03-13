
require "math"

print('Closing head on')

head_on = function(track)
    ownship:setTrack(track)

    t = simulation.createTarget()
    print("Target ", t:callsign())
    t:setSpeedKts(75);
    t:setAltFeet(2000);
    t:setTrack((track + 180) % 360) -- reciprocal to ownship
    t:setEmitter(Emitter.GLIDER) 
    
    lat, lon = ownship:getLocation()
    t:setLatitude(lat)
    t:setLongitude(lon)
    
    north = 3000 * math.cos(math.rad(track))
    east = 3000 * math.sin(math.rad(track))
    t:move(north, east);  -- 3 km away up track

    print("Ownship: ", ownship:getLocation())
    print("T: ", t:latitude(), t:longitude())

    for i = 1,90,1 do
        simulation.tick();
        dlat = t:latitude() - ownship:latitude()
        dlon = t:longitude() - ownship:longitude()
        print(i, t:latitude(), t:longitude(), ownship:distanceTo(t), dlat, dlon )
    end
end


print("Ownship properties")
print(ownship:callsign())
print(ownship:latitude())
print(ownship:longitude())


ownship:setSpeedKts(75)
ownship:setAltFeet( 2000 )

for track = 0,360, 45 do 
    head_on(track)
end

