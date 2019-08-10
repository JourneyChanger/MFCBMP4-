// PDF2Pic.cpp : 定义控制台应用程序的入口点。
//

#include "pch.h"

#include "PDF2Pic.h"
#include <direct.h>  
#include <io.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

//CWinApp theApp;

using namespace std;

typedef   int   (WINAPI *GS_NEW_INSTANCE)(void **pinstance, void *caller_handle);
typedef   int   (WINAPI *GS_INIT_WITH_ARGS)(void  *pinstance, int argc, const char ** argv);
typedef   int   (WINAPI *GS_EXIT)(void *pinstance);
typedef   void   (WINAPI *GS_DELETE_INSTANCE)(void *pinstance);

#define string std::string

//*tchar是TCHAR类型指针，*_char是char类型指针   
//void TcharToChar (const TCHAR * tchar, char * _char)  
//{  
//	int iLength ;  
//	//获取字节长度   
//	iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);  
//	//将tchar值赋给_char    
//	WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);   
//}  

BOOL CopyFile(CString strOld, CString strNew)
{
	USES_CONVERSION;
	FILE* oldFile;
	FILE* newFile;

	_unlink(strNew);
	oldFile = fopen(strOld, "rb");
	if( oldFile == NULL ) return FALSE;
	newFile = fopen(strNew, "wb");
	if( newFile == NULL ) 
	{
		fclose(oldFile);
		return FALSE;
	}

	fseek(oldFile, 0, SEEK_END);
	DWORD dwLen = ftell(oldFile);
	fseek(oldFile, 0, SEEK_SET);
	int size = 10240;
	char* szBuf = new char[size + 1];
	DWORD len = 0;
	while( len < dwLen )
	{
		DWORD ret = fread(szBuf, 1, size, oldFile);
		fwrite(szBuf, 1, ret, newFile);

		len += ret;
	}

	fclose(oldFile);
	fclose(newFile);

	delete [] szBuf;
	return TRUE;
}

