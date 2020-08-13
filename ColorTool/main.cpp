#include "ColorTool.h"
#include <QtWidgets/QApplication>
#include <stdio.h>
extern "C"
{
#include "../lua/lua.h"
#include "../lua/lauxlib.h"
#include "../lua/lualib.h"
}
#include "../include/LuaBridge/LuaBridge.h"

class GameData
{
public:
	void Work()
	{
		printf("test work\n");
	}
	int x;
	static int g_x;
};
int GameData::g_x;

int add(lua_State* L)
{
	int x = luaL_checkinteger(L, 1);
	int y = luaL_checkinteger(L, 2);
	printf("%d",x+y);
	return 1;
}
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	/*ColorTool w;
	w.show();*/
	//初始化Lua （最后记得调用lua_close(lua_state)释放）
	lua_State* lua_state = luaL_newstate();
	//加载Lua基本库
	luaL_openlibs(lua_state);
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("test")
		.beginClass<GameData>("GameData")
		.addConstructor<void(*) (void)>()//无参构造函数的注册
		.addData("x", &GameData::x)//注册变量到lua
		.addStaticData("g_x", &GameData::g_x)//注册静态变量到lua
		.addFunction("Work", &GameData::Work)//注册test、方法到lua（addStaticFunction静态函数注册也类似）
		.addFunction("工作", &GameData::Work)//注册test、方法到lua（addStaticFunction静态函数注册也类似）
		.endClass()		
		.endNamespace();
	GameData test;
	luabridge::setGlobal(lua_state, &test, "GameData");//注册test_lua对象到lua
	/*lua_register(lua_state,"加法", add);
	lua_register(lua_state, "add", add);*/
	//运行lua脚本
	luaL_dofile(lua_state, "a.lua");
	//关闭Lua
	lua_close(lua_state);
	return a.exec();
}
