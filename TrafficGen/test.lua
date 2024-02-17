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


ownship.setSpeed(100)
ownship.setHeight(2000)
ownship.setTrack(90)

t = simulation.createTarget()

t.wombles("orinoco");

lat, lon = ownship.getLocation()
t.latitude = lat
t.longitude =lon

print("Ownship: ", ownship.getLocation())
print("T: ", t.latitude, t.longitude)


-- for i = 1,3,1 do
--     simulation.tick();
--     print(i, t1.latitude, t1.longitude)
-- end