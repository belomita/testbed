#ifndef tabfileutf16_h__
#define tabfileutf16_h__

#include <string>
#include <vector>
#include <map>

class CUnicodeTabFile
{
public:
	CUnicodeTabFile();
	~CUnicodeTabFile();

	bool Init(const char* pcszFileName);
	bool UnInit(void);

	int GetRowCount(void);
	int GetColCount(void);

	const wchar_t* GetData(int nCol, int nRow);
	const wchar_t* GetData(const wchar_t* pwszColName, int nRow);

private:
	typedef std::vector<wchar_t> TextBuffer;
	typedef std::vector<int>     OffsetTable;

	int				m_nSize;
	int				m_nRowCount;
	int				m_nColCount;
	std::string     m_strFileName;
	TextBuffer      m_oTextBuffer;
	OffsetTable     m_oOffsetTable;

	typedef std::map<std::wstring, int, std::less<std::wstring>> MAP_NAME_2_COL;
	MAP_NAME_2_COL	m_mapName2Col;
};

#endif // tabfileutf16_h__
