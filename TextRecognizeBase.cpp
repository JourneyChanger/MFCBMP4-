//////////////////////////////////////////////////////////////////
// TextRecognizeBase.cpp: implementation of the CTextRecognizeBase class.
// CREATED ON : 2009-1-6 23:37:33
// AUTHOR	  : Ôø·²·æ
// COMMENTS   :  
//////////////////////////////////////////////////////////////////

#include "pch.h"
#include "TextRecognizeBase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
using namespace NCUT_OCR;

CTextRecognizeBase::CTextRecognizeBase()
{
	m_strError = "";
	m_strText = "";
	m_bCorrect = FALSE;

	m_nTotal = 0;
	m_nDubiousCount = 0;
}

CTextRecognizeBase::~CTextRecognizeBase()
{

}
