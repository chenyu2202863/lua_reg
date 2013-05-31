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
	char test6(bool suc)
	{
		return 'a';
	}
};


void test7(const std::pair<const char *, std::uint32_t> &buffer)
{

}

std::vector<int> test8(const std::vector<int> &v)
{
	return v;
}

std::vector<std::pair<int, std::string>> test9(int n, const std::vector<std::pair<int, std::string>> &v)
{
	return v;
}

std::pair<std::string, std::uint32_t> test10(const std::pair<std::string, std::uint32_t> &val)
{
	return val;
}

int _tmain(int argc, _TCHAR* argv[])
{
	auto module = script::LuaModule::create();
	lua_State *state = module->getLuaState();
	
	using namespace std::placeholders;

	lua::reg(state, "test1", &test1);
	lua::reg(state, "test2", &test2);
	lua::reg(state, "test3", &test3);
	lua::reg(state, "test4", &test4);
	lua::reg(state, "test5", &test5);

	test_t t;
	lua::reg(state, "test6", std::ref(t), &test_t::test6);
	lua::reg(state, "test7", &test7);
	lua::reg(state, "test8", &test8);
	lua::reg(state, "test9", &test9);
	lua::reg(state, "test10", &test10);


	module->executeScriptFile("test.lua");

	
	system("pause");
	script::LuaModule::destroy(module);
	return 0;
}

