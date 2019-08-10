#ifndef __HWOCRSDK
#define __HWOCRSDK

//2003.12.24
//dy
//����


#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#define  MULTICOL	0x0000	//Multi colum ;			����
#define  SINGLECOL 	0x0001	//single colum ;		����
#define  OFFICIAL	0x0002	//official document ;	����(��ɫͼ�����Ч)


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

//���������
enum STYLE{
	HOR_TEXT,  //Horizontal text zone
	VER_TEXT,  //Vertical text zone
	ENG_TEXT,  //English zone in Chinese document
	FORM,      // Form zone
	PICTURE,   //Picture zone
	SEAL,      //����, ���Ժ������ı����ص�, һ��ͼ��ֻ����1��
};

enum LANGUAGE{
	GB,
	BIG5,
	ENGLISH,
	JPN,    //����
	KOREA,  //����
};

enum EXPORTCODE
{
	EXPORT_GBK,EXPORT_GB,EXPORT_BIG5,EXPORT_ENG, 
	EXPORT_SHIFT_JIS, //����
	EXPORT_HANGUL, //����
};

// added by wangweina 2006.12.7
// ������ã�SINGLE_PDF ����pdf�� DOUBLE_PDF ˫��pdf
//           UNICODE_TXT,unicode�����txt
//           UTF8_TXT��utf-8�����txt
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
//char	hex		Font		����  
//'0'   0x30	SongTi		����
//'1'	0x31	FangSong	����
//'2'	0x32	KaiTi		����
//'3'	0x33	HeiTi		����
//'4'	0x34	YouYuan		��Բ
//'5'	0x35	LiShu		����

//about nConfidence
//'A'        ����
//'D'        ����
//'E'������  ��ʶ������


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
	SOFTLOCK_ERROR, //���������δ���ӣ������������δ��װ
	CHAR_NOINFO,  //Getcharinfo()���ַ�û���ַ���Ϣ�����磺�ո��Ʊ�����س����з�
	CHAR_OUTOFTOTAL, //Getcharinfo()���������������ַ���
	CHAR_NOCHAR,   //Getcharinfo(),��ҳû������ʶ����
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

		Code      : ���ʶ������������
					EXPORT_GBK, GBK��
					EXPORT_GB,  GB��,�������� (���)
					EXPORT_BIG5,BIG5�룬���� 
					EXPORT_ENG, ��Ӣ��, ��Ӣ�Ĳ���ϵͳ�½���ʹ�ø�ѡ�
								����pdf����ļ��е�ĳЩ�ַ����ɼ���  

	    Language  : ����ͼ���ĵ����ͣ����ò�ͬ��ʶ����ģ�
					GB,   ���ڼ������Ļ���Ӣ����
					BIG5, �������ĵ�
					ENGLISH, ���ڴ�Ӣ���ĵ�, ʶ��Ӣ��Ч������GB

        OutputSetting: ����ļ���ʽ����
		               SINGLE_PDF, ����ļ���ʽΪpdfʱ�����������ʽΪ����pdf
					   DOUBLE_PDF, ����ļ���ʽΪpdf�����������˫��pdf�Ĺ���ʱ��
					                ���������ʽΪ˫��pdf
		               

  ע�⣺Language Ϊ ENGLISHʱ���벻Ҫ��η�������HWOCRInit(), 
  ����������ʾ�� RTK initialize failure. ����Ӣ��ʶ����ĵ�bug.

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
					����ʼ������HWOCRInit����EXPORTCODEΪEXPORT_BIG5ʱ(����ʶ��)����֧�����pdf�ļ�. 
					
		bSkew:      TRUE, to do image skew correct. ʶ��֮ǰ����ͼ����б����(���3������)��
					FALSE, not to do  image skew correct

	   nOption    : Two options can be used to initialize the SDK,
	                
					MULTICOL: ������Specified that the document has multi-column layout, if this
					          option isn't specified, by default,SDK will regard the document
							  as one column text.��ʾ�������ͼ��, ������2�����ϰ���鲢�ţ�
	                SINGLECOL: ����
					OFFICIAL: ���ģ�1.2 �治֧�ֹ���ѡ�


ȱʡ�趨�� ����бУ���� ����


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
		bDeskew:   TRUE, to do image skew correct. ͼ����б����(���3������)��
					FALSE, not to do  image skew correct
	   nOption    : Two options can be used to initialize the SDK,
	                
					MULTICOL: ������Specified that the document has multi-column layout, if this
					          option isn't specified, by default,SDK will regard the document
							  as one column text.��ʾ�������ͼ��, ������2�����ϰ���鲢�ţ�
	                SINGLECOL: ����
					OFFICIAL: ���ģ�1.2 �治֧�ֹ���ѡ�

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
	SUCCESS,     //�ɹ�	
	CHAR_NOINFO,  //Getcharinfo()���ַ�û���ַ���Ϣ�����磺�ո��Ʊ�����س����з�
	CHAR_OUTOFTOTAL, //Getcharinfo()���������������ַ���
	CHAR_NOCHAR,   //Getcharinfo(),��ҳû������ʶ����

*/



__declspec(dllexport)  
ERRCODE WINAPI HWOCRGetRecoTxtNum(
int &nTotal,
int &nDubiousCount);
/*
Function: Calculate suspicious char number and total char number after recognition.
		���ر�ҳʶ�������ַ������Ϳ�����������
Parameter:
         nTotal: total char count.�Ʊ�����س����кͿո񲻼�������!!!(��Ϊ�ո񡢻س����к��Ʊ��û�п��Ŷ�)
		 nDubiousCount: Dubious char count.

		int &nTotal: ����ʶ�����ַ�������˫�ֽں�������1���ַ�
		int &nDubiousCount:���ؿ������ַ�������˫�ֽں�������1���ַ�
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
�ͷ��ڴ�
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


//������һ
/*
	if ( HWOCRInit(szBuf, EXPORT_GB, ENGLISH ) != SUCCESS ) //ʶ��Ӣ���ĵ�
	{
		AfxMessageBox("OCR engine init error!");
		return;
	}

	int nCount;
	nCount = 0;
	
	while (nCount++ < 1 ) //ʶ����� 1 ��, ��ѭ�����
	{
		
		HWOCRRecognizeFile(
			"c:\\test1.tif",
			"c:\\test1.rtf", 
			DESKEW | MULTICOL
			);

	}
		  
	HWOCRTerm(); //����SDK �ͷ��ڴ��, ����!!! 

*/


#ifdef __cplusplus
}
#endif

#endif