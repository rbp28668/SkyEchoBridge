print('Hello World')

showmeta = function(t)
 
    local meta = getmetatable(t)
    if meta == nil then
        print ("Null Metatable")
    else 
 
        print("Metatable: ", meta)
        for k,v in pairs(meta) do
            print("Item ", k,v)
        end
    end
end

showtable = function(t)
     for k,v in pairs(t) do
        print(k, " = ", v)
    end
end




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