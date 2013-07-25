function test_call(param1, param2, param3, param4)
	print(param1)
	print(param2)
	print(param3)
	print(param4)

	ret_num = param1 + 10
	
	return {param1, param2}
end


function NVL_ASSERT(p, errMsg)
	if not p then
		--DEBUG(errMsg)
		error("NVL_ASSERT: " .. errMsg)
	end
end


function test_call2(param1, param2, param3)
	
	NVL_ASSERT(false, "asd")
	print(param1)
	print(param2)
	print(param3)
end