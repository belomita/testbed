#include "LuaTable.h"
#include <cassert>
#include <cstdio>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

void DestoryLuaObject(CLuaObject& object)
{
	switch(object.objType)
	{
	case CLuaObject::ELOT_BOOLEAN:
		delete object.boolValue;
		object.boolValue = NULL;
		break;

	case CLuaObject::ELOT_STRING:
		delete object.stringValue;
		object.stringValue = NULL;
		break;

	case CLuaObject::ELOT_NUMBER:
		delete object.numberValue;
		object.numberValue = NULL;
		break;
	case CLuaObject::ELOT_TABLE:
		delete object.tableValue;
		object.tableValue = NULL;
		break;
	}
	object.objType = CLuaObject::ELOT_UNKNOWN;
}

void DumpLuaObject(CLuaObject& object, int depth, bool bWithType)
{
	switch(object.objType)
	{
	case CLuaObject::ELOT_BOOLEAN:
		printf(*(object.boolValue) ? "true" : "false");
		printf(bWithType ? " : boolean": "");
		break;

	case CLuaObject::ELOT_STRING:
		printf(object.stringValue->c_str());
		printf(bWithType ? " : string": "");
		break;

	case CLuaObject::ELOT_NUMBER:
		printf("%.2f", *(object.numberValue));
		printf(bWithType ? " : number": "");
		break;
	case CLuaObject::ELOT_TABLE:		
		object.tableValue->DumpTable(depth + 1, bWithType);
		break;
	}
}

void PrintIndent(int depth)
{
	for (int i = 0; i < depth; i++)
		printf("\t");
}
CLuaTable::CLuaTable( lua_State* state, const char* tableName )
	: m_luaState(state)
	, m_tableName(tableName)
	, m_objList(NULL)
	, m_objDict(NULL)
{

}

CLuaTable::~CLuaTable()
{
	if (m_objDict != NULL)
	{
		ObjectDict::iterator it = m_objDict->begin();
		for (; it != m_objDict->end(); ++it)
		{
			DestoryLuaObject(it->second);
		}
		delete m_objDict;
		m_objDict = NULL;
	}

	if (m_objList != NULL)
	{
		ObjectList::iterator it = m_objList->begin();
		for (; it != m_objList->end(); ++it)
		{
			DestoryLuaObject(*it);
		}
		delete m_objList;
		m_objList = NULL;
	}
	m_luaState = NULL;
}

bool CLuaTable::LoadTable(int tableIndex, bool loadSubTable /* = true */)
{
	assert(m_luaState);

	assert(m_objList == NULL);
	assert(m_objDict == NULL);

	lua_State* L = m_luaState;
	assert(lua_istable(L, tableIndex));
	
	m_objList = new ObjectList;
	m_objDict = new ObjectDict;

	tableIndex--;

	lua_pushnil(L);
	while (lua_next(L, tableIndex) != 0) 
	{
		/* uses 'key' (at index -2) and 'value' (at index -1) */
		int keyType   = lua_type(L, -2);
		int valueType = lua_type(L, -1);

		CLuaObject* newObjPtr = NULL;
		switch(keyType)
		{
		case LUA_TSTRING:
			newObjPtr = &(*m_objDict)[std::string(lua_tostring(L, -2))];
			break;
		case LUA_TNUMBER:
			m_objList->push_back(CLuaObject());
			newObjPtr = &(m_objList->back());
			break;
		default:
			printf("key type '%s' of table not support.\n", lua_typename(L, keyType));
			break;
		}

		if (newObjPtr != NULL)
		{
			CLuaObject& newObj = *newObjPtr;
			switch (valueType)
			{
			case LUA_TSTRING:
				{
					newObj.objType = CLuaObject::ELOT_STRING;
					newObj.stringValue = new std::string(lua_tostring(L, -1));
					break;
				}
			case LUA_TNUMBER:
				{
					newObj.objType = CLuaObject::ELOT_NUMBER;
					newObj.numberValue = new double(lua_tonumber(L, -1));
					break;
				}
			case LUA_TBOOLEAN:
				{
					newObj.objType   = CLuaObject::ELOT_BOOLEAN;
					newObj.boolValue = new bool(lua_toboolean(L, -1) ? true : false);
					break;
				}
			case LUA_TTABLE:
				{
					newObj.objType = CLuaObject::ELOT_TABLE;
					std::string subTableName;
					if (keyType == LUA_TSTRING)
					{
						subTableName = lua_tostring(L, -2);
					}
					else
					{
						char name[20];
						sprintf_s(name, 20, "%d", m_objList->size() - 1);
						subTableName = name;
					}
					newObj.tableValue = new CLuaTable(L, subTableName.c_str());
					if (loadSubTable)
					{
						newObj.tableValue->LoadTable(-1, false);
					}
					break;
				}
			default:
				printf("value type '%s' of table not support.\n", lua_typename(L, valueType));
				break;
			}
		}
		
		/* removes 'value'; keeps 'key' for next iteration */
		lua_pop(L, 1);
	}

	return true;
}

void CLuaTable::UnloadTable()
{

}

bool CLuaTable::GetInt( const char* keyName, int& value )
{
	CLuaObject* objPtr = GetObject(keyName);
	if (objPtr && objPtr->objType == CLuaObject::ELOT_NUMBER)
	{
		value = (int)*(objPtr->numberValue);
		return true;
	}
	return false;
}

