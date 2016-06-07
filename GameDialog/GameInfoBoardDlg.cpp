#include "GameInfoBoardDlg.h"

GameInfoBoardDlg::GameInfoBoardDlg()
{
	m_pCenterNode = (LPBoardMsgNode)malloc(sizeof(BoardMsgNode) * MAX_BOARDMSG_NUM);
	ZeroMemory(m_pCenterNode, sizeof(BoardMsgNode) * MAX_BOARDMSG_NUM);
	m_font = new GfxFont("����", 12, FALSE, FALSE, FALSE);
	m_font->SetColor(ARGB(255, 0, 255, 0));
	m_font->SetKerningHeight(2.0f);

	for(int i = 0; i < MAX_BOARDMSG_NUM; ++i)
	{
		if(i == 0)
		{
			m_pCenterNode[i].pPre = &m_pCenterNode[MAX_BOARDMSG_NUM - 1];
			m_pCenterNode[i].pNext = &m_pCenterNode[1];
		}
		else if(i == MAX_BOARDMSG_NUM - 1)
		{
			m_pCenterNode[i].pNext = &m_pCenterNode[0];
			m_pCenterNode[i].pPre = &m_pCenterNode[MAX_BOARDMSG_NUM - 2];
		}
		else
		{
			m_pCenterNode[i].pNext = &m_pCenterNode[i + 1];
			m_pCenterNode[i].pPre = &m_pCenterNode[i - 1];
		}
	}

	m_pFirstShowNode = m_pLastShowNode = m_pCenterNode;
	m_rcClient.left = 5;
	m_rcClient.top = 5;
}

GameInfoBoardDlg::~GameInfoBoardDlg()
{
	free(m_pCenterNode);
	delete m_font;
}

/*
bool GameInfoBoardDlg::InsertBoardMsg(const char* lpszMsg)
{
	if(m_pLastShowNode->pNext == m_pFirstShowNode)
	{
		//	��Ϣ����������
		if(strlen(lpszMsg) >= MAX_BOARDMSG_LENGTH - 1)
		{
			return false;
		}
		else
		{
			//	���ƶ���
			m_pFirstShowNode = m_pFirstShowNode->pNext;
			strcpy(m_pLastShowNode->szMsg, lpszMsg);
			m_pLastShowNode->dwInsertTime = ::GetTickCount();
			m_pLastShowNode = m_pLastShowNode->pNext;
			return true;
		}
	}
	else
	{
		//	û�г���
		if(strlen(lpszMsg) >= MAX_BOARDMSG_LENGTH - 1)
		{
			return false;
		}
		else
		{
			strcpy(m_pLastShowNode->szMsg, lpszMsg);
			m_pLastShowNode->dwInsertTime = ::GetTickCount();
			m_pLastShowNode = m_pLastShowNode->pNext;
			return true;
		}
	}
}*/

bool GameInfoBoardDlg::InsertBoardMsg(const char* _fmt, ...)
{
	va_list va;

	if(m_pLastShowNode->pNext == m_pFirstShowNode)
	{
		//	��Ϣ����������
		if(strlen(_fmt) >= MAX_BOARDMSG_LENGTH - 1)
		{
			return false;
		}
		else
		{
			//	���ƶ���
			m_pFirstShowNode = m_pFirstShowNode->pNext;

			va_start(va, _fmt);
			vsprintf(m_pLastShowNode->szMsg, _fmt, va);
			va_end(va);
			
			m_pLastShowNode->dwInsertTime = ::GetTickCount();
			m_pLastShowNode->dwCol = ARGB(255, 0, 255, 0);
			m_pLastShowNode = m_pLastShowNode->pNext;
			return true;
		}
	}
	else
	{
		//	û�г���
		if(strlen(_fmt) >= MAX_BOARDMSG_LENGTH - 1)
		{
			return false;
		}
		else
		{
			va_start(va, _fmt);
			vsprintf(m_pLastShowNode->szMsg, _fmt, va);
			va_end(va);

			m_pLastShowNode->dwInsertTime = ::GetTickCount();
			m_pLastShowNode->dwCol = ARGB(255, 0, 255, 0);
			m_pLastShowNode = m_pLastShowNode->pNext;
			return true;
		}
	}
}

