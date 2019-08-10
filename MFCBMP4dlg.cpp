// MFCBMP4dlg.cpp: 实现文件
//

#include "pch.h"
#include "MFCBMP4.h"
#include "MFCBMP4dlg.h"
#include "afxdialogex.h"
#include "project.h"
#include "MFCBMP4View.h"


// MFCBMP4dlg 对话框

IMPLEMENT_DYNAMIC(MFCBMP4dlg, CDialogEx)

MFCBMP4dlg::MFCBMP4dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, name(_T(""))
	, title(_T(""))
	, author(_T(""))
	, company(_T(""))
	, keyword(_T(""))
	, time(_T(""))
	, en_title(_T(""))
	, en_author(_T(""))
	, en_company(_T(""))
	, en_keyword(_T(""))
	, Reference(_T(""))
	, DOI(_T(""))
	, CNAbstract(_T(""))
	, ENAbstract(_T(""))
{

}

MFCBMP4dlg::~MFCBMP4dlg()
{
}

void MFCBMP4dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, ID_NAME, name);
	DDX_Text(pDX, IDC_TITLE, title);
	DDX_Text(pDX, IDC_AUTHOR, author);
	DDX_Text(pDX, IDC_COMPANY, company);
	DDX_Text(pDX, IDC_KEYWORD, keyword);
	DDX_Text(pDX, IDC_TIME, time);
	DDX_Text(pDX, IDC_EN_TITLE, en_title);
	DDX_Text(pDX, IDC_EN_AUTHOR, en_author);
	DDX_Text(pDX, IDC_EN_COMPANY, en_company);
	DDX_Text(pDX, IDC_EN_KEYWORD, en_keyword);
	DDX_Text(pDX, IDC_REFERENCE, Reference);
	DDX_Text(pDX, IDC_EDIT_DOI, DOI);
	DDX_Text(pDX, IDC_EDIT_CNAbstract, CNAbstract);
	DDX_Text(pDX, IDC_EDIT_ENAbstract, ENAbstract);
}


BEGIN_MESSAGE_MAP(MFCBMP4dlg, CDialogEx)
END_MESSAGE_MAP()


// MFCBMP4dlg 消息处理程序


bool MFCBMP4dlg::setData(Analysis& A)
{
	if (A.DOI.IsEmpty()) DOI = "---";
	else DOI = A.DOI;

	if (A.NumJournalDate.IsEmpty()) time = "---";
	else time = A.NumJournalDate;

	name = A.CNJournal;
	title = A.CNTitle;
	author = A.CNAuthor;
	company = A.CNCompany;
	keyword = A.CNKeyWords;
	
	en_title = A.ENTitle;
	en_author = A.ENAuthor;
	en_company = A.ENCompany;
	en_keyword = A.ENKeyWords;
	Reference = A.Reference;
	
	CNAbstract = A.CNAbstract;
	ENAbstract = A.ENAbstract;

	// TODO: 在此处添加实现代码.
	return false;
}


int MFCBMP4dlg::addData(Analysis& A)
{
	Reference += A.Reference;
	// TODO: 在此处添加实现代码.
	return 0;
}
//弃用
//弃用 #define max_data_save 5
//弃用      保存格式:
//弃用      	中文!@!英文
//弃用      
//弃用      保存顺序
//弃用      	有中英文的:
//弃用      		标题 作者 公司 摘要 关键词
//弃用      	仅单个:
//弃用      		期刊 时间 DOI 参考文献
//弃用
//弃用		***  大块内容分割
//弃用      -*-  小块内容分割
//弃用      ---  占位内容
bool MFCBMP4dlg::saveData()
{
	// TODO: 在此处添加实现代码.
	FILE* p = fopen("data\\index.txt","r+");
	if (!p)
	{
		p = fopen("data\\index.txt", "w");
		fprintf(p, "0");
		rewind(p);
	}

	int ct = 0;
	int e;
	e = fscanf(p, "%d", &ct);
	rewind(p);
	fprintf(p, "%d", ++ct);
	fclose(p);

	save("title", title, en_title);
	save("author", author, en_author);
	save("company", company,en_company);
	save("abstract", CNAbstract, ENAbstract);
	save("key", keyword, en_keyword);

	save("name", name);
	save("time", time);
	save("doi", DOI);

	save(ct, Reference);
	save(ct);
	return true;
}

