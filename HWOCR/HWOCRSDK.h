//*****************************************************************************
//* Hanwang Technology Co. Ltd.
//* Copyright(C) 2003-2009, all rights reserved.
//*
//* The following code is copyrighted and contains proprietary information
//* and trade secrets of Hanwang Technology Co. Ltd.
//*
//*****************************************************************************

#ifndef __HWOCRSDK
#define __HWOCRSDK

#if defined(_WIN32) && !defined(__WINDOWS__)
#include <windows.h>
#endif

#define  MULTICOL	0x0000	//Multi colum ;			����
#define  SINGLECOL 	0x0001	//single colum ;		����
#define  OFFICIAL	0x0002	//official document ;	����(��ɫͼ�����Ч)

#ifdef _WIN32
#define DLLEXPORT
#define STDCALL __stdcall
#else
#define DLLEXPORT
#define STDCALL
#endif

//////////////////////////////////////////////////////////////////////
//
// Data Types
//
//////////////////////////////////////////////////////////////////////

// ���������
typedef int STYLE;

#define  HOR_TEXT	0		// Horizontal text zone
#define  VER_TEXT	1		// Vertical text zone
#define  ENG_TEXT	2		// English zone in Chinese document
#define  FORM		3		// Form zone
#define  PICTURE	4		// Picture zone
#define  SEAL		5		// ����, ���Ժ������ı����ص�, һ��ͼ��ֻ����1��

// ��������
typedef int LANGUAGE;

#define  GB			0		// ����
#define  BIG5		1		// ����
#define  ENGLISH	2		// Ӣ��
#define  JPN		3		// ����
#define  KOREA		4		// ����
#define  FINNISH	5		// ����
#define  FRENCH		6		// ����
#define  GERMAN		7		// ����
#define  SPANISH	8		// ��������
#define  ITALIAN	9		// �������
#define  SWEDISH	10		// �����
#define  DANISH		11		// ������
#define	 NORWEGIAN	12		// Ų����
#define  DUTCH		13		// ������
#define  PORTUGUESE	14		// ��������

// Export Code
typedef int EXPORTCODE;

#define  EXPORT_GBK			0	
#define  EXPORT_GB			1		
#define  EXPORT_BIG5		2		
#define  EXPORT_ENG			3		
#define  EXPORT_SHIFT_JIS	4	//����
#define  EXPORT_HANGUL		5	//����
#define  EXPORT_1252		6	//1252

// Output Setting
typedef int OUTPUTSETTING;
#define  SINGLE_PDF		0
#define  DOUBLE_PDF		1
#define  UNICODE_TXT	2
#define  UTF8_TXT		3

// 	The State Of The Return Value 
typedef int ERRCODE;
#define  SUCCESS			0
#define  INIT_ERROR			1		//initialization failed
#define  UNSUPPORT_IMAGE	2		//unsupported image format
#define  BAD_FILE			3		// file operating error ( reading or writing)
#define  BAD_WRITING		4		// writing file error
#define  NO_MEMORY			5		// no enough memory
#define  NO_RECRESULT		6		//no recognized result
#define  SOFTLOCK_ERROR		7		//���������δ���ӣ������������δ��װ
#define  CHAR_NOINFO		8		//Getcharinfo()���ַ�û���ַ���Ϣ�����磺�ո��Ʊ�����س����з�
#define  CHAR_OUTOFTOTAL	9		//Getcharinfo()���������������ַ���
#define  CHAR_NOCHAR		10		//Getcharinfo(),��ҳû������ʶ����
#define  OTHER_ERROR		11		// errors not listed above
#define  DLL_ERROR			12		// dll load error
#define  PARA_ERROR			13		// ��������
#define  LOCATE_NONE		14		// ��������޽��

//////////////////////////////////////////////////////////////////////
//
// Exported Functions
//
//////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

// Function: HWOCRInit70
//			Initialize the Han Wang SDK, this function must be called before calling the other functions.
// Parameters:	
//			pEngineDir	-	The directory where the HWOCR engine is located.
//			Code		-	Recognition result code.
//			Language	-	Language showed on input image.
//			OutputSetting - Single or double layer pdf document.
// Return value:
//			The State Of The Return Value 
// Comments:
//			Code: ���ʶ������������
//					EXPORT_GBK,		GBK��
//					EXPORT_GB,		GB��,�������� (���)
//					EXPORT_BIG5,	BIG5�룬���� 
//					EXPORT_ENG,		��Ӣ��, ��Ӣ�Ĳ���ϵͳ�½���ʹ�ø�ѡ�����pdf����ļ��е�ĳЩ�ַ����ɼ���  
//
//			Language: ����ͼ���ĵ����ͣ����ò�ͬ��ʶ����ģ�
//					GB,   ���ڼ������Ļ���Ӣ����
//					BIG5, �������ĵ�
//					ENGLISH, ���ڴ�Ӣ���ĵ�, ʶ��Ӣ��Ч������GB
//
//			OutputSetting: ����ļ���ʽ����
//		            SINGLE_PDF, ����ļ���ʽΪpdfʱ�����������ʽΪ����pdf
//					DOUBLE_PDF, ����ļ���ʽΪpdf�����������˫��pdf�Ĺ���ʱ�����������ʽΪ˫��pdf

