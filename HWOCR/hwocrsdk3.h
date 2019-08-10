#ifndef __HWOCRSDK3_H
#define __HWOCRSDK3_H

#include "Hwocrsdk2.h"

// Character recognition result
typedef  struct
{
	RECT  rcChar;	//locate and size of char
	BYTE  nFont;
	WORD  wCandidate[10];	//all of the candidate char
	BYTE  nConfidence; //The confidence of the first character in WCandidate
} CHARINFO;


#ifdef __cplusplus
extern "C" {
#endif

	// Function: HWOCRLocate70
	//			Layout analysis. User should use HWOCROpenImageFile() to load
	//			an image into memory before this action.
	// Parameters:	
	//			nMaxRgnNum	-	Input parameter,specify the size of pImgRgn
	//			nImgRgnNum	-	Output parameter, contain the total number of regions
	//			pImgRgn		-   Output parameter, user must allocate enough memory 
	//							before calling the function
	//			bDeskew		-   TRUE, to do image skew correct. 图像倾斜矫正(倾角3度以内)。
	//							FALSE, not to do  image skew correct
	//			nOption		-	Two options can be used to initialize the SDK,   
	//							MULTICOL: 多栏，Specified that the document has multi-column layout, if this
	//					        option isn't specified, by default,SDK will regard the document
	//							as one column text.表示处理多栏图像, 即存在2个以上版面块并排；
	//							SINGLECOL: 单栏
	//							OFFICIAL: 公文（1.2 版不支持公文选项）
	// Return value:
	//			The state of the return value.
	//
	DLLEXPORT  ERRCODE STDCALL HWOCRLocate70( int nMaxRgnNum, int &nImgRgnNum, 
		IMGRGN *pImgRgn, BOOL bDeskew, int nOption  );

	// Function: HWOCROpenInfo70
	//			Prepare to get the layout and recognition information.
	// Parameters:	
	//			pImageFile	-	the full path of an image file
	// Return value:
	//			The state of the return value.
	//
	DLLEXPORT  ERRCODE STDCALL HWOCROpenInfo70( char *pImageFile );

	// Function: HWOCRCloseInfo70
	//			End of geting information, the function must be called before
	//          geting another image's information using HWOCROpenInfo70.
	// Parameters:	
	// Return value:
	//			The state of the return value. 
	//
	DLLEXPORT  ERRCODE STDCALL HWOCRCloseInfo70();

	// Function: HWOCRGetLayoutInfo70
	//			Get the layout information, see also HWOCROpenInfo70().
	// Parameters:	
	//			nMaxRgn		-	Input parameter, limit the maximum number of regions to get
	//			nRgnNum		-	Output parameter, return the total number of regions.
	//			pImgRgn		-	regions
	// Return value:
	//			The state of the return value.     
	//
	DLLEXPORT  ERRCODE STDCALL HWOCRGetLayoutInfo70( int nMaxRgn, int &nRgnNum, IMGRGN *pImgRgn );

	// Function: HWOCRGetCharInfo70
	//			Get the character information, see also HWOCROpenInfo70().
	// Parameters:	
	//			dwCharNo	-	Input parameter, specify the number of character's information to get.
	//			CharInfo	-	Output parameter, contain the character information
	// Return value:
	//			The state of the return value.   
	//
	DLLEXPORT  ERRCODE STDCALL HWOCRGetCharInfo70( DWORD dwCharNo, CHARINFO &CharInfo );

	// Function: HWOCRSetLayoutInfo70
	//			Set the layout information to an image file
	// Parameters:	
	//			pImageFile	-	A black and white image file
	//			nWidth		-	Width of the image in pixel
	//			nHeight		-   Height of the image in pixel
	//			nRgnNum		-   The total number of regions
	//			pImgRgn		-   The pointer to region
	// Return value:
	//			The state of the return value. 
	//
	DLLEXPORT  ERRCODE STDCALL HWOCRSetLayoutInfo70( char *pImageFile,int nWidth, int nHeight,
		int nRgnNum, IMGRGN *pImgRgn );

	// Function: HWOCRIsModified70
	//			Checking if the image has been modified during 
	//          the recognition process. If modified, the result 
	//			image is saved as the same name as pImgFile with .bki
	//			extension, this bki file is in the same directory 
	//			with the bmp file. and the BKI file is in TIFF format 
	//			in fact.
	// Parameters:	
	//			pImgFile	-	Image file to be checked
	// Return value:
	//			The state of the return value. 
	//
	DLLEXPORT  BOOL STDCALL HWOCRIsModified70(char *pImgFile);


	DLLEXPORT  char* STDCALL HW_Progress01();


#ifdef __cplusplus
}
#endif

#endif