//////////////////////////////////////////////////////////////////
// Dib.cpp: implementation of the CDib class.
// MODIFIED ON : 2009-1-7 20:58:09
// AUTHOR	  : 曾凡锋
// COMMENTS   :  
//////////////////////////////////////////////////////////////////

#include "pch.h"
#include "Dib.h"
#include <afxadv.h>
#include <io.h>
#include "HWOCR.h"
//#include "PaperBase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//extern CJournalDataApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDib
IMPLEMENT_DYNAMIC(CDib, CObject)

CDib::CDib()
{
	m_pBMI = NULL;
	m_pBits = NULL;
	m_pPalette = NULL;

	m_nXHistgram = NULL;
	m_nYHistgram = NULL;
	m_nYPartHist = NULL;
}

CDib::~CDib()
{
	Free();
}

void CDib::Free()
{
	// Make sure all member data that might have been allocated is freed.
	if (m_pBits)
	{
		GlobalFreePtr(m_pBits);
		m_pBits = NULL;
	}
	if (m_pPalette)
	{
		m_pPalette->DeleteObject();
		delete m_pPalette;
		m_pPalette = NULL;
	}
	if (m_pBMI)
	{
		GlobalFreePtr(m_pBMI);
		m_pBMI = NULL;
	}

	delete []  m_nXHistgram;
	delete []  m_nYHistgram;
	delete [] m_nYPartHist;
	m_nXHistgram = NULL;
	m_nYHistgram = NULL;
	m_nYPartHist = NULL;
}

CDib::CDib(const CDib& rhs)
{
	m_pBMI = NULL;
	m_pBits = NULL;
	m_pPalette = NULL;
	m_nXHistgram = NULL;
	m_nYHistgram = NULL;
	m_nYPartHist = NULL;

	*this = rhs;
}

CDib& CDib::operator = (const CDib& rhs)
{
	if( this == &rhs || rhs.IsValid() == FALSE )
		return *this;

	SetDatas(rhs.m_pBMI, rhs.m_pBits);

	delete [] m_nXHistgram;
	delete [] m_nYHistgram;
	delete [] m_nYPartHist;
	
	m_nXHistgram = new int[GetWidth()];
	m_nYHistgram = new int[GetHeight()];
	m_nYPartHist = new int[GetHeight()];
	
	//暂时无用,被删
	/*memcpy(m_nXHistgram, rhs.m_nXHistgram, GetWidth()*sizeof(int));
	memcpy(m_nYHistgram, rhs.m_nYHistgram, GetHeight()*sizeof(int));
	memcpy(m_nYPartHist, rhs.m_nYPartHist, GetHeight()*sizeof(int));*/

	return *this;
}

void CDib::SetDatas(LPBITMAPINFO pBMI, LPBYTE lpBits)
{
	Free();
	
	// Allocate memory for DIB
	DWORD cb = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD);
	if( pBMI->bmiHeader.biBitCount == 1 )
		cb = sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD);
	else if( pBMI->bmiHeader.biBitCount == 2 )
		cb = sizeof(BITMAPINFOHEADER) + 4 * sizeof(RGBQUAD);
	else if( pBMI->bmiHeader.biBitCount == 4 )
		cb = sizeof(BITMAPINFOHEADER) + 16 * sizeof(RGBQUAD);
	else if( pBMI->bmiHeader.biBitCount == 8 )
		cb = sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
	m_pBMI = (LPBITMAPINFO)GlobalAllocPtr(GHND, cb);
	if (m_pBMI == NULL)
		return;

	memcpy(m_pBMI, pBMI, cb);

	// read the bits.
	DWORD dwSize = m_pBMI->bmiHeader.biSizeImage;
	m_pBits = (LPBYTE)GlobalAllocPtr(GHND, dwSize);

	if(!m_pBits )
	{
		GlobalFreePtr(m_pBMI);
		m_pBMI = NULL;
		return;
	}
	
	memcpy(m_pBits, lpBits, dwSize);

	CreatePalette();

}

/*************************************************************************
 *
 * Paint()
 *
 * Parameters:
 * HDC hDC          - DC to do output to
 * LPRECT lpDCRect  - rectangle on DC to do output to
 * LPRECT lpDIBRect - rectangle of DIB to output into lpDCRect
 * CPalette* pPal   - pointer to CPalette containing DIB's palette
 *
 * Return Value:
 * BOOL             - TRUE if DIB was drawn, FALSE otherwise
 *
 * Description:
 *   Painting routine for a DIB.  Calls StretchDIBits() or
 *   SetDIBitsToDevice() to paint the DIB.  The DIB is
 *   output to the specified DC, at the coordinates given
 *   in lpDCRect.  The area of the DIB to be output is
 *   given by lpDIBRect.
 *
 ************************************************************************/
BOOL CDib::Paint(HDC hDC, LPRECT lpDCRect, LPRECT lpDIBRect) const
{
	if (!m_pBMI)
		return FALSE;

	HPALETTE hPal = NULL;           // Our DIB's palette
	HPALETTE hOldPal = NULL;        // Previous palette

	// Get the DIB's palette, then select it into DC
	if (m_pPalette != NULL)
	{
		hPal = (HPALETTE) m_pPalette->m_hObject;

		// Select as background since we have
		// already realized in forground if needed
		hOldPal = ::SelectPalette(hDC, hPal, TRUE);
	}

	/* Make sure to use the stretching mode best for color pictures */
	::SetStretchBltMode(hDC, COLORONCOLOR);

	/* Determine whether to call StretchDIBits() or SetDIBitsToDevice() */
	BOOL bSuccess;
	if ((RECTWIDTH(lpDCRect)  == RECTWIDTH(lpDIBRect)) &&
	   (RECTHEIGHT(lpDCRect) == RECTHEIGHT(lpDIBRect)))
		bSuccess = ::SetDIBitsToDevice(hDC,        // hDC
								   lpDCRect->left,             // DestX
								   lpDCRect->top,              // DestY
								   RECTWIDTH(lpDCRect),        // nDestWidth
								   RECTHEIGHT(lpDCRect),       // nDestHeight
								   lpDIBRect->left,            // SrcX
								   (int)GetHeight() -
									  lpDIBRect->top -
									  RECTHEIGHT(lpDIBRect),     // SrcY
								   0,                          // nStartScan
								   (WORD)GetHeight(),             // nNumScans
								   m_pBits,                    // lpBits
								   m_pBMI,                     // lpBitsInfo
								   DIB_RGB_COLORS);            // wUsage
   else
	  bSuccess = ::StretchDIBits(hDC,            // hDC
							   lpDCRect->left,               // DestX
							   lpDCRect->top,                // DestY
							   RECTWIDTH(lpDCRect),          // nDestWidth
							   RECTHEIGHT(lpDCRect),         // nDestHeight
							   lpDIBRect->left,              // SrcX
							   lpDIBRect->top,               // SrcY
							   RECTWIDTH(lpDIBRect),         // wSrcWidth
							   RECTHEIGHT(lpDIBRect),        // wSrcHeight
							   m_pBits,                      // lpBits
							   m_pBMI,                       // lpBitsInfo
							   DIB_RGB_COLORS,               // wUsage
							   SRCCOPY);                     // dwROP

	/* Reselect old palette */
	if (hOldPal != NULL)
	{
		::SelectPalette(hDC, hOldPal, TRUE);
	}

   return bSuccess;
}

BOOL CDib::Paint(HDC hDC, LPRECT lpDCRect, LPRECT lpDIBRect, DWORD dwRop) const
{
	if (!m_pBMI)
		return FALSE;
	
	HPALETTE hPal = NULL;           // Our DIB's palette
	HPALETTE hOldPal = NULL;        // Previous palette
	
	// Get the DIB's palette, then select it into DC
	if (m_pPalette != NULL)
	{
		hPal = (HPALETTE) m_pPalette->m_hObject;
		
		// Select as background since we have
		// already realized in forground if needed
		hOldPal = ::SelectPalette(hDC, hPal, TRUE);
	}
	
	/* Make sure to use the stretching mode best for color pictures */
	::SetStretchBltMode(hDC, COLORONCOLOR);
	
	/* Determine whether to call StretchDIBits() or SetDIBitsToDevice() */
	BOOL bSuccess;
	bSuccess = ::StretchDIBits(hDC,            // hDC
					lpDCRect->left,               // DestX
					lpDCRect->top,                // DestY
					RECTWIDTH(lpDCRect),          // nDestWidth
					RECTHEIGHT(lpDCRect),         // nDestHeight
					lpDIBRect->left,              // SrcX
					lpDIBRect->top,               // SrcY
					RECTWIDTH(lpDIBRect),         // wSrcWidth
					RECTHEIGHT(lpDIBRect),        // wSrcHeight
					m_pBits,                      // lpBits
					m_pBMI,                       // lpBitsInfo
					DIB_RGB_COLORS,               // wUsage
					dwRop);                       // dwROP
	
	/* Reselect old palette */
	if (hOldPal != NULL)
	{
		::SelectPalette(hDC, hOldPal, TRUE);
	}
	
	return bSuccess;
}

/*************************************************************************
 *
 * CreatePalette()
 *
 * Return Value:
 * TRUE if succesfull, FALSE otherwise
 *
 * Description:
 *
 * This function creates a palette from a DIB by allocating memory for the
 * logical palette, reading and storing the colors from the DIB's color table
 * into the logical palette, creating a palette from this logical palette,
 * and then returning the palette's handle. This allows the DIB to be
 * displayed using the best possible colors (important for DIBs with 256 or
 * more colors).
 *
 ************************************************************************/

BOOL CDib::CreatePalette()
{
	if (!m_pBMI)
		return FALSE;

   //get the number of colors in the DIB
   WORD wNumColors = GetNumColors();

   if (wNumColors != 0)
   {
		// allocate memory block for logical palette
		HANDLE hLogPal = ::GlobalAlloc(GHND, sizeof(LOGPALETTE) + sizeof(PALETTEENTRY)*wNumColors);

		// if not enough memory, clean up and return NULL
		if (hLogPal == 0)
			return FALSE;

		LPLOGPALETTE lpPal = (LPLOGPALETTE)::GlobalLock((HGLOBAL)hLogPal);

		// set version and number of palette entries
		lpPal->palVersion = PALVERSION;
		lpPal->palNumEntries = (WORD)wNumColors;

		for (int i = 0; i < (int)wNumColors; i++)
		{
			lpPal->palPalEntry[i].peRed = m_pBMI->bmiColors[i].rgbRed;
			lpPal->palPalEntry[i].peGreen = m_pBMI->bmiColors[i].rgbGreen;
			lpPal->palPalEntry[i].peBlue = m_pBMI->bmiColors[i].rgbBlue;
			lpPal->palPalEntry[i].peFlags = 0;
		}

		/* create the palette and get handle to it */
		if (m_pPalette)
		{
			m_pPalette->DeleteObject();
			delete m_pPalette;
		}
		m_pPalette = new CPalette;
		BOOL bResult = m_pPalette->CreatePalette(lpPal);
		::GlobalUnlock((HGLOBAL) hLogPal);
		::GlobalFree((HGLOBAL) hLogPal);
		return bResult;
	}

	return TRUE;
}

/*************************************************************************
 *
 * GetWidth()
 *
 * Return Value:
 * DWORD            - width of the DIB
 *
 * Description:
 *
 * This function gets the width of the DIB from the BITMAPINFOHEADER
 * width field 
 *
 ************************************************************************/

DWORD CDib::GetWidth() const
{
	if (!m_pBMI)
		return 0;

	/* return the DIB width */
	return m_pBMI->bmiHeader.biWidth;
}


/*************************************************************************
 *
 * GetHeight()
 *
 * Return Value:
 * DWORD            - height of the DIB
 *
 * Description:
 *
 * This function gets the height of the DIB from the BITMAPINFOHEADER
 * height field 
 *
 ************************************************************************/

DWORD CDib::GetHeight() const
{
	if (!m_pBMI)
		return 0;
	
	/* return the DIB height */
	return m_pBMI->bmiHeader.biHeight;
}

/*************************************************************************
 *
 * GetSize()
 *
 * Return Value:
 * DWORD            - size of the DIB
 *
 * Description:
 *
 * This function gets the size of the DIB from the BITMAPINFOHEADER
 * imagesize field 
 *
 ************************************************************************/

DWORD CDib::GetSize() const
{
	if (!m_pBMI)
		return 0;
	
	/* return the DIB size */
	return m_pBMI->bmiHeader.biSizeImage;
}



/*************************************************************************
 *
 * PaletteSize()
 *
 * Return Value:
 * WORD             - size of the color palette of the DIB
 *
 * Description:
 *
 * This function gets the size required to store the DIB's palette by
 * multiplying the number of colors by the size of an RGBQUAD 
 *
 ************************************************************************/

WORD CDib::PaletteSize() const
{
	if (!m_pBMI)
		return 0;

	return GetNumColors() * sizeof(RGBQUAD);
}


/*************************************************************************
 *
 * GetNumColors()
 *
 * Return Value:
 * WORD             - number of colors in the color table
 *
 * Description:
 *
 * This function calculates the number of colors in the DIB's color table
 * by finding the bits per pixel for the DIB (whether Win3.0 or other-style
 * DIB). If bits per pixel is 1: colors=2, if 4: colors=16, if 8: colors=256,
 * if 24, no colors in color table.
 *
 ************************************************************************/

WORD CDib::GetNumColors() const
{
	if (!m_pBMI)
		return 0;

	WORD wBitCount;  // DIB bit count

	/*  The number of colors in the color table can be less than 
	 *  the number of bits per pixel allows for (i.e. lpbi->biClrUsed
	 *  can be set to some value).
	 *  If this is the case, return the appropriate value.
	 */

	DWORD dwClrUsed;

	dwClrUsed = m_pBMI->bmiHeader.biClrUsed;
	if (dwClrUsed != 0)
		return (WORD)dwClrUsed;

	/*  Calculate the number of colors in the color table based on
	 *  the number of bits per pixel for the DIB.
	 */
	wBitCount = m_pBMI->bmiHeader.biBitCount;

	/* return number of colors based on bits per pixel */
	switch (wBitCount)
	{
		case 1:
			return 2;

		case 4:
			return 16;

		case 8:
			return 256;

		default:
			return 0;
	}
}

/*************************************************************************
 *
 * Save()
 * Saves the specified DIB into the specified CFile.  The CFile
 * is opened and closed by the caller.
 *
 * Parameters:
 * CFile& file - open CFile used to save DIB
 *
 * Return value: Number of saved bytes or CFileException
 *
 *************************************************************************/

DWORD CDib::Save(const CString& filename) const
{
	CFile file(filename, CFile::modeCreate|CFile::modeWrite);
	return Save(file);
}

