#include "GameStatusDlg.h"
#include "../GameScene/GameResourceManager.h"
//////////////////////////////////////////////////////////////////////////
GameStatusDlg::GameStatusDlg()
{
	m_rcClient.top = 5;
	m_rcClient.bottom = m_rcClient.top + 22;
	m_rcClient.right = m_rcClient.left = WINDOW_WIDTH - 157 - 10;

	m_pRender = new hgeSprite(0, 0, 0, 0, 0);
}

void GameStatusDlg::Update(float _dt)
{
	DWORD dwTick = GetTickCount();

	StatusInfoItemList::iterator begIter = m_xStatusList.begin();
	bool bNeedAlign = false;
	
	for(begIter;
		begIter != m_xStatusList.end();
		)
	{
		StatusInfoItem* pItem = *begIter;

		if(dwTick > pItem->stInfo.dwExpireTime)
		{
			begIter = m_xStatusList.erase(begIter);
			SAFE_DELETE(pItem);
			bNeedAlign = true;
		}
		else if(pItem->stInfo.dwExpireTime - dwTick < 8000)
		{
			if(!pItem->bAlphaActivated)
			{
				pItem->bAlphaActivated = true;
				pItem->dwAlphaActivateTime = dwTick;
				pItem->bAlphaDown = true;
			}
			else
			{
				if(dwTick - pItem->dwAlphaActivateTime > 100)
				{
					if(pItem->bAlphaDown)
					{
						DWORD dwAlpha = pItem->dwAlphaValue - 10;
						if(dwAlpha > 255)
						{
							dwAlpha = 0;
							pItem->bAlphaDown = false;
							pItem->dwAlphaValue = 0;
						}
						else
						{
							pItem->dwAlphaValue = dwAlpha;
						}
					}
					else
					{
						DWORD dwAlpha = pItem->dwAlphaValue + 10;
						if(dwAlpha > 255)
						{
							dwAlpha = 255;
							pItem->bAlphaDown = true;
							pItem->dwAlphaValue = 255;
						}
						else
						{
							pItem->dwAlphaValue = dwAlpha;
						}
					}
				}
			}

			++begIter;
		}
		else
		{
			pItem->bAlphaActivated = false;
			pItem->dwAlphaActivateTime = 0;
			pItem->dwAlphaValue = 255;
			++begIter;
		}
	}

	if(bNeedAlign)
	{
		AlignItems();
	}
}

bool GameStatusDlg::ProcUserCmd(const POINT& _mp)
{
	if(m_rcClient.left == m_rcClient.right)
	{
		return false;
	}

	if(!PtInRect(&m_rcClient, _mp))
	{
		return false;
	}

	GameMainOptUI::GetInstance()->GetIntroDlg()->ShowNothing();
	DWORD dwTick = GetTickCount();

	StatusInfoItemList::const_iterator begIter = m_xStatusList.begin();
	StatusInfoItemList::const_iterator endIter = m_xStatusList.end();
	int nIndex = 0;

	for(begIter;
		begIter != endIter;
		++begIter)
	{
		StatusInfoItem* pItem = *begIter;

		if(PtInRect(&pItem->stRcItem, _mp))
		{
			char szMsgTip[MAX_PATH];

			DWORD dwLeftTime = pItem->stInfo.dwExpireTime - dwTick;
			DWORD dwLeftSec = (dwLeftTime) / 1000;
			if(dwLeftSec > 60)
			{
				//	����60�� ���÷�����ʾ
				sprintf(szMsgTip, "[%s] ʣ��ʱ��: %d m", GetStatusString(pItem) , dwLeftSec / 60);
			}
			else
			{
				sprintf(szMsgTip, "[%s] ʣ��ʱ��: %d s", GetStatusString(pItem) , dwLeftSec);
			}
			GameMainOptUI::GetInstance()->GetIntroDlg()->ShowString(szMsgTip);

			return true;
		}

		++nIndex;
	}

	return false;
}

void GameStatusDlg::Render()
{
	if(m_rcClient.left == m_rcClient.right)
	{
		return;
	}

	GameTextureManager* pTex = GameResourceManager::GetInstance()->GetTexs(RES_STATUS);
	HTEXTURE tex = 0;
	if(NULL == pTex)
	{
		return;
	}

	StatusInfoItemList::const_iterator begIter = m_xStatusList.begin();
	StatusInfoItemList::const_iterator endIter = m_xStatusList.end();
	int nIndex = 0;

	for(begIter;
		begIter != endIter;
		++begIter)
	{
		StatusInfoItem* pItem = *begIter;

		tex = pTex->GetTexture(pItem->stInfo.nStatusID);

		if(0 == tex)
		{
			continue;
		}

		m_pRender->SetTexture(tex);
		m_pRender->SetTextureRect(0, 0, 24, 22);

		if(pItem->bAlphaDown)
		{
			m_pRender->SetColor(ARGB(pItem->dwAlphaValue, 255, 255, 255));
		}
		else
		{
			m_pRender->SetColor(ARGB_WHITE);
		}

		m_pRender->Render(pItem->stRcItem.left, pItem->stRcItem.top);

		++nIndex;
	}
}

