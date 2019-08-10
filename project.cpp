#include "pch.h"
#include"project.h"

#ifdef myTest
int myTestct = 0;
#endif // mytest

//四边界m_maigin + 页眉页脚m_nHeader;m_nFooter;m_HeaderLine
bool Analysis::First()
{
	//全投影与四边界
	ProjectAll();
	//寻找页眉页脚 (线法)
	m_nFooterLine = m_nHeaderLine = -1;
	for (int i = 0; i < m_I.Count && m_I.LineStart[i] < m_dib.GetBmpInfo()->bmiHeader.biHeight / 3; i++)
	{
		if (m_I.LineHeight[i] <= 10)//找线
		{
			m_nHeaderLine = i;
			break;
		}
	}
	for (int i = m_I.Count - 1; i >= 1 && m_I.LineStart[i] > m_dib.GetBmpInfo()->bmiHeader.biHeight / 3 * 2; i--)
	{
		if (m_I.LineHeight[i] <= 10)
		{
			m_nFooterLine = i;
			break;
		}
	}

	return 0;
}
//读文件
bool Analysis::Read(const char* strSrc , const char* strName)
{
	CDib tempDib;

	if (!tempDib.Read(strSrc))
		return false;

	if (tempDib.GetBmpInfo()->bmiHeader.biBitCount == 8)
	{
		m_dib = tempDib;
	}
	else
	{
		tempDib.Grizzle(m_dib);
	}

	strFilePath = strSrc;
	strFileName = strName;
	strTempFilePath = strSrc;
	strTempFilePath.Replace(strName, "temp.bmp");

	m_nRowLenght = (m_dib.GetBmpInfo()->bmiHeader.biWidth * 8 + 31) / 32 * 4;  //灰度图每行字节数

	m_bOpen = 1;

	m_I.Initial();

	First();

	return true;
}

//首页
bool Analysis::Do()
{
	if (!m_bOpen)
	{
		printf("without opening!!!\n");
		exit(0);
	}

	Header();

	Title();
	return true;
}