bool MFCBMP4dlg::save(CString filename, CString& str1, CString& str2)//中英文
{
	FILE* p = fopen("data\\index_" + filename + ".txt", "a");
	fprintf(p, "%s!@!%s\n", str1, str2);
	fclose(p);
	return true;
}
bool MFCBMP4dlg::save(CString filename, CString& str)//中或英
{
	FILE* p = fopen("data\\index_" + filename + ".txt", "a");
	fprintf(p, "%s\n", str);
	fclose(p);
	return true;
}
bool MFCBMP4dlg::save(int ct ,CString& str)//reference
{
	CString filename;
	filename.Format("data\\index_reference_%d.txt", ct);
	FILE* p = fopen(filename, "w");
	fprintf(p, "%s\n", str);
	fclose(p);
	return true;
}
bool MFCBMP4dlg::save(int ct)//保存全部数据(不含reference)
{
	CString filename;
	filename.Format("data\\data_%d.txt", ct);
	FILE* p = fopen(filename, "w");
	
	fprintf(p, "%s*\n",title);
	fprintf(p, "%s*\n",en_title);
	fprintf(p, "%s*\n",author);
	fprintf(p, "%s*\n",en_author);
	fprintf(p, "%s*\n",company);
	fprintf(p, "%s*\n",en_company);
	fprintf(p, "%s*\n",CNAbstract);
	fprintf(p, "%s*\n",ENAbstract);
	fprintf(p, "%s*\n",keyword);
	fprintf(p, "%s*\n",en_keyword);
	fprintf(p, "%s*\n", name);
	fprintf(p, "%s*\n", time);
	fprintf(p, "%s*\n", DOI);

	fclose(p);
	return true;
}

#define max_temp 5000
bool MFCBMP4dlg::pull(int index)//拉取检索内容
{
	CString filename;
	filename.Format("data\\data_%d.txt", index);

	FILE* p = fopen(filename, "r");
	if (!p) return false;

	char temp[max_temp]; GetLine(p, temp);
	title = temp; GetLine(p, temp);
	en_title = temp; GetLine(p, temp);
	author = temp; GetLine(p, temp);
	en_author = temp; GetLine(p, temp);
	company = temp; GetLine(p, temp);
	en_company = temp; GetLine(p, temp);
	CNAbstract = temp; GetLine(p, temp);
	ENAbstract = temp; GetLine(p, temp);
	keyword = temp; GetLine(p, temp);
	en_keyword = temp; GetLine(p, temp);
	name = temp; GetLine(p, temp);
	time = temp; GetLine(p, temp);
	DOI = temp;

	GetReference(index);

	fclose(p);
	return true;
}
void MFCBMP4dlg::GetLine(FILE* p, char* str)
{
	memset(str, 0, max_temp);
	for (int i = 0;i < max_temp; i++)
	{
		str[i] = fgetc(p);
		if (str[i] == '*')
		{
			str[++i] = fgetc(p);
			if (str[i] == '\n')
			{
				str[i] = 0;
				str[i - 1] = '\0';
				return;
			}
		}
	}
}
void MFCBMP4dlg::GetReference(int index)
{
	CString filename;
	filename.Format("data\\index_reference_%d.txt", index);
	char* tempRefer;
	FILE* pf = fopen(filename, "r");  //fclose
	if (!pf) return;
	fseek(pf, 0, SEEK_END);
	long lSize = ftell(pf);

	tempRefer = (char*)malloc(lSize + 1); //free
	rewind(pf);
	fread(tempRefer, sizeof(char), lSize, pf);
	tempRefer[lSize] = '\0';
	Reference = tempRefer;
	free(tempRefer);
	fclose(pf);
}