int PDFToPic(const char* szPDFName, const char* szOutDir, const char* szType, int& nCt)
{	
	HINSTANCE   hDllInst; 
	USES_CONVERSION;
	hDllInst = LoadLibrary("PDFTool.dll"); 
	if(hDllInst == NULL) 
	{
		printf("Load Dll Error\n");
		return -1;
	} 

	GS_NEW_INSTANCE gsapi_new_instance; 
	gsapi_new_instance = (GS_NEW_INSTANCE)GetProcAddress(hDllInst, "gsapi_new_instance"); 
	if(gsapi_new_instance == NULL) 
	{
		printf("Get Function From DLL Error\n");
		return -1;
	}
	GS_INIT_WITH_ARGS gsapi_init_with_args = (GS_INIT_WITH_ARGS)GetProcAddress(hDllInst, "gsapi_init_with_args"); 
	if(gsapi_init_with_args == NULL) 
	{
		printf("Get Function From DLL Error\n");
		return -1;
	}
	GS_EXIT gsapi_exit = (GS_EXIT)GetProcAddress(hDllInst, "gsapi_exit"); 
	if(gsapi_exit == NULL) 
	{
		printf("Get Function From DLL Error\n");
		return -1;
	}
	GS_DELETE_INSTANCE gsapi_delete_instance = (GS_DELETE_INSTANCE)GetProcAddress(hDllInst, "gsapi_delete_instance"); 
	if(gsapi_delete_instance == NULL) 
	{
		printf("Get Function From DLL Error\n");
		return -1;
	}

	if(access(szOutDir, 0) == -1)
	{
		int ret = mkdir(szOutDir);
		if(ret == -1)
		{
			printf("Dir Create Error\n");
			return -1;
		}
	}

	CString strPDF;
	strPDF.Format(_T("%s"), szPDFName);
	int n = strPDF.ReverseFind('\\');
	if( n < 0 ) return -1;

	CString strFile = strPDF.Mid(n + 1);
	CString strDir = strPDF.Left(n);
	CString strTmp;
	strTmp.Format(_T("%s\\tmp.pdf"), strDir);
	if( !CopyFile(strPDF, strTmp) )
		return -1;

	int   code; 
	void* minst;	
	const char* gsargv[20]; 
	int gsargc; 
	char str1[100];
	char str2[100];
	char str3[1000];
	char str4[20];
	char szOutPutFile[1000];

	sprintf(str4, "-sDEVICE=%s", szType);
	if(strcmp(szType, "jpeg")
		&& strcmp(szType, "bmp256"))
	{
		printf("The Type Must Be jpeg or bmp256\n");
		return -1;
	}

	gsargv[0] = "pdf2jpg"; /* actual value doesn't matter */ 
	gsargv[1] = "-dNOPAUSE"; 
	gsargv[2] = "-dBATCH"; 
	gsargv[3] = "-dPARANOIDSAFER"; 
	//gsargv[4] = "-sDEVICE=bmp256";
	gsargv[4] = str4;
	gsargv[5] = "-q"; 
	gsargv[6] = "-dQUIET"; 
	gsargv[7] = "-dNOPROMPT"; 
	gsargv[8] = "-dMaxBitmap=500000000"; 
	gsargv[9] = str1; 
	gsargv[10] = str2;
	gsargv[11] = "-dAlignToPixels=0"; 
	gsargv[12] = "-dGridFitTT=0"; 
	gsargv[13] = "-dTextAlphaBits=4"; 
	gsargv[14] = "-dGraphicsAlphaBits=4"; 
	gsargv[15] = "-dBMP256Q=100"; 
	gsargv[16] = "-dPDFFitPage"; 
	gsargv[17] = "-r300";
	gsargv[18] = str3; 
	gsargv[19] = strTmp; 
	gsargc=20; 

	//循环遍历所有页数
	int i;
	int count = 0;
	for(i = 1; ; i++) 
	{
		sprintf(str1, "-dFirstPage=%d", i);
		sprintf(str2, "-dLastPage=%d", i);	

		if(strcmp(szType, "jpeg") == 0)
			sprintf(szOutPutFile, "%s\\%d.jpg", szOutDir, i);
		else 
			sprintf(szOutPutFile, "%s\\%d.bmp", szOutDir, i);

		sprintf(str3, "-sOutputFile=%s", szOutPutFile);

		code = (*gsapi_new_instance)(&minst, NULL); 
		code = gsapi_init_with_args(minst, gsargc, gsargv); 
		code = gsapi_exit(minst); 
		gsapi_delete_instance(minst); 

		//判断是否到了页尾
		if (_access(szOutPutFile,0) != 0) break;
		count++;
	} ;	

	nCt = count;


	_unlink(strTmp);
	if( count <= 0 )
	{
		//释放DLL
		FreeLibrary(hDllInst); 
		printf(("转换失败\n"));
		return -1;
	}

	CString strOld, strNew;
	strFile = strFile.Left(strFile.GetLength() - 4);
	for(i = 1; i <= count; i++)
	{
		if(strcmp(szType, "jpeg") == 0)
		{
			strOld.Format(_T("%s\\%d.jpg"), A2W(szOutDir), i);
			strNew.Format(_T("%s\\%s_%d.jpg"), A2W(szOutDir), strFile, i);
		}
		else 
		{
			strOld.Format(_T("%s\\%d.bmp"), A2W(szOutDir), i);
			strNew.Format(_T("%s\\%s_%d.bmp"), A2W(szOutDir), strFile, i);
		}
		CopyFile(strOld, strNew);
		_unlink(strOld);

	}

	//释放DLL
	FreeLibrary(hDllInst); 
	printf(("转换成功\n"));
	return 1;
}
	

int PDF2Pic(CString pos, CString name, int& nCt)
{
	int nRetCode = 0;

	//// 初始化 MFC 并在失败时显示错误
	//if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	//{
	//	// TODO: 更改错误代码以符合您的需要
	//	_tprintf(_T("错误: MFC 初始化失败\n"));
	//	nRetCode = 1;
	//}
	//else
	//{
	//	// TODO: 在此处为应用程序的行为编写代码。
	//}


						//if(argc != 4)
						//{
						//	printf("用法: PDF2Pic PDF全路径名 输出图片格式（j/b) 输出图片保存路径\n");
						//	return 1;
						//}

	//char szPDFName[260];
	char* szPDFName = pos.GetBuffer();
	char szOutDir[260];
	char* szName = name.GetBuffer();
	//char szName[260];
	//char szType[10];

	//memset(szPDFName, 0, sizeof(char)*260);
	memset(szOutDir, 0, sizeof(char)*260);
	//memset(szType, 0, sizeof(char)*10);
	//memset(szName, 0, sizeof(char)*10);

	//TcharToChar(pos, szPDFName);
	//TcharToChar(name, szName);
	szPDFName = pos.GetBuffer();
	strcpy(szOutDir, szPDFName);
	strcpy(strstr(szOutDir, szName),"temp\\");


						//TcharToChar(argv[2], szType);

						//strlwr(szType);
						//if(strcmp(szType, "j") && strcmp(szType, "b"))
						//{
						//	printf("输出图片格式须为 j或b！\n");
						//	return -1;
						//}
						//if(!strcmp(szType, "j"))
						//	strcpy_s(szType, 8, "jpeg");
						//else 
						//	strcpy_s(szType, 8, "bmp256");


	PDFToPic(szPDFName, szOutDir, "bmp256", nCt);

	return nRetCode;
}