DWORD CDib::Save(CFile& file) const
{
	BITMAPFILEHEADER bmfHdr; // Header for Bitmap file
	DWORD dwDIBSize;

	if (m_pBMI == NULL)
		return 0;

	// Fill in the fields of the file header

	// Fill in file type (first 2 bytes must be "BM" for a bitmap)
	bmfHdr.bfType = DIB_HEADER_MARKER;  // "BM"

	// Calculating the size of the DIB is a bit tricky (if we want to
	// do it right).  The easiest way to do this is to call GlobalSize()
	// on our global handle, but since the size of our global memory may have
	// been padded a few bytes, we may end up writing out a few too
	// many bytes to the file (which may cause problems with some apps).
	//
	// So, instead let's calculate the size manually (if we can)
	//
	// First, find size of header plus size of color table.  Since the
	// first DWORD in both BITMAPINFOHEADER and BITMAPCOREHEADER conains
	// the size of the structure, let's use this.
	dwDIBSize = *(LPDWORD)&m_pBMI->bmiHeader + PaletteSize();  // Partial Calculation

	// Now calculate the size of the image
	if ((m_pBMI->bmiHeader.biCompression == BI_RLE8) || (m_pBMI->bmiHeader.biCompression == BI_RLE4))
	{
		// It's an RLE bitmap, we can't calculate size, so trust the
		// biSizeImage field
		dwDIBSize += m_pBMI->bmiHeader.biSizeImage;
	}
	else
	{
		DWORD dwBmBitsSize;  // Size of Bitmap Bits only

		// It's not RLE, so size is GetWidth (DWORD aligned) * Height
		dwBmBitsSize = BMPWIDTHBYTES((m_pBMI->bmiHeader.biWidth)*((DWORD)m_pBMI->bmiHeader.biBitCount)) * m_pBMI->bmiHeader.biHeight;
		dwDIBSize += dwBmBitsSize;

		// Now, since we have calculated the correct size, why don't we
		// fill in the biSizeImage field (this will fix any .BMP files which
		// have this field incorrect).
		m_pBMI->bmiHeader.biSizeImage = dwBmBitsSize;
	}

	// Calculate the file size by adding the DIB size to sizeof(BITMAPFILEHEADER)
	bmfHdr.bfSize = dwDIBSize + sizeof(BITMAPFILEHEADER);
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;

	/*
	 * Now, calculate the offset the actual bitmap bits will be in
	 * the file -- It's the Bitmap file header plus the DIB header,
	 * plus the size of the color table.
	 */
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + m_pBMI->bmiHeader.biSize + PaletteSize();

	// Write the file header
	file.Write((LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER));
	DWORD dwBytesSaved = sizeof(BITMAPFILEHEADER); 

	// Write the DIB header
	UINT nCount = sizeof(BITMAPINFO) + (GetNumColors()-1)*sizeof(RGBQUAD);
	dwBytesSaved += nCount; 
	file.Write(m_pBMI, nCount);
	
	// Write the DIB bits
	DWORD dwBytes = m_pBMI->bmiHeader.biBitCount * GetWidth();
  // Calculate the number of bytes per line
	if (dwBytes%32 == 0)
		dwBytes /= 8;
	else
		dwBytes = dwBytes/8 + (32-dwBytes%32)/8 + (((32-dwBytes%32)%8 > 0) ? 1 : 0); 
	nCount = dwBytes * GetHeight();
	dwBytesSaved += nCount; 
	file.Write(m_pBits, nCount);

	return dwBytesSaved;
}


/*************************************************************************
 *
 * Function:  Read (CFile&)
 *
 * Purpose:  Reads in the specified DIB file into a global chunk of
 *			 memory.
 *
 *  Returns:  Number of read bytes.
 *
 *************************************************************************/

DWORD CDib::Read(const CString& filename) 
{
	CString str = filename;
	str.MakeLower();
	if( str.Right(4) == _T(".jpg") )
	{
		HBITMAP hB = LoadAnImage(filename);
		if( hB )   // 2012.04.27
		{
			CBitmap* pBmp   =   CBitmap::FromHandle(hB);
			DWORD dwRet = ReadFromObject(*pBmp);
			::DeleteObject(hB);
			return dwRet;
		}
		else return 0;
	}

	CFile file;
	if( !file.Open(filename, CFile::modeRead | CFile::shareDenyRead) )
		return 0;
	return Read(file);
}


DWORD CDib::Read(CFile& file)
{
	// Ensures no memory leaks will occur
	Free();
	
	BITMAPFILEHEADER bmfHeader;

	// Go read the DIB file header and check if it's valid.
	if (file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
		return 0;
	if (bmfHeader.bfType != DIB_HEADER_MARKER)
		return 0;
	DWORD dwReadBytes = sizeof(bmfHeader);

	// Allocate memory for DIB
	m_pBMI = (LPBITMAPINFO)GlobalAllocPtr(GHND, bmfHeader.bfOffBits - sizeof(BITMAPINFOHEADER) + 256*sizeof(RGBQUAD));
	if (m_pBMI == 0)
		return 0;

	// Read header.
	if (file.Read(m_pBMI, bmfHeader.bfOffBits-sizeof(BITMAPFILEHEADER)) != (UINT)(bmfHeader.bfOffBits-sizeof(BITMAPFILEHEADER)))
	{
		GlobalFreePtr(m_pBMI);
		m_pBMI = NULL;
		return 0;
	}
	dwReadBytes += bmfHeader.bfOffBits-sizeof(BITMAPFILEHEADER);

	int nLineBits = GetBytePerLine(m_pBMI->bmiHeader.biWidth, m_pBMI->bmiHeader.biBitCount);
	m_pBMI->bmiHeader.biSizeImage		= nLineBits * m_pBMI->bmiHeader.biHeight;

	DWORD dwLength = DWORD(file.GetLength());
	// Go read the bits.
	m_pBits = (LPBYTE)GlobalAllocPtr(GHND, dwLength - bmfHeader.bfOffBits);
	if (m_pBits == 0)
	{
		GlobalFreePtr(m_pBMI);
		m_pBMI = NULL;
		return 0;
	}
	
	if (file.Read(m_pBits, dwLength-bmfHeader.bfOffBits) != (dwLength - bmfHeader.bfOffBits))
	{
		GlobalFreePtr(m_pBMI);
		m_pBMI = NULL;
		GlobalFreePtr(m_pBits);
		m_pBits = NULL;
		return 0;
	}
	dwReadBytes += dwLength - bmfHeader.bfOffBits;

	delete [] m_nXHistgram;
	delete [] m_nYHistgram;
	delete [] m_nYPartHist;
	m_nXHistgram = new int[GetWidth()];
	m_nYHistgram = new int[GetHeight()];
	m_nYPartHist = new int[GetHeight()];
	memset(m_nXHistgram, 0, GetWidth()*sizeof(int));
	memset(m_nYHistgram, 0, GetHeight()*sizeof(int));
	memset(m_nYPartHist, 0, GetHeight()*sizeof(int));

	CreatePalette();
	return dwReadBytes;
}

//---------------------------------------------------------------------
//
// Function:   ReadFromObject
//
// Purpose:    Initializes from the given CBitmap object.  
//
// Returns:    Number of read bytes.
//
//---------------------------------------------------------------------

DWORD CDib::ReadFromObject(CBitmap& cbmp, BOOL bGrayed)
{
	DWORD dwLen = 0;
	
	try
	{
		Free();

		BITMAP bm;
		cbmp.GetBitmap(&bm);

		m_pBMI = (LPBITMAPINFO)GlobalAllocPtr(GHND, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD));

		m_pBMI->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
		m_pBMI->bmiHeader.biWidth			= bm.bmWidth;
		m_pBMI->bmiHeader.biHeight 			= bm.bmHeight;
		m_pBMI->bmiHeader.biPlanes 			= 1;
		m_pBMI->bmiHeader.biBitCount		= bm.bmPlanes * bm.bmBitsPixel;
		m_pBMI->bmiHeader.biCompression		= BI_RGB;
		// each scan line of the image is aligned on a dword (32bit) boundary
		int nLineBits = (((m_pBMI->bmiHeader.biWidth * m_pBMI->bmiHeader.biBitCount) + 31) & ~31) / 8;
		m_pBMI->bmiHeader.biSizeImage		= nLineBits * m_pBMI->bmiHeader.biHeight;
		m_pBMI->bmiHeader.biXPelsPerMeter	= 0;
		m_pBMI->bmiHeader.biYPelsPerMeter	= 0;
		m_pBMI->bmiHeader.biClrUsed		= 0;
		m_pBMI->bmiHeader.biClrImportant	= 0;
		// compute the size of the  infoheader and the color table
		int ncolors = 0;
		dwLen  += m_pBMI->bmiHeader.biSize + ncolors*sizeof(RGBQUAD) + m_pBMI->bmiHeader.biSizeImage;

		m_pBits = (LPBYTE)GlobalAllocPtr(GHND, m_pBMI->bmiHeader.biSizeImage);
		if (m_pBits == 0 )
		{
			GlobalFreePtr(m_pBMI);
			m_pBMI = NULL;
			return 0;
		}
		LPBYTE pTmp = (LPBYTE)GlobalAllocPtr(GHND, m_pBMI->bmiHeader.biSizeImage);
		if (pTmp == 0 )
		{
			GlobalFreePtr(m_pBMI);
			m_pBMI = NULL;
			GlobalFreePtr(m_pBits);
			m_pBits = NULL;
			return 0;
		}
		
		if (cbmp.GetBitmapBits(m_pBMI->bmiHeader.biSizeImage, pTmp) == 0)
		{
			GlobalFreePtr(m_pBMI);
			m_pBMI = NULL;
			GlobalFreePtr(m_pBits);
			m_pBits = NULL;
			return 0;
		}
		else
		{
			for(int i = 0; i < m_pBMI->bmiHeader.biHeight; i++)
			{
				memcpy( &m_pBits[(m_pBMI->bmiHeader.biHeight-1-i)*nLineBits] , &pTmp[i*nLineBits], nLineBits*sizeof(BYTE) );
			}

			delete [] m_nXHistgram;
			delete [] m_nYHistgram;
			delete [] m_nYPartHist;
			m_nXHistgram = new int[GetWidth()];
			m_nYHistgram = new int[GetHeight()];
			m_nYPartHist = new int[GetHeight()];
			memset(m_nXHistgram, 0, GetWidth()*sizeof(int));
			memset(m_nYHistgram, 0, GetHeight()*sizeof(int));
			memset(m_nYPartHist, 0, GetHeight()*sizeof(int));
		}
	}
	catch(...)
	{
		return 0;
	}

	return dwLen;
}

#ifdef _DEBUG
void CDib::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
}
#endif

//////////////////////////////////////////////////////////////////////////
//// Clipboard support

//---------------------------------------------------------------------
//
// Function:   CopyToHandle
//
// Purpose:    Makes a copy of the DIB to a global memory block.  Returns
//             a handle to the new memory block (NULL on error).
//
// Returns:    Handle to new global memory block.
//
//---------------------------------------------------------------------

HGLOBAL CDib::CopyToHandle() const
{
	CSharedFile file;
	try
	{
		if (Save(file)==0)
			return 0;
	}
	catch (CFileException* e)
	{
		e->Delete();
		return 0;
	}
		
	return file.Detach();
}

//---------------------------------------------------------------------
//
// Function:   ReadFromHandle
//
// Purpose:    Initializes from the given global memory block.  
//
// Returns:    Number of read bytes.
//
//---------------------------------------------------------------------

DWORD CDib::ReadFromHandle(HGLOBAL hGlobal)
{
	CSharedFile file;
	file.SetHandle(hGlobal, FALSE);
	DWORD dwResult = Read(file);
	file.Detach();
	return dwResult;
}

//////////////////////////////////////////////////////////////////////////
//// Serialization support

void CDib::Serialize(CArchive& ar) 
{
	CFile* pFile = ar.GetFile();
	ASSERT(pFile != NULL);
	if (ar.IsStoring())
	{	// storing code
		if (pFile)
		{
			Save(*pFile);
		}
	}
	else
	{	// loading code
		if (pFile)
		{
			Read(*pFile);
		}
	}
}

// 静态成员函数

//---------------------------------------------------------------------
// Function:   ReadFromHandle
// Purpose:    open a jpg/gif file.  
HBITMAP CDib::LoadAnImage(const char* FileName)
{
   // Use IPicture stuff to use JPG / GIF files
	IPicture* p;
	IStream* s;
	HGLOBAL hG;
	void* pp;
	FILE* fp;


	// Read file in memory
	errno_t err = fopen_s(&fp, FileName,"rb");
	if (!fp) return NULL;

	fseek(fp,0,SEEK_END);
	int fs = ftell(fp);
	fseek(fp,0,SEEK_SET);
	hG = GlobalAlloc(GPTR,fs);
	if (!hG)
	{
		fclose(fp);
		return NULL;
	}
	pp = (void*)hG;
	fread(pp,1,fs,fp);
	fclose(fp);

	// Create an IStream so IPicture can
	CreateStreamOnHGlobal(hG,false,&s);
	if (!s)
	{
		GlobalFree(hG);
		return NULL;
	}

	OleLoadPicture(s,0,false,IID_IPicture,(void**)&p);

	if (!p)
	{
		s->Release();
		GlobalFree(hG);
		return NULL;
	}
	s->Release();
	GlobalFree(hG);

	HBITMAP hB = 0;
	p->get_Handle((unsigned int*)&hB);

	// Copy the image. Necessary, because upon p's release,
	// the handle is destroyed.
	HBITMAP hBB = (HBITMAP)CopyImage(hB,IMAGE_BITMAP,0,0,LR_COPYRETURNORG);

	p->Release();
	return hBB;
}

//---------------------------------------------------------------------
// Function:   ReadFromHandle
// Purpose:    change a bmp file to jpg file and save.  
void CDib::BMP2JPG(LPSTR szBmp, BOOL bDelete)
{
	HINSTANCE hInst;
	CString sJPG = szBmp;

	try
	{
		sJPG = sJPG.Left(sJPG.GetLength()-3);
		sJPG += "jpg";
		CHANGE *pFunc;/*/D:\\Program Files\\Borland\\CBuilder6\\Projects\\*/
		hInst = ::LoadLibrary("jpg.dll");
		pFunc = (CHANGE*)::GetProcAddress(hInst,"BmpToJpg");
		Sleep(100); // 必须延时
		pFunc(szBmp, sJPG.GetBuffer(MAX_PATH));

		sJPG.ReleaseBuffer();
		::FreeLibrary(hInst);
		if( bDelete )
		{
			_unlink(szBmp);
		}
	}
	catch(...)
	{
	}
}


HBITMAP CDib::Capture(CDC* pDC, CRect& rcDst, CRect& rcSrc)
{ 
	CBitmap 	bitmap;
	CDC 		memdc;
	
	memdc.CreateCompatibleDC(pDC); 
    bitmap.CreateCompatibleBitmap(pDC, rcDst.Width(), rcDst.Height());
	CBitmap* poldbitmap = memdc.SelectObject(&bitmap);
	memdc.StretchBlt(0, 0, rcDst.Width() , rcDst.Height(), pDC, rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height(),SRCCOPY); 
	// create logical palette if device support a palette
	CPalette pal;
	if( pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
	{
		UINT nsize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * 256);
		LOGPALETTE *plp = (LOGPALETTE *) new BYTE[nsize];
		plp->palVersion = 0x300;
		plp->palNumEntries = GetSystemPaletteEntries(pDC->GetSafeHdc(), 0, 255, plp->palPalEntry );
		// create the palette
		pal.CreatePalette( plp );
		delete[] plp;
	}
	memdc.SelectObject(poldbitmap);
	
	ReadFromObject(bitmap);
	
	return (HBITMAP)bitmap;
}

HBITMAP CDib::Capture(CWnd *pWnd, CRect& rcDst, CRect& rcSrc)
{ 
	CBitmap 	bitmap;
	CWindowDC	dc(pWnd);
	CDC 		memdc;
	
	memdc.CreateCompatibleDC(&dc); 
    bitmap.CreateCompatibleBitmap(&dc, rcDst.Width(), rcDst.Height());
	CBitmap* poldbitmap = memdc.SelectObject(&bitmap);
	memdc.StretchBlt(0, 0, rcDst.Width() , rcDst.Height(), &dc, rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height(),SRCCOPY); 
	// create logical palette if device support a palette
	CPalette pal;
	if( dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
	{
		UINT nsize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * 256);
		LOGPALETTE *plp = (LOGPALETTE *) new BYTE[nsize];
		plp->palVersion = 0x300;
		plp->palNumEntries = GetSystemPaletteEntries( dc, 0, 255, plp->palPalEntry );
		// create the palette
		pal.CreatePalette( plp );
		delete[] plp;
	}
	memdc.SelectObject(poldbitmap);

	ReadFromObject(bitmap);

	return (HBITMAP)bitmap;
}

