print('Hello World')

ownship.setSpeed(100)
ownship.setHeight(2000);
ownship.setTrack(90);
t1 = simulation.createTarget()

-- Have a look at the metatable to check it's what we expect
do
    local meta = getmetatable(t1)

    print("Metatable: ", meta)
    for k,v in pairs(meta) do
        print("Item ", k,v)
    end
end 


lat, lon = ownship.getLocation()
t1.latitude = lat
t1.longitude =lon

print("Ownship: ", ownship.getLocation())
print("T1: ", t1.latitude, t1.longitude)
for i = 1,3,1 do
    simulation.tick();
    print(i, t1.latitude, t1.longitude)
end