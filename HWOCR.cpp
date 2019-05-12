#include "pch.h"
#include "MFCBMP4.h"
#include "HWOCR.h"
#include "Dib.h"
#include "FilterWord.h"

#ifdef HW_TEST_VERSION
#pragma comment(lib, "hwsdk.lib")
#else
#pragma comment(lib, "hwsdk70sp.lib")
#endif

extern CMFCBMP4App theApp;;

using namespace NCUT_OCR;

CHWOCR::CHWOCR()
{
	m_bInitialized = FALSE;
	m_strError = "";

	m_dblRate = 0;  // 2010.06.22
}

CHWOCR::~CHWOCR()
{
}
#define MY_TEST
BOOL CHWOCR::Initialize(HWND hWnd)
{
	///////////��ʼ��OCR//////////
	//	EXPORTCODE Code = EXPORT_GB;//������ֵ�����,EXPORT_GB,  GB��,�������� (���)	
	USES_CONVERSION;
	TCHAR szBuf[256];
	sprintf_s(szBuf, "%s\\Bin", theApp.GetWorkDirectory());

	CString str;
//	theApp.GetRegistryValue(_T("OCR"), _T("BinPath"), str, szBuf);
	strcpy_s(szBuf, 256, str);

	//!!!Ocr engine and your app must be installed in the same directory
	ERRCODE errCode;
	EXPORTCODE Code = EXPORT_GBK;  // 2012.06.03
	LANGUAGE Language = GB; // 2012.06.03
#ifdef HW_TEST_VERSION
	errCode = HWOCRInit(W2A(szBuf), Code, Language, SINGLE_PDF);	
#else
#ifdef MY_TEST
	strcpy_s(szBuf, 256, "E:\\Bin - 2");
	strcpy_s(szBuf, 256, "E:\\Develop\\IntelliReader(F2)\\Bin - 2");

	CString m_strThisDir;
	char a[260];
	GetModuleFileName(0, a, 260);
	m_strThisDir = a;
	m_strThisDir = m_strThisDir.Left(m_strThisDir.Find("Debug"));
	m_strThisDir += "Bin - 2";

	strcpy_s(szBuf, 256, m_strThisDir);

//  strcpy_s(szBuf, 256, "D:\\Desktop\\λͼ\\Bin - 2");
//	strcpy_s(szBuf, 256, "E:\\0-��ѧ\\��Ŀѵ��\\Bin - 2");
//	strcpy_s(szBuf, 256, "G:\\Develop\\Bin - 2");
//	strcpy_s(szBuf, 256, "E:\\��Ŀ\\JournalData��װ\\Bin");
	
#endif
	errCode = HWOCRInit70((szBuf), Code, Language, SINGLE_PDF);	
#endif
	if (errCode != SUCCESS)
	{
#ifdef ENGLISH_VERSION //JIANGJIN_20120605
		m_strError = _T("OCR Initializing failed!");
#else
		m_strError = _T("OCR��ʼ��ʧ��!");
//		AfxMessageBox(m_strError);
#endif

		return FALSE;
	}

	m_bInitialized = TRUE;
	return TRUE;
}

void CHWOCR::Uninitialize()  // 2012.01.12
{
	if( m_bInitialized )
	{
#ifdef HW_TEST_VERSION
		HWOCRTerm(); // �����ر�
#else
		HWOCRTerm70(); // �����ر�
#endif

		m_bInitialized = FALSE;
	}
}

double CHWOCR::GetRecoRate() // 2010.06.17 ���ʶ����
{
	return m_dblRate;
}

BOOL CHWOCR::Layout(const CDib& dib, int nMaxRgnNum, int &nImgRgnNum, LAYOUT *pLayout)
{
	CString strFileName;
	strFileName.Format("%s\\layout.bmp", theApp.GetWorkDirectory());
	if( dib.Save(strFileName) > 0 )
	{
		BOOL bRet = Layout(strFileName, nMaxRgnNum, nImgRgnNum, pLayout);
		return bRet;
	}

	return FALSE;
}

