//////////////////////////////////////////////////////////////////
// FilterWord.cpp
// CREATED ON : 2012-07-23 18:45:23
// AUTHOR	  : Ôø·²·æ
// COMMENTS   :  
//////////////////////////////////////////////////////////////////

#include "pch.h"
#include "FilterWord.h"

using namespace NCUT_OCR;

CFilterWord::CFilterWord(void)
{
	m_pszFilterWord = NULL;  
	m_nFilterWordNum = 0;

	GetFilterWord();  
}

CFilterWord::~CFilterWord(void)
{
	delete [] m_pszFilterWord;   
}

void CFilterWord::GetFilterWord() 
{
	delete [] m_pszFilterWord;  
	m_pszFilterWord = NULL;   

	TCHAR szWorkDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szWorkDir);

	CString strFile;
	strFile.Format(_T("%s\\FilterWord.txt"), szWorkDir);
	FILE* fp;
	errno_t err = fopen_s(&fp, strFile,_T("rb"));
	if ( fp != NULL ) 
	{
		fseek(fp,0,SEEK_END);
		int len = ftell(fp);
		m_pszFilterWord = new char[len + 1];
		char* pTmpBuf = new char[len + 1];
		m_nFilterWordNum = 0;
		fseek(fp,0,SEEK_SET);
		fread(pTmpBuf, 1, len, fp);
		pTmpBuf[len] = '\0';
		fclose(fp);

		for(int i = 0; i < len; )
		{
			if( (UCHAR)pTmpBuf[i] >= 160 )
			{
				m_pszFilterWord[m_nFilterWordNum*2] = pTmpBuf[i];
				m_pszFilterWord[m_nFilterWordNum*2 + 1] = pTmpBuf[i + 1];
				i += 2;
				m_nFilterWordNum++;
			}
			else
			{
				i++;
			}
		}
		m_pszFilterWord[2*m_nFilterWordNum] = '\0';
		delete [] pTmpBuf;
	}
}

BOOL CFilterWord::IsSpecialWord(UCHAR* ch)  
{
	for(int i = 0; i < m_nFilterWordNum; i++)
	{
		if( ch[0] == (UCHAR)m_pszFilterWord[2*i] && ch[1] == (UCHAR)m_pszFilterWord[2*i + 1] )
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CFilterWord::FileterWord(CString& strText, const char* pTxtBuf)  
{	
	if( m_nFilterWordNum <= 0 )
	{
		strText = pTxtBuf;
		return;
	}
	UINT len = strlen(pTxtBuf);

	unsigned char *pTemBuf = new unsigned char[len + 1];
	memset(pTemBuf, 0, len);
	int nSingleNum = 0;	//  2012.6.18 WS Í³¼Æµ¥×Ö½Ú£¬·ÀÖ¹´Ê×é´íÉ¾

	UINT i, j = 0;
	unsigned char ch[3];
	for(i = 0; i < len; )
	{
		ch[0] = pTxtBuf[i];
		ch[1] = pTxtBuf[i+1];
		if( ch[0] >= 160 ) // ºº×Ö
		{
			if( IsSpecialWord(ch) == TRUE )
			{
				if( UINT(nSingleNum%2) == i%2 )
				{
					i += 2;
					continue;
				}
			}
			if( i < len ) pTemBuf[j++] = pTxtBuf[i++]; // 2012.11.04
			if( i < len ) pTemBuf[j++] = pTxtBuf[i++]; // 2012.11.04
		}
		else
		{
			nSingleNum++;
			if(ch[0] == 32 && ch[1] == 32)	//¿Õ¸ñ
			{
				i += 1;
				continue;
			}
			if( i < len ) pTemBuf[j++] = pTxtBuf[i++]; // 2012.11.04
		}
	}
	pTemBuf[j] = '\0';

	strText = pTemBuf;	//  2012.6.14 WS
	delete [] pTemBuf;

}

