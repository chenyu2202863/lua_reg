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