BOOL CHWOCR::Layout(const CString& strFileName, int nMaxRgnNum, int &nImgRgnNum, LAYOUT *pLayout)
{
	// ������������ǰ�����ȳ�ʼ��
	BOOL bOldInitialized = m_bInitialized;
	if( !m_bInitialized && !Initialize(NULL) )
	{
		return FALSE;
	}

	BOOL bRet = TRUE;
	IMGRGN *pImgRgn = new IMGRGN[nMaxRgnNum];
	try
	{
		CString strText = ("");  // 2012.11.04
		
		char szFileName[MAX_PATH];
		strcpy_s(szFileName, MAX_PATH, strFileName);
		ERRCODE errCode = HWOCROpenImageFile70((szFileName));
		if( errCode == SUCCESS )
		{
			// ʶ����������ʱ�Ĳ���
			int iOption = MULTICOL;//SINGLECOL;
			int	nImgRgnNum = 0;	//ʶ���İ�����Ŀ
			
			// �������
			BOOL bSkew = TRUE;  // 2012.06.03
			ERRCODE errcode = HWOCRLocate70(nMaxRgnNum, nImgRgnNum, pImgRgn, bSkew, iOption);
			int i;

#if 0 
CDib dib;
dib.Read(szFileName);
int j = nImgRgnNum;
for(i = 0; i < j; i++)
{
	if( pImgRgn[i].rect.left  > 50 ) pImgRgn[i].rect.left = 0;
	CDib dd;
	dib.Cut(pImgRgn[i].rect.left, pImgRgn[i].rect.top, pImgRgn[i].rect.right - pImgRgn[i].rect.left, pImgRgn[i].rect.bottom - pImgRgn[i].rect.top, dd);
	CString str;
	str.Format("e:\\xxx-%d.bmp", i);
	dd.Save(str);
}
#endif

			for(i = 0; i < nImgRgnNum && i < nMaxRgnNum ; i++)
			{
				pLayout[i].rect = pImgRgn[i].rect;
				if( pImgRgn[i].style == HOR_TEXT || pImgRgn[i].style == VER_TEXT )
				{
					pLayout[i].style = TEXT;
				}
				else if( pImgRgn[i].style == PICTURE )
				{
					pLayout[i].style = IMAGE;
				}
				else if( pImgRgn[i].style == FORM )
				{
					pLayout[i].style = TABLE;
				}
				else if( pImgRgn[i].style == ENG_TEXT )
				{
					pLayout[i].style = EN_TEXT;
				}
			}

		}
		else
			bRet = FALSE;

	}
	catch (CException* )
	{
		bRet =  FALSE;
	}

	delete [] pImgRgn;
	
	if( !bOldInitialized ) Uninitialize();
	return bRet;
}