//中间页
bool Analysis::DoMiddle()
{
	if (!m_bOpen)
	{
		printf("without opening!!!\n");
		exit(0);
	}

	int width = m_margin.right - m_margin.left + 1;
	

	Integrate tempCol;
	//int temptop = m_nHeaderLine + 1;
	//int tempbottom;

	//if (m_nFooterLine == -1)
	//{
	//	for (int i = temptop + 1; i < m_I.Count ; i++)
	//	{
	//		if (m_I.getPreSpace(i) >= 40)
	//		{
	//			tempbottom = i;
	//			break;
	//		}
	//	}
	//}
	//else
	//{
	//	tempbottom = m_nFooterLine;
	//}
	//for (int i = temptop + 1; i < tempbottom; i++)
	//{
	//	if (m_I.getPreSpace(i) >= 40)
	//	{
	//		ProjectCol(m_I.LineStart[temptop], m_I.LineStart[i] - 1, m_margin.left, m_margin.right, tempCol);
	//		if (tempCol.SpaceHeight[tempCol.FindInSpace(m_dib.GetBmpInfo()->bmiHeader.biWidth / 2)] > 50)
	//		{
	//			tempbottom = i;
	//			break;
	//		}
	//	}
	//}


	ProjectCol(m_I.LineStart[m_nHeaderLine + 1], m_margin.bottom, m_margin.left, m_margin.right, tempCol);
	int cur = tempCol.getMaxSpace();

	int middle_left = tempCol.LineStart[cur - 1] + tempCol.LineHeight[cur - 1];
	int middle_right = tempCol.LineStart[cur];

	int height = m_margin.bottom - m_I.LineStart[m_nHeaderLine + 1] + 1;

	Integrate tempLeft;
	ProjectLine(m_I.LineStart[m_nHeaderLine + 1], m_margin.bottom, m_margin.left,middle_left, tempLeft);

	CString	tempStr;

	for (int i = 0; i < tempLeft.Count; i++)
	{
		if (tempLeft.LineHeight[i] > 15 && tempLeft.LineHeight[i] < 60)
		{
			CUTandOCR(tempLeft.LineStart[i], tempLeft.LineStart[i] + tempLeft.LineHeight[i], m_margin.left, middle_left, tempStr);
			if (!bReference)
			{
				tempStr.Replace(" ", "");
				CString temptemp = tempStr;
				if (tempStr.Find("参考文献") != -1 || temptemp.MakeLower().Find("reference") != -1)
				{
					bReference = true;
				}
			}
			else
			{
				CString temptemp = tempStr;
				temptemp.Replace(" ", "");
				int x1 = temptemp.Find("[");
				int x2 = temptemp.Find("]");
				int flagNum = true;

				for (int i = x1 + 1; i < x2; i++)
				{
					if (temptemp[i] < '0' || temptemp[i] > '9')
					{
						flagNum = false;
						break;
					}
				}

				if (x1 != -1 && x2 != -1 && x1 < x2 && flagNum)
				{
					Reference += "\r\n" + tempStr;
				}
				else
				{
					Reference += tempStr;
				}
			}
		}
	}

	Integrate tempRight;
	ProjectLine(m_I.LineStart[m_nHeaderLine + 1], m_margin.bottom, middle_right, m_margin.right, tempRight);
	//CString	tempStr;
	for (int i = 0; i < tempRight.Count; i++)
	{
		if (tempRight.LineHeight[i] > 15 && tempRight.LineHeight[i] < 60)
		{
			CUTandOCR(tempRight.LineStart[i], tempRight.LineStart[i] + tempRight.LineHeight[i], middle_right, m_margin.right, tempStr);
			if (!bReference)
			{
				tempStr.Replace(" ", "");
				if (tempStr.Find("参考文献") != -1 || tempStr.MakeLower().Find("reference") != -1)
				{
					bReference = true;
				}
			}
			else
			{
				CString temptemp = tempStr;
				temptemp.Replace(" ", "");
				int x1 = temptemp.Find("[");
				int x2 = temptemp.Find("]");
				int flagNum = true;

				for (int i = x1 + 1; i < x2; i++)
				{
					if (temptemp[i] < '0' || temptemp[i] > '9')
					{
						flagNum = false;
						break;
					}
				}

				if (x1 != -1 && x2 != -1 && x1 < x2 && flagNum)
				{
					Reference += "\r\n" + tempStr;
				}
				else
				{
					Reference += tempStr;
				}
			}
		}
	}

	Reference.Trim();
	//AfxMessageBox(Reference);

#ifdef myTest
	CString filePath; filePath.Format("data\\%d_REFERENCE.txt", myTestct++);
	FILE* pct = fopen(filePath, "w");

	fprintf(pct, "%s", Reference);

	fclose(pct);
#endif // myTest

	return true;
}




