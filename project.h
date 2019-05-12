#pragma once
#include <iostream>
#include "Dib.h"
#include "HWOCR.h"

using namespace std;

extern int myTestct;

struct Integrate
{
	int Count;
	int LineHeight[200];
	int LineStart[200];

	int SpaceHeight[200];//��ǰ��
	int SpaceStart[200];


	Integrate()
	{
		Count = 0;
		memset(LineHeight, 0, 200 * sizeof(int));
		memset(LineStart, 0, 200 * sizeof(int));
		memset(SpaceHeight, 0, 200 * sizeof(int));
		memset(SpaceStart, 0, 200 * sizeof(int));
	}
	bool Initial()
	{
		Count = 0;
		memset(LineHeight, 0, 200 * sizeof(int));
		memset(LineStart, 0, 200 * sizeof(int));
		memset(SpaceHeight, 0, 200 * sizeof(int));
		memset(SpaceStart, 0, 200 * sizeof(int));
		return true;
	}
	bool toIntegrate(int* data, int str, int nct)
	{
		int i, j;
		for (i = 0; i < nct; i++)
		{
			if (data[i])
			{
				LineStart[Count] = i + str;
				for (j = i; data[j] && j <= nct; j++)
					;
				LineHeight[Count] = j - i;
				i = j - 1;
				Count++;
			}
			else
			{
				SpaceStart[Count] = i + str;
				for (j = i; !data[j] && j <= nct; j++)
					;
				SpaceHeight[Count] = j - i;
				i = j - 1;
			}
		}

#ifdef myTest
		CString filePath;
		filePath.Format("data\\%d_Integrate.txt", myTestct++);
		FILE* f = fopen(filePath, "w");
		for (int i = 0; i < Count; i++)
		{
			fprintf(f, "��ǰ��%3d;��%3d���и�%3d\n", SpaceHeight[i], i, LineHeight[i]);
		}
		int max = -1, cur = -1;
		for (int i = 0; i < Count; i++)//ȥ������
			if (max < SpaceHeight[i])
			{
				max = SpaceHeight[i];
				cur = i;
			}

		//fprintf(f,"���հ׵��м�ֵΪ%d\n\n", (2 * SpaceStart[cur] + SpaceHeight[cur]) / 2);
		//����	C6385	�ӡ�this->SpaceStart���ж�ȡ��������Ч: �ɶ���СΪ��4000�����ֽڣ������ܶ�ȡ�ˡ� - 4�����ֽڡ�	MFCBMP4	D : \DESKTOP\MFCBMP4 + \project.h	63

		fclose(f);
#endif // 0
		return true;
	}
	//����ֵΪcur
	int getMaxSpace()
	{
		int max = -1, cur = -1;
		for (int i = 0; i < Count; i++)
		{
			if (max < SpaceHeight[i])
			{
				max = SpaceHeight[i];
				cur = i;
			}
		}
		return cur;
	}
	int getMaxLine()
	{
		int max = -1, cur = -1;
		for (int i = 0; i < Count; i++)
		{
			if (max < LineHeight[i])
			{
				max = LineHeight[i];
				cur = i;
			}
		}
		return cur;
	}
	int getSecondMaxLine()
	{
		int max = -1, cur = -1 , max_cur = getMaxLine();
		for (int i = 0; i < Count; i++)
		{
			if (i != max_cur && max < LineHeight[i])
			{
				max = LineHeight[i];
				cur = i;
			}
		}
		return cur;

	}

	int getPreSpace(int cur)
	{
		return SpaceHeight[cur];
	}

	int getLineHeight(int cur)
	{
		return LineHeight[cur];
	}
	
	int getNextSpace(int cur)
	{
		return SpaceHeight[cur + 1];
	}

	int FindInSpace(int data)
	{
		int cur = 0;
		for (int i = 1; i < Count; i++)
		{
			if (data <LineStart[i] && data >LineStart[i] - SpaceHeight[i])
			{
				return i;
			}
		}
		return -1;
	}
};

class Analysis
{
public:
	bool ProjectAll();
	bool ProjectCol(int top, int bottom, int left, int right, Integrate& I);
	bool ProjectLine(int top, int bottom, int left, int right, Integrate& I);

	bool isChinese(CString& src);

	bool HeaderProcess(CString str);
	bool Header();
	