BOOL CHWOCR::Do(const CString& strFileName) 
{
	m_dblRate = 0;  // 2010.06.23

	USES_CONVERSION;
	// ������������ǰ�����ȳ�ʼ��
	if( !m_bInitialized && !Initialize(NULL) )
	{
		return FALSE;
	}

	try
	{
		CString strText = ("");  // 2012.11.04
		int nMaxRgnNum = 256;
		IMGRGN *pImgRgn = new IMGRGN[nMaxRgnNum];

#ifdef HW_TEST_VERSION
		if ( HWOCROpenImage(hDest) ) //����
#else
		char szFileName[MAX_PATH];
		strcpy_s(szFileName, MAX_PATH, strFileName);
		ERRCODE errCode = HWOCROpenImageFile70((szFileName));
		if( errCode == SUCCESS )
#endif
		{
			// 2012.11.04 �������˶ദ�޸�

			// ʶ����������ʱ�Ĳ���
			int iOption = MULTICOL;//SINGLECOL;

			int	nImgRgnNum = 0;	//ʶ���İ�����Ŀ

			// �������
			BOOL bSkew = TRUE;  // 2012.06.03
#ifdef HW_TEST_VERSION
			ERRCODE errcode = HWOCRLocate(nMaxRgnNum, nImgRgnNum, pImgRgn, bSkew, iOption);
#else
			ERRCODE errcode = HWOCRLocate70(nMaxRgnNum, nImgRgnNum, pImgRgn, bSkew, iOption);
#endif
			BOOL bOpen = TRUE;

			if( nImgRgnNum > 1 )
			{
				int num = 0;
				int* mark = new int[nImgRgnNum];
				memset(mark, 0, nImgRgnNum*sizeof(int));
				for(int j = 0; j < nImgRgnNum; j++)
				{
					if( abs(pImgRgn[j].rect.right - pImgRgn[j].rect.left < 50) &&  abs(pImgRgn[j].rect.bottom - pImgRgn[j].rect.top) < 50 )
					{
						mark[j] = 1;
						num++;
					}
				}
				if( num == nImgRgnNum - 1 )
				{
					for(int j = 0; j < nImgRgnNum; j++)
					{
						if( mark[j] == 0 )
						{
							if( j == 0 ) break;
							memcpy(&pImgRgn[0], &pImgRgn[j], sizeof(IMGRGN));
						}
					}
					nImgRgnNum = 1;
				}
			}
#if 0
CDib dib;
dib.Read(szFileName);
int j = nImgRgnNum;
for(int i = 0; i < j; i++)
{
	if( pImgRgn[i].rect.left  > 50 ) pImgRgn[i].rect.left = 0;
	CDib dd;
	dib.Cut(pImgRgn[i].rect.left, pImgRgn[i].rect.top, pImgRgn[i].rect.right - pImgRgn[i].rect.left, pImgRgn[i].rect.bottom - pImgRgn[i].rect.top, dd);
	CString str;
	str.Format("e:\\xxx-%d.bmp", i);
	dd.Save(str);
}
#endif
			if( bOpen )
			{
				//struct _timeb timebuffer1;
				//_ftime_s( &timebuffer1 );
				//int m1 = timebuffer1.millitm;

				unsigned char dot[3];
				dot[0] = 163; dot[1] = 174; dot[2] = '\0';

				CFilterWord FilterWord;
				CString strPart;
				int nTxtLen = 6000;
				char *pTxtBuf = new char[nTxtLen];
				int nTotal = 0;
				int nDubiousCount = 0;
				for(int i = 0; i < nImgRgnNum; i++)
				{
					int nLen = 6000;
					char *pBuf = new char[nLen];
					if( !Recognize(pImgRgn[i], pBuf, nLen, nTotal, nDubiousCount) )
					{
						continue;
					}
					strPart.Replace((char*)dot, "��");
					FilterWord.FileterWord(strPart, pBuf); // 2012.07.23					
					strPart.Replace((char*)dot, "��");
					strText += strPart;
					strText += _T(" ");
				}
				delete [] pTxtBuf;	

				//struct _timeb timebuffer2;
				//_ftime_s( &timebuffer2 );
				//int m2 = int((timebuffer2.time - timebuffer1.time)*1000 +  timebuffer2.millitm - timebuffer1.millitm);
	
//				FILE* f;
//				errno_t err	= fopen_s(f, "e:\\my001.txt","wt");
//				if( f )
//				{
//					fprintf(f, "%s", (strText));
//					fclose(f);
//				}

				m_nTotal = nTotal;
				m_nDubiousCount = nDubiousCount;
				if( nTotal > 0 )
				{
					m_dblRate = 1.0 - (double) nDubiousCount / (double)nTotal;
				}

#ifdef HW_TEST_VERSION
				HWOCRCloseImage();
#else
				HWOCRCloseImage70();
#endif
			}
		}
		else
		{
			delete [] pImgRgn;
			pImgRgn = NULL;

			return FALSE;
		}
		delete [] pImgRgn;
		pImgRgn = NULL;


		int n = strFileName.ReverseFind('.');
		if( n > 0 )
		{
			CString str = strFileName.Left(n) + ".PST";
			_unlink(str);
			str = strFileName.Left(n) + ".BKI";
			_unlink(str);
		}

		//���ֵ����
		if( !strText.IsEmpty() )
		{
			m_strText = strText;
			m_strError = ("");
		}
		else
		{
#ifdef ENGLISH_VERSION //JIANGJIN_20120605
			m_strError = _T("Recognizing failed!");
#else
			m_strError = _T("ʶ��ʧ��");
#endif

			return FALSE;
		}

	}
	catch (CException* )
	{
#ifdef ENGLISH_VERSION //JIANGJIN_20120605
		m_strError = _T("Recognizing failed!");
#else
		m_strError = _T("ʶ��ʧ��");
#endif

		return FALSE;
	}

	return TRUE;
}

