#ifndef LUACONFIG_H__
#define LUACONFIG_H__

struct lua_State;

class CLuaTable;
class CLuaConfig
{
public:
	CLuaConfig();
	~CLuaConfig();

	bool Init(lua_State* L = 0);
	void UnInit();

	bool LoadFile(const char* filename);
	void UnLoad();




private:
	lua_State*    m_luaState;
	CLuaTable*    m_globalTable;
};
#endif // LUACONFIG_H__