DLLEXPORT  ERRCODE STDCALL HWOCRInit70(char *pEngineDir, EXPORTCODE Code,LANGUAGE Language ,
									OUTPUTSETTING OutputSetting );

// Function: HWOCRTerm70
//			Termine the Han Wang OCR SDK
// Parameters:	
// Return value:
//			The state of the return value.	 
DLLEXPORT  ERRCODE STDCALL HWOCRTerm70();


// Function: HWOCRRecognizeFile70
//			Recognize an image file and save the result in pExportFile.	
//			HWOCRInit70 must be called before this function
// Parameters:	
//			pImageFile	-	The full path of an image file, the engine will read the image
//							according to the file name's extension, it can be in TIFF and BMP formats.
//			pExportFile	-	The full path of the output file, the filename extension must
//							be one of { txt, rtf, pdf, htm, xls}
//							����ʼ������HWOCRInit70����EXPORTCODEΪEXPORT_BIG5ʱ(����ʶ��)����֧�����pdf�ļ�. 				
//			bSkew		-	TRUE, to do image skew correct. ʶ��֮ǰ����ͼ����б����(���3������)��
//							FALSE, not to do  image skew correct
//			nOption		-	Three options can be used to initialize the SDK,	                
//							MULTICOL:	Specified that the document has multi-column layout, if this
//									option isn't specified, by default,SDK will regard the document
//									as one column text.
//									��ʾ�������ͼ��, ������2�����ϰ���鲢�ţ�
//							SINGLECOL:	����
//							OFFICIAL:	���ģ�1.2 �治֧�ֹ���ѡ�
// Return value:
//			The state of the return value.
// Comments:
//			Default setting	-	no skew correct, MULTICOL

DLLEXPORT  ERRCODE STDCALL HWOCRRecognizeFile70( char *pImageFile, char *pExportFile, 
											  BOOL bDeskew, int nOption );
 
// Function: HWOCRSetAttr70
//			Setting OCR environmental properties 
// Parameters:	
//			Code		-	Recognition result code.
//			Language	-	Language showed on input image.
//			OutputSetting - Single or double layer pdf document.
// Return value:
//			The state of the return value.
// Comments:

DLLEXPORT  ERRCODE STDCALL HWOCRSetAttr70( EXPORTCODE Code, LANGUAGE Language, 
										  OUTPUTSETTING OutputSetting  );

// Function: HWOCRSetPdfPressRatio70
//			Setting PDF Color image compression coefficient 
// Parameters:	
//			nPressRatio -	Compression coefficient 
// Return value:
//			The state of the return value.
// Comments:
//			nPressRatio:	1-100֮�������
DLLEXPORT ERRCODE STDCALL HWOCRSetPdfPressRatio70(int nPressRatio = 80);

// Function: HWOCRSetPdfImageType70
//			Setting PDF Color image compression type 
// Parameters:	
//			nICType -	Compression type 
//			1 :	JPEG
//			2 : JPEG 2000
// Return value:
//			The state of the return value.
DLLEXPORT ERRCODE STDCALL HWOCRSetPdfImageType70( int nICType  );

// Function: HWOCRGetImageOrientation70
//			Get orientation of the image
// Parameters:	
//			pImageFile	-	image file path name.
// Return value:
//			ͼ����:
//			-1��	ʧ��
//			0��		����
//			1��		˳ʱ��90��Ϊ��ȷ����
//			2��		180�ȷ���
//			3��		˳ʱ��270��Ϊ��ȷ����

DLLEXPORT int STDCALL HWOCRGetImageOrientation70(char *pImageFile);

// Function: HWOCRSetImageColorInvert70
//			Set Image Color Invert Flag
// Parameters:	
//			nFlag	-	-1: ǿ�Ʒ�ɫ, 
//						 0: �Զ���⣬������������з�ɫ, 
//						 1: ������ɫ����
//
DLLEXPORT  ERRCODE STDCALL HWOCRSetImageColorInvert70( int nFlag );

#ifdef __cplusplus
}
#endif

#endif
