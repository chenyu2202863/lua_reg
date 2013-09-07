
print(test13(false, 1, "test"))

func = function() print("111") end

print(test15("test123", func, 10))

-- handler = function(n, m) 
-- 	print("handler")
-- 	return true, "", 11, "response msg data"
-- end

-- test16(handler)

-- local print_func = function(t) print(t) end
-- test17(print_func, "haha")

-- local func_func = function(func) func() end

-- test18(func_func, func)

-- print(test19(2323))

-- print(test20())



ff = foo_t:new(2);
--ff1 = foo_t:new(10);
--ff:add(1, 4)

sub_ff = ff:get_pointer()


base = sub_ff:get_base()
base:print()