BOOL CHWOCR::Do(CDib& dib) 
{
	m_dblRate = 0;  // 2010.06.23

	USES_CONVERSION;
	LPBITMAPINFO pBMI = dib.GetBmpInfo();
	if( !pBMI ) return FALSE;
	int nSizePlt = dib.GetPaletteSize();
	DWORD dwImageSize = GetBytePerLine(pBMI->bmiHeader.biWidth, pBMI->bmiHeader.biBitCount) * pBMI->bmiHeader.biHeight;
	int nSizeTotal = sizeof(BITMAPINFOHEADER) + nSizePlt + dwImageSize;
	HANDLE hDest = GlobalAlloc( GHND,nSizeTotal);

	//����ͼ������, pDIBBmpData: bitmap��ʽ����
	if( hDest ) 
	{
		LPBYTE pDest = (LPBYTE)GlobalLock(hDest);
		memcpy( pDest, &pBMI->bmiHeader, sizeof(BITMAPINFOHEADER) );
		if( nSizePlt > 0)
		{
			memcpy(pDest + sizeof(BITMAPINFOHEADER),&pBMI->bmiColors,nSizePlt);
		}
		LPBYTE pSrc = dib.GetBits();  
		memcpy(pDest + sizeof(BITMAPINFOHEADER) + nSizePlt, pSrc, dwImageSize);

	}
	else
	{
		return FALSE;
	}
	GlobalUnlock(hDest);

	// ������������ǰ�����ȳ�ʼ��
	if( !m_bInitialized && !Initialize(NULL) )
	{
		return FALSE;
	}

	try
	{
		CString strText = _T("");  // 2012.11.04
		int nMaxRgnNum = 256;
		IMGRGN *pImgRgn = new IMGRGN[nMaxRgnNum];

#ifdef HW_TEST_VERSION
		if ( HWOCROpenImage(hDest) ) //����
#else
		if ( HWOCROpenImage70(hDest) ) //����
#endif
		{
			// 2012.11.04 �������˶ദ�޸�

			// ʶ����������ʱ�Ĳ���
			int iOption = MULTICOL;//SINGLECOL;

			int	nImgRgnNum = 0;	//ʶ���İ�����Ŀ

			// �������
			BOOL bSkew = TRUE;  // 2012.06.03
#ifdef HW_TEST_VERSION
			ERRCODE errcode = HWOCRLocate(nMaxRgnNum, nImgRgnNum, pImgRgn, bSkew, iOption);
#else
			ERRCODE errcode = HWOCRLocate70(nMaxRgnNum, nImgRgnNum, pImgRgn, bSkew, iOption);
#endif
			BOOL bOpen = TRUE;

			if( !bOpen )  // 2012.11.19
			{
#ifdef HW_TEST_VERSION
				if ( HWOCROpenImage(hDest) ) //����
#else
				if ( HWOCROpenImage70(hDest) ) //����
#endif
				{
					bOpen = TRUE;
				}
			}
			if( bOpen )
			{
				CFilterWord FilterWord;
				CString strPart;
				int nTxtLen = 6000;
				char *pTxtBuf = new char[nTxtLen];
				int nTotal = 0;
				int nDubiousCount = 0;
				for(int i = 0; i < nImgRgnNum; i++)
				{
					int nLen = 6000;
					char *pBuf = new char[nLen];
					if( !Recognize(pImgRgn[i], pBuf, nLen, nTotal, nDubiousCount) )
					{
						continue;
					}

					FilterWord.FileterWord(strPart, pBuf); // 2012.07.23					
					strText += strPart;
					strText += _T(" ");
				}
				delete [] pTxtBuf;	

				if( nTotal > 0 )
				{
					m_dblRate = 1.0 - (double) nDubiousCount / (double)nTotal;
				}

#ifdef HW_TEST_VERSION
				HWOCRCloseImage();
#else
				HWOCRCloseImage70();
#endif
			}
		}
		else
		{
#ifdef HW_TEST_VERSION
			HWOCRTerm(); // �����ر�
#else
			HWOCRTerm70(); // �����ر�
#endif
			delete [] pImgRgn;
			pImgRgn = NULL;

			if(hDest)
			{
				GlobalUnlock(hDest);
				GlobalFree(hDest);
			}
			return FALSE;
		}
		delete [] pImgRgn;
		pImgRgn = NULL;
		if(hDest)
		{
			GlobalUnlock(hDest);
			GlobalFree(hDest);
		}

#ifdef HW_TEST_VERSION
		HWOCRTerm(); // �����ر�
#else
		HWOCRTerm70(); // �����ر�
#endif

		//���ֵ����
		if( !strText.IsEmpty() )
		{
			//			pMyData->SetText(W2A(strText.LockBuffer())) ;
			//			strText.UnlockBuffer();
			m_strError = _T("");
		}
		else
		{
			//			pMyData->SetText(("")); 
#ifdef ENGLISH_VERSION //JIANGJIN_20120605
			m_strError = _T("Recognizing failed!");
#else
			m_strError = _T("ʶ��ʧ��");
#endif

			return FALSE;
		}

	}
	catch (CException* )
	{
#ifdef HW_TEST_VERSION
		HWOCRTerm(); // �����ر�
#else
		HWOCRTerm70(); // �����ر�
#endif
		if(hDest)
		{
			GlobalUnlock(hDest);
			GlobalFree(hDest);
		}
		//		pMyData->SetText(""); 
#ifdef ENGLISH_VERSION //JIANGJIN_20120605
		m_strError = _T("Recognizing failed!");
#else
		m_strError = _T("ʶ��ʧ��");
#endif

		return FALSE;
	}


	return TRUE;
}


BOOL CHWOCR::Recognize(IMGRGN ImgRgn, LPSTR pszText, int& nTextLen, int& nTotal, int& nDubiousCount)  // 2012.11.04
{
	ERRCODE errcode;
#ifdef HW_TEST_VERSION
	errcode = HWOCRRecognize(1, &ImgRgn);
	if( errcode != SUCCESS ) return FALSE;

	errcode = HWOCRGetRecoTxt(nTextLen,pszText);  // ����ı�
	if( errcode != SUCCESS ) return FALSE;

	int nT, nD;
	errcode =  HWOCRGetRecoTxtNum(nT, nD);
	if( errcode == SUCCESS )
	{
		nTotal += nT;
		nDubiousCount += nD;
	}
#else
	errcode = HWOCRRecognize70(1, &ImgRgn);
	if( errcode != SUCCESS ) 
		return FALSE;

	errcode = HWOCRGetRecoTxt70(nTextLen,pszText);  // ����ı�
	if( errcode != SUCCESS ) return FALSE;

	int nT, nD;
	errcode =  HWOCRGetRecoTxtNum70(nT, nD);
	if( errcode == SUCCESS )
	{
		nTotal += nT;
		nDubiousCount += nD;
	}
#endif

	return TRUE;
}

