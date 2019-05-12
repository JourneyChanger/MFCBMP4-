
// MFCBMP4View.cpp: CMFCBMP4View 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MFCBMP4.h"
#endif

#include "MFCBMP4Doc.h"
#include "MFCBMP4View.h"
#include "MFCBMP4dlg.h"
#include "SelectContentDlg.h"
#include "getSearchingTextDlg.h"

#include"PDF2Pic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "HWOCR.h"
#include "Dib.h"
#include <io.h>

// CMFCBMP4View

IMPLEMENT_DYNCREATE(CMFCBMP4View, CScrollView)

BEGIN_MESSAGE_MAP(CMFCBMP4View, CScrollView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_OPEN, &CMFCBMP4View::OnFileOpen)
	ON_COMMAND(ID_FLUSH, &CMFCBMP4View::OnFlush)
	ON_COMMAND(ID_RECOGNIZE, &CMFCBMP4View::OnRecognize)
	ON_COMMAND(ID_SEARCH, &CMFCBMP4View::OnSearch)
END_MESSAGE_MAP()

// CMFCBMP4View 构造/析构

CMFCBMP4View::CMFCBMP4View() noexcept
{
	// TODO: 在此处添加构造代码
	m_nFileCt = -1;

}

CMFCBMP4View::~CMFCBMP4View()
{
}

BOOL CMFCBMP4View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CScrollView::PreCreateWindow(cs);
}

// CMFCBMP4View 绘图

void CMFCBMP4View::OnDraw(CDC* pDC)
{
	CMFCBMP4Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	
	// TODO: 在此处为本机数据添加绘制代码
	if (m_nFileCt >= 0 && m_strFileType == "pdf") // 多图模式;
	{
		CString FilePath;

		CRect   thisWindow;
		//GetWindowRect(&thisWindow);//得到所处窗口的绝对坐标;    
		GetClientRect(&thisWindow);//得到相对坐标

		int x = 0, y = 0;
		const int singleWidth = thisWindow.Width() / 3;
		const int singleHeight = (int)thisWindow.Height() / 0.9;
		for (int i = 1; i <= m_nFileCt; i++)
		{
			FilePath.Format("data\\%d.bmp", i);
			CImage tempDraw;
			tempDraw.Load(FilePath);
			tempDraw.Draw(pDC->GetSafeHdc(), x, y, singleWidth, singleHeight);
			x += singleWidth;
			if (x >= 3 * singleWidth) // 每行三张图
			{
				x = 0;
				y += singleHeight;
			}
		}
	}
	else if(!image.IsNull()) // 单图模式
	{
		image.Draw(pDC->GetSafeHdc(), 0, 0);
	}
}

void CMFCBMP4View::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	CMFCBMP4Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CSize sizeTotal;
	sizeTotal.cx = sizeTotal.cy = 4000;
	SetScrollSizes(MM_TEXT, sizeTotal);
	// TODO: 计算此视图的合计大小
	//sizeTotal.cx = sizeTotal.cy = 100;
	//sizeTotal.cx = sizeTotal.cy = 4000;
	//SetScrollSizes(MM_TEXT, sizeTotal);
	//SetScrollSizes(MM_TEXT, CSize(800, 600));
	
	
	// TODO: calculate the total size of this view
	
}


// CMFCBMP4View 打印

BOOL CMFCBMP4View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMFCBMP4View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CMFCBMP4View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CMFCBMP4View 诊断

#ifdef _DEBUG
void CMFCBMP4View::AssertValid() const
{
	CScrollView::AssertValid();
}

void CMFCBMP4View::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CMFCBMP4Doc* CMFCBMP4View::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCBMP4Doc)));
	return (CMFCBMP4Doc*)m_pDocument;
}
#endif //_DEBUG