//全投影(四边界)+部分纵横投影(记得考虑行反序
bool Analysis::ProjectAll()
{
	int height = m_dib.GetBmpInfo()->bmiHeader.biHeight;
	int width = m_dib.GetBmpInfo()->bmiHeader.biWidth;
	
	int* m_ColProj = new int[width]; 		//纵向统计
	int* m_LineProj = new int[height];	 	//横向统计
	
	::memset(m_ColProj, 0, width * sizeof(int));
	::memset(m_LineProj, 0, height * sizeof(int));


	LPBYTE pdata = m_dib.GetBits();
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			if (*(pdata+i * m_nRowLenght + j) <= 100)
			{
				m_ColProj[j]++;
				m_LineProj[height - i - 1]++;
			}
#ifdef myTest
	CString filePath; filePath.Format("data\\%d_Line.txt", myTestct++);
	FILE* pct1 = fopen(filePath, "w");
	filePath.Format("data\\%d_Col.txt", myTestct++);
	FILE* pct2 = fopen(filePath, "w");
	if (!pct1 || !pct2)
	{
		printf("创建文件失败！\n");
		exit(0);
	}

	for (int i = 0; i < height; i++)
		fprintf(pct1, "%d\n", m_LineProj[i]);
	for (int i = 0; i < width; i++)
		fprintf(pct2, "%d\n", m_ColProj[i]);

	fclose(pct1);
	fclose(pct2);
#endif // 0

	//////////////////////////////////////////////////////确定四边界,缩小投影范围
	for (int i = 0; i < m_dib.GetBmpInfo()->bmiHeader.biHeight; i++)
		if (m_LineProj[i])
		{
			m_margin.top = i;
			break;
		}
	for (int i = m_dib.GetBmpInfo()->bmiHeader.biHeight - 1; i >= 0; i--)
		if (m_LineProj[i])
		{
			m_margin.bottom = i;
			break;
		}
	for (int i = 0; i < m_dib.GetBmpInfo()->bmiHeader.biWidth; i++)
		if (m_ColProj[i])
		{
			m_margin.left = i;
			break;
		}
	for (int i = m_dib.GetBmpInfo()->bmiHeader.biWidth - 1; i >= 0; i--)
		if (m_ColProj[i])
		{
			m_margin.right = i;
			break;
		}

	//行距与行高
	m_I.toIntegrate(m_LineProj, 0, m_dib.GetBmpInfo()->bmiHeader.biHeight);

	delete[] m_ColProj; 		//纵向统计
	delete[] m_LineProj;		//横向统计

	m_ColProj = m_LineProj = NULL;
	return true;
}
bool Analysis::ProjectCol(int top, int bottom, int left, int right, Integrate& I)
{
	int ntop = m_dib.GetHeight() - bottom - 1;
	int nbottom = m_dib.GetHeight() - top - 1;
	int width = right - left + 1;

	int* pCol = new int[width];
	::memset(pCol, 0, width * sizeof(int));
	
	LPBYTE pdata = m_dib.GetBits();

	for (int i = ntop; i <= nbottom; i++)
		for (int j = left; j <= right; j++)
			if (*(pdata + i * m_nRowLenght + j) <= 100)
				pCol[j - left]++;

#ifdef myTest
	CString filePath; filePath.Format("data\\%d_ColPart.txt", myTestct++);
	FILE* pct = fopen(filePath, "w");
	if (!pct)
	{
		printf("创建文件失败！\n");
		exit(0);
	}
	for (int i = 0; i < width; i++)
		fprintf(pct, "%d\n", pCol[i]);
	fclose(pct);
#endif // 0


	I.toIntegrate(pCol, left, width);
	delete[] pCol;

	return true;
}
bool Analysis::ProjectLine(int top, int bottom, int left, int right, Integrate& I)
{
	int ntop = m_dib.GetHeight() - bottom - 1;
	int nbottom = m_dib.GetHeight() - top - 1;

	int height = nbottom - ntop + 1;
	int* pLine = new int[height];
	::memset(pLine, 0, height * sizeof(int));

	LPBYTE pdata = m_dib.GetBits();

	for (int i = nbottom; i >= ntop; i--)
		for (int j = left; j <= right; j++)
			if (*(pdata + i * m_nRowLenght + j) <= 100)
				pLine[nbottom - i]++;

#ifdef myTest
	CString filePath; filePath.Format("data\\%d_LinePart.txt", myTestct++);
	FILE* pct = fopen(filePath, "w");
	if (!pct)
	{
		printf("创建文件失败！\n");
		exit(0);
	}
	for (int i = 0; i < height; i++)
		fprintf(pct, "%d\n", pLine[i]);
	fclose(pct);
#endif // 0

	I.toIntegrate(pLine, top, height);

	delete[] pLine;
	return true;
}


//期刊的 名称 卷期 时间 DOI
bool Analysis::HeaderProcess(CString str)
{
	if (NumJournalDate.IsEmpty() && (str.Find("年") != -1 || str.Find("月") != -1))
	{
		NumJournalDate = str.Trim();
		return true;
	}
	else if (NumJournalNo.IsEmpty() && (str.Find("卷") != -1 || str.Find("期") != -1 || str.Find("Vol") != -1 || str.Find("No") != -1))
	{
		NumJournalNo = str.Trim();
		return true;
	}
	else if (CNJournal.IsEmpty() && ((str.Find("报") != -1) || isChinese(str)))
	{
		CNJournal = str.Trim();
		return true;
	}
	else if(ENJournal.IsEmpty() && str.Find("Journal") != -1 )
	{
		ENJournal = str.Trim();
		return true;
	}
	else if (DOI.IsEmpty())
	{
		if (isDOI(str))
		{
			return true;
		}
	}
	return false;
}
bool Analysis::Header()
{
	CDib tempDib;
	
	int* lineS = m_I.LineStart;
	int* lineH = m_I.LineHeight;
	int left = m_margin.left;
	int right = m_margin.right;
	int width = right - left + 1;

	int spaceCt;
	int spaceCur[7];

	for (int i = 0; i < m_nHeaderLine; i++)
	{
		if (lineH[i] >= 20)
		{			
			spaceCt = 0;
			::memset(spaceCur, 0, 5 * sizeof(int));
			Integrate tempInte;
			ProjectCol(lineS[i], lineS[i] + lineH[i] - 1, left, right, tempInte);
			for (int j = 0; j < tempInte.Count; j++)
			{
				if (tempInte.SpaceHeight[j] > 120) // > 35   //   > 100
				{
					spaceCur[spaceCt] = j;
					spaceCt++;
				}
			}
			int tempLeft = left;
			int tempRight = right;
			int j = 0;
			int tempWidth;
			do
			{
				tempRight = tempInte.SpaceStart[spaceCur[j]] + tempInte.SpaceHeight[spaceCur[j]] / 2;
				tempWidth = tempRight - tempLeft + 1;
				m_dib.Cut(tempLeft, lineS[i], tempWidth, lineH[i], tempDib);
				tempDib.Save(strTempFilePath);

				hw.Do(strTempFilePath);

				HeaderProcess(hw.GetText());

				tempLeft = tempRight;
				j++;
			} while (j < spaceCt);
	
			tempRight = right;
			tempWidth = tempRight - tempLeft + 1;
			m_dib.Cut(tempLeft, lineS[i], tempWidth, lineH[i], tempDib);
			tempDib.Save(strTempFilePath);

			hw.Do(strTempFilePath);
			
			HeaderProcess(hw.GetText());
		}
	}

	Integrate tempI;
	ProjectCol(lineS[1], lineS[1] + lineH[1] - 1, left, right, tempI);

	_unlink(strTempFilePath);
	return 0;
}