HANDLE CDib::DDBtoDIB( CBitmap& bitmap, DWORD dwCompression, CPalette* ppal ) 
{
	BITMAP			bm;
	BITMAPINFOHEADER	bi;
	LPBITMAPINFOHEADER 	lpbi;
	DWORD			dwLen;
	HANDLE			hDib;
	HANDLE			handle;
	HDC 			hdc;
	HPALETTE		hPal;

	ASSERT( bitmap.GetSafeHandle() );
	// the function has no arg for bitfields
	if( dwCompression == BI_BITFIELDS )
		return NULL;
	// if a palette has not been supplied use defaul palette
	hPal = (HPALETTE) ppal->GetSafeHandle();
	if (hPal==NULL)
		hPal = (HPALETTE) GetStockObject(DEFAULT_PALETTE );
	// get bitmap information
	bitmap.GetObject(sizeof(bm),(LPSTR)&bm);
	// initialize the bitmapinfoheader
	bi.biSize		= sizeof(BITMAPINFOHEADER);
	bi.biWidth		= bm.bmWidth;
	bi.biHeight 		= bm.bmHeight;
	bi.biPlanes 		= 1;
	bi.biBitCount		= bm.bmPlanes * bm.bmBitsPixel;
	bi.biCompression	= dwCompression;
	bi.biSizeImage		= 0;
	bi.biXPelsPerMeter	= 0;
	bi.biYPelsPerMeter	= 0;
	bi.biClrUsed		= 0;
	bi.biClrImportant	= 0;
	// compute the size of the  infoheader and the color table
	int ncolors = (1 << bi.biBitCount); if( ncolors> 256 ) 
		ncolors = 0;
	dwLen  = bi.biSize + ncolors * sizeof(RGBQUAD);
	// we need a device context to get the dib from
	hdc = GetDC(NULL);
	hPal = SelectPalette(hdc,hPal,FALSE);
	RealizePalette(hdc);

	// allocate enough memory to hold bitmapinfoheader and color table
	hDib = GlobalAlloc(GMEM_FIXED,dwLen);

	if (!hDib){
		SelectPalette(hdc,hPal,FALSE);
		ReleaseDC(NULL,hdc);
		return NULL;
	}

	lpbi = (LPBITMAPINFOHEADER)hDib;

	*lpbi = bi;

	// call getdibits with a NULL lpbits param, so the device driver 
	// will calculate the bisizeimage field 
	GetDIBits(hdc, (HBITMAP)bitmap.GetSafeHandle(), 0L, (DWORD)bi.biHeight,
			(LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS );

	bi = *lpbi;

	// if the driver did not fill in the bisizeimage field, then compute it
	// each scan line of the image is aligned on a dword (32bit) boundary
	if (bi.biSizeImage == 0){
		bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8) 
						* bi.biHeight;

		// if a compression scheme is used the result may infact be larger
		// increase the size to account for this.
		if (dwCompression != BI_RGB)
			bi.biSizeImage = (bi.biSizeImage * 3) / 2;
	}

	// realloc the buffer so that it can hold all the bits
	dwLen += bi.biSizeImage;
	if (handle = GlobalReAlloc(hDib, dwLen, GMEM_MOVEABLE))
		hDib = handle;
	else{
		GlobalFree(hDib);

		// reselect the original palette
		SelectPalette(hdc,hPal,FALSE);
		ReleaseDC(NULL,hdc);
		return NULL;
	}

	// get the bitmap bits
	lpbi = (LPBITMAPINFOHEADER)hDib;

	// finally get the dib
	BOOL bgotbits = GetDIBits( hdc, (HBITMAP)bitmap.GetSafeHandle(),
				0L,				// start scan line
				(DWORD)bi.biHeight,		// # of scan lines
				(LPBYTE)lpbi 			// address for bitmap bits
				+ (bi.biSize + ncolors * sizeof(RGBQUAD)),
				(LPBITMAPINFO)lpbi,		// address of bitmapinfo
				(DWORD)DIB_RGB_COLORS);		// use rgb for color table

	if( !bgotbits )
	{
		GlobalFree(hDib);
		
		SelectPalette(hdc,hPal,FALSE);
		ReleaseDC(NULL,hdc);
		return NULL;
	}
	SelectPalette(hdc,hPal,FALSE);
	ReleaseDC(NULL,hdc);

	return hDib;
}

void CDib::CopyBitmapToClipboard( ) 
{ 
	CBitmap bitmap;
	CPalette* ppal = m_pPalette;

	bitmap.CreateBitmap(m_pBMI->bmiHeader.biWidth, m_pBMI->bmiHeader.biHeight, m_pBMI->bmiHeader.biPlanes,
		m_pBMI->bmiHeader.biBitCount, NULL);
	bitmap.SetBitmapBits(m_pBMI->bmiHeader.biWidth*m_pBMI->bmiHeader.biHeight, m_pBits);
	HBITMAP hh = (HBITMAP)bitmap.GetSafeHandle() ;

	::OpenClipboard(NULL); 
	::EmptyClipboard() ; 
	if( ppal ) 
	{
		::SetClipboardData (CF_PALETTE, ppal->GetSafeHandle() ) ; 
	}
	::SetClipboardData (CF_BITMAP, hh) ; 
	::CloseClipboard () ; 
	bitmap.Detach(); 
	if( ppal ) 
	{
		ppal->Detach(); 
	}
} 

void CDib::CopyWndToClipboard( CWnd *pWnd, int x, int y, int nWidth, int nHeight )
{
	CBitmap 	bitmap;
	CClientDC	dc(pWnd);
	CDC 		memDC;
	CRect		rect;
	
	memDC.CreateCompatibleDC(&dc);
	pWnd->GetWindowRect(rect);
	if( nWidth > rect.Width() - x )
		nWidth = rect.Width() - x;
	if( nHeight > rect.Height() - y )
		nHeight = rect.Height() - y;
	bitmap.CreateCompatibleBitmap(&dc, nWidth, nHeight);
	
	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
	memDC.StretchBlt(0, 0, nWidth, nHeight, &dc, x, y, nWidth, nHeight, SRCCOPY);
	
	pWnd->OpenClipboard() ;
	EmptyClipboard() ;
	SetClipboardData (CF_BITMAP, bitmap.GetSafeHandle() ) ;
	CloseClipboard () ;
	
	memDC.SelectObject(pOldBitmap);
	bitmap.Detach();
}

BOOL CDib::Grizzle(CDib& TmpDib)
{
	return Grizzle(*this, TmpDib);
}

BOOL CDib::Grizzle(CDib& SrcDib, CDib& TmpDib)
{	
	LPBYTE pData = SrcDib.GetBits();
	LPBITMAPINFO pBMI = SrcDib.GetBmpInfo();
	if( !pBMI )
		return FALSE;

	int *nXHistgram = new int[SrcDib.GetWidth()];
	int *nYHistgram = new int[SrcDib.GetHeight()];
	int *nYPartHist = new int[SrcDib.GetHeight()];
	memset(nXHistgram, 0, SrcDib.GetWidth()*sizeof(int));
	memset(nYHistgram, 0, SrcDib.GetHeight()*sizeof(int));
	memset(nYPartHist, 0, SrcDib.GetHeight()*sizeof(int));

	if( pBMI->bmiHeader.biBitCount == 8 )
	{
		int nThreshold = 127;
		long nBytePerLine = (pBMI->bmiHeader.biWidth*pBMI->bmiHeader.biBitCount + 31)/32*4;//图像每行所占实际字节数（须是4的倍数）
		for(int i = 0; i < pBMI->bmiHeader.biHeight; i++)
		{
			BYTE* pSrcTemp = (pData + i*nBytePerLine);
			for(int j = 0; j < pBMI->bmiHeader.biWidth; j++)
			{
				BYTE ngrayscale = *(pSrcTemp + j);
				if( ngrayscale < nThreshold ) 
				{
					nXHistgram[j]++;
					nYHistgram[i]++;
					if( j < pBMI->bmiHeader.biWidth/2 ) nYPartHist[i]++;
				}
			}
		}
//		SrcDib.SetXHistgram(nXHistgram);
//		SrcDib.SetYHistgram(nYHistgram);
//		SrcDib.SetYPartHist(nYPartHist);
		delete [] nXHistgram;
		delete [] nYHistgram;
		delete [] nYPartHist;
		return FALSE;
	}

	long nNewBytePerLine = GetBytePerLine(pBMI->bmiHeader.biWidth,8);
	DWORD dwNewSize = pBMI->bmiHeader.biHeight*nNewBytePerLine;

	LPBYTE pNewBits = (LPBYTE)GlobalAllocPtr(GHND, dwNewSize);
	if( !pNewBits ) return FALSE;
	DWORD cb = sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
	LPBITMAPINFO pNewBMI = (LPBITMAPINFO)GlobalAllocPtr(GHND, cb);
	if( !pNewBMI )
	{
		GlobalFreePtr(pNewBits);
		pNewBits = NULL;
		return FALSE;
	}

	TmpDib.Free();

	long nBytePerLine = (pBMI->bmiHeader.biWidth*pBMI->bmiHeader.biBitCount + 31)/32*4;//图像每行所占实际字节数（须是4的倍数）
	int nThreshold = 127;

	//24位真彩
	if(pBMI->bmiHeader.biBitCount == 24)
	{
		RGBTRIPLE *pRGBData = NULL;
		byte ngrayscale	 = 0;
		for(int i = 0; i < pBMI->bmiHeader.biHeight; i++)
		{
			BYTE* pSrcTemp = (pData + i*nBytePerLine);
			BYTE* pDstTemp = pNewBits + i*nNewBytePerLine;
			BYTE LastCol = 0;  // 2013.06.12
			for(int j = 0; j < pBMI->bmiHeader.biWidth; j++)
			{
				pRGBData = (RGBTRIPLE *)(pSrcTemp) + j;
				ngrayscale = (byte)(0.299*pRGBData->rgbtRed 
					+ 0.587*pRGBData->rgbtGreen
					+ 0.114*pRGBData->rgbtBlue);
				

				*(pDstTemp + j) = ngrayscale;
				if( ngrayscale < nThreshold ) 
				{
					nXHistgram[j]++;
					nYHistgram[i]++;
					if( j < pBMI->bmiHeader.biWidth/2 ) nYPartHist[i]++;
				}
			}
		}
	}
	else if(pBMI->bmiHeader.biBitCount == 32)//32位真彩
	{
		RGBQUAD *pRGBData = NULL;
		byte ngrayscale	 = 0;
		for(int i=0; i < pBMI->bmiHeader.biHeight; i++)
		{
			BYTE* pSrcTemp = (pData + i*nBytePerLine);
			BYTE* pDstTemp = pNewBits + i*nNewBytePerLine;
			BYTE LastCol = 0;  // 2013.06.12
			for(int j=0; j < pBMI->bmiHeader.biWidth; j++)
			{
				pRGBData = (RGBQUAD *)(pSrcTemp) + j;
				ngrayscale = (byte)(0.299*pRGBData->rgbRed 
					+ 0.587*pRGBData->rgbGreen
					+ 0.114*pRGBData->rgbBlue);
				
				*(pDstTemp + j) = ngrayscale;

				if( ngrayscale < nThreshold ) 
				{
					nXHistgram[j]++;
					nYHistgram[i]++;
					if( j < pBMI->bmiHeader.biWidth/2 ) nYPartHist[i]++;
				}
			}
		}
	}
	
	memcpy(pNewBMI, pBMI, sizeof(BITMAPFILEHEADER));
	
	pNewBMI->bmiHeader.biBitCount = 8;
	pNewBMI->bmiHeader.biSizeImage = dwNewSize;
	for( int i =0;i < 256;i++) // 灰度调色板
	{
		pNewBMI->bmiColors[i].rgbBlue = i;
		pNewBMI->bmiColors[i].rgbGreen = i;
		pNewBMI->bmiColors[i].rgbRed = i;
	}
	
	TmpDib.SetDatas(pNewBMI, pNewBits);
//	TmpDib.SetXHistgram(nXHistgram);
//	TmpDib.SetYHistgram(nYHistgram);
//	TmpDib.SetYPartHist(nYPartHist);

	delete [] nXHistgram;
	delete [] nYHistgram;
	delete [] nYPartHist;

	if (pNewBits)
	{
		GlobalFreePtr(pNewBits);
		pNewBits = NULL;
	}
	if (pNewBMI)
	{
		GlobalFreePtr(pNewBMI);
		pNewBMI = NULL;
	}

	return TRUE;
}