void CMFCBMP4View::OnFileOpen()//打开位图文件
{
	// TODO: 在此添加命令处理程序代码
#if 1
	//m_analy.InitMyAnaly();  //失败

	CFileDialog dlg(TRUE, _T(".bmp"), _T("*.bmp"), OFN_HIDEREADONLY |
		OFN_OVERWRITEPROMPT, _T("位图文件(*.bmp)|*.bmp|PDF文件(*.pdf)|*.pdf|JPEG文件(*.jpg)|*.jpg||"));
	if (dlg.DoModal() == IDOK)
	{
		bool wasOpen = m_analy.isOpen();
		m_analy.HardInit();

		m_strWorkDir  = dlg.GetPathName();
		m_strFileName = dlg.GetFileName();
		m_strFileType = dlg.GetFileExt().MakeLower();

		m_strThisDir;
		char a[260];
		GetModuleFileName(0, a, 260);
		m_strThisDir = a;
		m_strThisDir = m_strThisDir.Left(m_strThisDir.Find("Debug"));

		if (m_strFileType == "pdf")
		{
			if (wasOpen)//删除上次生成的文件
			{
				CString delFilePath;
				for (int i = 1; ; i++)
				{
					delFilePath.Format("data//%d.bmp", i);
					if (_access(delFilePath, 0) != -1)
					{
						_unlink(delFilePath);
					}
					else
					{
						break;
					}
				}
			}

			//转化为bmp文件
			PDF2Pic(m_strWorkDir, m_strFileName, m_nFileCt);
			m_analy.setOpen();
			Invalidate();
		}
		else//if(m_strFileType == "bmp" || m_strFileType == "jpg")
		{
			m_nFileCt = -1;
			if (!image.IsNull())//初始化
			{
				image.Destroy();
			}
			if (image.Load(dlg.GetPathName()) == S_OK)
			{
				Invalidate();
				if (m_strWorkDir.IsEmpty()) { return; }
				if (!m_analy.Read(m_strWorkDir, m_strFileName))
				{
					AfxMessageBox("未打开");
					return;
				}
			}
		}
	}
	




#elif 0  //测试OCR与CUT结果
	CFileDialog dlg(TRUE, _T(".bmp"), _T("*.bmp"), OFN_HIDEREADONLY |
		OFN_OVERWRITEPROMPT, _T("位图文件(*.bmp)|*.bmp|JPEG文件(*.jpg)|*.jpg||"));
	if (dlg.DoModal() == IDOK)
	{
		if (!image.IsNull())
		{
			image.Destroy();
		}
		image.Load(dlg.GetPathName());
		Invalidate();
	}
	CHWOCR hw;
	CString temp = dlg.GetPathName();
	hw.Do(temp);
	
	MessageBox(hw.GetText());
	FILE* f = fopen("data\\textocr.txt", "w");
	fprintf(f,"---\n%s---\n", hw.GetText());
	fclose(f);

	int a = 2;
#elif 0	//测试文字处理
	CString	a="asdfDOIsafa";
	
	char b[200] = {0};

	int cur = a.Find("DOI");
	for (int i = cur; i < a.GetLength(); i++)
	{
		b[i - cur] = a[i];
	}
	AfxMessageBox(b);

#elif 0 //测试PDF的打开
	char szPath[MAX_PATH];
	sprintf(szPath, "%s\\Image", theApp.GetWorkDirectory());

	CFileDialog dlg(TRUE, _T(".pdf"), _T("*.pdf"), OFN_HIDEREADONLY |
		OFN_OVERWRITEPROMPT, _T("pdf文件(*.pdf)|*.pdf"));
	dlg.m_ofn.lpstrInitialDir = szPath;

	if (dlg.DoModal() == IDOK)
	{
		CString Path = dlg.GetPathName();
		CString Name = dlg.GetFileName();
		//转化为bmp文件
		PDF2Pic(Path, Name,m_nFileCt);

		m_strFileName = Path;
		m_strFileName.Replace(Name, _T("temp\\"));
	}
#endif
}

void CMFCBMP4View::OnFlush()
{	
	string dirPath = "data";

	struct _finddata_t fb;   //查找相同属性文件的存储结构体
	char  path[250];
	//should notice here,the type of handle is long long.
	long long   handle;
	int  resultone;
	int   noFile;            //对系统隐藏文件的处理标记

	noFile = 0;
	handle = 0;

	//制作路径
	strcpy(path, dirPath.c_str());
	strcat(path, "/*");

	handle = _findfirst(path, &fb);
	//找到第一个匹配的文件
	if (handle != 0)
	{
		//当可以继续找到匹配的文件，继续执行
		while (0 == _findnext(handle, &fb))
		{
			//windows下，常有个系统文件，名为“..”,对它不做处理
			noFile = strcmp(fb.name, "..");

			if (0 != noFile)
			{
				//制作完整路径
				memset(path, 0, sizeof(path));
				strcpy(path, dirPath.c_str());
				strcat(path, "/");
				strcat(path, fb.name);
				//属性值为16，则说明是文件夹，迭代
				/*if (fb.attrib == 16)
				{
					delete_directory(path);
				}*/
				//非文件夹的文件，直接删除。对文件属性值的情况没做详细调查，可能还有其他情况。
				/*else
				{*/
				remove(path);
				/*}*/
			}
		}
		//关闭文件夹，只有关闭了才能删除。找这个函数找了很久，标准c中用的是closedir
		//经验介绍：一般产生Handle的函数执行后，都要进行关闭的动作。
		_findclose(handle);
		AfxMessageBox("清理缓存成功");
	}

	//移除文件夹
	//resultone = rmdir(dirPath.c_str());


	// TODO: 在此添加命令处理程序代码
}

