lua_reg
=======

C++ register function to lua

Copyright (c) 2013, Chen Yu. All rights reserved.

Use of this source code is governed by a BSD-style license that can be found in the License file.

一个lua与C++交互粘合层，支持C++函数导出到lua、C++调用lua函数、C++引用lua对象，等等。

设计目标：接口简单、不依赖第三方库

	std::allocator<char> std_allocator;
	luareg::state_t state(std_allocator);

	try
	{
		test_t t;

		// free function & class method
		luareg::module(state, "cpp")
			<< lua::def("test0", &test0)
			<< lua::def("test1", &test1)
			<< lua::def("test2", &test2)
			<< lua::def("test3", &test3)
			<< lua::def("test4", &test4)
			<< lua::def("test5", &test5)
			<< lua::def("test6", &t, &test_t::test6);

		luareg::module(state, "cpp")
			<< lua::def("test7", &test7)
			<< lua::def("test8", &test8)
			<< lua::def("test9", &test9)
			<< lua::def("test10", &test10)
			<< lua::def("test11", &t, &test_t::test11)
			<< lua::def("test12", &test12)
			<< lua::def("test13", &test13)
			<< lua::def("test14", &test14)
			<< lua::def("test15", &test15)
			<< lua::def("test16", &test16)
			<< lua::def("test17", &test17)
			<< lua::def("test18", &test18)
			//<< lua::def("test19", [](int n, const std::string &msg)->int{ return n; })
			<< lua::def("test20", &test20);

		base_t base;
		luareg::module(state)
			<< lua::def("test21", &test21)
			<< lua::def("base_print", &base, &base_t::print);

		// object method
	
		luareg::module(state, "cpp")
			[
				luareg::class_t<foo_t>(state, "foo_t")
				<< luareg::constructor<int>()
				<< luareg::destructor()
				<< luareg::def("add", &foo_t::add)
				<< luareg::def("get", &foo_t::get)
				<< luareg::def("get_pointer", &foo_t::get_pointer)
				<< luareg::def("get_base", &foo_t::get_base)
			]
			<< lua::def("test21", &test21);

		lua::execute(state, "test.lua");
	}
	catch (const luareg::parameter_error_t &e)
	{
		std::cout << e.what() << std::endl;
		e.dump(std::cout);
	}
	catch(const luareg::fatal_error_t &e)
	{
		std::cout << e.what() << std::endl;
		e.dump(std::cout);
	}

	try
	{
		lua::execute(state, "test2.lua");
		std::pair<int, std::string> n = lua::call(state, "test_call", 1, "haha", 10.2, false);

		auto val = std::make_pair("test abc", 10.2);
		lua::call(state, "test_call2", 1, "haha", val);
	}
	catch(const luareg::fatal_error_t &e)
	{
		std::cout << e.what() << std::endl;
		e.dump(std::cout);
	}