/*
void CDib::SetXHistgram(int *nXHistgram)
{
	delete [] m_nXHistgram;
	m_nXHistgram = new int[GetWidth()];
	memcpy(m_nXHistgram, nXHistgram, GetWidth()*sizeof(int));
}

void CDib::SetYHistgram(int *nYHistgram)
{
	delete [] m_nYHistgram;
	m_nYHistgram = new int[GetHeight()];
	memcpy(m_nYHistgram, nYHistgram, GetHeight()*sizeof(int));
}

void CDib::SetYPartHist(int *nYPartHist)
{
	delete [] m_nYPartHist;
	m_nYPartHist = new int[GetHeight()];
	memcpy(m_nYPartHist, nYPartHist, GetHeight()*sizeof(int));
}

BOOL CDib::Keyword(CString& strKeyword, BOOL bEN)
{
	CString str = strKeyword;
	int n0 = 0;
	int k = 6;
	if( !bEN )
		n0 = strKeyword.Find("关键词");
	else
	{
		str.MakeLower();
		const char* key[] = { "keywords", "key words", "keyword", "key word", "\0" };
		for(int i = 0; i < 4; i++)
		{
			CString strFind = key[i];
			strFind = strFind.Left(strFind.GetLength() - 2);
			n0 = str.Find(strFind);
			if( n0 >= 0 )
			{
				k = strlen(key[i]);
				int n1 = str.Find(" ", n0 + k);
				if( (n1 - n0 - k) >= 0 && (n1 -n0 - k) < 3 )
				{
					k = n1 - n0;
				}
				else n1 = -1;
				break;
			}
		}
	}
	
	if( n0  < 0 ) return FALSE;
	
	str = strKeyword.Mid(n0 + k);
	int n1 = str.Find("\r");
	str = str.Left(n1);
	
	n1 = str.Find("]");
	if( n1 >= 0 && n1 < 4 ) 
		str = str.Mid(n1 + 1);
	else
	{
		n1 = str.Find(":");
		int n2 = str.Find("：");
		if( n1 >= 0 && n1 < 4 ) 
			str = str.Mid(n1 + 1);
		if( n2 >= 0 && n2 < 5 ) 
			str = str.Mid(n2 + 2);
	}
	str.TrimLeft();
	str.TrimRight();
	
	return TRUE;
}

BOOL CDib::Row(CTextRecognizeBase* pOCR, PAPER_LAYOUT& layout, LineProperty& lineproperty, BOOL bFirst, BOOL bSplit, int nThreshold)
{
	LPBITMAPINFO pBMI = GetBmpInfo();
	if( !pBMI || pBMI->bmiHeader.biBitCount != 8 )
		return FALSE;
	
	if( pBMI->bmiHeader.biBitCount != 8 )
		return FALSE;
	
	int nWidth = GetWidth();
	int nHeight = GetHeight();
	
	int i;
	CPoint p1, p2, p3;

#ifdef MY_TEST	
	FILE* f;
	errno_t err = fopen_s(&f, "e:\\012345.txt","wt");
	for(i = 0; i < nHeight; i++)
	{
		fprintf(f, "%d\n", m_nYHistgram[i]);
	}
	fclose(f);
	err = fopen_s(&f, "e:\\112345.txt","wt");
	for(i = 0; i < nHeight; i++)
	{
		fprintf(f, "%d\n", m_nYPartHist[i]);
	}
	fclose(f);
#endif

	int nNormalH = GetHeight()*2/(3*50);
	int nNormalD = GetHeight()*1/(3*50);
	CRect LinePos[100];
	int LineLan[100];
	CRect rect;
	int nLine = 6;
	if( !bFirst )
	{
		rect.bottom = nHeight - 11;
		rect.top = nHeight*2/3;
		rect.left = 0;
		rect.right = nWidth - 1;
	}
	else
	{
		nLine = 80; 
		rect.bottom = nHeight - 11;
		rect.top = int(nHeight*0.15);
		rect.left = 0;
		rect.right = nWidth - 1;
	}
	
	Line(m_nYHistgram, rect, 1, LinePos, nLine, 35);  // 反向  2017.07.17->35
	if( nLine > 0 ) 
	{
		memcpy(lineproperty.m_LinePos, LinePos, nLine*sizeof(CRect));
		lineproperty.m_nLineNum = nLine;
	}

	int nHeaderTop = 0;
	int nHeaderBottom = 0;
	int nTitleTop = 0;
	int nTitleBottom = 0;
	int nEnTitleTop = 0;
	int nEnTitleBottom = 0;
	int nFooterTop = 0;
	int nFooterBottom = 0;
	if( bFirst )
	{
		CDib TmpDib;
		int n = 0;
		CString strFile;
		strFile.Format("%s\\temp.bmp", theApp.GetWorkDirectory());
		_unlink(strFile);
		memset(LineLan, 0, 100*sizeof(int));
		
		BOOL bKeyword = FALSE;
		BOOL bENKeyword = FALSE;
		int nEN0 = -1;
		int nEN1 = -1;
		for(i = 0; i < nLine; i++)
		{
			Cut(LinePos[i].left - n, GetHeight() - 1 - LinePos[i].bottom - n, 
				LinePos[i].Width() + 2*n, LinePos[i].Height() + 2*n, TmpDib);
			TmpDib.Save(strFile);
			
			CString strText;
			if( pOCR->Do(strFile) )
			{
				strText = pOCR->GetText();
				strText.TrimLeft();
				BOOL bRet = IsEnglish(strText);
				if( bRet ) 
				{
					LineLan[i] = 1; // 英文
					if( nEN0 == -1 )
					{
						nEN0 = i;
					}
					else 
					{
						nEN1 = i;
					}

					if( !bENKeyword && i > 4 ) 
					{
						bENKeyword = Keyword(strText, TRUE);
						if( bENKeyword ) 
						{
							lineproperty.m_Style[i] = EN_KEYWORD;
						}
					}
				}
				else
				{
					if( nEN1 == -1 )
					{
						nEN0 = -1;
					}

					if( !bKeyword && i > 4 ) 
					{
						bKeyword = Keyword(strText, FALSE);
						if( bKeyword ) 
						{
							lineproperty.m_Style[i] = KEYWORD;
						}
					}
					if( !bKeyword && i > 4 )   // 2017.09.25
					{
						int d1 = LinePos[i - 1].top - LinePos[i].bottom;  // 上面行距
						int d2 = LinePos[i].top - LinePos[i + 1].bottom;  // 下面行距
						if( d1 > 1.2*nNormalD && d2 > 3* d1 )					
						{
							int n = strText.Find("关键"); 
							if( n >= 0  && n <= 2 ) bKeyword = TRUE;
						}
						if( bKeyword ) 
						{
							lineproperty.m_Style[i] = KEYWORD;
						}
					}
				}

				lineproperty.m_nLineLan[i] = LineLan[i];
			}
			else
			{
				_unlink(strFile);
				continue;
			}
			_unlink(strFile);
		}

		if( nEN0 >= 0 && nEN1 > nEN0 )
		{
			layout.ENTitle = LinePos[nEN0];
			layout.ENTitle.top = LinePos[nEN1].top;
			if( nEnTitleTop == 0 ) nEnTitleTop = nEN0;
			if( nEnTitleBottom == 0 ) nEnTitleBottom = nEN1;
		}

		if( bKeyword )
		{
			for(i = nLine - 3 ; i >= 1; i--)
			{
				int d1 = LinePos[i - 1].top - LinePos[i].bottom;  // 上面行距
				int h = LinePos[i].bottom - LinePos[i].top;       // 行高
				int d2 = LinePos[i].top - LinePos[i + 1].bottom;  // 下面行距
				
				if( (h < 10 && d1 > 2*nNormalD && d2 > 2*nNormalD) || 
					(h > 0.5*nNormalH && d2 > 3.5*nNormalD) || 
					(h > 0.5*nNormalH && d1 < 1.5* nNormalD && d2 > 2.5*nNormalD) ) // 标题区
				{
					layout.Title.top = LinePos[i].top - 3;
					if( nTitleBottom == 0 ) nTitleBottom = i;
					break;
				}
			}
		}
		else
		{
			layout.Title.top = LinePos[nLine - 1].top - 3;
			if( nTitleBottom == 0 ) nTitleBottom = nLine - 1;
		}
	}

	BOOL bHeader = FALSE;
	for(i = 0; i < nLine - 2; i++)
	{
		int d1 = 0;
		if( i > 0 ) d1 = LinePos[i - 1].top - LinePos[i].bottom;  // 上面行距
		int h = LinePos[i].bottom - LinePos[i].top;       // 行高
		int d2 = LinePos[i].top - LinePos[i + 1].bottom;  // 下面行距
		
		if( !bHeader  && h >= 10 && h < 15 )
		{
			int max = 0;
			for(int j = LinePos[i].top; j < LinePos[i].bottom; j++)
			{
				max = (m_nYHistgram[j] > max) ? m_nYHistgram[j] : max;
			}
			if( max > nWidth/2 ) h = 8;
		}
		if( !bHeader && ( h < 10 || (d2 > 3.3*nNormalD) ) )  // 页眉
		{
			layout.Header.top = LinePos[i].top;
			layout.Header.bottom = LinePos[0].bottom + 15;   // 2017.07.12
			nHeaderTop = 0;
			nHeaderBottom = i;
			if( h < 10 )
			{
				// 判断下一行
				d1 = d2;  // 上面行距
				h = LinePos[i + 1].bottom - LinePos[i + 1].top;       // 行高
				d2 = LinePos[i + 1].top - LinePos[i + 2].bottom;     // 下面行距
				if( h < 10 ) continue;
				if( d1 < 0.5* nNormalD || (h < 1.2*nNormalH && d1 < 1.5* nNormalD && d2 > 2.8 * nNormalD) )
				{
					layout.Header.top = LinePos[i+1].top - 3;
					nHeaderBottom = i + 1;
				}
			}
			
			bHeader = TRUE;
			break;
		}
	}
	if( bFirst )
	{
		if( layout.Title.top > 0 )
		{
			if( bHeader )
			{
				layout.Title.bottom = layout.Header.top - 5;
				nTitleTop = nHeaderBottom + 1;
			}
			else
			{
				layout.Title.bottom = LinePos[0].bottom;
				nTitleTop = 0;
			}
		}
	}

	nLine = 12;
	rect.bottom = nHeight - 11;
	rect.top = 10;

	if( bSplit && !bFirst ) 
	{
		nLine = 80;
	}

	Line(m_nYPartHist, rect, 0, LinePos, nLine, int(nWidth/2*0.01));  // 正向, 2018.04.17
	
	// 页脚部分
	BOOL bFooter = FALSE;
	for(i = 1 ; i < nLine - 1 ; i++ )
	{
		int d1 = LinePos[i + 1].top - LinePos[i].bottom;  // 上面行距
		int h = LinePos[i].bottom - LinePos[i].top;       // 行高
		int d2 = LinePos[i].top - LinePos[i - 1].bottom;  // 下面行距
		int prev_h, prev_d1;

		if( bFirst )
		{
			if( h < 10 && i < nLine - 2 ) // 一条线
			{
				for(int k = LinePos[i].top; k <= LinePos[i].bottom; k++)
				{
					if( m_nYPartHist[k] > 0.1*nWidth )
					{
						layout.Footer.bottom = LinePos[i].bottom;
						layout.Footer.top = LinePos[0].top - 3;
						break;
					}
				}
				break;
			}
			if( !bFooter && d2 > 2*d1 && d1 > nNormalD && h > 0.5*nNormalH ) // 宽距离
			{
				layout.Footer.bottom = LinePos[i].top - 10;
				layout.Footer.top = LinePos[0].top - 3;
				bFooter = TRUE;
			}
		}
		else
		{
			if( h < 10 && i < nLine - 2 ) // 一条线
			{
				for(int k = LinePos[i].top; k <= LinePos[i].bottom; k++)
				{
					if( m_nYPartHist[k] > 0.1*nWidth )
					{
						layout.Footer.bottom = LinePos[i].bottom;
						layout.Footer.top = LinePos[0].top - 3;
						break;
					}
				}
				break;
			}
			if( d2 > 2*d1 && d1 > 0.3*nNormalD && d1 < 2* nNormalD && h > 0.5*nNormalH ) // 宽距离
			{
				layout.Footer.bottom = LinePos[i].top - 10;
				layout.Footer.top = LinePos[0].top - 3;

				if( layout.Footer.bottom > nHeight*0.12 ) 
				{
					layout.Footer.bottom = layout.Footer.top;
					layout.Footer.top = 0;
				}
				break;
			}

			if( i > 1 && abs(h-prev_h) < 8 && abs(d1-prev_d1) < 8 ) 
			{
				break;
			}

			prev_d1 = d1;
			prev_h = h;

			if( i > 10 ) break;
		}
	}

	if( !bFirst && nLine > 10 )
	{
		int mid = nLine/2; 
		for(i = 0; i < mid - 2; i++)
		{
			int d1 = LinePos[mid + i + 1].top - LinePos[mid + i].bottom;  // 上面行距
			int h = LinePos[mid + i].bottom - LinePos[mid + i].top;       // 行高
			int d2 = LinePos[mid + i].top - LinePos[mid + i - 1].bottom;  // 下面行距
			if( h < nNormalH/2 && d1 > nNormalD && d2 > nNormalD )
			{
				layout.nSplit = (LinePos[mid + i].bottom + LinePos[mid + i].top)/2;
				break;
			}

			d1 = LinePos[mid - i + 1].top - LinePos[mid - i].bottom;  // 上面行距
			h = LinePos[mid - i].bottom - LinePos[mid - i].top;       // 行高
			d2 = LinePos[mid - i].top - LinePos[mid - i - 1].bottom;  // 下面行距
			if( h < nNormalH/2 && d1 > nNormalD && d2 > nNormalD )
			{
				layout.nSplit = (LinePos[mid - i].bottom + LinePos[mid - i].top)/2;
				break;
			}
		}
	}

	if( layout.nSplit > 0 )
	{
		int nLine = 12;
		rect.bottom = layout.nSplit + int(nHeight*0.2);
		rect.top = layout.nSplit;
		BOOL bFooter = FALSE;
		
		Line(m_nYPartHist, rect, 0, LinePos, nLine, int(nWidth*0.01));  // 正向

		for(i = 1 ; i < nLine - 1 ; i++ )
		{
			int d1 = LinePos[i + 1].top - LinePos[i].bottom;  // 上面行距
			int h = LinePos[i].bottom - LinePos[i].top;       // 行高
			int d2 = LinePos[i].top - LinePos[i - 1].bottom;  // 下面行距

			if( h < nNormalH/2 && i < nLine - 2 ) // 一条线
			{
				for(int k = LinePos[i].top; k <= LinePos[i].bottom; k++)
				{
					if( m_nYPartHist[k] > 0.1*nWidth )
					{
						layout.Footer.bottom = LinePos[i].bottom;
						layout.Footer.top = LinePos[0].top - 3;
						bFooter = TRUE;
						break;
					}
				}
				break;
			}
//			if( d2 > 2*d1 && d1 > 0.3*nNormalD && d1 < 2* nNormalD && h > 0.5*nNormalH ) // 宽距离
//			{
//				layout.Footer.bottom = LinePos[i].top;
//				layout.Footer.top = LinePos[0].top - 3;
//				bFooter = TRUE;
//				break;
//			}

		}

		if( !bFooter )
		{
			nLine = 12;
			Line(m_nYHistgram, rect, 0, LinePos, nLine);  // 正向
			
			for(i = 1 ; i < nLine - 1 ; i++ )
			{
				int d1 = LinePos[i + 1].top - LinePos[i].bottom;  // 上面行距
				int h = LinePos[i].bottom - LinePos[i].top;       // 行高
				int d2 = LinePos[i].top - LinePos[i - 1].bottom;  // 下面行距
				
//				if( d2 > 2*d1 && d1 > 0.3*nNormalD && d1 < 2* nNormalD && h > 0.5*nNormalH ) // 宽距离
//				{
//					layout.Footer.bottom = LinePos[i].top;
//					layout.Footer.top = LinePos[0].top - 3;
//					bFooter = TRUE;
//					break;
//				}

			}
		}

		if( bFooter )
		{
			layout.nSplit = 0;
			layout.LeftCol[0].top = layout.Footer.bottom;
			layout.RightCol[0].top = layout.Footer.bottom;
		}
	}

	if( bSplit && ( layout.nSplit == 0 || !bFooter) )   // 2017.07.13, 英文标题可能在论文最后
	{
		for(i = nLine - 2 ; i >= 1 ; i-- )
		{
			int d1 = LinePos[i + 1].top - LinePos[i].bottom;  // 行距
			int d2 = LinePos[i].top - LinePos[i - 1].bottom;  // 行距
			if( d1 > 0.03*nHeight && d2 > 1.1*nNormalD && IsEnglish(pOCR, LinePos[i]) )
			{
				if( i > 5 )
				{
					layout.LeftCol[0].top = LinePos[i + 1].top - 5;
					layout.RightCol[0].top = LinePos[i + 1].top - 5;
					if( layout.Header.top > 0 )
					{
						layout.LeftCol[0].bottom = layout.Header.top - 5;
						layout.RightCol[0].bottom = layout.Header.top - 5;
					}

					layout.ENTitle = layout.Header;
					layout.ENTitle.bottom = layout.LeftCol[0].top - 15;
					layout.ENTitle.top = layout.Footer.bottom + 5;

					CRect rcTmp = layout.ENTitle;					
					int nWidth = rcTmp.Width();
					int* nXHistgram = new int[nWidth];
					if( CDib::CalculateXHistgram(*this, rcTmp, nXHistgram) )
					{
						int count = 0;
						int n0 = int(0.4*nWidth);
						int n1 = int(0.6*nWidth);
						for(int k = n0; k <= n1; k++)
						{
							if( nXHistgram[k] < 2 )
							{
								count++;
							}
							else if( count > 0 ) break;
						}
						if( count > 50 ) 
						{
							layout.LeftCol[0].top = layout.ENTitle.top;
							layout.RightCol[0].top = layout.ENTitle.top;
							layout.ENTitle.top = layout.ENTitle.bottom = 0;
						}
					}
#ifdef MY_TEST	
FILE* f ;
errno_t err = fopen_s(&f, "e:\\012345.txt","wt");
for(i = 0; i < nWidth; i++)
{
	fprintf(f, "%d\n", nXHistgram[i]);
}
fclose(f);
#endif

					delete [] nXHistgram;
				}
				break;
			}
		}
		if( layout.ENTitle.Height() == 0 )
		{
			if( bFooter )
			{
				layout.LeftCol[0].top = layout.Footer.bottom + 5;
				layout.RightCol[0].top = layout.Footer.bottom + 5;
			}
			else
			{
				layout.LeftCol[0].top = 5;
				layout.RightCol[0].top = 5;
			}
		}
	}

	if( bFirst )
	{
		if( layout.Title.top > 0 )
		{
			if( bFooter )
			{
				layout.Title.top = layout.Footer.bottom + 3;

				for(i = lineproperty.m_nLineNum - 1; i > nTitleTop; i--)
				{
					if( i < lineproperty.m_nLineNum - 1 )
					{
						if( lineproperty.m_LinePos[i].top >= layout.Title.top && lineproperty.m_LinePos[i + 1].bottom < layout.Title.top )
						{
							nTitleBottom = i;
						}
					}
					else
					{
						if( lineproperty.m_LinePos[i].top >= layout.Title.top )
						{
							nTitleBottom = i;
						}
					}
				}
			} 
			for(i = nHeaderTop; i <= nHeaderBottom; i++)
			{
				if( lineproperty.m_Style[i] == 0 ) lineproperty.m_Style[i] = PAGE_HEADER;
			}
			for(i = nTitleTop; i <= nTitleBottom; i++)
			{
				if(i >= nEnTitleTop && i <= nEnTitleBottom)
				{
					if( lineproperty.m_Style[i] == 0 ) lineproperty.m_Style[i] = EN_TITLE;
					continue;
				}
				if( lineproperty.m_Style[i] == 0 ) lineproperty.m_Style[i] = TITLE;
			}
		}
		else if( layout.Title.top == 0 )
		{
			CRect rc = layout.Header;
			rc.top = layout.Footer.bottom;
			rc.bottom = layout.Header.top;
			int nHeight = rc.Height();
			int nPart = nHeight/10;

			CRect rcTmp;
			rcTmp.left = int((rc.left + rc.right)/2 - rc.Width()*0.1);
			rcTmp.right = int((rc.left + rc.right)/2 + rc.Width()*0.1);
			int nWidth = rcTmp.Width();
			int* nXHistgram = new int[nWidth];

			int x0 = 0;
			int x1 = 0;
			int len = 0;
			for(i = int(nPart*0.1) ; i < int(nPart*0.8); i++)
			{
				rcTmp.top = rc.top + i*10;
				rcTmp.bottom = rcTmp.top + 10;
				if( CDib::CalculateXHistgram(*this, rcTmp, nXHistgram) )
				{
					BOOL bTitle = FALSE;
					if( x0 >0 && x1 > x0 && len > 10 )
					{
						for(int j = x0 + 4; j <= x1 - 4; j++)
						{
							if( nXHistgram[j] > 0 ) 
							{
								bTitle = TRUE;
								break;
							}

						}
						if( !bTitle )
						{
							int j = 0;
							BOOL bLeft = FALSE;
							for(j = 0; j <= x0 - 4; j++)
							{
								if( nXHistgram[j] > 0 ) 
								{
									bLeft = TRUE;
									break;
								}
							}
							if( bLeft )
							{
								for(j = x0; j > x0 - 100; j--)
								{
									if( nXHistgram[j] > 0 ) 
									{
										bLeft = FALSE;
										break;
									}
								}
							}
							if( bLeft )
							{
								BOOL bRight = FALSE;
								for(j = x1; j < nWidth - 1; j++)
								{
									if( nXHistgram[j] > 0 ) 
									{
										bRight = TRUE;
										break;
									}
								}
								if( !bRight ) bTitle = TRUE;

							}

						}
					}
					if( bTitle )
					{
						layout.Title = layout.Header;
						layout.Title.top = rcTmp.top - 15;
						layout.Title.bottom = layout.Header.top - 1;
						break;
					}

					if( x0 == 0 && x1 == 0 && len == 0 )
					{
						int j;
						for(j = nWidth/2 + 1; j < nWidth - 1; j++)
						{
							if( nXHistgram[j] > 0 ) break;
							x1 = j;
							len++;
						}
						for(j = nWidth/2 ; j > 0 ; j--)
						{
							if( nXHistgram[j] > 0 ) break;
							x0 = j;
							len++;
						}
					}


					int kkk = 0;

				}
			}
		}
	}
	
	return TRUE;
}

BOOL CDib::Row(int* nRow, int nThreshold)
{
	LPBITMAPINFO pBMI = GetBmpInfo();
	if( !pBMI || pBMI->bmiHeader.biBitCount != 8 )
		return FALSE;

	if( pBMI->bmiHeader.biBitCount != 8 )
		return FALSE;

	int nWidth = GetWidth();
	int nHeight = GetHeight();

	int i;
	CPoint p1, p2, p3;

#ifdef MY_TEST
	FILE* f ;
	errno_t err = fopen_s(&f, "e:\\012345.txt","wt");
	for(i = 0; i < nHeight; i++)
	{
		fprintf(f, "%d\n", m_nYHistgram[i]);
	}
	fclose(f);
#endif

	for(i = nHeight - 21; i > nHeight*2/3; i--)
	{
		p1.x = i+2;
		p1.y = m_nYHistgram[i+2];
		p2.x = i+1;
		p2.y = m_nYHistgram[i+1];
		p3.x = i;
		p3.y = m_nYHistgram[i];
		double s = inflexion(p1, p2, p3);
		
		if( s < 0 && m_nYHistgram[i] > nWidth*0.03 )
		{
			if( i + 10 < nHeight - 1 ) nRow[1] = i + 10;
			else nRow[1] = nHeight - 1;
			break;
		}
	}
	
	for(i = 20; i < nHeight/3; i++)
	{
		p1.x = i-2;
		p1.y = m_nYHistgram[i-2];
		p2.x = i-1;
		p2.y = m_nYHistgram[i-1];
		p3.x = i;
		p3.y = m_nYHistgram[i];
		double s = inflexion(p1, p2, p3);

		if( s < 0 && m_nYHistgram[i+1] > nWidth*0.01  )
		{
			nRow[0] = i - 10;
			break;
		}
	}


	return TRUE;
}

bool CDib::judge(int* nHistgram, int s, bool inc, int nThreshold)
{
	CPoint p[3];
	int pos;
	int j = 0;
	int i = s;
	if( inc )
	{
		for(; i <= s + 2; i++, j++)
		{
			p[j].x = i;
			p[j].y = nHistgram[i];
		}
		pos = s + 2;
	}
	else
	{
		for(; i >= s - 2; i--, j++)
		{
			p[j].x = -i;
			p[j].y = nHistgram[i];
		}
		pos = s - 2;
	}
	double val = inflexion(p[0], p[1], p[2]);
	
	if( val < 0 && nHistgram[pos] > nThreshold )
	{
		return true;
	}
	
	return false;
}

BOOL CDib::LRMargin(PAPER_LAYOUT& layout)
{
	LPBITMAPINFO pBMI = GetBmpInfo();
	if( !pBMI || pBMI->bmiHeader.biBitCount != 8 )
		return FALSE;
	
	if( pBMI->bmiHeader.biBitCount != 8 )
		return FALSE;
	
	LPBYTE pData = GetBits();
	long nBytePerLine = GetBytePerLine(pBMI->bmiHeader.biWidth,8);
	int nWidth = GetWidth();
	int nHeight = GetHeight();
	int *nHistgram = GetXHistgram();
	
	int i;
	int nColThreshold = 40;
	for(i = 7; i < nWidth/3; i++)
	{
		if( judge(nHistgram, i-2, true, nColThreshold) )
		{
			layout.Header.left = i - 5;
			layout.Footer.left = i - 5;
			layout.Title.left = i - 5;
			layout.LeftCol[0].left = i - 5;
			break;
		}
	}
	for(i = nWidth - 8; i > nWidth*2/3; i--)
	{
		if( judge(nHistgram, i+2, false, nColThreshold) )
		{
			layout.Header.right = i + 4;
			layout.Footer.right = i + 4;
			layout.Title.right = i + 4;
			layout.RightCol[0].right = i + 5;
			break;
		}
	}

	return TRUE;
}

BOOL CDib::Column(int* nColumn, int y0, int y1, int nThreshold )
{
	LPBITMAPINFO pBMI = GetBmpInfo();
	if( !pBMI || pBMI->bmiHeader.biBitCount != 8 )
		return FALSE;
	
	if( pBMI->bmiHeader.biBitCount != 8 )
		return FALSE;
	
	LPBYTE pData = GetBits();
	long nBytePerLine = GetBytePerLine(pBMI->bmiHeader.biWidth,8);
	int nWidth = GetWidth();
	int nHeight = GetHeight();
	int *nHistgram = new int[nWidth];
	memset(nHistgram, 0, nWidth*sizeof(int));
	nColumn[0] = nColumn[1] = nColumn[2] = nColumn[3] = 0;

	int i;

	int mid = nWidth/2;
	for(int j = y0; j < y1; j++)
	{
		if( pData[j*nBytePerLine + mid] < nThreshold ) nHistgram[mid]++;					
	}
	
	int nColThreshold = 25;
	if( nHistgram[mid] > nColThreshold )
	{
		for(i = mid; i < mid + nWidth*1/6; i++)
		{
			for(int j = y0; j < y1; j++)
			{
				if( pData[j*nBytePerLine + i] < nThreshold ) nHistgram[i]++;					
			}
			if( i - mid < 3 ) continue;
			
			if( nHistgram[i] < 2 )
			{
				if( judge(nHistgram, i-2, false, nColThreshold) )
				{
					nColumn[1] = i + 4;
					i++;
				}
			}
			if( nColumn[1] > 0 && i - nColumn[1] > 3 )
			{
				if( judge(nHistgram, i-2, true, nColThreshold) )
				{
					nColumn[2] = i - 5;
					break;
				}
			}
		}
	}
	else
	{
		for(i = mid; i > mid - nWidth*1/6 ; i--)
		{
			for(int j = y0; j < y1; j++)
			{
				if( pData[j*nBytePerLine + i] < nThreshold ) nHistgram[i]++;					
			}
			if( mid - i < 3 ) continue;
			
			if( judge(nHistgram, i + 2, false, nColThreshold) )
			{
				nColumn[1] = i + 4;
				break;
			}
		}
		for(i = mid + 1; i < mid + nWidth*1/6; i++)
		{
			for(int j = y0; j < y1; j++)
			{
				if( pData[j*nBytePerLine + i] < nThreshold ) nHistgram[i]++;					
			}
			
			if( i - mid < 3 ) continue;
			
			if( judge(nHistgram, i-2, true, nColThreshold) )
			{
				nColumn[2] = i - 5;
				break;
			}
		}
	}

	return TRUE;
}

BOOL CDib::Column(PAPER_LAYOUT& layout, int nThreshold)
{
	LPBITMAPINFO pBMI = GetBmpInfo();
	if( !pBMI || pBMI->bmiHeader.biBitCount != 8 )
		return FALSE;
	
	if( pBMI->bmiHeader.biBitCount != 8 )
		return FALSE;
	
	int nHeight = GetHeight();
	int nColumn[4];
	nColumn[0] = nColumn[1] = nColumn[2] = nColumn[3] = 0;

	int k = 10;
	int n = nHeight - 10 - k;

	if( layout.Footer.bottom > 0 )
	{
		k = layout.Footer.bottom;
		n = nHeight - 11 - k;
	}
	if( layout.Footer.top == 0 && layout.Footer.bottom == 0 && layout.LeftCol[0].top > 0 )
	{
		k = layout.LeftCol[0].top;
	}
	if( layout.Title.top == 0 && layout.Header.top > 0 )
	{
		n = layout.Header.top - k;
	}
	if( layout.Title.top > 0 )
	{
		n = layout.Title.top - k;
	}
	
	if( !Column(nColumn, k, k + n, nThreshold) )
	{
		return FALSE;
	}

	layout.LeftCol[0].top = layout.RightCol[0].top = k;
	layout.LeftCol[0].bottom = layout.RightCol[0].bottom = k + n;

	if( layout.nSplit > 0 )
	{
		layout.LeftCol[0].top = layout.RightCol[0].top = layout.nSplit;
	}

	if( nColumn[1] > 0 && nColumn[2] > nColumn[1] )
	{
		layout.LeftCol[0].right = nColumn[1];
		layout.RightCol[0].left = nColumn[2];
	}
	else
	{
		layout.LeftCol[0].right = layout.RightCol[0].right;
		layout.RightCol[0].left = layout.RightCol[0].right = 0;
	}

	return TRUE;
}

BOOL CDib::Column(int* nColumn, int* nRow, int nThreshold, BOOL bMix)
{
	LPBITMAPINFO pBMI = GetBmpInfo();
	if( !pBMI || pBMI->bmiHeader.biBitCount != 8 )
		return FALSE;

	if( pBMI->bmiHeader.biBitCount != 8 )
		return FALSE;

	LPBYTE pData = GetBits();
	long nBytePerLine = GetBytePerLine(pBMI->bmiHeader.biWidth,8);
	int nWidth = GetWidth();
	int nHeight = GetHeight();
	int *nHistgram = new int[nWidth];
	memset(nHistgram, 0, nWidth*sizeof(int));
	nColumn[0] = nColumn[1] = nColumn[2] = nColumn[3] = 0;

	int n, k;
	int m = nRow[1] - nRow[0];
	if( nRow[0] > 0 )
	{
		n = nRow[1] - nRow[0]; 
		n = ( n < m ) ? n : m;
		k = nRow[0];
	}
	else
	{
		k = 0;
		n = nHeight > m ? m : nHeight;
		if( n == 0 ) n = nHeight;
	}

	int nColThreshold = 10;
	int i;
	CPoint p1, p2, p3;

	for(i = 5; i < nWidth/3; i++)
	{
		for(int j = k; j < k + n; j++)
		{
			if( pData[j*nBytePerLine + i] < nThreshold ) 
			{
				nHistgram[i]++;					
			}
		}
		if( i < 7 ) continue;

		if( judge(nHistgram, i-2, true, nColThreshold) )
		{
			nColumn[0] = i - 5;
			break;
		}
	}
	for(i = nWidth - 6; i > nWidth*2/3; i--)
	{
		for(int j = k; j < k + n; j++)
		{
			if( pData[j*nBytePerLine + i] < nThreshold ) 
			{
				nHistgram[i]++;					
			}
		}
		if( i > nWidth - 8 ) continue;

		if( judge(nHistgram, i+2, false, nColThreshold) )
		{
			nColumn[3] = i + 4;
			break;
		}
	}

	if( bMix )
	{
		n = int((nRow[1] - nRow[0]) * 0.2); 
		n = ( n > 300 ) ? n : 300;
		k = int((nRow[1] - nRow[0]) * 0.1);
		k = ( n < 300 ) ? n : 300;
		k += nRow[0];

	}
	else
	{
		n = int((nRow[1] - nRow[0]) * 0.3); 
		n = ( n > 500 ) ? n : 500;
		k = int((nRow[1] - nRow[0]) * 0.05);
		k = ( n < 150 ) ? n : 150;
		k = nRow[1] - n - k;
	}

	int mid = nWidth/2;
	for(int j = k; j < k + n; j++)
	{
		if( pData[j*nBytePerLine + mid] < nThreshold ) nHistgram[mid]++;					
	}

	nColThreshold = 25;
	if( nHistgram[mid] > nColThreshold )
	{
		for(i = mid; i < mid + nWidth*1/6; i++)
		{
			for(int j = k; j < k + n; j++)
			{
				if( pData[j*nBytePerLine + i] < nThreshold ) nHistgram[i]++;					
			}
			if( i - mid < 3 ) continue;

			if( nHistgram[i] < 2 )
			{
				if( judge(nHistgram, i-2, false, nColThreshold) )
				{
					nColumn[1] = i + 4;
					i++;
				}
			}
			if( nColumn[1] > 0 && i - nColumn[1] > 3 )
			{
				if( judge(nHistgram, i-2, true, nColThreshold) )
				{
					nColumn[2] = i - 5;
					break;
				}
			}
		}
	}
	else
	{
		for(i = mid; i > mid - nWidth*1/6 ; i--)
		{
			for(int j = k; j < k + n; j++)
			{
				if( pData[j*nBytePerLine + i] < nThreshold ) nHistgram[i]++;					
			}
			if( mid - i < 3 ) continue;

			if( judge(nHistgram, i + 2, false, nColThreshold) )
			{
				nColumn[1] = i + 4;
				break;
			}
		}
		for(i = mid + 1; i < mid + nWidth*1/6; i++)
		{
			for(int j = k; j < k + n; j++)
			{
				if( pData[j*nBytePerLine + i] < nThreshold ) nHistgram[i]++;					
			}

			if( i - mid < 3 ) continue;
			
			if( judge(nHistgram, i-2, true, nColThreshold) )
			{
				nColumn[2] = i - 5;
				break;
			}
		}
	}

	return TRUE;
}

BOOL CDib::IsEnglish(const CString& strText)    // 2019.09.20
{
	CString str = strText;
	str.Replace(" ", "");
	str.Replace("\"", "");
	str.Replace("-", "");
	str.Replace("<", "");  // 2017.11.24
	int len = str.GetLength();
	if( len <= 0 ) return FALSE;
	char* pBuf = new char[len + 1];
	char*s0 = new char[len + 1];
	char*s1 = new char[len + 1];
	BOOL bRet = FALSE;
	memset(pBuf, 0, (len + 1)*sizeof(char));
	memset(s0, 0, (len + 1)*sizeof(char));
	memset(s1, 0, (len + 1)*sizeof(char));

	strncpy_s(pBuf, len + 1, str, len);

	sscanf_s(pBuf,"%[a-zA-Z:;,./()0-9']",s0, len);
	if( strlen(s0) <= 0 )
	{
		sscanf_s(pBuf,"%[^a-zA-Z:;,./()0-9']%[a-zA-Z:;,.()/0-9']",s1,len, s0, len);
		if( strlen(s0) <= 0 ) 
		{
			delete [] pBuf;
			delete [] s0;
			delete [] s1;
			return FALSE;
		}
	}

	str = s0;
	for(int i= 0; i < 10; i++)
	{
		char sz[2];
		sz[1] = 0;
		_itoa_s(i, sz, 2, 10);
		str.Replace(sz, "");
	}
	if( str.GetLength() > 50 || str.GetLength() > int(0.5*len) ) 
	{
		bRet = TRUE;
	}

	delete [] pBuf;
	delete [] s0;
	delete [] s1;
	return bRet;
}

BOOL CDib::IsEnglish(CTextRecognizeBase* pOCR, CRect rect)
{
	if( !pOCR || !pOCR->IsInitialized() ) return FALSE;

	CDib TmpDib;
	int n = 10;
	int x = (rect.left - n >= 0 ) ? rect.left - n : 0;
	int y = (GetHeight() - 1 - rect.bottom - n >= 0) ? GetHeight() - 1 - rect.bottom - n : 0;
	int nWidth = ( rect.Width() + 2*n >= int(GetWidth()) ) ? GetWidth() - 1 : rect.Width() + 2*n;
	Cut(rect.left - n, GetHeight() - 1 - rect.bottom - n, nWidth, rect.Height() + 2*n, TmpDib);
	CString str;
	str.Format("%s\\temp.bmp", theApp.GetWorkDirectory());
	_unlink(str);
	TmpDib.Save(str);

	BOOL bEN = FALSE;
	if( pOCR->Do(str) )
	{
		CString strText = pOCR->GetText();
		strText.TrimLeft();

		bEN = IsEnglish(strText);

	}

	_unlink(str);
	return bEN;
}

BOOL CDib::Reference(CTextRecognizeBase* pOCR, CRect* LinePos, int nLine, CString& str)  // 
{
	if( !pOCR || !pOCR->IsInitialized() ) return FALSE;

	str = "";
	CStringList strLines;
	try
	{
		for(int i = nLine - 1; i >= 0 ; i-- )
		{
			CDib TmpDib;
			int n = 2;
			Cut(LinePos[i].left - n, GetHeight() - 1 - LinePos[i].bottom - n, LinePos[i].Width() + 2*n, LinePos[i].Height() + 2*n, TmpDib);
			CString str;
			str.Format("%s\\temp.bmp", theApp.GetWorkDirectory());
			_unlink(str);
			TmpDib.Save(str);
			
			if( pOCR->Do(str) )
			{
				CString strText = pOCR->GetText();
				strText.TrimLeft();
				CString str = strText;
				str.MakeLower();
				if( strText.Find("参考文献") >= 0 || str.Find("reference") >= 0 )
				{
					break;
				}
				
				strLines.AddHead(strText);
			}
		}

		while( !strLines.IsEmpty() )
		{
			CString s = strLines.RemoveHead();
			str += s;
		}
	}
	catch(...)
	{
	}

	return TRUE;
}

BOOL CDib::Reference(CTextRecognizeBase* pOCR, CRect* LinePos, int nLine, CStringList& strList, int& nReferLine, BOOL bReference)  // 
{
	if( !pOCR || !pOCR->IsInitialized() ) return FALSE;
	
	strList.RemoveAll();
	try
	{
		for(int i = nLine - 1; i >= 0 ; i-- )
		{
			CDib TmpDib;
			int n = 2;
			Cut(LinePos[i].left - n, GetHeight() - 1 - LinePos[i].bottom - n, LinePos[i].Width() + 2*n, LinePos[i].Height() + 2*n, TmpDib);

#ifdef MY_TEST
int *X = TmpDib.GetXHistgram();
FILE* f;
errno_t err = fopen_s(&f, "e:\\412345.txt","wt");
if( f && X )
{
	for(int ii = 0; ii < LinePos[i].Width(); ii++)
	{
		fprintf(f, "%d\n", X[ii]);
	}
	fclose(f);
}

#endif
			CString str;
			str.Format("%s\\temp.bmp", theApp.GetWorkDirectory());
			_unlink(str);
			TmpDib.Save(str);
			
			if( pOCR->Do(str) )
			{
				CString strText = pOCR->GetText();
				strText.TrimLeft();
				int n = strText.Find("\r");
				if( n > 0 ) strText = strText.Left(n);
				strText.TrimRight();
				CString str = strText;
				str.MakeLower();
				CString strCN = strText.Left(10);
				if( !bReference && strText.GetLength() <= 24 && (strCN.Find("参考文献") >= 0 || str.Find("reference") >= 0) )
				{
					bReference = TRUE;
					nReferLine = i;
					break;
				}
				if( !bReference && strText.GetLength() <= 24 && strCN.Find("参考") >= 0 )
				{
					bReference = TRUE;
					nReferLine = i;
					break;
				}
				
				strList.AddHead(strText);
			}
		}	
		
		if( !bReference ) strList.RemoveAll();
	}
	catch(...)
	{
	}
	
	return TRUE;
}

BOOL CDib::Reference(CTextRecognizeBase* pOCR, CRect* LinePos, int nLine, CReferLineList& referList, int& nReferLine, BOOL bReference)  // 通过缩进分开每个文献
{
	if( !pOCR || !pOCR->IsInitialized() ) return FALSE;
	
	try
	{
		CReferenceLine* pLine = NULL;
		int nMax = 100;
		for(int i = 0; i < nLine ; i++ )
		{
			CDib TmpDib;
			int n = 2;
			if( LinePos[i].Height() < 20 )
			{
				int mid = (LinePos[i].top + LinePos[i].bottom)/2;
				LinePos[i].top = (mid - 10) > 0 ? mid - 10 : 0;  // 2018.07.19
				LinePos[i].bottom = mid + 14;
			}
			Cut(LinePos[i].left - n, GetHeight() - 1 - LinePos[i].bottom - n, LinePos[i].Width() + 2*n, LinePos[i].Height() + 2*n, TmpDib);

			if( bReference )
			{
				int *X = TmpDib.GetXHistgram();
				int k;
				for(k = 0; k < 150; k++)
				{
					if( X[k] > 0 ) break;
				}
				if( nMax < k ) nMax = k;
				if( k < nMax * 0.3 )
				{
					if( pLine ) referList.AddTail(pLine);
					pLine = new CReferenceLine;
				}
#ifdef MY_TEST
TmpDib.Save("e:\\8888.bmp");
FILE* f;
errno_t err = fopen_s(&f, "e:\\412345.txt","wt");
for(int ii = 0; ii < LinePos[i].Width(); ii++)
{
	fprintf(f, "%d\n", X[ii]);
}
fclose(f);
#endif
			}
			

			CString str;
			str.Format("%s\\temp.bmp", theApp.GetWorkDirectory());
			_unlink(str);
			TmpDib.Save(str);
			
			if( pOCR->Do(str) )
			{
				CString strText = pOCR->GetText();
				strText.Replace(" ","");
				int n = strText.Find("\r");
				if( n > 0 ) strText = strText.Left(n);
				CString str = strText;
				str.MakeLower();
				CString strCN = strText.Left(10);
				if( !bReference && strText.GetLength() <= 24 && (strCN.Find("参考文献") >= 0 || str.Find("reference") >= 0) )
				{
					bReference = TRUE;
					nReferLine = i;
				}
				if( !bReference && strText.GetLength() <= 24 && strCN.Find("参考") >= 0 )
				{
					bReference = TRUE;
					nReferLine = i;
				}
				if( strText.GetLength() <= 0 ) 
				{
					if( pLine ) referList.AddTail(pLine);
					break;
				}
				
				if( pLine )
				{
					pLine->m_strLine.AddTail(strText);
				}
			}
			else
			{
				delete pLine;
				pLine = NULL;
			}

			if( i == nLine - 1 )
			{
				if( pLine ) referList.AddTail(pLine);
			}
		}		
	}
	catch(...)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CDib::ENLayout(CTextRecognizeBase* pOCR, CRect* LinePos, STYLE* style, int nLine, PAPER_LAYOUT paperlayout, LAYOUT* layout, int& num) // 2017.09.29
{
	int nNormalH = GetHeight()*2/(3*50);
	int nNormalD = GetHeight()*1/(3*50);

	// 英文部分
	int i = 0;
	int d1, h, d2;
	int k = num;
	int titleLine = 1;
	int nStartLine = 0;  // 2017.11.26

	while( 1 )
	{
		if( style[i] == EN_TITLE || IsEnglish(pOCR, LinePos[i]) ) // 2017.09.29
		{
			if( i > 0 ) d1 = LinePos[i - 1].top - LinePos[i].bottom;  // 上面行距
			else d1 = int(1.5*nNormalD);
			h = LinePos[i].bottom - LinePos[i].top;       // 行高
			d2 = LinePos[i].top - LinePos[i + 1].bottom;  // 下面行距
			if( d1 > 2* nNormalD ) nStartLine = i;  // 2017.11.26
			if( d1 > 1.0*nNormalD && h > 0.8*nNormalH && d2 > 1.4* nNormalD )  // 2017.09.29
				break;
		}
		i++;
		if( i >= nLine - 1 ) return FALSE;
	}
	titleLine = i + 1;

	layout[k].rect = LinePos[nStartLine];  // 2017.09.29  // 2017.11.26
	layout[k].rect.top = LinePos[i].top;
	layout[k].style = EN_TITLE;
	i++;

	d1 = LinePos[i - 1].top - LinePos[i].bottom;  // 上面行距
	h = LinePos[i].bottom - LinePos[i].top;       // 行高
	d2 = LinePos[i].top - LinePos[i + 1].bottom;  // 下面行距
	if( h > 0.8 * nNormalH && d1 < 1.2 * nNormalD && d2 > 2* nNormalD )
	{
		layout[k].rect.top = LinePos[i].top;
		titleLine++;
	}

	// 2017.09.29
	int a_d;  // 摘要行距
	int a_h;    // 摘要行高
	BOOL bENKeyword = FALSE;
	int j;
	for(j = titleLine; j <= nLine - 1; j++)
	{
		if( style[j] == EN_KEYWORD )
		{
			bENKeyword = TRUE;
			i = j - 1;

			a_d = LinePos[i - 1].top - LinePos[i].bottom;  // 摘要行距
			a_h = LinePos[i].bottom - LinePos[i].top;    // 摘要行高

			layout[k + 3].rect = LinePos[j];
			layout[k + 3].style = EN_KEYWORD;
			if( j < nLine - 1 && style[j+1] == EN_TITLE )
			{
				layout[k + 3].rect.top = LinePos[j + 1].top;
			}
		}
	}

	if( !bENKeyword )  // 2017.09.29
	{
		i = nLine - 1;
		if( LinePos[i].Height() <= 12 )  i--;
	
		int nENLine = 0;
		while( 1 )
		{
			if( i < 0 ) return FALSE;
			if( !IsEnglish(pOCR, LinePos[i]) ) 
			{
				nENLine = 0;
				i--;
				continue;
			}
			else
			{
				if( nENLine == 0 ) nENLine = i;
			}
			a_d = LinePos[i - 1].top - LinePos[i].bottom;  // 摘要行距
			a_h = LinePos[i].bottom - LinePos[i].top;    // 摘要行高
			if( a_h > 0.35* nNormalH && a_h < 1.5*nNormalH && a_d < 2 * nNormalD ) break;
			i--;		
		}
		layout[k + 3].rect = LinePos[nENLine];
		layout[k + 3].style = EN_KEYWORD;
		i--;
	}

	BOOL bAuthor = FALSE;
	BOOL bAffiliation = FALSE;
	int nUintH = 0;

	i -= 2;  // 2017.09.20
	for(; i >= titleLine; i--)
	{
		d1 = LinePos[i - 1].top - LinePos[i].bottom;  // 上面行距
		h = LinePos[i].bottom - LinePos[i].top;       // 行高
		d2 = LinePos[i].top - LinePos[i + 1].bottom;  // 下面行距

		if( !bAffiliation && abs(h-a_h) < 3 && abs(d2-a_d) < 3 ) continue;

		if( !bAffiliation && (d2 > 2*nNormalD) )// || abs(a_d - d2) > 5 || abs(a_h - h) > 5) )
		{
			if( !bENKeyword ) layout[k + 3].rect.bottom = LinePos[i+1].bottom;  // 2017.09.29
			layout[k + 2].rect = LinePos[i];
			layout[k + 2].style = EN_AFFILIATION;
			nUintH = h;
			bAffiliation = TRUE;
			continue;
		}
		
		if( bAffiliation && !bAuthor && h >= 0.4*nNormalH )  // 作者
		{
			if( (i-titleLine >= 1 && d2 > 2*nNormalD && d1 > 2*nNormalD) || abs(nUintH-h) > 5 || i-titleLine == 0) 
			{
				layout[k + 2].rect.bottom = LinePos[i+1].bottom;
				layout[k + 1].rect = LinePos[i];
				layout[k + 1].rect.bottom = LinePos[titleLine].bottom;
				layout[k + 1].style = EN_AUTHOR;
				bAuthor = TRUE;
				
				break;
			}
		}			
	}
	
	num += 4;


	return TRUE;
}

BOOL CDib::IsDOI(CTextRecognizeBase* pOCR, CRect rect) // 2018.07.19
{
	if( !pOCR || !pOCR->IsInitialized() ) return FALSE;

	CDib TmpDib;
	int n = 10;
	int x = (rect.left - n >= 0 ) ? rect.left - n : 0;
	int y = (GetHeight() - 1 - rect.bottom - n >= 0) ? GetHeight() - 1 - rect.bottom - n : 0;
	int nWidth = ( rect.Width() + 2*n >= int(GetWidth()) ) ? GetWidth() - 1 : rect.Width() + 2*n;
	Cut(rect.left - n, GetHeight() - 1 - rect.bottom - n, nWidth, rect.Height() + 2*n, TmpDib);
	CString str;
	str.Format("%s\\temp.bmp", theApp.GetWorkDirectory());
	_unlink(str);
	TmpDib.Save(str);

	BOOL bDOI = FALSE;
	if( pOCR->Do(str) )
	{
		CString strText = pOCR->GetText();
		strText.TrimLeft();
		strText.MakeLower();
		int n = strText.Find("doi");
		if( n >= 0 )	 bDOI = TRUE;
	}

	_unlink(str);
	return bDOI;
}

BOOL CDib::Layout(CTextRecognizeBase* pOCR, CRect* LinePos, STYLE* style, int nLine, PAPER_LAYOUT paperlayout, LAYOUT* layout, int& num, int nType)
{
	if(num < 20 ) return FALSE;

	int nNormalH = GetHeight()*2/(3*50);
	int nNormalD = GetHeight()*1/(3*50);
	int k = 0;   // 版面计数

	try
	{
		if( nType == 1 )  // 标题部分
		{
			if( nLine < 3 ) return FALSE;

			layout[k].rect = paperlayout.Header;
			layout[k].style = PAGE_HEADER;
			k++;

			BOOL bTitle = FALSE;
			BOOL bAuthor = FALSE;
			BOOL bAffiliation = FALSE;
			BOOL bKeyword = FALSE;
			int d1, h , d2;

			if( nLine <= 10 )
			{
				int a_d = LinePos[nLine-2].top - LinePos[nLine-1].bottom;  // 摘要行距
				int a_h = LinePos[nLine-1].bottom - LinePos[nLine-1].top;  // 摘要行高
				int nA_Top = LinePos[nLine-1].top;
				for(int i = nLine - 2; i >= 0; i--)
				{
					int h = LinePos[i].bottom - LinePos[i].top;    // 行高
					int d = 3 *nNormalD;
					if( i > 0 ) d = LinePos[i-1].top - LinePos[i].bottom;  // 上面行距

					if( !bKeyword )
					{
						if( abs(h - a_h) < 2 && abs(d - a_d) < 2 ) continue;
						if( d > 2*nNormalD )
						{
							layout[k].rect = LinePos[i];
							layout[k].rect.top = nA_Top;
							layout[k].style = KEYWORD;
							k++;
							bKeyword = TRUE;
							continue;
						}
					}
					if( bKeyword && !bAffiliation && ((h >= 0.65*nNormalH && d > 1.2*nNormalD) || i == 2 )  )   // 2017.11.20
					{
						layout[k].rect = LinePos[i];
						layout[k].rect.top = layout[k-1].rect.bottom + 2;
						layout[k].style = AFFILIATION;
						k++;
						bAffiliation = TRUE;
						continue;
					}
					if( bAffiliation && !bAuthor && h >= 0.7*nNormalH && d > 2*nNormalD )
					{
						layout[k].rect = LinePos[i];
						layout[k].rect.top = layout[k-1].rect.bottom + 2;
						layout[k].style = AUTHOR;
						k++;
						bAuthor = TRUE;
						continue;
					}
					if( bAuthor && !bTitle && h > 1.0*nNormalH && d < 2*nNormalD && i > 0)
					{
						layout[k].rect = LinePos[i];
						layout[k].rect.top = layout[k-1].rect.bottom + 2;
						layout[k].style = EN_TITLE;
						k++;
						continue;
					}
					if( bAuthor && !bTitle && h > 1.5*nNormalH && d > 2*nNormalD )
					{
						layout[k].rect = LinePos[i];
						layout[k].rect.top = layout[k-1].rect.bottom + 2;
						layout[k].rect.bottom = LinePos[0].bottom;
						layout[k].style = TITLE;
						k++;
						bTitle = TRUE;
						break;
					}
				}

			}
			else
			{
				int nAuthorLine = 0;
				int nAuthorH = 0;
				int oldh = h = 0;
				int i;
				for(i = 0; i < nLine - 2; i++)
				{
					if( i > 0 ) d1 = LinePos[i - 1].top - LinePos[i].bottom;  // 上面行距
					else d1 = paperlayout.Title.bottom - LinePos[i].bottom - 1;
					if( i > 0 ) oldh = h;
					h = LinePos[i].bottom - LinePos[i].top;       // 行高
					d2 = LinePos[i].top - LinePos[i + 1].bottom;  // 下面行距

					if( !bAuthor && bTitle && h > 1.0*nNormalH && d1 < 2*nNormalD && d2 > 2* nNormalD && i > 0 && IsEnglish(pOCR, LinePos[i]) )  // ???
					{
						layout[k].rect = LinePos[i];
						layout[k].rect.top = layout[k-1].rect.bottom + 2;
						layout[k].style = EN_TITLE;
						k++;
						continue;
					}

					if( i > 0 && !bTitle &&  ( double(h) / oldh < 0.9 || double(d1) / d2 > 2.2) )    // 09.07 / 09.19 // 2018.07.19
					{
						i--;

						if( !IsDOI(pOCR, LinePos[i]) ) 
						{
							layout[k].rect = LinePos[i];
							layout[k].style = TITLE;
							h = oldh;

							int hh = LinePos[i+1].bottom - LinePos[i+1].top;      // 行高
							if( abs(hh - h) < 0.1*h && d1 < 1.2*h )
							{
								layout[k].rect.top = LinePos[i+1].top;
								i++;
							}

						}
						else
						{
							i++;
							layout[k].rect = LinePos[i];
							layout[k].style = TITLE;

						}

						k++;
						bTitle = TRUE;
						continue;

					}

					if( !bTitle && ((h > 1.5*nNormalH && d2 > 2*nNormalD) || 
						(d1 > 2*nNormalD && h > 1.25*nNormalH && d2 > 1.2*nNormalD)) )  // 标题
					{
						layout[k].rect = LinePos[i];
						layout[k].style = TITLE;
						
						int hh = LinePos[i+1].bottom - LinePos[i+1].top;      // 行高
						if( abs(hh - h) < 0.1*h )
						{
							layout[k].rect.top = LinePos[i+1].top;
							i++;
						}
						
						k++;
						bTitle = TRUE;
						continue;
					}			
					if( bTitle && !bAuthor && h >= 0.7*nNormalH && d1 > 2*nNormalD )  // 作者
					{
						layout[k].rect = LinePos[i];
						layout[k].rect.bottom = layout[k-1].rect.top + 2;
						layout[k].style = AUTHOR;
						nAuthorLine = i;
						nAuthorH = layout[k].rect.Height();

						k++;
						bAuthor = TRUE;
						continue;
					}			
					if( bAuthor && !bAffiliation && h >= 0.6*nNormalH && d1 > 1.5*nNormalD && d2 > 2*nNormalD )  // 单位
					{
						layout[k-1].rect.top = LinePos[i-1].top;
						layout[k].rect = LinePos[i];
						layout[k].rect.bottom = layout[k-1].rect.top + 2;
						layout[k].style = AFFILIATION;
						k++;
						bAffiliation = TRUE;
						break;
					}			
					if( bAuthor && !bAffiliation && h >= 0.6*nNormalH  && ( d1 > 1.5*nNormalD || abs(h-nAuthorH) > 6 ) )  // 单位
					{
						layout[k-1].rect.top = LinePos[i-1].top;
						layout[k].rect = LinePos[i];
						layout[k].rect.bottom = layout[k-1].rect.top + 2;
						layout[k].style = AFFILIATION;
						bAffiliation = TRUE;
						continue;
					}			
					if( bAffiliation && ( d2 > 2*nNormalD || d2 > 1.5*d1 ) )  // 09.07
					{
						layout[k].rect.top = LinePos[i].top;
						k++;
						break;
					}
				}

				i++;

				while( i < nLine && (style[i] == EN_TITLE || style[i] == EN_KEYWORD) )  // 2017.09.29
				{
					i++;
				}

				int a_d = LinePos[i].top - LinePos[i+1].bottom;  // 摘要行距
				int a_h = LinePos[i].bottom - LinePos[i].top;    // 摘要行高
				int nA_Bottom = LinePos[i].bottom;
				i++;
				int j = 0;
				for(; i <= nLine - 1; i++, j++)
				{
					int h = LinePos[i].bottom - LinePos[i].top;      // 行高
					int d = 0;
					if( i < nLine - 1 ) d = LinePos[i].top - LinePos[i + 1].bottom;  // 下面行距
					if( abs(h - a_h) < 2 && abs(d - a_d) < 2 ) continue;
					if( j >= 3 && IsEnglish(pOCR, LinePos[i]) )
					{
						int d1 = LinePos[i - 1].top - LinePos[i].bottom;  // 上面行距
						if( d1 > a_d + 5 && h > a_h + 5 && d2 > 2* nNormalD )  // 2017.07.04
						{
							layout[k].rect = LinePos[i - 1];
							layout[k].rect.bottom = nA_Bottom;
							layout[k].style = KEYWORD;
							k++;
							i--;
							break;
						}
					}
					if( d > 2*nNormalD || i == nLine - 1 )
					{
						layout[k].rect = LinePos[i];
						layout[k].rect.bottom = nA_Bottom;
						layout[k].style = KEYWORD;
						k++;
						i++;
						break;
					}
				}

				for(j = i ; j < i + 4 && j < nLine - 1; j++)
				{
					if( IsEnglish(pOCR, LinePos[j]) )
					{
						if( i != j )
						{
							layout[k - 1].rect.top = LinePos[j - 1].top - 5;
							i = j;
						}
						break;
					}
				}

				CRect ENLinePos[100];
				STYLE ENStyle[100];     // 2017.09.29
				memset(ENStyle, 0, 100*sizeof(STYLE));
				if( paperlayout.ENTitle.Height() > 10 )
				{
					int nEn0 = -1;
					int nEn1 = -1;
					int i;
					for(i = 0; i < nLine; i++)
					{
						if( nEn0 == - 1 && style[i] == EN_TITLE )
						{
							nEn0 = i;
							break;
						}
					}
					for(i = nLine - 1; i >= nEn0; i--)
					{
						if( nEn1 == - 1 && (style[i] == EN_TITLE || style[i] == EN_KEYWORD) )
						{
							nEn1 = i;
							break;
						}
					}

					if( nEn0 < 0 || nEn1 < 0 )
					{
						for(i = 0; i < nLine; i++)
						{
							if( nEn0 == - 1 && abs(LinePos[i].bottom - paperlayout.ENTitle.bottom) < 10 )
							{
								nEn0 = i;
								continue;
							}
							if( nEn1 == - 1 && abs(LinePos[i].top - paperlayout.ENTitle.top) < 10 )
							{
								nEn1 = i;
								break;
							}
						}
					}

					if( nEn0 >= 0 && nEn1 > nEn0 )
					{
						int nENLine = nEn1 - nEn0 + 1;
						if( nENLine > 0 && nENLine < 100 )
						{
							memcpy(ENLinePos, &LinePos[nEn0], nENLine*sizeof(CRect));
							memcpy(ENStyle, &style[nEn0], nENLine*sizeof(STYLE));  // 2017.09.29
							int oldk = k;
							BOOL bRet = ENLayout(pOCR, ENLinePos, ENStyle, nENLine, paperlayout, layout, k); // 2017.09.29
							if( !bRet || oldk == k )
							{
								paperlayout.ENTitle.top = paperlayout.ENTitle.bottom = 0;
							}
						}
					}
				}

				if( paperlayout.ENTitle.Height() <= 0 )
				{
					int nENLine = nLine - i;
					if( nENLine > 0 && nENLine < 100 )
					{
						memcpy(ENLinePos, &LinePos[i], nENLine*sizeof(CRect));
						memcpy(ENStyle, &style[i], nENLine*sizeof(STYLE)); // 2017.09.29
						ENLayout(pOCR, ENLinePos, ENStyle, nENLine, paperlayout, layout, k);  // 2017.09.29
					}
				}

			}  
		}
		else if( nType == 2 )  // 文献部分
		{

		}

		num = k;
	}
	catch (CException* )
	{
	}

#if 0
	for(int i = 0; i < k; i++)
	{
		CDib dd;
		int n = 10;
		Cut(layout[i].rect.left - n, GetHeight() - 1 - layout[i].rect.bottom - n, 
			layout[i].rect.Width() + 2*n, layout[i].rect.Height() + 2*n, dd);
		CString str;
		str.Format("e:\\yyy-%d.bmp", i);
		_unlink(str);
		dd.Save(str);
	}
#endif

	return TRUE;
}

BOOL CDib::Line(CRect rect, int direction, CRect* LinePos, int& nLine)
{
	int nWidth = rect.Width();
	int nRowThreshold = int(0.01*nWidth);
	return Line(GetYHistgram(), rect, direction, LinePos, nLine, nRowThreshold);
}

BOOL CDib::Line(int* nHistgram, CRect rect, int direction, CRect* LinePos, int& nLine, int nRowThreshold)
{
	int nHeight = rect.Height();
	int nWidth = rect.Width();
	int n[500];
	int count = 0;
	BOOL bZero = TRUE;
	CPoint p1, p2, p3;
	int i;

	try
	{
		if( nRowThreshold <= 0 ) nRowThreshold = int(0.03*nWidth);

		if( direction == 0 )
		{
			for(i = 2 + rect.top; i < rect.top + nHeight - 2; i++)
			{
				if( bZero )
				{
					p1.x = i-2;
					p1.y = nHistgram[i-2];
					p2.x = i-1;
					p2.y = nHistgram[i-1];
					p3.x = i;
					p3.y = nHistgram[i];
					double s = inflexion(p1, p2, p3);
					
					if( s < 0  )
					{
						if( nHistgram[i + 1] > nRowThreshold )
						{
							n[count] = i - 1;
							count++;
							bZero = FALSE;
						}
					}
				}
				else
				{				
					p1.x = -(i+2);
					p1.y = nHistgram[i+2];
					p2.x = -(i+1);
					p2.y = nHistgram[i+1];
					p3.x = -i;
					p3.y = nHistgram[i];
					double s = inflexion(p1, p2, p3);
					
					if( s < 0 )
					{
						if( nHistgram[i + 1] < 3  )
						{
							n[count] = i + 1;
							count++;
							bZero = TRUE;
						}
					}
				}
				if( nLine > 0 && count >= 2*nLine ) break;
			}
		}
		else
		{
			for(i = nHeight - 3 + rect.top; i >= 2 + rect.top; i--)
			{
				if( bZero )
				{
					p1.x = -(i+2);
					p1.y = nHistgram[i+2];
					p2.x = -(i+1);
					p2.y = nHistgram[i+1];
					p3.x = -i;
					p3.y = nHistgram[i];
					double s = inflexion(p1, p2, p3);
					
					if( s < 0 )
					{
						if( nHistgram[i - 1] > nRowThreshold )
						{
							n[count] = i + 1;
							count++;
							bZero = FALSE;
						}
					}
				}
				else
				{
					
					p1.x = (i-2);
					p1.y = nHistgram[i-2];
					p2.x = (i-1);
					p2.y = nHistgram[i-1];
					p3.x = i;
					p3.y = nHistgram[i];
					double s = inflexion(p1, p2, p3);
					
					if( s < 0 )
					{
						if( nHistgram[i - 1] < 3  )
						{
							n[count] = i - 1;
							count++;
							bZero = TRUE;
						}
					}
				}
				if( nLine > 0 && count >= 2*nLine ) break;
			}
		}

		nLine = 0;
		for(i = 0; i < count/2; i++ )
		{
			LinePos[nLine] = rect;
			if( n[2*i] > n[2*i+1] )
			{
				LinePos[nLine].bottom = n[2*i];
				LinePos[nLine].top = n[2*i+1];
			}
			else
			{
				LinePos[nLine].bottom = n[2*i+1];
				LinePos[nLine].top = n[2*i];
			}

			if( nLine > 0 && LinePos[i].Height() > 15 && abs(LinePos[i - 1].bottom - LinePos[i].top) < 4 )
			{
				LinePos[i - 1].bottom = LinePos[i].bottom;
			}
			else nLine++;
		}
	}
	catch (CException* )
	{
	}

	return TRUE;
}

BOOL CDib::Line(CRect* LinePos, int& nLine, CRect rect, int nThreshold)
{
	LPBITMAPINFO pBMI = GetBmpInfo();
	if( !pBMI || pBMI->bmiHeader.biBitCount != 8 )
		return FALSE;
	
	if( pBMI->bmiHeader.biBitCount != 8 )
		return FALSE;

	int* nHistgram = new int[pBMI->bmiHeader.biHeight];
	memset(nHistgram, 0, pBMI->bmiHeader.biHeight*sizeof(int));
	try
	{
		int i;
		LPBYTE pData = GetBits();
		long nBytePerLine = GetBytePerLine(pBMI->bmiHeader.biWidth,8);

		int s0 = rect.left;
		int s1 = s0 + rect.Width() / 2;

		for(i = rect.top; i < rect.bottom; i++)
		{
			for(int j = s0 ; j < s1; j++)
			{
				if( pData[i*nBytePerLine + j] < nThreshold ) nHistgram[i]++;
			}
		}

#if 0
FILE* f;
errno_t err = fopen_s(f, "e:\\12345.txt","wt");
for(i = rect.top; i < rect.bottom; i++)
{
	fprintf(f, "%d\n", nHistgram[i]);
}
fclose(f);
#endif

		int nWidth = rect.Width();
		int n[500];
		int count = 0;
		BOOL bZero = TRUE;
		CPoint p1, p2, p3;

		if( nHistgram[rect.bottom - 1] > 6 && nHistgram[rect.bottom - 2] > 6)
		{
			n[count] = rect.bottom - 1;
			count++;
			bZero = FALSE;
		}

		for(i = rect.bottom - 3; i >= rect.top + 2; i--)
		{
			if( bZero )
			{
				p1.x = -(i+2);
				p1.y = nHistgram[i+2];
				p2.x = -(i+1);
				p2.y = nHistgram[i+1];
				p3.x = -i;
				p3.y = nHistgram[i];
				double s = inflexion(p1, p2, p3);
				
				if( s < 0 )
				{
					int k = 0;
				}
				if( s < 0 && nHistgram[i- 1] > nWidth*0.03  )
				{
					n[count] = i + 3;
					if( count > 0 ) 
					{
						if( abs(n[count] - n[count-1]) > 6 ) 
						{
							count++;
							bZero = FALSE;
						}
					}
					else 
					{
						count++;
						bZero = FALSE;
					}
				}
			}
			else
			{

				p1.x = (i-2);
				p1.y = nHistgram[i-2];
				p2.x = (i-1);
				p2.y = nHistgram[i-1];
				p3.x = i;
				p3.y = nHistgram[i];
				double s = inflexion(p1, p2, p3);
				
				if( s < 0 )
				{
					if( nHistgram[i - 1] < 3  )
					{
						n[count] = i - 1;
						if( count > 0 ) 
						{
							if( abs(n[count] - n[count-1]) > 0 ) 
							{
								count++;
								bZero = TRUE;
							}
						}
					}
				}
			}
		}

		for(i = 0; i < count/2; i++ )
		{
			LinePos[nLine] = rect;
			if( n[2*i] > n[2*i+1] )
			{
				LinePos[nLine].bottom = n[2*i];
				LinePos[nLine].top = n[2*i+1];
			}
			else
			{
				LinePos[nLine].bottom = n[2*i+1];
				LinePos[nLine].top = n[2*i];
			}
			nLine++;
		}
	}
	catch (CException* )
	{
	}

	delete [] nHistgram;

	return TRUE;
}

BOOL CDib::CalculateXHistgram(const CDib& dib, CRect rc, int* nXHistgram)
{
	LPBITMAPINFO pBMI = dib.GetBmpInfo();
	if( !pBMI || pBMI->bmiHeader.biBitCount != 8 )
		return FALSE;
	
	LPBYTE pData = dib.GetBits();
	long nBytePerLine = GetBytePerLine(pBMI->bmiHeader.biWidth,8);
	int nWidth = rc.Width();
	memset(nXHistgram, 0, nWidth*sizeof(int));
	
	int nThreshold = 127;
	for(int j = rc.left; j < rc.right && j < rc.left + nWidth; j++)
	{
		for(int i = rc.top; i < rc.bottom; i++)
		{
			BYTE ngrayscale = *(pData + i* nBytePerLine + j);
			if( ngrayscale < nThreshold ) 
			{
				nXHistgram[j - rc.left]++;
			}
		}
	}
	return TRUE;

}

BOOL CDib::CalculateYHistgram(const CDib& dib, int left, int width, int* nYHistgram)
{
	LPBITMAPINFO pBMI = dib.GetBmpInfo();
	if( !pBMI || pBMI->bmiHeader.biBitCount != 8 )
		return FALSE;

	LPBYTE pData = dib.GetBits();
	long nBytePerLine = GetBytePerLine(pBMI->bmiHeader.biWidth,8);
	int nHeight = pBMI->bmiHeader.biHeight;
	int nWidth = pBMI->bmiHeader.biWidth;
	memset(nYHistgram, 0, nHeight*sizeof(int));

	if( left < 0 ) left = 0;
	int nThreshold = 127;
	for(int i = 0; i < nHeight; i++)
	{
		for(int j = left; j < left + width && j < nWidth; j++)
		{
			BYTE ngrayscale = *(pData + i* nBytePerLine + j);
			if( ngrayscale < nThreshold ) 
			{
				nYHistgram[i]++;
			}
		}
	}
	return TRUE;
}

BOOL CDib::JudgeMark(const CDib& dib, int left, int width, int height)
{
	int* nYHistgram = new int[height];

	CalculateYHistgram(dib, left, width, nYHistgram);

#if 0
	FILE* f;
	errno_t err = fopen_s(f, "e:\\312345.txt","wt");
	for(int i = 0; i < height; i++)
	{
		fprintf(f, "%d\n", nYHistgram[i]);
	}
	fclose(f);
#endif

	BOOL bFind = TRUE;
	int j;
	for(j = 0; j < height/3; j++)
	{
		if( nYHistgram[j] >= 2) 
		{
			bFind = FALSE;
			break;
		}
	}
	if( bFind ) 
	{
		bFind = FALSE;
		for( ; j < height*2/3; j++)
		{
			if( nYHistgram[j] >= 4) 
			{
				bFind = TRUE;
				break;
			}
		}
		if( bFind )
		{
			int n = j;
			int c = 0;
			for( ; j < n + height/3; j++)
			{
				if( nYHistgram[j] < 3) 
				{
					c++;
				}
			}
			if( c > 0.3 * height /3 )
			{
				bFind = FALSE;
			}
		}
	}

	delete [] nYHistgram;

	return bFind;
}

BOOL CDib::LocateMark(CRect* Mark, int& num, CRect Line, int nThreshold)
{
	LPBITMAPINFO pBMI = GetBmpInfo();
	if( !pBMI || pBMI->bmiHeader.biBitCount != 8 )
		return FALSE;

	if( Line.Height() < 15 )
	{
		return FALSE;
	}

	try
	{
		int nWidth = Line.Width();
		int nHeight = Line.Height();
		CDib TmpDib;

		Cut(Line.left, GetHeight() - 1 - Line.bottom, nWidth, nHeight, TmpDib);
		int* X = TmpDib.GetXHistgram();
#ifdef MY_TEST
TmpDib.Save("e:\\12345.bmp");
FILE* f;
errno_t err = fopen_s(&f, "e:\\412345.txt","wt");
if( f && X )
{
	for(int ii = 0; ii < Line.Width(); ii++)
	{
		fprintf(f, "%d\n", X[ii]);
	}
	fclose(f);
}

#endif
		
		int max[200];
		int pos[200];
		UINT n = 0;
		UINT i;
		for(i = 0; i < UINT(nWidth) && n < 200 ; i++)
		{
			if( X[i] > nHeight * 0.3 && X[i] < nHeight * 0.9 )
			{
				if( n == 0 && X[i+1] > X[i] )  continue;
				if( n > 0 && ( X[i-1] > X[i] || X[i+1] > X[i] ) ) continue;
				
				if( n > 0 )
				{
					if( abs(int(i - pos[n-1])) > 3*nHeight || abs(int(i - pos[n-1])) < 0.1*nHeight )
					{
						max[n-1] = X[i];
						pos[n-1] = i;
						i += 1;
						continue;
					}
				}
				max[n] = X[i];
				pos[n] = i;
				n++;
				i += 1;
			}
		}
		
		for(i = 1; i < n; i++)
		{
			BOOL bCandiate = FALSE;
			
			UINT x1 = i - 1;
			UINT x2 = i;
			int cc = 0;
			for(cc = 0; cc < 8; cc++)
			{
				if( (max[x2 + cc] - max[x1]) > 2 ) break;  // 2个峰尖 应该基本等高
				if( abs(max[x1] - max[x2 + cc]) <= 2 )
				{
					int w = pos[x2 + cc] - pos[x1];
					int h = Line.Height();
					if( cc == 0 && w > int(0.5 * h) && w <= 3 * h  )  // 2017.11.20
					{
						bCandiate = TRUE;
						x2 = x2 + cc;
						break;
					}
					if( cc > 0 && w > int(0.4 * h) && w <= 3 * h  )
					{
						bCandiate = TRUE;
						x2 = x2 + cc;
						break;
					}
					if( w > 3* h )
					{
						break;
					}
				}
			}
			
			if( bCandiate )
			{
				CDib dd;
				
				TmpDib.Cut(pos[x1] - 1, 0, pos[x2] - pos[x1] + 4, nHeight, dd);
#ifdef MY_TEST
				dd.Save("E:\\23456.bmp");
#endif
				
				BOOL bFind = JudgeMark(TmpDib, pos[x1] - 1, pos[x2] - pos[x1] + 4, nHeight);

				if( !bFind && pos[x2] - pos[x1] >= 2*Line.Height() )
				{
					int k = 0;
				}

				if( bFind )
				{
					UINT xx1 = pos[x1] - 1;
					UINT xx2 = pos[x2] + 3;
					if( x1 > 0 && x2 < n - 1 )
					{
						x1--;
						x2++;
						if( abs(max[x1] - max[x2]) <= 2 )
						{
							BOOL bFind1 = JudgeMark(TmpDib, pos[x1] - 1, pos[x2] - pos[x1] + 4, nHeight);
							if( bFind1 )
							{
								xx1 = pos[x1] - 1;
								xx2 = pos[x2] + 3;
							}
						}
					}
					
					if( xx2 - xx1 > nHeight*0.4 )
					{
						Mark[num].top = long(Line.top + nHeight*0.2);
						Mark[num].bottom = long(Line.bottom + nHeight*0.2);
						Mark[num].left = Line.left + xx1;
						Mark[num].right = Line.left + xx2;
						num++;
						
						i = x2;
					}
				}
			}
		}
	}
	catch (CException* )
	{
	}

	return TRUE;

}
*/
int CDib::GetPaletteSize() const
{
	return GetPaletteSize(m_pBMI);
}

