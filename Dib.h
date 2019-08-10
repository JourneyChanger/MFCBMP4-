// Dib.h: interface for the CDib class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIB_H__1ED3852D_BCF1_4D0C_B09C_90DE4D962CBD__INCLUDED_)
#define AFX_DIB_H__1ED3852D_BCF1_4D0C_B09C_90DE4D962CBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "Global.h"
#include "TextRecognizeBase.h"

typedef  bool(__stdcall CHANGE)(char *,char *); // change bmp to jpg function

/* DIB constants */
#define PALVERSION   0x300

/* Dib Header Marker - used in writing DIBs to files */
#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')

/* DIB Macros*/
#define RECTWIDTH(lpRect)     ((lpRect)->right - (lpRect)->left)
#define RECTHEIGHT(lpRect)    ((lpRect)->bottom - (lpRect)->top)

// BNPWIDTHBYTES performs DWORD-aligning of DIB scanlines.  The "bits"
// parameter is the bit count for the scanline (biWidth * biBitCount),
// and this macro returns the number of DWORD-aligned bytes needed
// to hold those bits.

#define BMPWIDTHBYTES(bits)     (((bits) + 31) / 32 * 4)
#define GetBytePerLine(w,b)		(((w)*(b)+31)/32*4)

// 原来定义在windowsx.h里的宏
#define     GlobalPtrHandle(lp)         \
                ((HGLOBAL)GlobalHandle(lp))
#define     GlobalLockPtr(lp)                \
                ((BOOL)GlobalLock(GlobalPtrHandle(lp)))
#define     GlobalUnlockPtr(lp)      \
                GlobalUnlock(GlobalPtrHandle(lp))
#define     GlobalAllocPtr(flags, cb)        \
                (GlobalLock(GlobalAlloc((flags), (cb))))
#define     GlobalFreePtr(lp)                \
                (GlobalUnlockPtr(lp), (BOOL)GlobalFree(GlobalPtrHandle(lp)))
enum
{
	PAGE_HEADER	= 1,		// 页眉
	TITLE      	= 2,		// 标题
	AUTHOR     	= 3,		// 作者
	AFFILIATION	= 4,		// 单位
	KEYWORD    	= 5,		// 关键词
	FOOTER     	= 6,		// 页脚
	PAGE_NUMBER	= 7,		// 页码
	REFERENCE  	= 8,		// 参考文献
	EN_TITLE   	= 12,		// 英文标题
	EN_AUTHOR  	= 13,		// 英文作者
	EN_AFFILIATION	= 14,	// 英文单位
	EN_KEYWORD 	= 15,		// 英文关键词
	TEXT		= 21,       // 中文文本
	EN_TEXT		= 22,       // 英文文本
	IMAGE		= 23,       // 图
	TABLE       = 24,       // 表
};

#pragma pack(1)
typedef struct 
{
	CRect Header;    // 高度为0则没有
	CRect Title;     // 标题/作者/单位等，高度为0则没有
	CRect ENTitle;   // 最后页单独的英文标题区，高度为0则没有
	CRect LeftCol[20];   // 左栏   // 2018.07.19
	CRect RightCol[20];  // 右栏，宽度为0 则是单栏
	CRect Footer;       // 高度为0则没有
	int nSplit;         // 最后一页和其它文章分割
} PAPER_LAYOUT;
#pragma pack()

class CReferenceLine
{
public:
	~CReferenceLine() { m_strLine.RemoveAll(); }
	CStringList m_strLine;
};
typedef CTypedPtrList<CPtrList, CReferenceLine*> CReferLineList;


using namespace NCUT_OCR;

class LineProperty;

class CDib : public CObject
{
	DECLARE_DYNAMIC(CDib)

// Constructors
public:
	CDib();

	CDib(const CDib& ); //拷贝
	CDib& operator = (const CDib& ); //赋值

// Attributes
protected:
	LPBYTE m_pBits;//数据文件
	LPBITMAPINFO m_pBMI;//图片信息
	CPalette* m_pPalette;//调色板

	int* m_nXHistgram;
	int* m_nYHistgram;
	int* m_nYPartHist;

public:
	DWORD GetWidth()     const;
	DWORD GetHeight()    const;
	DWORD GetSize()		 const;
	WORD  GetNumColors() const;
	void  Free();

	int GetPaletteSize() const;
	int GetPaletteSize(LPBITMAPINFO pBMI) const;

	BOOL  IsValid()   const 
	{ 
		return (m_pBits != NULL); 
	}
	LPBYTE GetBits() const 
	{
		return m_pBits;
	}
	LPBITMAPINFO GetBmpInfo() const
	{
		return m_pBMI;
	}
	CPalette* GetPalette() const
	{
		return m_pPalette;
	}

	int* GetXHistgram()
	{
		return m_nXHistgram;
	}
	int* GetYHistgram()
	{
		return m_nYHistgram;
	}
	int* GetYPartHist()
	{
		return m_nYPartHist;
	}
//	void SetXHistgram(int *);
//	void SetYHistgram(int *);
//	void SetYPartHist(int *);

