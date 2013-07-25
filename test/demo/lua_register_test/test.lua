test1(1)

v = test2(1,2,"test")
print(v)

v = test3("test")
print(v)

v = test4(true)
print(v)

v = test5("test", "lua")
print(v)

v = test6(true)
print(v)

test7("1234567890")

v = test8({1,2,3,4,5,6,7,8,9});
for index = 1, table.getn(v) do
	print(index, v[index])
end

v = test9(10, {{1, "qwe"}, {2, "asd"}, {3, "red"}})

local vv = {{1, "qwe"}, {2, "asd"}, {3, "red"}}
for index = 1, #v do
	local t = v[index]

	print(vv[index])

	for j = 1, #t do
		print(t[j])
	end
end

v = test10({"123", 456})
for index = 1, table.getn(v) do
	print(index, v[index])
end

v = test12({{"chen", {123,456}}, {"yu", {456,789}}})
for index = 1, table.getn(v) do
	print(index, v[index])
end

print("\n")
v = test13({2, "chenyu", false, {456, "asd"}})
for index = 1, table.getn(v) do
	print(index, v[index])
end

v = test14({{1, "1"}, {2, "2"}, {3, "3"}})
for index = 1, table.getn(v) do
	print(index, v[index])
end


func = function() print("111") end

test15(func)

test16(function(n, m) print(n, m) end)

local print_func = function(t) print(t) end
test17(print_func, "haha")

local func_func = function(func) func() end

test18(func_func, func)

print(test19(2323))