int CDib::GetPaletteSize(LPBITMAPINFO pBMI) const
{
	if( !pBMI ) return 0;
	
	int nSizePalette = 0;
	switch(pBMI->bmiHeader.biBitCount) 
	{
	case 24:
		break;
	case 32:
		break;
	case 8:
		nSizePalette = 256;
		break;
	case 4:
		nSizePalette = 16;
		break;
	case 1:
		nSizePalette = 2;
		break;
	default:
		break;
	}
	return nSizePalette* sizeof(RGBQUAD);
}

// cut 
BOOL CDib::Cut(int x, int y, int width, int height, CDib& dib)  
{
	if( height <= 0 || width <= 0 ) return FALSE; // 2017.11.20
	if( (DWORD)(x + width) > GetWidth() || (DWORD)(y + height) > GetHeight() )
	{
		return FALSE;
	}
	
	BITMAPINFO bmpinfo;
	memcpy(&bmpinfo, m_pBMI, sizeof(BITMAPINFO));
	bmpinfo.bmiHeader.biWidth = width;
	bmpinfo.bmiHeader.biHeight = height;
	bmpinfo.bmiHeader.biSizeImage = BMPWIDTHBYTES((width)*((DWORD)bmpinfo.bmiHeader.biBitCount)) * height;
	int nOldLineBits = GetBytePerLine(m_pBMI->bmiHeader.biWidth, m_pBMI->bmiHeader.biBitCount);
	int nLineBits = GetBytePerLine(width, bmpinfo.bmiHeader.biBitCount);
	
	int *nXHistgram = new int[width];
	int *nYHistgram = new int[height];
	int *nYPartHist = new int[height];
	memset(nXHistgram, 0, width*sizeof(int));
	memset(nYHistgram, 0, height*sizeof(int));
	memset(nYPartHist, 0, height*sizeof(int));
	try
	{
		int nThreshold = 127;

		if( m_pBMI->bmiHeader.biBitCount == 24 || m_pBMI->bmiHeader.biBitCount == 32 )
		{
			BYTE* pBits = new BYTE[nLineBits*height + 1];
			int nByte = m_pBMI->bmiHeader.biBitCount/8;
			x = x/nByte*nByte;  
			
			for(int i = height-1; i >= 0; i--)
			{
				for(int j = 0; j < width; j++)
				{
					for(int k = 0; k < nByte; k++)
					{
						if( ((GetHeight()-y)-i) >= 0 && ((GetHeight()-y)-i) < GetHeight() ) // 2011.04.06
							pBits[(height-1-i)*nLineBits + j*nByte + k] = m_pBits[((GetHeight()-y)-i)*nOldLineBits + (x+j)*nByte + k];
					}
				}
			}
			dib.SetDatas(&bmpinfo, pBits);
			
			delete [] pBits;
		}
		else if( m_pBMI->bmiHeader.biBitCount == 8 )
		{
			BYTE* pBits = new BYTE[nLineBits*height + 1];
			
			for(int i = height-1; i >= 0; i--)
			{
				for(int j = 0; j < width; j++)
				{
					if( ((GetHeight()-y)-i) >= 0 && ((GetHeight()-y)-i) < GetHeight() ) // 2011.04.06
						pBits[(height-1-i)*nLineBits + j] = m_pBits[((GetHeight()-y)-i)*nOldLineBits + (x+j)];

					BYTE ngrayscale = pBits[(height-1-i)*nLineBits + j];
					if( ngrayscale < nThreshold ) 
					{
						nXHistgram[j]++;
						nYHistgram[i]++;
						if( j < width/2 ) nYPartHist[i]++;
					}

				}
			}
			// 2012.11.17
			DWORD cb = sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
			LPBITMAPINFO pNewBMI = (LPBITMAPINFO)GlobalAllocPtr(GHND, cb);
			memcpy(pNewBMI, m_pBMI, cb);
			pNewBMI->bmiHeader.biWidth = width;
			pNewBMI->bmiHeader.biHeight = height;
			pNewBMI->bmiHeader.biSizeImage = BMPWIDTHBYTES((width)*((DWORD)pNewBMI->bmiHeader.biBitCount)) * height;
			dib.SetDatas(pNewBMI, pBits);
			GlobalFreePtr(pNewBMI);
			
			delete [] pBits;

//			dib.SetXHistgram(nXHistgram);
//			dib.SetYHistgram(nYHistgram);
//			dib.SetYPartHist(nYPartHist);
		}
		else
		{
			delete [] nXHistgram;
			delete [] nYHistgram;
			delete [] nYPartHist;
			return FALSE;
		}
	}
	catch (CException* )
	{
		delete [] nXHistgram;
		delete [] nYHistgram;
		delete [] nYPartHist;
		return FALSE;
	}
	
	delete [] nXHistgram;
	delete [] nYHistgram;
	delete [] nYPartHist;
	return TRUE;
}

