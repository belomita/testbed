#include "LuaConfig.h"
#include "LuaTable.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

CLuaConfig::CLuaConfig()
	: m_luaState(NULL)
	, m_globalTable(NULL)
{

}

CLuaConfig::~CLuaConfig()
{
	UnInit();
}

bool CLuaConfig::Init( lua_State* L /*= NULL*/ )
{
	if (L == NULL)
	{
		L = lua_open();
		luaL_openlibs(L);
	}

	m_luaState = L;
	return true;
}

void CLuaConfig::UnInit()
{
	if (m_luaState != NULL)
	{
		lua_close(m_luaState);
	}
	if (m_globalTable != NULL)
	{
		delete m_globalTable;
		m_globalTable = NULL;
	}
}

bool CLuaConfig::LoadFile( const char* filename )
{
	lua_State* L = m_luaState;

	int ret = luaL_dofile(L, filename);
	if (ret != 0)
	{
		const char* errmsg = lua_tostring(L, -1);
		if (errmsg)
		{
			printf("Error occurred: %s", errmsg);
		}
		return false;
	}

	const char* globalTableName = "ConfigTable";
	m_globalTable = new CLuaTable(m_luaState, globalTableName);
	
	lua_getglobal(L, globalTableName);
	bool loadRet = m_globalTable->LoadTable(-1, true);
	if (!loadRet)
	{
		delete m_globalTable;
		m_globalTable = NULL;

		printf("Load ConfigTable Error.\n");
		return false;
	}
	m_globalTable->DumpTable(0, true);
	m_globalTable->DumpTable(0, false);

	float width = 0.0f;
	m_globalTable->GetFloat("width", width);
	std::string name;
	m_globalTable->GetString("name", name);
	return true;
}

void CLuaConfig::UnLoad()
{
	if (m_globalTable)
	{
		delete m_globalTable;
		m_globalTable = NULL;
	}
}