	bool CUTandOCR(int top, int bottom, int left, int right, CString& Dst);
	bool Title();
	bool isDOI(CString tempStr);
	bool isENKey(CString tempStr , int i);

	bool First();
	bool Read(const char* strSrc, const char* strName);

	bool isOpen()
	{
		return m_bOpen;
	}
	bool setOpen()
	{
		return m_bOpen = true;
	}
	bool isReconized()
	{
		return m_bReconized;
	}

	bool Do();
	
	bool DoReference();
	
	bool HardInit()
	{
		m_margin.SetRectEmpty();
		//m_dib.....
		m_I.Initial();

		m_bOpen = false;
		m_bReconized = false;
		bReference = false;
		
		m_nRowLenght = -1;

		CNTitle = CNKeyWords = CNAuthor =
		CNCompany = CNJournal = NumJournalNo =
		NumJournalDate = ENJournal = ENTitle =
		ENKeyWords = ENAuthor = ENCompany =
		Reference = DOI = ENAbstract = CNAbstract =

		strFilePath = 
		strFileName = 
		strTempFilePath =  "";

		m_CNTitleLine = 
		m_CNKeyWordsLine =
		m_CNAuthorLine =
		m_CNAbstractLine =
		m_CNKeyLine =
		m_ENTitleLine =
		m_ENKeyWordsLine =
		m_ENAuthorLine =
		m_ENAbstractLine =
		m_ENKeyLine =
		m_nHeaderLine =
		m_nFooterLine = 0;
		return true;
	}
	bool SoftInit()
	{
		m_margin.SetRectEmpty();

		m_nRowLenght = -1;

		m_bOpen = false;

		Reference = "";
		//delete[] m_LineProj;
		//delete[] m_ColProj;
		//m_LineProj = NULL;
		//m_ColProj = NULL;

		return true;
	}
	Analysis()
	{ 
		m_margin.SetRectEmpty();
		//m_dib.....
		m_I.Initial();

		m_bOpen = false;
		m_bReconized = false;
		bReference = false;

		m_nRowLenght = -1;

		CNTitle = CNKeyWords = CNAuthor =
			CNCompany = CNJournal = NumJournalNo =
			NumJournalDate = ENJournal = ENTitle =
			ENKeyWords = ENAuthor = ENCompany =
			Reference = DOI = ENAbstract = CNAbstract =

			strFilePath =
			strFileName =
			strTempFilePath = "";

		m_CNTitleLine =
			m_CNKeyWordsLine =
			m_CNAuthorLine =
			m_CNAbstractLine =
			m_CNKeyLine =
			m_ENTitleLine =
			m_ENKeyWordsLine =
			m_ENAuthorLine =
			m_ENAbstractLine =
			m_ENKeyLine =
			m_nHeaderLine =
			m_nFooterLine = 0;
	}
	~Analysis()
	{
		//delete[] m_LineProj;
		//delete[] m_ColProj;
		//m_LineProj = NULL;
		//m_ColProj = NULL;
	}

	CString CNTitle;
	CString CNKeyWords;
	CString CNAuthor;
	CString CNCompany;
	CString CNAbstract;

	CString CNJournal;
	CString NumJournalNo;
	CString NumJournalDate;
	CString ENJournal;

	CString ENTitle;
	CString ENKeyWords;
	CString ENAuthor;
	CString ENCompany;
	CString	ENAbstract;

	CString Reference;
	bool bReference;

	CString DOI;
private:
	CHWOCR hw;

	int m_nRowLenght;			//��������

	CString strFilePath;
	CString strFileName;
	CString strTempFilePath;

	bool m_bOpen;
	
	bool m_bReconized;

	//Ϊ����׼��
	int m_CNTitleLine;
	int m_CNKeyWordsLine;
	int m_CNAuthorLine;
	int m_CNAbstractLine;
	int m_CNKeyLine;//ֻ��¼��һ��

	int m_ENTitleLine;
	int m_ENKeyWordsLine;
	int m_ENAuthorLine;
	int m_ENAbstractLine;
	int m_ENKeyLine;//ֻ��¼��һ��

	//ҳüҳ�� -1��ʾ��
	int m_nHeaderLine;
	int m_nFooterLine;

public:
	//���и�
	Integrate m_I;
	//�߽�
	CRect m_margin;
	CDib m_dib;
};