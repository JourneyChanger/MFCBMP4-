#ifndef __HWOCRSDK
#define __HWOCRSDK

//2003.12.24
//dy
//公文


#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#define  MULTICOL	0x0000	//Multi colum ;			多栏
#define  SINGLECOL 	0x0001	//single colum ;		单栏
#define  OFFICIAL	0x0002	//official document ;	公文(彩色图像才有效)


/*
struct IMGRGN is used to record the rect infomation 
(size, position, layout style)
recognized by Function HWOCRLocate
 or indicated by users
*/
typedef struct {
	RECT  rect;
	enum STYLE style;
} IMGRGN;

//版面块属性
enum STYLE{
	HOR_TEXT,  //Horizontal text zone
	VER_TEXT,  //Vertical text zone
	ENG_TEXT,  //English zone in Chinese document
	FORM,      // Form zone
	PICTURE,   //Picture zone
	SEAL,      //红章, 可以和其它文本块重叠, 一幅图像只能有1个
};

enum LANGUAGE{
	GB,
	BIG5,
	ENGLISH,
	JPN,    //日文
	KOREA,  //韩文
};

enum EXPORTCODE
{
	EXPORT_GBK,EXPORT_GB,EXPORT_BIG5,EXPORT_ENG, 
	EXPORT_SHIFT_JIS, //日文
	EXPORT_HANGUL, //韩文
};

// added by wangweina 2006.12.7
// 输出设置，SINGLE_PDF 单层pdf， DOUBLE_PDF 双层pdf
//           UNICODE_TXT,unicode编码的txt
//           UTF8_TXT，utf-8编码的txt
enum OUTPUTSETTING
{
	SINGLE_PDF, DOUBLE_PDF,UNICODE_TXT, UTF8_TXT
};


typedef  struct{
	RECT  rcChar;	//locate and size of char
	BYTE  nFont;
	WORD  wCandidate[10];	//all of the candidate char
	BYTE  nConfidence; //The confidence of the first character in WCandidate
} CHARINFO;

//about nFont value:
//char	hex		Font		字体  
//'0'   0x30	SongTi		宋体
//'1'	0x31	FangSong	仿宋
//'2'	0x32	KaiTi		楷体
//'3'	0x33	HeiTi		黑体
//'4'	0x34	YouYuan		幼圆
//'5'	0x35	LiShu		隶书

//about nConfidence
//'A'        可信
//'D'        可疑
//'E'或其它  拒识不可信


typedef struct
{
	RECT	rcBound;	// region bounds relative to whole image, pixel unit
	BYTE	iAttribute;	// RGNTYPE_xxx region type
	BYTE	iField;		// field index (only used DOCOCR version)
}
TW_REGION, FAR * TW_LPREGION;

typedef struct 
{
	RECT	rcBlock;			// block rectangle in image
	BYTE	nFontType;			// Font type information
	WORD	wCandidate[5];		// Candidate, first is result
	BYTE	nConfidence[5];		// Confidence
}
TW_CHARINFO, * TW_LPCHARINFO;

typedef enum {
	SUCCESS, 
	INIT_ERROR,  //initialization failed
	UNSUPPORT_IMAGE,//unsupported image format
	BAD_FILE,  // file operating error ( reading or writing)
	BAD_WRITING,// writing file error
	NO_MEMORY,  // no enough memory
	NO_RECRESULT,//no recognized result
	SOFTLOCK_ERROR, //软件加密锁未连接，或加密锁驱动未安装
	CHAR_NOINFO,  //Getcharinfo()该字符没有字符信息，比如：空格，制表符，回车换行符
	CHAR_OUTOFTOTAL, //Getcharinfo()传入索引超过总字符数
	CHAR_NOCHAR,   //Getcharinfo(),本页没有文字识别结果
	OTHER_ERROR // errors not listed above
} ERRCODE;  //return code

__declspec(dllexport)  ERRCODE WINAPI 
HWOCRInit(char *pEngineDir, EXPORTCODE Code,LANGUAGE Language ,
		  OUTPUTSETTING OutputSetting = SINGLE_PDF);
 /*
  Function: 
        Initialize the Han Wang SDK, this function must be called
		before calling the other functions.

  Parameter:  
        pEngineDir: The directory where the HWOCR engine is located
		Code      : Recognition result code
		Language  : Language showed on input image

		Code      : 输出识别结果编码类型
					EXPORT_GBK, GBK码
					EXPORT_GB,  GB码,简体中文 (最常用)
					EXPORT_BIG5,BIG5码，繁体 
					EXPORT_ENG, 纯英文, 在英文操作系统下建议使用该选项，
								否则pdf输出文件中的某些字符不可见。  

	    Language  : 输入图像文档类型（调用不同的识别核心）
					GB,   用于简体中文或中英混排
					BIG5, 繁体字文档
					ENGLISH, 用于纯英文文档, 识别英文效果好于GB

        OutputSetting: 输出文件格式设置
		               SINGLE_PDF, 输出文件格式为pdf时，设置输出格式为单层pdf
					   DOUBLE_PDF, 输出文件格式为pdf，并且有输出双层pdf的功能时，
					                设置输出格式为双层pdf
		               

  注意：Language 为 ENGLISH时，请不要多次反复调用HWOCRInit(), 
  否则会出现提示： RTK initialize failure. 这是英文识别核心的bug.

 */