void GameStatusDlg::RemoveStatus(int _nStatusID)
{
	StatusInfoItemList::iterator begIter = m_xStatusList.begin();
	StatusInfoItemList::const_iterator endIter = m_xStatusList.end();

	for(begIter;
		begIter != endIter;
		++begIter)
	{
		StatusInfoItem* pListItem = *begIter;

		if(pListItem->stInfo.nStatusID == _nStatusID)
		{
			delete pListItem;
			m_xStatusList.erase(begIter);
			return;
		}
	}
}

void GameStatusDlg::AddStatus(int _nStatusID, DWORD _dwLastTime)
{
	DWORD dwTick = GetTickCount();

	StatusInfoItem* pItem = NULL;
	int nExtraInf = 0;
	
	//	search for already exists item
	StatusInfoItemList::iterator begIter = m_xStatusList.begin();
	StatusInfoItemList::const_iterator endIter = m_xStatusList.end();

	if (_nStatusID == GSTATUS_SUITSAMELEVEL)
	{
		nExtraInf = LOWORD(_dwLastTime);
		_dwLastTime = HIWORD(_dwLastTime);
	}

	for(begIter;
		begIter != endIter;
		++begIter)
	{
		StatusInfoItem* pListItem = *begIter;

		if(pListItem->stInfo.nStatusID == _nStatusID)
		{
			pListItem->stInfo.dwExpireTime = dwTick + _dwLastTime;
			pListItem->stInfo.nParam = nExtraInf;
			m_xStatusList.erase(begIter);
			pItem = pListItem;
			break;
		}
	}
	
	if(NULL == pItem)
	{
		pItem = new StatusInfoItem;
		pItem->stInfo.nStatusID = _nStatusID;
		pItem->stInfo.dwExpireTime = dwTick + _dwLastTime;
		pItem->stInfo.nParam = nExtraInf;
	}

	if (_nStatusID == GSTATUS_SUITSAMELEVEL)
	{
		pItem->stInfo.dwExpireTime = 0xffffffff;
	}
	
	//int nSize = m_xStatusList.size();

	/*pItem->stRcItem.right = m_rcClient.right - (24 + 10) * nSize;
	pItem->stRcItem.left = pItem->stRcItem.right - 24;
	pItem->stRcItem.top = m_rcClient.top;
	pItem->stRcItem.bottom = m_rcClient.bottom;*/
	

	//m_rcClient.left = pItem->stRcItem.left;
	m_xStatusList.push_back(pItem);
	AlignItems();
}

void GameStatusDlg::AlignItems()
{
	StatusInfoItemList::const_iterator begIter = m_xStatusList.begin();
	StatusInfoItemList::const_iterator endIter = m_xStatusList.end();

	if(m_xStatusList.empty())
	{
		return;
	}

	int nIndex = 0;

	for(begIter;
		begIter != endIter;
		++begIter)
	{
		StatusInfoItem* pListItem = *begIter;

		pListItem->stRcItem.right = m_rcClient.right - nIndex * (24 + 8);
		pListItem->stRcItem.left = pListItem->stRcItem.right - 24;
		pListItem->stRcItem.top = m_rcClient.top;
		pListItem->stRcItem.bottom = m_rcClient.bottom;

		++nIndex;
	}

	m_rcClient.left = m_rcClient.right - (nIndex - 1) * (24 + 8) - 24;
}

//////////////////////////////////////////////////////////////////////////
const char* GameStatusDlg::GetStatusString(StatusInfoItem* _pItem)
{
	if(_pItem->stInfo.nStatusID >= 200)
	{
		switch(_pItem->stInfo.nStatusID)
		{
		case GSTATUS_DOUBLEDROP:
			{
				return "˫������";
			}break;
		case GSTATUS_POISON:
			{
				return "�ж�";
			}break;
		case GSTATUS_SHIELD:
			{
				return "ħ����";
			}break;
		case GSTATUS_ENERGYSHIELD:
			{
				return "��������";
			}break;
		case GSTATUS_CHARMAC:
			{
				return "��ʥս����";
			}break;
		case GSTATUS_JINGANG:
			{
				return "��ղ���";
			}break;
		case GSTATUS_SUITSAMELEVEL:
			{
				if (1 == _pItem->stInfo.nParam)
				{
					return "��Ʒ��װ ���1.03��";
				}
				else if(2 == _pItem->stInfo.nParam)
				{
					return "������װ ���1.06��"; 
				}
				else if(3 == _pItem->stInfo.nParam)
				{
					return "��Ͼ��װ ���1.09��"; 
				}
				else if(4 == _pItem->stInfo.nParam)
				{
					return "������װ ���1.12��"; 
				}
				else if(5 == _pItem->stInfo.nParam)
				{
					return "׿Խ��װ ���1.15��"; 
				}
				else if(6 == _pItem->stInfo.nParam)
				{
					return "�Ƿ���װ ���1.20��"; 
				}
				else if(7 == _pItem->stInfo.nParam)
				{
					return "��˵��װ ���1.30��"; 
				}
				else if(8 == _pItem->stInfo.nParam)
				{
					return "ʷʫ��װ ���1.40��"; 
				}
			}break;
		}
	}

	return "";
}