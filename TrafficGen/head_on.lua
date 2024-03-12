print('Closing head on')


print("Ownship properties")
print(ownship:callsign())
print(ownship:latitude())
print(ownship:longitude())


ownship:setSpeedKts(75)
ownship:setAltFeet( 2000 )
ownship:setTrack(0)

t = simulation.createTarget()
print("Target ", t:callsign())
t:setSpeedKts(75);
t:setAltFeet(2000);
t:setTrack(180)

lat, lon = ownship:getLocation()
t:setLatitude(lat)
t:setLongitude(lon)
t:move(3000,0);  -- move north 3km

print("Ownship: ", ownship:getLocation())
print("T: ", t:latitude(), t:longitude())

for i = 1,180,1 do
    simulation.tick();
    print(i, t:latitude(), t:longitude(), ownship:distanceTo(t))
end