__declspec(dllexport)  ERRCODE WINAPI HWOCRSetAttr( 
EXPORTCODE Code,
LANGUAGE Language,
OUTPUTSETTING OutputSetting = SINGLE_PDF
 );

    
//__declspec(dllexport)  ERRCODE WINAPI
//HWOCRRecognizeFile( char *pImageFile, char *pExportFile, int nOption);
__declspec(dllexport)  ERRCODE WINAPI
HWOCRRecognizeFile( char *pImageFile, char *pExportFile, BOOL bDeskew, int nOption );
 /*
 Function: 
	   Recognize an image file and save the result in pExportFile.	
	   HWOCRInit must be called before this function


 Parameter:
       pImageFile : The full path of an image file, the engine will read the image
	                according to the file name's extension, it can be in TIFF and
					BMP formats.
	   pExportFile: The full path of the output file, the filename extension must
	                be one of { txt, rtf, pdf, htm, xls}
					当初始化函数HWOCRInit参数EXPORTCODE为EXPORT_BIG5时(繁体识别)，不支持输出pdf文件. 
					
		bSkew:      TRUE, to do image skew correct. 识别之前进行图像倾斜矫正(倾角3度以内)。
					FALSE, not to do  image skew correct

	   nOption    : Two options can be used to initialize the SDK,
	                
					MULTICOL: 多栏，Specified that the document has multi-column layout, if this
					          option isn't specified, by default,SDK will regard the document
							  as one column text.表示处理多栏图像, 即存在2个以上版面块并排；
	                SINGLECOL: 单栏
					OFFICIAL: 公文（1.2 版不支持公文选项）


缺省设定： 不倾斜校正， 多栏


*/




__declspec(dllexport)  ERRCODE WINAPI
 HWOCROpenImageFile(char *pImageFile );
/*
 Function:  
        Load an image file into memory.
		User should call HWOCRCloseImage() to free the image,
		or the image will be retained in memory.

 Parameter:
        pImageFile: Contain the full path of an input image file
*/

__declspec(dllexport)  BOOL WINAPI
HWOCROpenImage( HANDLE hDib );
/*
 Function:  
        Load an image into memory.
		User should call HWOCRCloseImage() to free the image,
		or the image will be retained in memory.

 Parameter:
        hDib: Handle to DIB
*/


//__declspec(dllexport)  ERRCODE WINAPI
// HWOCRLocate( int nMaxRgnNum, int &nImgRgnNum, IMGRGN *pImgRgn, int nOption  );
__declspec(dllexport)  ERRCODE WINAPI
 HWOCRLocate( int nMaxRgnNum, int &nImgRgnNum, IMGRGN *pImgRgn, BOOL bDeskew, int nOption  );
/*
Function:
      Layout analysis. User should use HWOCROpenImageFile() to load
	  an image into memory before this action.

parameter:
      nMaxRgnNum: Input parameter,specify the size of pImgRgn
      nImgRgnNum: Output parameter, contain the total number of regions
	  pImgRgn:    Output parameter, user must allocate enough memory 
	              before calling the function
		bDeskew:   TRUE, to do image skew correct. 图像倾斜矫正(倾角3度以内)。
					FALSE, not to do  image skew correct
	   nOption    : Two options can be used to initialize the SDK,
	                
					MULTICOL: 多栏，Specified that the document has multi-column layout, if this
					          option isn't specified, by default,SDK will regard the document
							  as one column text.表示处理多栏图像, 即存在2个以上版面块并排；
	                SINGLECOL: 单栏
					OFFICIAL: 公文（1.2 版不支持公文选项）

*/

__declspec(dllexport)  ERRCODE WINAPI
 HWOCRRecognize( int nImgRgnNum, IMGRGN *pImgRgn );
/*
Function: Recognize the loaded image in regions specified by pImgRgn.
          

Parameter:
         nImgRgnNum: The region total number
		 pImgRgn:    regions.
*/

__declspec(dllexport)  ERRCODE WINAPI HWOCRGetRecoTxt(
int &nTxtLen ,
char *pTxtBuf );
/*
Function: Get recognize result 
          
Parameter:
         nTxtLen: as input parameter,specify the buffer length, 
				  as output parameter,return the actual length of the result
		 pTxtBuf: buffer allocated by caller to store the result text

return:
	SUCCESS,     //成功	
	CHAR_NOINFO,  //Getcharinfo()该字符没有字符信息，比如：空格，制表符，回车换行符
	CHAR_OUTOFTOTAL, //Getcharinfo()传入索引超过总字符数
	CHAR_NOCHAR,   //Getcharinfo(),本页没有文字识别结果

*/



