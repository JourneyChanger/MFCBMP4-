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

#define  MULTICOL	0x0000	//Multi colum ;			多栏
#define  SINGLECOL 	0x0001	//single colum ;		单栏
#define  OFFICIAL	0x0002	//official document ;	公文(彩色图像才有效)

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

// 版面块属性
typedef int STYLE;

#define  HOR_TEXT	0		// Horizontal text zone
#define  VER_TEXT	1		// Vertical text zone
#define  ENG_TEXT	2		// English zone in Chinese document
#define  FORM		3		// Form zone
#define  PICTURE	4		// Picture zone
#define  SEAL		5		// 红章, 可以和其它文本块重叠, 一幅图像只能有1个

// 语言设置
typedef int LANGUAGE;

#define  GB			0		// 简体
#define  BIG5		1		// 繁体
#define  ENGLISH	2		// 英语
#define  JPN		3		// 日文
#define  KOREA		4		// 韩文
#define  FINNISH	5		// 芬兰
#define  FRENCH		6		// 法语
#define  GERMAN		7		// 德语
#define  SPANISH	8		// 西班牙语
#define  ITALIAN	9		// 意大利语
#define  SWEDISH	10		// 瑞典语
#define  DANISH		11		// 丹麦语
#define	 NORWEGIAN	12		// 挪威语
#define  DUTCH		13		// 荷兰语
#define  PORTUGUESE	14		// 葡萄牙语

// Export Code
typedef int EXPORTCODE;

#define  EXPORT_GBK			0	
#define  EXPORT_GB			1		
#define  EXPORT_BIG5		2		
#define  EXPORT_ENG			3		
#define  EXPORT_SHIFT_JIS	4	//日文
#define  EXPORT_HANGUL		5	//韩文
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
#define  SOFTLOCK_ERROR		7		//软件加密锁未连接，或加密锁驱动未安装
#define  CHAR_NOINFO		8		//Getcharinfo()该字符没有字符信息，比如：空格，制表符，回车换行符
#define  CHAR_OUTOFTOTAL	9		//Getcharinfo()传入索引超过总字符数
#define  CHAR_NOCHAR		10		//Getcharinfo(),本页没有文字识别结果
#define  OTHER_ERROR		11		// errors not listed above
#define  DLL_ERROR			12		// dll load error
#define  PARA_ERROR			13		// 参数错误
#define  LOCATE_NONE		14		// 版面分析无结果

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
//			Code: 输出识别结果编码类型
//					EXPORT_GBK,		GBK码
//					EXPORT_GB,		GB码,简体中文 (最常用)
//					EXPORT_BIG5,	BIG5码，繁体 
//					EXPORT_ENG,		纯英文, 在英文操作系统下建议使用该选项，否则pdf输出文件中的某些字符不可见。  
//
//			Language: 输入图像文档类型（调用不同的识别核心）
//					GB,   用于简体中文或中英混排
//					BIG5, 繁体字文档
//					ENGLISH, 用于纯英文文档, 识别英文效果好于GB
//
//			OutputSetting: 输出文件格式设置
//		            SINGLE_PDF, 输出文件格式为pdf时，设置输出格式为单层pdf
//					DOUBLE_PDF, 输出文件格式为pdf，并且有输出双层pdf的功能时，设置输出格式为双层pdf

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
//							当初始化函数HWOCRInit70参数EXPORTCODE为EXPORT_BIG5时(繁体识别)，不支持输出pdf文件. 				
//			bSkew		-	TRUE, to do image skew correct. 识别之前进行图像倾斜矫正(倾角3度以内)。
//							FALSE, not to do  image skew correct
//			nOption		-	Three options can be used to initialize the SDK,	                
//							MULTICOL:	Specified that the document has multi-column layout, if this
//									option isn't specified, by default,SDK will regard the document
//									as one column text.
//									表示处理多栏图像, 即存在2个以上版面块并排；
//							SINGLECOL:	单栏
//							OFFICIAL:	公文（1.2 版不支持公文选项）
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
//			nPressRatio:	1-100之间的整数
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
//			图像方向:
//			-1，	失败
//			0，		正向
//			1，		顺时针90度为正确方向
//			2，		180度方向
//			3，		顺时针270度为正确方向

DLLEXPORT int STDCALL HWOCRGetImageOrientation70(char *pImageFile);

// Function: HWOCRSetImageColorInvert70
//			Set Image Color Invert Flag
// Parameters:	
//			nFlag	-	-1: 强制反色, 
//						 0: 自动检测，如条件满足进行反色, 
//						 1: 不做反色处理
//
DLLEXPORT  ERRCODE STDCALL HWOCRSetImageColorInvert70( int nFlag );

#ifdef __cplusplus
}
#endif

#endif