//中英文的标题\关键字\作者\公司 (DOI
bool Analysis::isChinese(CString& src)
{
	int len = src.GetLength();

	for (int i = 0; i < src.GetLength(); i++)
	{
		if ((BYTE)src[i] >= 0xb0)
		{
			return true;
		}
	}
	return false;
}

bool Analysis::CUTandOCR(int top, int bottom, int left, int right,CString& Dst)
{
	CDib tempDib;//剪切用
	m_dib.Cut(left, top, right - left + 1, bottom - top + 1, tempDib);
	tempDib.Save(strTempFilePath);
	hw.Do(strTempFilePath);
	Dst = hw.GetText().Trim();
	_unlink(strTempFilePath);
	return true;
}
bool Analysis::Title()
{
	CString tempStr;
	int* lineS = m_I.LineStart;
	int* lineH = m_I.LineHeight;
	int left = m_margin.left;
	int right = m_margin.right;

	int upRegion = m_dib.GetBmpInfo()->bmiHeader.biHeight / 3;
	int downRegion = upRegion * 2;

	Integrate tempCol;
	int temptop = m_nHeaderLine + 1;
	int tempbottom;

	if (m_nFooterLine == -1)
	{
		for (int i = m_I.Count - 1; i > 3; i--)
		{
			if (m_I.getPreSpace(i) >= 40)
			{
				tempbottom = i;
				break;
			}
		}
	}
	else
	{
		tempbottom = m_nFooterLine;
	}
	for (int i = tempbottom - 1; i > m_nHeaderLine ; i--)
	{
		if (m_I.getPreSpace(i) >= 40)
		{
			ProjectCol(m_I.LineStart[i], m_I.LineStart[tempbottom] - 1, m_margin.left, m_margin.right, tempCol);
			if (tempCol.SpaceHeight[tempCol.FindInSpace(m_dib.GetBmpInfo()->bmiHeader.biWidth / 2)] > 50)
			{
				tempbottom = i;
				break;
			}
		}
	}

	//tempbottom :正文开头行
	for (int i = m_nHeaderLine + 1; i < tempbottom; i++)
	{
		if (lineH[i] > 20 && lineH[i] < 110)
		{
			CUTandOCR(lineS[i], lineS[i] + lineH[i] - 1, left, right, tempStr);

			if (CNTitle.IsEmpty())
			{
				if (m_I.getLineHeight(i) >= 50 && m_I.getPreSpace(i) >= 60 && isChinese(tempStr))
				{
					CNTitle = tempStr;
					m_CNTitleLine = i;

					CString previewStr;
					CUTandOCR(lineS[i + 1], lineS[i + 1] + lineH[i + 1] - 1, left, right, previewStr);
					if (abs(m_I.getLineHeight(i) - m_I.getLineHeight(i + 1)) <= 5 && isChinese(previewStr))
					{
						CNTitle += " " + previewStr;
						m_CNTitleLine++;
						i++;
					}

					//用位置判断作者
					CUTandOCR(lineS[i + 1], lineS[i + 1] + lineH[i + 1] - 1, left, right, previewStr);
					if (isChinese(previewStr))
					{
						CNAuthor = previewStr;
					}
					continue;
				}
			}
			if (CNAuthor.IsEmpty())
			{					
				if (m_I.getLineHeight(i) > 35 && m_I.getPreSpace(i) > 40 && i > m_CNTitleLine && isChinese(tempStr))
				{
						CNAuthor = tempStr;

						m_CNAuthorLine = i;
						continue;
				}
			}
			if (CNAbstract.IsEmpty())
			{
				CString temptmep = tempStr;
				temptmep.Replace(" ","");
				if (temptmep.Find("摘要") != -1)
				{
 					CNAbstract += tempStr;
					m_CNAbstractLine = i;
					continue;
				}
			}
			//todo:有两行?
			if (CNKeyWords.IsEmpty())
			{
				if (tempStr.Find("关键词") != -1)
				{
					tempStr.Replace("关键词", "");
					tempStr.Replace(":", "");
					tempStr.Replace("：","");
					CNKeyWords = tempStr.Trim();

					m_CNKeyLine = i;
					continue;
				}
			}

			if (DOI.IsEmpty())
			{
				if (isDOI(tempStr))
				{
					continue;
				}
			}

			if (ENTitle.IsEmpty())
			{
				if ((m_I.getLineHeight(i) >= 35 && m_I.getPreSpace(i) > 40) || m_I.getLineHeight(i) >= 60)
				{
					if (!isChinese(tempStr))
					{
						ENTitle = tempStr;
						m_ENTitleLine = i;
						//两行
						CString previewStr;
						CUTandOCR(lineS[i + 1], lineS[i + 1] + lineH[i + 1] - 1, left, right, previewStr);
						if (m_I.getLineHeight(i) >= 60 || (m_I.getPreSpace(i) >= 70 && abs(m_I.getPreSpace(i) - m_I.getNextSpace(i)) >= 30) && (m_I.getNextSpace(i) < 35)&& abs(m_I.getLineHeight(i) - m_I.getLineHeight(i + 1) <= 9))
						{
							if (!isChinese(previewStr))
							{
								ENTitle += " " + previewStr;
								m_ENTitleLine++;
								i++;
							}
						}

						//用位置判断作者
						CUTandOCR(lineS[i + 1], lineS[i + 1] + lineH[i + 1] - 1, left, right, previewStr);
						if (!isChinese(previewStr))
						{
							ENAuthor = previewStr;
						}

						continue;
					}
				}
			}
			if (ENKeyWords.IsEmpty())
			{
				if (isENKey(tempStr,i))
				{
					continue;
				}
			}
			if (ENAuthor.IsEmpty())
			{
				if (ENAuthor.IsEmpty())
				{
					if ((m_CNAuthorLine + 1 == i) || (m_I.getLineHeight(i) > 35 && m_I.getPreSpace(i) > 37) && i > m_ENTitleLine && !isChinese(tempStr))
					{
						ENAuthor = tempStr;

						continue;
					}
				}
			}
			if (ENAbstract.IsEmpty())
			{
				CString temptmep = tempStr;
				temptmep.MakeLower();
				if (temptmep.Find("abstract") != -1)
				{
					ENAbstract += tempStr;
					m_ENAbstractLine = i;
					continue;
				}
			}
			if (CNCompany.IsEmpty() || (ENCompany.IsEmpty() && !ENTitle.IsEmpty()))
			{			
				int flag1 = (tempStr.Find("(") == 0)||(tempStr.Find("（") == 0)||(tempStr.Find("f") == 0);
				int flag2 = (tempStr.ReverseFind(')') == tempStr.GetLength() - 1) ||
							(tempStr.ReverseFind('1') == tempStr.GetLength() - 1) ||
							(tempStr.Find("）") == tempStr.GetLength() - 2);
				if (flag1 && !flag2)
				{
					CString tempPreView1, tempPreView2; //多行的情况,最多检测多两行 (待改
					CUTandOCR(lineS[i + 1], lineS[i + 1] + lineH[i + 1] - 1, left, right, tempPreView1);
					CUTandOCR(lineS[i + 2], lineS[i + 2] + lineH[i + 2] - 1, left, right, tempPreView2);

					int flag3 = (tempPreView1.ReverseFind(')') == tempPreView1.GetLength() - 1) ||
							(tempPreView1.ReverseFind('1') == tempPreView1.GetLength() - 1) ||
							(tempPreView1.Find("）") == tempPreView1.GetLength() - 2);
					
					int flag4 = (tempPreView2.ReverseFind(')') == tempPreView2.GetLength() - 1) ||
							(tempPreView2.ReverseFind('1') == tempPreView2.GetLength() - 1) ||
							(tempPreView2.Find("）") == tempPreView2.GetLength() - 2);

					if (flag3)
					{
						if (CNCompany.IsEmpty() && isChinese(tempPreView1))
						{
							CNCompany = tempStr + tempPreView1;

							continue;
						}
						else if (ENCompany.IsEmpty() && !isChinese(tempPreView1) && i > m_ENTitleLine)
						{
							ENCompany = tempStr + tempPreView1;

							continue;
						}
					}
					else if (flag4)
					{
						if (CNCompany.IsEmpty() && isChinese(tempPreView2))
						{
							CNCompany = tempStr + tempPreView1 + tempPreView2;

							continue;
						}
						else if (ENCompany.IsEmpty() && !isChinese(tempPreView2) && i > m_ENTitleLine)
						{
							ENCompany = tempStr + tempPreView1 + tempPreView2;

							continue;
						}
					}
				}

				if (flag1 && flag2)
				{
					if (CNCompany.IsEmpty() )
					{
						CNCompany = tempStr;

						continue;
					}
					else if (ENCompany.IsEmpty() && i > m_ENTitleLine)
					{
						ENCompany = tempStr;

						continue;
					}
				}
			}
		}
	}

	if (!CNAbstract.IsEmpty() && !CNKeyWords.IsEmpty() && m_CNAbstractLine < m_CNKeyLine)
	{
		for (int i = m_CNAbstractLine + 1; i < m_CNKeyLine - 1;i++)
		{
			CUTandOCR(lineS[i + 1], lineS[i + 1] + lineH[i + 1] - 1, left, right, tempStr);
			CNAbstract += tempStr;
		}
	}

	if (!ENAbstract.IsEmpty() && !ENKeyWords.IsEmpty() && m_ENAbstractLine < m_ENKeyLine)
	{
		for (int i = m_ENAbstractLine + 1; i < m_ENKeyLine - 1; i++)
		{
			CUTandOCR(lineS[i + 1], lineS[i + 1] + lineH[i + 1] - 1, left, right, tempStr);
			ENAbstract += tempStr;
		}
	}
	return true;
}