__declspec(dllexport)  
ERRCODE WINAPI HWOCRGetRecoTxtNum(
int &nTotal,
int &nDubiousCount);
/*
Function: Calculate suspicious char number and total char number after recognition.
		返回本页识别结果的字符总数和可疑字总数，
Parameter:
         nTotal: total char count.制表符，回车换行和空格不计算在内!!!(因为空格、回车换行和制表符没有可信度)
		 nDubiousCount: Dubious char count.

		int &nTotal: 返回识别结果字符总数，双字节汉字算作1个字符
		int &nDubiousCount:返回可疑字字符总数，双字节汉字算作1个字符
*/



__declspec(dllexport)  ERRCODE WINAPI HWOCRExportToFile(
char *pFileName);
/*
Function: save recognize result to file
          
Parameter:
		pFileName:  result file name
*/

	              
__declspec(dllexport)  ERRCODE WINAPI
 HWOCRCloseImage( );
/*
Function: Free the image loaded by  HWOCROpenImageFile(.)

*/


__declspec(dllexport)  ERRCODE WINAPI HWOCRTerm();
/*
Function: termine the Han Wang OCR SDK
释放内存
*/
      


 
__declspec(dllexport)  ERRCODE WINAPI HWOCROpenInfo( char *pImageFile );
/*
Function: Prepare to get the layout and recognition information

Parameter:
         pImageFile: the full path of an image file
*/
     
__declspec(dllexport)  ERRCODE WINAPI
HWOCRGetLayoutInfo( int nMaxRgn, int &nRgnNum, IMGRGN *pImgRgn );
/*
Function: Get the layout  information, see also HWOCROpenInfo(.)

Parameter:
       nMaxRgn: Input parameter, limit the maximum number of regions to get
       nRgnNum: Output parameter, return the total number of regions.
	   pImgRgn: regions
*/


//__declspec(dllexport)  ERRCODE WINAPI HWOCRGetCharInfoCount( DWORD & dwCharCount);

__declspec(dllexport)  ERRCODE WINAPI 
HWOCRGetCharInfo( DWORD dwCharNo, CHARINFO &CharInfo );
/*
Function: Get the character information, see also HWOCROpenInfo(.)

Parameter:
       dwCharNo: Input parameter, specify the number of character's 
	             information to get.
	   CharInfo: Output parameter, contain the character information	 
*/

__declspec(dllexport)  ERRCODE WINAPI HWOCRCloseInfo();
/*
Function: End of geting information, the function must be called before
          geting another image's information using HWOCROpenInfo.
*/



__declspec(dllexport)  ERRCODE WINAPI 
HWOCRSetLayoutInfo( char *pImageFile,int nWidth, 
						   int nHeight,int nRgnNum,
							IMGRGN *pImgRgn );
/*
Function: Set the layout information to an image file

Parameter:
          pImageFile: A black and white image file
		  nWidth:     Width of the image in pixel
		  nHeight:    Height of the image in pixel
		  nRgnNum:    The total number of regions
		  pImgRgn:    The pointer to region
*/


__declspec(dllexport)  BOOL WINAPI HWOCRIsModified(char *pImgFile);
/*
Function: Checking if the image has been modified during 
          the recognition process. If modified, the result 
		  image is saved as the same name as pImgFile with .bki
		  extension, this bki file is in the same directory 
		  with the bmp file. and the BKI file is in TIFF format 
		  in fact.
Parameter:
          pImgFile: Image file to be checked
*/

__declspec(dllexport) ERRCODE WINAPI HW_Start(int nCode,int nFont,BOOL bOutFull,HINSTANCE hApplicationInstance);
__declspec(dllexport) ERRCODE WINAPI HW_RecogRegionEx(
	LPBYTE * lpLines, int nWidth, int nHeight, 
	TW_LPREGION lpRegion,
	TW_LPCHARINFO lpCharInfo, int&nCharInfoSize, 
	int nLine, int nChar);
__declspec(dllexport) void  WINAPI HW_End(void);


//调用例一
/*
	if ( HWOCRInit(szBuf, EXPORT_GB, ENGLISH ) != SUCCESS ) //识别英文文档
	{
		AfxMessageBox("OCR engine init error!");
		return;
	}

	int nCount;
	nCount = 0;
	
	while (nCount++ < 1 ) //识别、输出 1 次, 可循环多次
	{
		
		HWOCRRecognizeFile(
			"c:\\test1.tif",
			"c:\\test1.rtf", 
			DESKEW | MULTICOL
			);

	}
		  
	HWOCRTerm(); //结束SDK 释放内存等, 必须!!! 

*/


#ifdef __cplusplus
}
#endif

#endif