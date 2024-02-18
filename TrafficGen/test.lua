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


print("====== T2 ======")
t2 = simulation.createTarget()
print("====== T2 END ======")

t2:setLatitude(42)
print(t2:latitude())




-- if(ownship == nil) then print ("Ownship is nil") else print("Ownship is not nil") end
-- print("Ownship table")
-- showtable(ownship)

-- print("Ownship metatable")
-- showmeta(ownship)

-- print("Ownship properties")
-- print(ownship.callsign)
-- print(ownship.latitude)
-- print(ownship.longitude)


-- ownship.speed = 100
-- ownship.height = 2000
-- ownship.track = 90

-- t = simulation.createTarget()



-- lat, lon = ownship.getLocation()
-- t.latitude = lat
-- t.longitude =lon
-- t.latitude = 42
-- t.longitude = 36

-- print("Ownship: ", ownship.getLocation())
-- print("T: ", t.latitude, t.longitude)


-- for i = 1,3,1 do
--     simulation.tick();
--     print(i, t1.latitude, t1.longitude)
-- end