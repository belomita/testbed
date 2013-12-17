#ifndef LUATABLE_H__
#define LUATABLE_H__

#include <string>
#include <vector>
#include <map>

struct lua_State;

class CLuaTable;
class CLuaObject
{
public:
	CLuaObject() : objType(ELOT_UNKNOWN), unknowValue(NULL) {}
	~CLuaObject()
	{
		objType = ELOT_UNKNOWN;
		unknowValue = NULL;
	}
	enum ELuaObjectType
	{
		ELOT_NUMBER,
		ELOT_STRING,
		ELOT_BOOLEAN,
		ELOT_TABLE,
		ELOT_UNKNOWN
	};
	ELuaObjectType  objType;
	union
	{
		double*       numberValue;
		std::string*  stringValue;
		bool*         boolValue;
		CLuaTable*    tableValue;
		CLuaObject*   unknowValue;
	};
};

class CLuaTable
{
public:
	CLuaTable(lua_State* state, const char* tableName);
	~CLuaTable();

	bool LoadTable(int tableIndex, bool loadSubTable = true);
	void UnloadTable();

	void DumpTable(int depth, bool withTypeInfo = false);

	bool GetInt   (const char* keyName, int&          value);
	bool GetUInt  (const char* keyName, unsigned int& value);
	bool GetFloat (const char* keyName, float&        value);
	bool GetBool  (const char* keyName, bool&         value);
	bool GetString(const char* keyName, std::string&  value);
	bool GetTable (const char* keyName, CLuaTable&    value);

	bool GetInt   (int tableIndex, int&          value);
	bool GetUInt  (int tableIndex, unsigned int& value);
	bool GetFloat (int tableIndex, float&        value);
	bool GetBool  (int tableIndex, bool&         value);
	bool GetString(int tableIndex, std::string&  value);
	bool GetTable (int tableIndex, CLuaTable&    value);

private:
	CLuaObject* GetObject(const char* keyName);
	CLuaObject* GetObject(int tableIndex);
private:
	lua_State       *m_luaState;
	std::string      m_tableName;

private:
	typedef std::vector<CLuaObject>            ObjectList;
	typedef std::map<std::string, CLuaObject>  ObjectDict;
	ObjectList      *m_objList;
	ObjectDict      *m_objDict;
};

#endif // LUATABLE_H__