bool GameInfoBoardDlg::InsertBoardMsg(DWORD _col, const char* _fmt, ...)
{
	va_list va;

	if(m_pLastShowNode->pNext == m_pFirstShowNode)
	{
		//	��Ϣ����������
		if(strlen(_fmt) >= MAX_BOARDMSG_LENGTH - 1)
		{
			return false;
		}
		else
		{
			//	���ƶ���
			m_pFirstShowNode = m_pFirstShowNode->pNext;

			va_start(va, _fmt);
			vsprintf(m_pLastShowNode->szMsg, _fmt, va);
			va_end(va);

			m_pLastShowNode->dwInsertTime = ::GetTickCount();
			m_pLastShowNode->dwCol = _col;
			m_pLastShowNode = m_pLastShowNode->pNext;
			return true;
		}
	}
	else
	{
		//	û�г���
		if(strlen(_fmt) >= MAX_BOARDMSG_LENGTH - 1)
		{
			return false;
		}
		else
		{
			va_start(va, _fmt);
			vsprintf(m_pLastShowNode->szMsg, _fmt, va);
			va_end(va);

			m_pLastShowNode->dwInsertTime = ::GetTickCount();
			m_pLastShowNode->dwCol = _col;
			m_pLastShowNode = m_pLastShowNode->pNext;
			return true;
		}
	}
}

bool GameInfoBoardDlg::InsertEquipErrorMsg(int _err)
{
	DWORD dwErrCol = ARGB(255, 255, 0, 0);
	switch(_err)
	{
	case REQ_ERR_LEVEL:
		{
			return InsertBoardMsg(dwErrCol, "���ĵȼ�����,�޷�װ��");
		}break;
	case REQ_ERR_DC:
		{
			return InsertBoardMsg(dwErrCol, "���Ĺ���������,�޷�װ��");
		}break;
	case REQ_ERR_MC:
		{
			return InsertBoardMsg(dwErrCol, "����ħ��������,�޷�װ��");
		}break;
	case REQ_ERR_SC:
		{
			return InsertBoardMsg(dwErrCol, "���ĵ�������,�޷�װ��");
		}break;
	case REQ_ERR_SEX:
		{
			return InsertBoardMsg(dwErrCol, "�����Ա𲻷�,�޷�װ��");
		}break;
	default:
		{
			return InsertBoardMsg(dwErrCol, "����δ֪ԭ��[%d]�޷�װ��", _err);
		}break;
	}
}

void GameInfoBoardDlg::ShowItemMsg(const ItemAttrib* _pItem)
{
	if(_pItem->type == ITEM_NO)
	{
		return;
	}
	if(m_pLastShowNode->pNext == m_pFirstShowNode)
	{
		//	��Ϣ����������
		{
			//	���ƶ���
			m_pFirstShowNode = m_pFirstShowNode->pNext;
			sprintf(m_pLastShowNode->szMsg, "���� %s", _pItem->name);
			m_pLastShowNode->dwInsertTime = ::GetTickCount();
			m_pLastShowNode->dwCol = ARGB(255, 0, 255, 0);
			m_pLastShowNode = m_pLastShowNode->pNext;
		}
	}
	else
	{
		//	û�г���
		{
			sprintf(m_pLastShowNode->szMsg, "���� %s", _pItem->name);
			m_pLastShowNode->dwInsertTime = ::GetTickCount();
			m_pLastShowNode->dwCol = ARGB(255, 0, 255, 0);
			m_pLastShowNode = m_pLastShowNode->pNext;
		}
	}
}

void GameInfoBoardDlg::Clear()
{
	ZeroMemory(m_pCenterNode->szMsg, sizeof(m_pCenterNode->szMsg));
	m_pLastShowNode = m_pFirstShowNode = m_pCenterNode;
}

void GameInfoBoardDlg::Update(float _dt)
{
	DWORD dwCurrentMs = ::GetTickCount();

	while(m_pFirstShowNode != m_pLastShowNode)
	{
		if(dwCurrentMs - m_pFirstShowNode->dwInsertTime> MSG_DELETE_TIME_MS)
		{
			m_pFirstShowNode = m_pFirstShowNode->pNext;
		}
		else
		{
			break;
		}
	}
}

void GameInfoBoardDlg::Render()
{
	LPBoardMsgNode pFirst = m_pFirstShowNode;
	DWORD dwLine = 0;
	while(pFirst != m_pLastShowNode)
	{
		//m_font->SetColor(pFirst->dwCol);
		AfxGetPrinter()->SetColor(pFirst->dwCol);
		AfxGetPrinter()->Print(m_rcClient.left, m_rcClient.top + DRAW_LINE_OFFSET * dwLine, pFirst->szMsg);
		++dwLine;
		pFirst = pFirst->pNext;
	}
}