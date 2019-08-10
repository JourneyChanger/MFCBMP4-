#ifndef __HWOCRSDK2_H
#define __HWOCRSDK2_H

#include "Hwocrsdk.h"


//////////////////////////////////////////////////////////////////////
//
// Struct Definition 
//
//////////////////////////////////////////////////////////////////////

// struct IMGRGN is used to record the rect infomation (size, position, layout style)
// recognized by Function HWOCRLocate70 or indicated by users
typedef struct 
{
	RECT  rect;
	STYLE style;
} IMGRGN;


#ifdef __cplusplus
extern "C" {
#endif

	// Function: HWOCROpenImageFile70
	//			Load an image file into memory.
	//			User should call HWOCRCloseImage70() to free the image,
	//			or the image will be retained in memory.
	// Parameters:	
	//			pImageFile -	Contain the full path of an input image file
	// Return value:
	//			The state of the return value.

	DLLEXPORT  ERRCODE STDCALL HWOCROpenImageFile70(char *pImageFile );

	// Function: HWOCROpenImage70
	//			Load an image into memory.
	//			User should call HWOCRCloseImage70() to free the image,
	//			or the image will be retained in memory.
	// Parameters:	
	//			hDib	-	Handle to DIB
	// Return value:
	//			
	DLLEXPORT  BOOL STDCALL HWOCROpenImage70( HANDLE hDib );

	// Function: HWOCRCloseImage70
	//			Free the image loaded by  HWOCROpenImageFile.
	// Parameters:	
	// Return value:
	//			The state of the return value.	              
	DLLEXPORT  ERRCODE STDCALL	HWOCRCloseImage70( );

	// Function: HWOCRRecognize70
	//			Recognize the loaded image in regions specified by pImgRgn.
	// Parameters:	
	//			nImgRgnNum	-	The region total number.
	//			pImgRgn		-   regions.
	// Return value:
	//			The state of the return value.
	//
	DLLEXPORT  ERRCODE STDCALL HWOCRRecognize70( int nImgRgnNum, IMGRGN *pImgRgn );

	// Function: HWOCRGetRecoTxt70
	//			Get recognize result.
	// Parameters:	
	//			nTxtLen	-	as input parameter,specify the buffer length, 
	//						as output parameter,return the actual length of the result
	//			pTxtBuf	-	buffer allocated by caller to store the result text
	// Return value:
	//			The state of the return value.
	//
	DLLEXPORT  ERRCODE STDCALL HWOCRGetRecoTxt70( int &nTxtLen , char *pTxtBuf );

	// Function: HWOCRGetRecoTxt270
	//			Get recognize result.
	// Parameters:	
	//			pTxtLen	-	as input parameter,specify the buffer length, 
	//						as output parameter,return the actual length of the result
	//			pTxtBuf	-	buffer allocated by caller to store the result text
	// Return value:
	//			The state of the return value.
	//
	DLLEXPORT  ERRCODE STDCALL HWOCRGetRecoTxt270( int* pTxtLen , char *pTxtBuf );

	// Function: HWOCRGetRecoTxtNum70
	//			Calculate suspicious char number and total char number after recognition.
	// Parameters:	
	//			nTotal	-	total char count.			
	//			nDubiousCount	-	Dubious char count.
	//
	// Return value:
	//			The state of the return value.
	// Comments:
	//			nTotal:			返回识别结果字符总数，双字节汉字算作1个字符
	//							制表符，回车换行和空格不计算在内!!!(因为空格、回车换行和制表符没有可信度)
	//			nDubiousCount:	返回可疑字字符总数，双字节汉字算作1个字符
	//
	DLLEXPORT ERRCODE STDCALL HWOCRGetRecoTxtNum70( int &nTotal, int &nDubiousCount);

	// Function: HWOCRExportToFile70
	//			Save recognize result to file.
	// Parameters:	
	//			pFileName	-  result file name
	// Return value:
	//			The state of the return value.
	//
	DLLEXPORT  ERRCODE STDCALL HWOCRExportToFile70( char *pFileName);

	


#ifdef __cplusplus
}
#endif

#endif