bool Analysis::isDOI(CString tempStr)
{
	CString temptemp = tempStr;
	if (temptemp.MakeLower().Find("doi") != -1)
	{
		int cur = temptemp.MakeLower().Find("doi");

		temptemp.Replace("doi", "");
		temptemp.Replace(":", "");
		temptemp.Replace("：", "");
		temptemp.Replace("号", "");

		char b[200] = { 0 };

		for (int i = cur; i < temptemp.GetLength(); i++)
		{
			b[i - cur] = temptemp[i];
		}
		DOI = b;
		DOI.Trim();
		return true;
	}
	return false;
}

bool Analysis::isENKey(CString tempStr , int i)
{
	CString	temptemp = tempStr;
	temptemp.Replace(" ", "");
	
	if (temptemp.MakeLower().Find("keyword") != -1)
	{
		int cur = tempStr.Find(":");
		if (cur != -1)
		{
			char a[200];
			int len = tempStr.GetLength();
			for (int i = cur + 1; i < len; i++)
			{
				a[i - cur - 1] = tempStr[i];
			}
			a[len] = '\0';
			ENKeyWords = a;
		}
		else
		{
			tempStr.Replace("Key words", "");
			tempStr.Replace("Keywords", "");
			ENKeyWords = tempStr;
		}
		m_ENKeyLine = i;  //只记录第一行
		//有两行
		CString previewStr;
		CUTandOCR(m_I.LineStart[i + 1], m_I.LineStart[i + 1] + m_I.LineHeight[i + 1] - 1, m_margin.left, m_margin.right, previewStr);
		if (abs(m_I.getPreSpace(i) - m_I.getNextSpace(i)) <= 7 && !isChinese(previewStr))
		{
			ENKeyWords += " " + previewStr;
			i++;
		}
		ENKeyWords.Trim();
		return true;
	}
	return false;
}