bool CLuaTable::GetInt( int tableIndex, int& value )
{
	CLuaObject* objPtr = GetObject(tableIndex);
	if (objPtr && objPtr->objType == CLuaObject::ELOT_NUMBER)
	{
		value = (int)*(objPtr->numberValue);
		return true;
	}
	return false;
}

bool CLuaTable::GetUInt( const char* keyName, unsigned int& value )
{
	CLuaObject* objPtr = GetObject(keyName);
	if (objPtr && objPtr->objType == CLuaObject::ELOT_NUMBER)
	{
		value = (unsigned int)*(objPtr->numberValue);
		return true;
	}
	return false;
}

bool CLuaTable::GetUInt( int tableIndex, unsigned int& value )
{
	CLuaObject* objPtr = GetObject(tableIndex);
	if (objPtr && objPtr->objType == CLuaObject::ELOT_NUMBER)
	{
		value = (unsigned int)*(objPtr->numberValue);
		return true;
	}
	return false;
}

bool CLuaTable::GetFloat( const char* keyName, float& value )
{
	CLuaObject* objPtr = GetObject(keyName);
	if (objPtr && objPtr->objType == CLuaObject::ELOT_NUMBER)
	{
		value = (float)*(objPtr->numberValue);
		return true;
	}
	return false;
	}

bool CLuaTable::GetFloat( int tableIndex, float& value )
{
	CLuaObject* objPtr = GetObject(tableIndex);
	if (objPtr && objPtr->objType == CLuaObject::ELOT_NUMBER)
	{
		value = (float)*(objPtr->numberValue);
		return true;
	}
	return false;
}

bool CLuaTable::GetBool( const char* keyName, bool& value )
{
	CLuaObject* objPtr = GetObject(keyName);
	if (objPtr && objPtr->objType == CLuaObject::ELOT_BOOLEAN)
	{
		value = (bool)*(objPtr->boolValue);
		return true;
	}
	return false;
}

bool CLuaTable::GetBool( int tableIndex, bool& value )
{
	CLuaObject* objPtr = GetObject(tableIndex);
	if (objPtr && objPtr->objType == CLuaObject::ELOT_BOOLEAN)
	{
		value = (bool)*(objPtr->boolValue);
		return true;
	}
	return false;
}

bool CLuaTable::GetString( const char* keyName, std::string& value )
{
	CLuaObject* objPtr = GetObject(keyName);
	if (objPtr && objPtr->objType == CLuaObject::ELOT_STRING)
	{
		value = (std::string)*(objPtr->stringValue);
		return true;
	}
	return false;
}

bool CLuaTable::GetString( int tableIndex, std::string& value )
{
	CLuaObject* objPtr = GetObject(tableIndex);
	if (objPtr && objPtr->objType == CLuaObject::ELOT_STRING)
	{
		value = (std::string)*(objPtr->stringValue);
		return true;
	}
	return false;
}

bool CLuaTable::GetTable( const char* keyName, CLuaTable& value )
{
	CLuaObject* objPtr = GetObject(keyName);
	if (objPtr && objPtr->objType == CLuaObject::ELOT_TABLE)
	{
		value = (CLuaTable)*(objPtr->tableValue);
		return true;
	}
	return false;
}

bool CLuaTable::GetTable( int tableIndex, CLuaTable& value )
{
	CLuaObject* objPtr = GetObject(tableIndex);
	if (objPtr && objPtr->objType == CLuaObject::ELOT_TABLE)
	{
		value = (CLuaTable)*(objPtr->tableValue);
		return true;
	}
	return false;
}

CLuaObject* CLuaTable::GetObject( const char* keyName )
{
	if (m_objDict == NULL)
		return NULL;

	ObjectDict::iterator it = m_objDict->find(keyName);
	if (it != m_objDict->end())
	{
		return &it->second;
	}
	return NULL;
}

CLuaObject* CLuaTable::GetObject( int tableIndex )
{
	if (m_objList == NULL)
		return NULL;

	if (tableIndex < 0 || tableIndex >= (int) m_objList->size())
		return NULL;
	
	return &((*m_objList)[tableIndex]);
}

void CLuaTable::DumpTable(int depth, bool withTypeInfo /*= false*/ )
{
	PrintIndent(depth);
	printf("{ \n");
	
	if (m_objList)
	{
		for (unsigned int i = 0; i < m_objList->size(); i++)
		{
			CLuaObject& obj = (*m_objList)[i];
			if (i > 0)
				printf(",\n");
			if(obj.objType != CLuaObject::ELOT_TABLE)
				PrintIndent(depth + 1);
			DumpLuaObject(obj, depth, withTypeInfo);
		}
	}

	if (m_objDict)
	{
		if (m_objDict->size() > 0)
			printf(",\n");

		ObjectDict::iterator it = m_objDict->begin();
		for (int i = 0; it != m_objDict->end(); ++it, ++i)
		{
			if (i > 0)
				printf(",\n");
			PrintIndent(depth + 1);
			printf("%s = ", it->first.c_str());
			if (it->second.objType == CLuaObject::ELOT_TABLE)
				printf("\n");
			DumpLuaObject(it->second, depth, withTypeInfo);
		}
	}
	printf("\n");
	PrintIndent(depth);
	printf("}");
	if (depth == 0)
		printf("\n");
}

