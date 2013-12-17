#include "tabfileutf16.h"
#include <stdio.h>
#include <assert.h>

static inline wchar_t _ReadWideChar(const char* buffer, int index)
{
	wchar_t tempchar;
	char* pTempChar = (char*)&tempchar;
	pTempChar[0] = buffer[index];
	pTempChar[1] = buffer[index + 1];

	return tempchar;
}
CUnicodeTabFile::CUnicodeTabFile()
{
	m_nSize = 0;
	m_nRowCount = 0;
	m_nColCount = 0;
}

CUnicodeTabFile::~CUnicodeTabFile()
{
	UnInit();
}

bool CUnicodeTabFile::Init( const char* pcszFileName )
{
	FILE* fp = fopen(pcszFileName, "rb");
	if (fp == NULL)
		return false;
	
	int nlen = 0;
	fseek(fp, 0, SEEK_END);
	nlen = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::vector<char> vecTextContent((size_t)nlen, 0);
	char* pCharBuffer = &vecTextContent[0];
	int nRealLen = (int)fread(pCharBuffer, 1, nlen, fp);
	assert(nRealLen == nlen);
	
	fclose(fp);

	m_strFileName = pcszFileName;
	if (vecTextContent.empty())
		return true;

	if (pCharBuffer[0] != (char)0xFF || pCharBuffer[1] != (char)0xFE)
	{
		printf("File %s is not UTF-16 little endian file!", pcszFileName);
		return false;
	}

	m_nColCount = 1;
	m_nRowCount = 0;

	m_oTextBuffer.resize((size_t)nRealLen / 2, 0);
	m_oOffsetTable.reserve(100);

	wchar_t* pUtf16Buffer = &m_oTextBuffer[0];
	int nUtf16Index = 0;
	int nCharIndex  = 2;
	int nStringIndex = 0;
	int nColumnIndex = 0;
	bool bInQuote = false;

	while (nCharIndex < nRealLen)
	{
		wchar_t& rWChar = pUtf16Buffer[nUtf16Index];
		wchar_t temp_wchar = _ReadWideChar(pCharBuffer, nCharIndex);

		switch (temp_wchar)
		{
		case L'\t':
			if (bInQuote)
			{
				rWChar = temp_wchar;
			}
			else
			{
				if (m_nRowCount == 0)    m_nColCount++;
				// Add new string field
				rWChar = 0;
				m_oOffsetTable.push_back(nStringIndex);
				nColumnIndex++;
				nStringIndex = nUtf16Index + 1;
			}
			break;
		case L'\r':
			if (bInQuote)
			{
				rWChar = temp_wchar;
			}
			else
			{
				wchar_t tempWCharNext = _ReadWideChar(pCharBuffer, nCharIndex + 2);
				// new line
				if (tempWCharNext == L'\n')
				{
					// add last new field of current line
					rWChar = 0;
					m_oOffsetTable.push_back(nStringIndex);
					nColumnIndex++;
					// fill remaining fields
					if (nColumnIndex < m_nColCount - 1)
					{
						nStringIndex = nUtf16Index;
						for (; nColumnIndex < m_nColCount; nColumnIndex++)
							m_oOffsetTable.push_back(nStringIndex);
					}
					nColumnIndex = 0;
					m_nRowCount++;
					// jump through '\n'
					nStringIndex = nUtf16Index + 2;
				}
				else
				{
					assert(false && "new line must end with '\r\n', missing '\n'");
				}
			}
			break;
		case L'\"':
			if (!bInQuote)
			{
				bInQuote = true;
				nStringIndex = nUtf16Index + 1;
			}
			else
			{
				wchar_t temWCharNext = _ReadWideChar(pCharBuffer, nCharIndex + 2);
				if (temWCharNext != L'\"')
				{
					bInQuote = false;
				}
				else
				{
					rWChar = temp_wchar;
					nCharIndex += 2;
				}
			}
			break;
		default:
			rWChar = temp_wchar;
			break;
		}
		nUtf16Index++;
		nCharIndex += 2;
	}
	// create column name to index map
	for (nColumnIndex = 0; nColumnIndex < m_nColCount;nColumnIndex++)
	{
		const wchar_t* pwszColName = GetData(nColumnIndex, 1);
		if (pwszColName == NULL || pwszColName[0] == 0)
		{
			continue;
		}
		//TODO:check duplicated columns
		m_mapName2Col[pwszColName] = nColumnIndex;
	}
	// remove empty rows
	for (int nRowIndex = m_nRowCount - 1; nRowIndex >= 0; nRowIndex--)
	{
		bool bEmptyRow = false;
		for (nColumnIndex = 0; nColumnIndex < m_nColCount; nColumnIndex++)
		{
			const wchar_t* pwszField = GetData(nColumnIndex, nRowIndex);
			if (pwszField && pwszField[0] != 0)
			{
				bEmptyRow = false;
				break;
			}
		}
		if (bEmptyRow)
			m_nRowCount--;
		else
			break;
	}
	return true;
}

bool CUnicodeTabFile::UnInit( void )
{
	return true;
}

int CUnicodeTabFile::GetRowCount( void )
{
	return m_nRowCount;
}

int CUnicodeTabFile::GetColCount( void )
{
	return m_nColCount;
}

const wchar_t* CUnicodeTabFile::GetData( int nCol, int nRow )
{
	if (nCol < 0 || nCol >= m_nColCount)
		return NULL;

	if (nRow < 0 || nRow >= m_nRowCount)
		return NULL;

	wchar_t* pUtf16Buffer = &m_oTextBuffer[0];
	return pUtf16Buffer + m_oOffsetTable[nRow * m_nColCount + nCol];
}

const wchar_t* CUnicodeTabFile::GetData( const wchar_t* pcszColName, int nRow )
{
	MAP_NAME_2_COL::iterator it;

	it = m_mapName2Col.find(pcszColName);
	if (it != m_mapName2Col.end())
	{
		return GetData(it->second, nRow);
	}
	return NULL;
}