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
	//��ʼ��Lua �����ǵõ���lua_close(lua_state)�ͷţ�
	lua_State* lua_state = luaL_newstate();
	//����Lua������
	luaL_openlibs(lua_state);
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("test")
		.beginClass<GameData>("GameData")
		.addConstructor<void(*) (void)>()//�޲ι��캯����ע��
		.addData("x", &GameData::x)//ע�������lua
		.addStaticData("g_x", &GameData::g_x)//ע�ᾲ̬������lua
		.addFunction("Work", &GameData::Work)//ע��test��������lua��addStaticFunction��̬����ע��Ҳ���ƣ�
		.addFunction("����", &GameData::Work)//ע��test��������lua��addStaticFunction��̬����ע��Ҳ���ƣ�
		.endClass()		
		.endNamespace();
	GameData test;
	luabridge::setGlobal(lua_state, &test, "GameData");//ע��test_lua����lua
	/*lua_register(lua_state,"�ӷ�", add);
	lua_register(lua_state, "add", add);*/
	//����lua�ű�
	luaL_dofile(lua_state, "a.lua");
	//�ر�Lua
	lua_close(lua_state);
	return a.exec();
}
