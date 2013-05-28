// lua_register_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "lua_reg.hpp"
#include "LuaModule.h"

void test1(int n)
{
}

int test2(int n, double d, const std::string &msg)
{
	return 10;
}

std::string test3(const std::string &msg)
{
	return msg;
}

bool test4(bool suc)
{
	return !suc;
}

std::string test5(const std::string &msg, const std::string &msg2)
{
	return msg + msg2;
}

struct test_t
{
	char test6()
	{
		return 'a';
	}
};


int _tmain(int argc, _TCHAR* argv[])
{
	auto module = script::LuaModule::create();
	lua_State *state = module->getLuaState();
	
	using namespace std::placeholders;

	lua::register_instance().reg(state, "test1", std::bind(&test1, _1), &test1);
	lua::register_instance().reg(state, "test2", std::bind(&test2, _1, _2, _3), &test2);
	lua::register_instance().reg(state, "test3", std::bind(&test3, _1), &test3);
	lua::register_instance().reg(state, "test4", std::bind(&test4, _1), &test4);
	lua::register_instance().reg(state, "test5", std::bind(&test5, _1, _2), &test5);

	test_t t;
	lua::register_instance().reg(state, "test6", std::bind(&test_t::test6, std::ref(t)), &test_t::test6);
	lua::register_instance().reg(state, "test7", [](const std::string &msg1, const std::string &msg2)->std::string{ return "1"; }, &test5);

	module->executeScriptFile("test.lua");

	
	system("pause");
	script::LuaModule::destroy(module);
	return 0;
}