void CMFCBMP4View::OnRecognize()
{
	// TODO: 在此添加命令处理程序代码
#if 0
	CString tempFile = m_strWorkDir;
	tempFile.Replace(m_strFileName, "cut\\");//删除文件名

	int ct = m_analy.m_I.Count;
	int* lineS = m_analy.m_I.LineStart;
	int* lineH = m_analy.m_I.LineHeight;
	int left = m_analy.m_margin.left;
	int right = m_analy.m_margin.right;
	for (int i = 0; i < ct; i++)
	{
		CDib tempdib;
		CString Temp;
		Temp.Format("%d_temp.bmp", i);
		m_analy.m_dib.Cut(left, lineS[i], right - left + 1, lineH[i], tempdib);
		tempdib.Save(tempFile + Temp);
	}
	AfxMessageBox("done!");
#elif 1
	if (!m_analy.isOpen())
	{
		AfxMessageBox("未打开图片");
		return;
	}
	else if (m_analy.isReconized())
	{
		MFCBMP4dlg dlg;
		dlg.setData(m_analy);
		dlg.DoModal();
		return;
	}
	else//open but unReconized
	{
		if (m_strFileType.MakeLower() == "pdf")
		{
			CString tempIndex;
			tempIndex.Format("data\\%d.bmp", 1);
			m_analy.Read(m_strThisDir + tempIndex, tempIndex);
			m_analy.Do();
			MFCBMP4dlg dlg;
			dlg.setData(m_analy);
			//dlg.DoModal();

			int cur = -1;
			for (cur = m_nFileCt; cur > 1; cur--)
			{
				m_analy.SoftInit();
				tempIndex.Format("data\\%d.bmp", cur);
				m_analy.Read(m_strThisDir + tempIndex, tempIndex);
				m_analy.DoReference();

				dlg.addData(m_analy);
				//dlg.DoModal();

				if (m_analy.bReference)
					break;
			}
			if (cur != 1)
			{
				for (cur++; cur <= m_nFileCt; cur++)
				{
					m_analy.SoftInit();
					tempIndex.Format("data\\%d.bmp", cur);
					m_analy.Read(m_strThisDir + tempIndex, tempIndex);
					m_analy.DoReference();

					dlg.addData(m_analy);
					//dlg.DoModal();
				}
			}
			dlg.DoModal();

			dlg.saveData();
		}
		else if (m_strFileType.MakeLower() == "bmp")
		{
			SelectContentDlg sc;
			sc.DoModal();
			if (sc.getSelIndex())
			{
				m_analy.Do();
				MFCBMP4dlg dlg;
				dlg.setData(m_analy);
				dlg.DoModal();
			}
			else
			{
				m_analy.DoReference();
				MFCBMP4dlg dlg;
				dlg.setData(m_analy);
				dlg.DoModal();
			}
		}

	}
#elif 0
	// TODO: 在此添加命令处理程序代码
	for (int i = 1; i <= m_nFileCt; i++)
	{
		CString strNum;
		strNum.Format("%d.bmp", i);
		CString	strBmpFile = m_strWorkDir + strNum;
		//////////////////////////////////
		m_dib.Read(strBmpFile);
		if (!m_dib.IsValid()) return;
		//////////////////////////////////
		m_dib.Grizzle(m_dib);
		//////////////////////////////////
		strNum.Format("%d_gray.bmp", i);
		CString	strBmpOut = m_strWorkDir + strNum;

		if (strBmpOut.IsEmpty())
		{
			return;
		}

		//if (m_pBitmapView && m_pBitmapView->IsProcess())
		//{
		m_dib.Save(strBmpOut);
		//}
		//////////////////////////////////
		CHWOCR hw;
		if (hw.Do(strBmpFile))
		{
			CString strText = hw.GetText();
			AfxMessageBox(strText);
		}
	}


	MFCBMP4dlg dlg;
	dlg.DoModal();
#endif // 0
}

void CMFCBMP4View::OnSearch()
{
	// TODO: 在此添加命令处理程序代码
	getSearchingTextDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		return;
	}

}
