#pragma once

namespace NCUT_OCR
{

class CFilterWord
{
public:
	CFilterWord(void);
	virtual ~CFilterWord(void);

	void FileterWord(CString& strText, const char* pTxtBuf);  
protected:
	char* m_pszFilterWord;   
	int m_nFilterWordNum;    

	void GetFilterWord();  
	BOOL IsSpecialWord(UCHAR* ); 

};

}