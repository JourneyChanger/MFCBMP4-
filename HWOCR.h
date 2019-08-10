#pragma once

#include "TextRecognizeBase.h"
#ifdef HW_TEST_VERSION
#include "hwocrsdk-test.h"
#else
#include "HWOCRSDK2.h"  // 2012.01.12
#include "HWOCRSDK3.h"  // 2012.01.12
#endif

namespace NCUT_OCR
{
	
class CHWOCR : public CTextRecognizeBase
{
public:
	CHWOCR(void);
	virtual ~CHWOCR(void);

	virtual BOOL Do(CDib& );
	virtual BOOL Do(const CString& );
	virtual BOOL Initialize(HWND );
	virtual void Uninitialize();  // 2012.01.12
	virtual double GetRecoRate(); // 2010.06.17 获得识别率
	virtual BOOL Layout(const CString& strFileName, int nMaxRgnNum, int &nImgRgnNum, LAYOUT *pLayout);
	virtual BOOL Layout(const CDib& dib, int nMaxRgnNum, int &nImgRgnNum, LAYOUT *pLayout);

	BOOL Recognize(IMGRGN ImgRgn, LPSTR pszText, int& nTextLen, int& nTotal, int& nDubiousCount);  // 2012.11.04

protected:
	double m_dblRate;  // 2010.06.22

};

}