	void SetDatas(LPBITMAPINFO , LPBYTE);
	HBITMAP Capture(CWnd *pWnd, CRect& rcDst, CRect& rcSrc);
	HBITMAP Capture(CDC *pDC, CRect& rcDst, CRect& rcSrc);

	BOOL Grizzle(CDib& TmpDib);
/*
	BOOL Row(int* nRow, int nThreshold = 127);
	BOOL Row(CTextRecognizeBase* pOCR, PAPER_LAYOUT& layout, LineProperty& lineproperty, BOOL bFirst = FALSE, int bSplit = FALSE, int nThreshold = 127);
	BOOL LRMargin(PAPER_LAYOUT& layout);
	BOOL Column(PAPER_LAYOUT& layout, int nThreshold = 127);
	BOOL Column(int* nColumn, int y0, int y1, int nThreshold = 127);
	BOOL Column(int* nColumn, int* nRow, int nThreshold = 127, BOOL bMix = FALSE);
	BOOL Line(CRect* LinePos, int& nLine, CRect rect, int nThreshold = 127);
	BOOL Line(int* nHistgram, CRect rect, int direction, CRect* LinePos, int& nLine, int nRowThreshold = 0);
	BOOL Line(CRect rect, int direction, CRect* LinePos, int& nLine);
	BOOL LocateMark(CRect* Mark, int& num, CRect Line, int nThreshold = 127);

	BOOL Layout(CTextRecognizeBase* pOCR, CRect* LinePos, STYLE* style, int nLine, PAPER_LAYOUT paperlayout, LAYOUT* layout, int& num, int nType = 1);  // nType : 1 - 第一页, 2 - 
	BOOL ENLayout(CTextRecognizeBase* pOCR, CRect* LinePos, STYLE* style, int nLine, PAPER_LAYOUT paperlayout, LAYOUT* layout, int& num); // 2017.09.29
	BOOL Reference(CTextRecognizeBase* pOCR, CRect* LinePos, int nLine, CString& str);  // 
	BOOL Reference(CTextRecognizeBase* pOCR, CRect* LinePos, int nLine, CStringList& strList, int& nReferLine, BOOL bReference);  // 没法区分每个文献
	BOOL Reference(CTextRecognizeBase* pOCR, CRect* LinePos, int nLine, CReferLineList& referList, int& nReferLine, BOOL bReference);  // 通过缩进分开每个文献

	BOOL IsEnglish(CTextRecognizeBase* pOCR, CRect rect);
	BOOL IsEnglish(const CString& str);    // 2019.09.20

	BOOL IsDOI(CTextRecognizeBase* pOCR, CRect rect);  // 2018.07.19
*/
	static HBITMAP LoadAnImage(const char* FileName);
	static void BMP2JPG(LPSTR szBmp, BOOL bDelete = TRUE);

	static HANDLE DDBtoDIB( CBitmap& bitmap, DWORD dwCompression, CPalette* ppal ) ;

	static BOOL Grizzle(CDib& SrcDib, CDib& TmpDib);

	//static BOOL JudgeMark(const CDib& dib, int left, int width, int height);
	//static BOOL CalculateYHistgram(const CDib& dib, int left, int width, int* nYHistgram);
	//static BOOL CalculateXHistgram(const CDib& dib, CRect rc, int* nXHistgram);

	//static bool judge(int* nHistgram, int s, bool inc, int nThreshold);
	static double inflexion(CPoint p1, CPoint p2, CPoint p3)
	{
		return -((p2.x-p1.x)*(p3.y-p1.y)+(p1.y-p2.y)*(p3.x-p1.x));
	}
	
	static BOOL Keyword(CString& strKeyword, BOOL bEN);

// Operations
public:
	BOOL  Paint(HDC, LPRECT, LPRECT) const;
	BOOL  Paint(HDC, LPRECT, LPRECT, DWORD) const;
	HGLOBAL CopyToHandle()           const;
	DWORD Save(CFile& file)          const;
	DWORD Save(const CString& filename) const;
	DWORD Read(CFile& file);
	DWORD Read(const CString& filename) ;
	DWORD ReadFromHandle(HGLOBAL hGlobal);
	DWORD ReadFromObject(CBitmap& , BOOL bGrayed = FALSE);
	void Invalidate() 	
	{ 
		Free(); 
	}

	BOOL Cut(int x, int y, int width, int height, CDib& dib);

	void CopyBitmapToClipboard();
	static void CopyWndToClipboard( CWnd *pWnd, int x, int y, int nWidth, int nHeight );

	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CDib();

protected:
	BOOL  CreatePalette();
	WORD  PaletteSize() const;

public:
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif

};


#endif // !defined(AFX_DIB_H__1ED3852D_BCF1_4D0C_B09C_90DE4D962CBD__INCLUDED_)
