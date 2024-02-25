


print("Ownship properties")
print(ownship:callsign())
print(ownship:latitude())
print(ownship:longitude())


ownship:setSpeedKts(65)
ownship:setAltFeet( 2000 )
ownship:setTrack(90)

-- A horde of targets from all directions
for degrees = 0,360,5 do
    t = simulation.createTarget()
    print("Target ", t:callsign())
    t:setSpeedKts(80);
    t:setAltFeet(2000);
    t:makeConflictingTo(ownship, 2, degrees)
end

print("Ownship: ", ownship:getLocation())

for i = 1,180,1 do
    simulation.tick();
    print(i, t:latitude(), t:longitude())
end