#include "GameDisplayDlg.h"
#include "../BackMir/BackMir.h"
#include "../GameScene/GameInfoManager.h"
#include "../GameScene/GameResourceManager.h"
#include "../../CommonModule/ItemGrowUtils.h"
#include "../../CommonModule/HideAttribHelper.h"
#include "../Common/gfx_utils.h"
#include "../../CommonModule/PotentialAttribHelper.h"
#include "../../CommonModule/StoveManager.h"
//////////////////////////////////////////////////////////////////////////
#define COLOR_SUITNAME			ARGB(255, 178, 234, 97)
#define COLOR_SUITNAMEITEM_ACTIVE	/*ARGB(255, 152, 199, 146)*/0xffBF3EFF
#define COLOR_SUITNAMEITEM_UNACTIVE	ARGB(255, 135, 135, 135)
#define COLOR_EXTRAATRRIB_ACTIVE	ARGB(255, 103, 224, 246)
#define COLOR_EXTRAATTRIB_UNACTIVE	ARGB(255, 135, 135, 135)
//////////////////////////////////////////////////////////////////////////
static const DWORD s_dwItemNameColor = ARGB(255, 195, 119, 69);

static const int s_nTitleHeight = 16;
static const int s_nTopBoundary = 5;
static const int s_nLeftBoundary = 5;
static const int s_nEachLineHeight = 13;
static const int s_nItemIconWidth = 48;
static const int s_nItemIconHeight = 48;
static const int s_nItemIntroGapX = 5;

static const char* s_pszItemLevel[] =
{
	"��ͨ",
	"��Ʒ",
	"����",
	"��Ͼ",
	"����",
	"׿Խ",
	"�Ƿ�",
	"��˵",
	"ʷʫ"
};
//////////////////////////////////////////////////////////////////////////
bool IsEquipItem(const ItemAttrib* _pItem)
{
	return pTheGame->IsEquipItem(*_pItem);
}
//////////////////////////////////////////////////////////////////////////
GameDisplayDlg::GameDisplayDlg()
{
	SetCustomBk();

	m_nScreenHeight = WINDOW_HEIGHT;
	m_nScreenWidth = WINDOW_WIDTH;

	ResetItemAttirb();

	m_pRender = new hgeSprite(0, 0, 0, 0, 0);

	if(!PotentialAttribHelper::Test())
	{
		::MessageBox(NULL, "potential error.", "error", MB_OK | MB_ICONERROR);
	}
}

GameDisplayDlg::~GameDisplayDlg()
{

}

void GameDisplayDlg::ResetItemAttirb()
{
	ZeroMemory(m_stItems, sizeof(m_stItems));
	ZeroMemory(&m_stShowItem, sizeof(ItemAttrib));
	m_nOwner = 0;
}

void GameDisplayDlg::ClearItems()
{
	if(m_xItemList.empty())
	{
		return;
	}

	std::list<DisplayItem*>::const_iterator begIter = m_xItemList.begin();
	std::list<DisplayItem*>::const_iterator endIter = m_xItemList.end();

	for(begIter;
		begIter != endIter;
		++begIter)
	{
		DisplayItem* pDisplayItem = *begIter;
		m_xItemFactory.PushFreePtr(pDisplayItem);
	}

	m_xItemList.clear();
}

DisplayItem* GameDisplayDlg::AddStringItem(const char* _pszText, int _nx, int _ny, DWORD _dwColor /* = 0 */, GfxFont* _pFont /* = NULL */)
{
	DisplayItem* pItem = m_xItemFactory.PopFreePtr();
	pItem->nShowType = SHOW_STRING;
	pItem->xText = _pszText;
	pItem->nPosX = _nx;
	pItem->nPosY = _ny;
	pItem->dwColor = _dwColor;
	pItem->pFont = _pFont;

	m_xItemList.push_back(pItem);
	return pItem;
}

DisplayItem* GameDisplayDlg::AddTextureItem(int _nMgrIndex, int _nTextureIndex, int _nx, int _ny)
{
	DisplayItem* pItem = m_xItemFactory.PopFreePtr();
	pItem->nShowType = SHOW_TEXTURE;
	pItem->nMgrIndex = _nMgrIndex;
	pItem->nTextureIndex = _nTextureIndex;
	pItem->nPosX = _nx;
	pItem->nPosY = _ny;

	m_xItemList.push_back(pItem);
	return pItem;
}

void GameDisplayDlg::Render()
{
	if(!IsVisible())
	{
		return;
	}

	if(m_xItemList.empty())
	{
		return;
	}
	if(m_rcClient.left == m_rcClient.right ||
		m_rcClient.top == m_rcClient.bottom)
	{
		return;
	}

	hgeResourceManager* pResMgr = pTheGame->GetGameRes();
	HTEXTURE tex = 0;

	GameTextureManager* pPngMgr = GameResourceManager::GetInstance()->GetTexs(RES_CUSTOM);

	{
		//	draw bk
		//tex = pResMgr->GetTexture("bmcolor");
		tex = pPngMgr->GetTexture(19);
		//tex = pPngMgr->GetTexture(16);

		if(tex)
		{
			RECT rcScale9 = {9, 9, 9, 9};
			RECT rcTex = {0, 0, 32, 32};
			Gfx_SetRenderState(kGfx_9Path_CenterStretch, 1);
			Gfx_Render9Path(m_pRender, tex, &rcTex, &rcScale9, m_rcClient.left, m_rcClient.top, RECT_WIDTH(m_rcClient), RECT_HEIGHT(m_rcClient));
			/*m_pRender->SetTexture(tex);
			m_pRender->SetTextureRect(0,
				0,
				m_rcClient.right - m_rcClient.left,
				m_rcClient.bottom - m_rcClient.top);
			m_pRender->Render(m_rcClient.left,
				m_rcClient.top);*/
		}
	}

	int nDrawX = 0;
	int nDrawY = 0;

	{
		//	render strings
		std::list<DisplayItem*>::const_iterator begIter = m_xItemList.begin();
		std::list<DisplayItem*>::const_iterator endIter = m_xItemList.end();

		for(begIter;
			begIter != endIter;
			++begIter)
		{
			DisplayItem* pDisplayItem = *begIter;

			if(pDisplayItem)
			{
				GfxFont* pFont = pDisplayItem->pFont;

				if(SHOW_STRING == pDisplayItem->nShowType)
				{
					if(NULL == pFont)
					{
						//	default font
						pFont = AfxGetFont();

						//	need line feed?
						if(pDisplayItem->nTextLineFeedWidth != 0)
						{
							pFont = AfxGetPrinter();
						}
					}

					pFont->SetColor(ARGB_WHITE);
					if(0 != pDisplayItem->dwColor)
					{
						pFont->SetColor(pDisplayItem->dwColor);
					}

					nDrawX = RELATIVE_X(pDisplayItem->nPosX);
					nDrawY = RELATIVE_Y(pDisplayItem->nPosY);

					if(pDisplayItem->nTextLineFeedWidth != 0)
					{
						TextPrinter* pPrinter = (TextPrinter*)pFont;
						float fPrevKerningHeight = pPrinter->GetKerningHeight();

						pPrinter->SetKerningHeight(pDisplayItem->nTextKerningHeight);
						pPrinter->LineFeedRender(nDrawX,
							nDrawY,
							pDisplayItem->xText.c_str(),
							pDisplayItem->nTextLineFeedWidth,
							false);
						pPrinter->SetKerningHeight(fPrevKerningHeight);
					}
					else
					{
						pFont->Print((float)nDrawX,
							(float)nDrawY,
							pDisplayItem->xText.c_str());
					}
				}
				else if(SHOW_TEXTURE == pDisplayItem->nShowType)
				{
					nDrawX = RELATIVE_X(pDisplayItem->nPosX);
					nDrawY = RELATIVE_Y(pDisplayItem->nPosY);

					GameTextureManager* pTexMgr = GameResourceManager::GetInstance()->GetTexs(pDisplayItem->nMgrIndex);
					if(pTexMgr)
					{
						tex = pTexMgr->GetTexture(pDisplayItem->nTextureIndex);
						if(tex)
						{
							int nTextureWidth = pTexMgr->GetTextureWidth(pDisplayItem->nTextureIndex);
							int nTextureHeight = pTexMgr->GetTextureHeight(pDisplayItem->nTextureIndex);

							if(pDisplayItem->bCenter)
							{
								nDrawX += (pDisplayItem->nCenterWidth - nTextureWidth) / 2;
								nDrawY += (pDisplayItem->nCenterHeight - nTextureHeight) / 2;
							}

							m_pRender->SetTexture(tex);
							m_pRender->SetTextureRect(0, 0,
								nTextureWidth, nTextureHeight);
							m_pRender->Render(nDrawX, nDrawY);
						}
					}
				}
			}
		}
	}
}

bool GameDisplayDlg::ProcUserCmd(const POINT& _mp)
{
	int nShowX = _mp.x;
	int nShowY = _mp.y;
	int nClientWidth = m_rcClient.right - m_rcClient.left;
	int nClientHeight = m_rcClient.bottom - m_rcClient.top;
	nShowX += 5;
	nShowY += 5;

	if(nShowX + nClientWidth > m_nScreenWidth)
	{
		int nOffset = nShowX + nClientWidth - m_nScreenWidth;
		nShowX -= nOffset;
	}
	if(nShowY + nClientHeight > m_nScreenHeight)
	{
		int nOffset = nShowY + nClientHeight - m_nScreenHeight;
		nShowY -= nOffset;
	}
	MoveTo(nShowX, nShowY);

	return true;
}

void GameDisplayDlg::SetOwnerEquip(const ItemAttrib* _pItems)
{
	//	�ѱ��˵�װ�����Ƶ��ڴ���
	for(int i = 0; i < sizeof(m_stItems) / sizeof(m_stItems[0]); ++i)
	{
		memcpy(&m_stItems[i], _pItems + i, sizeof(ItemAttrib));
	}
}

void GameDisplayDlg::SetOwnerEquip(const ItemAttrib* _pItem, int _nIndex)
{
	if(_nIndex < 0 ||
		_nIndex >= sizeof(m_stItems) / sizeof(m_stItems[0]))
	{
		return;
	}
	memcpy(&m_stItems[_nIndex], _pItem, sizeof(ItemAttrib));
}

void GameDisplayDlg::ShowItemAttrib(const ItemAttrib* _pItem, int _nOwner /* = -1 */)
{
	//	��ʾ������װ�� ��Ҫ��ǰ����������ұ��װ��
	int nDestOwner = _nOwner;

	if(-1 == nDestOwner)
	{
		//	(GamePlayer::GetInstance())
	}

	m_nOwner = nDestOwner;

	//	�ж���Ʒtag, tag��ͬ��������ʾ�б�
	if(m_stShowItem.tag != 0)
	{
		if(m_stShowItem.tag == _pItem->tag)
		{
			ShowDialog();
			return;
		}
	}

	memcpy(&m_stShowItem, _pItem, sizeof(ItemAttrib));

	GenShowItems();
	ShowDialog();
}

void GameDisplayDlg::GenShowItems()
{
	ClearItems();

	if(m_stShowItem.type == 0)
	{
		return;
	}
	//	Reset the original item attribute
	ItemAttrib stOriItem;
	ZeroMemory(&stOriItem, sizeof(ItemAttrib));
	if(IsEquipItem(&m_stShowItem))
	{
		stOriItem.id = m_stShowItem.id;
		if(GameInfoManager::GetInstance()->GetItemAttrib(m_stShowItem.id, &stOriItem))
		{
			//	Calculate the addition of the equipment
#define GET_ITEM_ADDITION(ATTRIB)	stOriItem.ATTRIB = m_stShowItem.ATTRIB - stOriItem.ATTRIB;
			GET_ITEM_ADDITION(maxAC);
			GET_ITEM_ADDITION(maxMAC);
			GET_ITEM_ADDITION(maxDC);
			GET_ITEM_ADDITION(maxMC);
			GET_ITEM_ADDITION(maxSC);
			GET_ITEM_ADDITION(lucky);
			GET_ITEM_ADDITION(atkSpeed);
			GET_ITEM_ADDITION(moveSpeed);
			GET_ITEM_ADDITION(accuracy);
			GET_ITEM_ADDITION(hide);
		}
		//else
		{
			//ZeroMemory(&m_oriItem, sizeof(ItemAttrib));
		}
	}

	//	start
	//int nRectHeight = 0;
	int nMaxWidth = 0;
	int nCurWidth = 0;

#define TEST_MAXWIDTH(width)				{if(width > nMaxWidth){nMaxWidth = width;}};
#define TEST_TEXTWIDTH(text, fsize)			{int nTextLength = GetTextWidth(text, fsize);TEST_MAXWIDTH(nTextLength);}

	int nDrawRectX = s_nLeftBoundary;
	int nDrawRectY = s_nTopBoundary;

	int nCurX = nDrawRectX;
	int nCurY = nDrawRectY;

	//	stove attrib
	const StoveAttribInfo* pStoveAttribInfo = NULL;

	char szText[MAX_PATH];
	//	first, quality
	int nQualityIndex = m_stShowItem.level;
	nQualityIndex = GetItemUpgrade(m_stShowItem.level);
	if(nQualityIndex < 0 ||
		nQualityIndex >= sizeof(s_pszItemLevel) / sizeof(s_pszItemLevel[0]))
	{
		nQualityIndex = 0;
	}

	char szItemName[40] = {0};
	strcpy(szItemName, m_stShowItem.name);
	
	if(IsEquipItem(&m_stShowItem) &&
		m_stShowItem.MP != 0)
	{
		int nPotentialIndex = PotentialAttribHelper::GetPotentialIndex(m_stShowItem.MP);
		pStoveAttribInfo = StoveManager::GetInstance()->GetStoveAttrib(nPotentialIndex);

		if(NULL != pStoveAttribInfo &&
			0 != pStoveAttribInfo->nAttribCount)
		{
			strcat(szItemName, "|");
			strcat(szItemName, pStoveAttribInfo->szName);
		}
	}

	//	name
	TEST_TEXTWIDTH(szItemName, 14);
	DisplayItem* pNameItem = AddStringItem(szItemName, nCurX, nCurY, ARGB_YELLOW, AfxGetFont14());
	//nRectHeight += s_nTitleHeight;
	int nStarHeight = 0;

	//	�Ƿ�Ҫ��װ��������
	if(IsEquipItem(&m_stShowItem) &&
		nQualityIndex > 0)
	{
		nCurWidth = 14 * nQualityIndex;
		TEST_MAXWIDTH(nCurWidth);
		nCurX = s_nLeftBoundary;
		nCurY = s_nTopBoundary + s_nTitleHeight;

		//	add title height
		nStarHeight = 14;

		for(int i = 0; i < nQualityIndex; ++i)
		{
			AddTextureItem(RES_CUSTOM, 20, nCurX + i * 14, nCurY);
		}
	}

	//	item icon
	nCurX = s_nLeftBoundary;
	nCurY = s_nTopBoundary;
	nCurY += s_nTitleHeight;
	nCurY += nStarHeight;
	AddTextureItem(RES_CUSTOM, 0, nCurX, nCurY);
	DisplayItem* pIconItem = AddTextureItem(RES_ITEMS, m_stShowItem.tex, nCurX, nCurY);
	pIconItem->bCenter = true;
	pIconItem->nCenterWidth = s_nItemIconWidth;
	pIconItem->nCenterHeight = s_nItemIconHeight;
	//nRectHeight += s_nItemIconHeight;

	int nIntroLine = 0;
	//	type
	sprintf(szText, "���ͣ�%s",
		GetItemDescribeName(&m_stShowItem));
	nCurWidth = s_nItemIconWidth + s_nItemIntroGapX + GetTextWidth(szText, 12);
	TEST_MAXWIDTH(nCurWidth);
	nCurX = nDrawRectX;
	nCurY = nDrawRectY;
	nCurX += s_nItemIconWidth + s_nItemIntroGapX;
	nCurY += s_nTitleHeight + 5 + nIntroLine * s_nEachLineHeight + nStarHeight;
	AddStringItem(szText, nCurX, nCurY);
	++nIntroLine;

	//	weight
	sprintf(szText, "������%d", m_stShowItem.weight);
	nCurWidth = s_nItemIconWidth + s_nItemIntroGapX + GetTextWidth(szText, 12);
	TEST_MAXWIDTH(nCurWidth);
	nCurX = nDrawRectX;
	nCurY = nDrawRectY;
	nCurX += s_nItemIconWidth + s_nItemIntroGapX;
	nCurY += s_nTitleHeight + 5 + nIntroLine * s_nEachLineHeight + nStarHeight;
	AddStringItem(szText, nCurX, nCurY);
	++nIntroLine;

	//	bind or not
	sprintf(szText, "%s", TEST_FLAG_BOOL(m_stShowItem.atkPois, 0x1) ? "[�Ѱ�]" : "[δ��]");
	nCurWidth = s_nItemIconWidth + s_nItemIntroGapX + GetTextWidth(szText, 12);
	TEST_MAXWIDTH(nCurWidth);
	nCurX = nDrawRectX;
	nCurY = nDrawRectY;
	nCurX += s_nItemIconWidth + s_nItemIntroGapX;
	nCurY += s_nTitleHeight + 5 + nIntroLine * s_nEachLineHeight + nStarHeight;
	AddStringItem(szText, nCurX, nCurY);
	++nIntroLine;

	//	details
	int nDetailLine = 0;
	int nDetailDrawX = s_nLeftBoundary;
	int nDetailDrawY = s_nTopBoundary + s_nTitleHeight + s_nItemIconHeight + s_nItemIntroGapX + nStarHeight;

	if(IsEquipItem(&m_stShowItem))
	{
		//	get stove multi
		float fDefenceMulti = 1.0f;
		float fAttackMulti = 1.0f;

		if(NULL != pStoveAttribInfo)
		{
			//	find enhance defence and attack attrib
			for(int i = 0; i < pStoveAttribInfo->nAttribCount; ++i)
			{
				int nAttribId = LOWORD(pStoveAttribInfo->dwAttribs[i]);
				int nAttribValue = HIWORD(pStoveAttribInfo->dwAttribs[i]);

				if(nAttribId == kStoveAttrib_EnhanceDefence)
				{
					fDefenceMulti += float(nAttribValue) / 100;
				}
				else if(nAttribId == kStoveAttrib_EnhanceAttack)
				{
					fAttackMulti += float(nAttribValue) / 100;
				}
			}
		}

		//	*equipment*
		//	equipment information
		nCurX = nDetailDrawX;
		nCurY = nDetailDrawY + nDetailLine * s_nEachLineHeight;
		nCurWidth = 5 * 12;
		TEST_MAXWIDTH(nCurWidth);
		AddStringItem("[װ����Ϣ]", nCurX, nCurY);
		++nDetailLine;

		//	dura
		nCurX = nDetailDrawX;
		nCurY = nDetailDrawY + nDetailLine * s_nEachLineHeight;
		int nDura = HIWORD(m_stShowItem.maxHP) / 1000;
		int nDuraMax = LOWORD(m_stShowItem.maxHP) / 1000;
		sprintf(szText, "�־ã�%d/%d", nDura, nDuraMax);
		nCurWidth = GetTextWidth(szText, 12);
		TEST_MAXWIDTH(nCurWidth);
		AddStringItem(szText, nCurX, nCurY);
		++nDetailLine;

		//	use time left
		if (m_stShowItem.id == 172)
		{
			// ���ͽ�ָ
			nCurX = nDetailDrawX;
			nCurY = nDetailDrawY + nDetailLine * s_nEachLineHeight;
			int nDiv = 25;
			if (pTheGame->GetGameMode() == GM_LOGIN)
			{
				nDiv = 100;
			}
			int nUseTimeLeft = LOWORD(m_stShowItem.maxHP) / nDiv;
			sprintf(szText, "��ʹ�ô�����%d", nUseTimeLeft);
			nCurWidth = GetTextWidth(szText, 12);
			TEST_MAXWIDTH(nCurWidth);
			AddStringItem(szText, nCurX, nCurY);
			++nDetailLine;
		}

		//	quality
		nCurX = nDrawRectX;
		nCurY = nDetailDrawY + nDetailLine * s_nEachLineHeight;
		sprintf(szText, "Ʒ�ʣ�%s", s_pszItemLevel[nQualityIndex]);
		nCurWidth = GetTextWidth(szText, 12);
		TEST_MAXWIDTH(nCurWidth);
		AddStringItem(szText, nCurX, nCurY, g_dwItemColor[nQualityIndex]);
		++nDetailLine;

		//	level
		int nLevel = GetItemGrade(m_stShowItem.id);
		nCurX = nDrawRectX;
		nCurY = nDetailDrawY + nDetailLine * s_nEachLineHeight;
		sprintf(szText, "����%d", nLevel);
		nCurWidth = GetTextWidth(szText, 12);
		TEST_MAXWIDTH(nCurWidth);
		AddStringItem(szText, nCurX, nCurY);
		++nDetailLine;

		//	a blank line
		++nDetailLine;

		//	equipment base attribute
		nCurX = nDetailDrawX;
		nCurY = nDetailDrawY + nDetailLine * s_nEachLineHeight;
		nCurWidth = 5 * 12;
		TEST_MAXWIDTH(nCurWidth);
		AddStringItem("[��������]", nCurX, nCurY);
		++nDetailLine;

		//	AC
		if(m_stShowItem.AC != 0 ||
			m_stShowItem.maxAC != 0)
		{
			if(m_stShowItem.type != ITEM_WEAPON &&
				m_stShowItem.type != ITEM_NECKLACE)
			{
				nCurX = nDetailDrawX;
				nCurY = nDetailDrawY;
				nCurY += s_nEachLineHeight * nDetailLine;
				
				int nMaxAC = m_stShowItem.maxAC;
				nMaxAC *= fDefenceMulti;

				if(stOriItem.maxAC != 0)
				{
					sprintf(szText, "������%d-%d(+%d)",
						m_stShowItem.AC, nMaxAC, stOriItem.maxAC);
					nCurWidth = GetTextWidth(szText, 12);
					TEST_MAXWIDTH(nCurWidth);
					AddStringItem(szText, nCurX, nCurY, ARGB_GREEN);
				}
				else
				{
					sprintf(szText, "������%d-%d",
						m_stShowItem.AC, nMaxAC);
					nCurWidth = GetTextWidth(szText, 12);
					TEST_MAXWIDTH(nCurWidth);
					AddStringItem(szText, nCurX, nCurY);
				}

				++nDetailLine;
			}
		}

		//	MAC
		if(m_stShowItem.MAC != 0 ||
			m_stShowItem.maxMAC != 0)
		{
			if(m_stShowItem.type != ITEM_WEAPON &&
				m_stShowItem.type != ITEM_NECKLACE)
			{
				nCurX = nDetailDrawX;
				nCurY = nDetailDrawY;
				nCurY += s_nEachLineHeight * nDetailLine;

				int nMaxMAC = m_stShowItem.maxMAC;
				nMaxMAC *= fDefenceMulti;

				if(stOriItem.maxMAC != 0)
				{
					sprintf(szText, "ħ����%d-%d(+%d)",
						m_stShowItem.MAC, nMaxMAC, stOriItem.maxMAC);
					nCurWidth = GetTextWidth(szText, 12);
					TEST_MAXWIDTH(nCurWidth);
					AddStringItem(szText, nCurX, nCurY, ARGB_GREEN);
				}
				else
				{
					sprintf(szText, "ħ����%d-%d",
						m_stShowItem.MAC, nMaxMAC);
					nCurWidth = GetTextWidth(szText, 12);
					TEST_MAXWIDTH(nCurWidth);
					AddStringItem(szText, nCurX, nCurY);
				}

				++nDetailLine;
			}
		}

		//	DC
		if(m_stShowItem.DC != 0 ||
			m_stShowItem.maxDC != 0)
		{
			nCurX = nDetailDrawX;
			nCurY = nDetailDrawY;
			nCurY += s_nEachLineHeight * nDetailLine;

			int nMaxDC = m_stShowItem.maxDC;
			nMaxDC *= fAttackMulti;

			if(stOriItem.maxDC != 0)
			{
				sprintf(szText, "������%d-%d(+%d)",
					m_stShowItem.DC, nMaxDC, stOriItem.maxDC);
				nCurWidth = GetTextWidth(szText, 12);
				TEST_MAXWIDTH(nCurWidth);
				AddStringItem(szText, nCurX, nCurY, ARGB_GREEN);
			}
			else
			{
				sprintf(szText, "������%d-%d",
					m_stShowItem.DC, nMaxDC);
				nCurWidth = GetTextWidth(szText, 12);
				TEST_MAXWIDTH(nCurWidth);
				AddStringItem(szText, nCurX, nCurY);
			}

			++nDetailLine;
		}

		//	MC
		if(m_stShowItem.MC != 0 ||
			m_stShowItem.maxMC != 0)
		{
			nCurX = nDetailDrawX;
			nCurY = nDetailDrawY;
			nCurY += s_nEachLineHeight * nDetailLine;

			int nMaxMC = m_stShowItem.maxMC;
			nMaxMC *= fAttackMulti;

			if(stOriItem.maxMC != 0)
			{
				sprintf(szText, "ħ����%d-%d(+%d)",
					m_stShowItem.MC, nMaxMC, stOriItem.maxMC);
				nCurWidth = GetTextWidth(szText, 12);
				TEST_MAXWIDTH(nCurWidth);
				AddStringItem(szText, nCurX, nCurY, ARGB_GREEN);
			}
			else
			{
				sprintf(szText, "ħ����%d-%d",
					m_stShowItem.MC, nMaxMC);
				nCurWidth = GetTextWidth(szText, 12);
				TEST_MAXWIDTH(nCurWidth);
				AddStringItem(szText, nCurX, nCurY);
			}

			++nDetailLine;
		}

		//	SC
		if(m_stShowItem.SC != 0 ||
			m_stShowItem.maxSC != 0)
		{
			nCurX = nDetailDrawX;
			nCurY = nDetailDrawY;
			nCurY += s_nEachLineHeight * nDetailLine;

			int nMaxSC = m_stShowItem.maxSC;
			nMaxSC *= fAttackMulti;

			if(stOriItem.maxSC != 0)
			{
				sprintf(szText, "������%d-%d(+%d)",
					m_stShowItem.SC, nMaxSC, stOriItem.maxSC);
				nCurWidth = GetTextWidth(szText, 12);
				TEST_MAXWIDTH(nCurWidth);
				AddStringItem(szText, nCurX, nCurY, ARGB_GREEN);
			}
			else
			{
				sprintf(szText, "������%d-%d",
					m_stShowItem.SC, nMaxSC);
				nCurWidth = GetTextWidth(szText, 12);
				TEST_MAXWIDTH(nCurWidth);
				AddStringItem(szText, nCurX, nCurY);
			}

			++nDetailLine;
		}

		//	Lucky
		if(m_stShowItem.lucky != 0)
		{
			if(m_stShowItem.type == ITEM_WEAPON ||
				m_stShowItem.type == ITEM_NECKLACE)
			{
				nCurX = nDetailDrawX;
				nCurY = nDetailDrawY;
				nCurY += s_nEachLineHeight * nDetailLine;
				//AfxGetPrinter()->SetColor(ARGB_WHITE);
				if(stOriItem.lucky != 0)
				{
					sprintf(szText, "���ˣ�+%d(+%d)",
						m_stShowItem.lucky, stOriItem.lucky);
					nCurWidth = GetTextWidth(szText, 12);
					TEST_MAXWIDTH(nCurWidth);
					AddStringItem(szText, nCurX, nCurY, ARGB_GREEN);
				}
				else
				{
					sprintf(szText, "���ˣ�+%d",
						m_stShowItem.lucky);
					nCurWidth = GetTextWidth(szText, 12);
					TEST_MAXWIDTH(nCurWidth);
					AddStringItem(szText, nCurX, nCurY);
				}

				++nDetailLine;
			}
		}

		//	AtkSpeed
		if(m_stShowItem.atkSpeed != 0)
		{
			if(m_stShowItem.type == ITEM_RING ||
				m_stShowItem.type == ITEM_NECKLACE)
			{
				nCurX = nDetailDrawX;
				nCurY = nDetailDrawY;
				nCurY += s_nEachLineHeight * nDetailLine;
				//AfxGetPrinter()->SetColor(ARGB_WHITE);
				if(stOriItem.atkSpeed != 0)
				{
					sprintf(szText, "�����ٶȣ�+%d(+%d)",
						m_stShowItem.atkSpeed, stOriItem.atkSpeed);
					nCurWidth = GetTextWidth(szText, 12);
					TEST_MAXWIDTH(nCurWidth);
					AddStringItem(szText, nCurX, nCurY, ARGB_GREEN);
				}
				else
				{
					sprintf(szText, "�����ٶȣ�+%d",
						m_stShowItem.atkSpeed);
					nCurWidth = GetTextWidth(szText, 12);
					TEST_MAXWIDTH(nCurWidth);
					AddStringItem(szText, nCurX, nCurY);
				}

				++nDetailLine;
			}
		}

		//	MoveSpeed
		if(m_stShowItem.moveSpeed != 0)
		{
			//if(m_pItemAttrib->type == ITEM_RING ||
			//m_pItemAttrib->type == ITEM_NECKLACE)
			{
				nCurX = nDetailDrawX;
				nCurY = nDetailDrawY;
				nCurY += s_nEachLineHeight * nDetailLine;
				//AfxGetPrinter()->SetColor(ARGB_WHITE);
				if(stOriItem.moveSpeed != 0)
				{
					sprintf(szText, "�ƶ��ٶȣ�+%d(+%d)",
						m_stShowItem.moveSpeed, stOriItem.moveSpeed);
					nCurWidth = GetTextWidth(szText, 12);
					TEST_MAXWIDTH(nCurWidth);
					AddStringItem(szText, nCurX, nCurY, ARGB_GREEN);
				}
				else
				{
					sprintf(szText, "�ƶ��ٶȣ�+%d",
						m_stShowItem.moveSpeed);
					nCurWidth = GetTextWidth(szText, 12);
					TEST_MAXWIDTH(nCurWidth);
					AddStringItem(szText, nCurX, nCurY);
				}

				++nDetailLine;
			}
		}

		//	Accuracy
		if(m_stShowItem.accuracy != 0)
		{
			if(m_stShowItem.type == ITEM_BRACELAT ||
				m_stShowItem.type == ITEM_NECKLACE ||
				m_stShowItem.type == ITEM_WEAPON)
			{
				nCurX = nDetailDrawX;
				nCurY = nDetailDrawY;
				nCurY += s_nEachLineHeight * nDetailLine;
				//AfxGetPrinter()->SetColor(ARGB_WHITE);
				if(stOriItem.accuracy != 0)
				{
					sprintf(szText, "׼ȷ��+%d(+%d)",
						m_stShowItem.accuracy, stOriItem.accuracy);
					nCurWidth = GetTextWidth(szText, 12);
					TEST_MAXWIDTH(nCurWidth);
					AddStringItem(szText, nCurX, nCurY, ARGB_GREEN);
				}
				else
				{
					sprintf(szText, "׼ȷ��+%d",
						m_stShowItem.accuracy);
					nCurWidth = GetTextWidth(szText, 12);
					TEST_MAXWIDTH(nCurWidth);
					AddStringItem(szText, nCurX, nCurY);
				}

				++nDetailLine;
			}
		}

		//	Hide
		if(m_stShowItem.hide != 0)
		{
			if(m_stShowItem.type == ITEM_BRACELAT ||
				m_stShowItem.type == ITEM_NECKLACE)
			{
				nCurX = nDetailDrawX;
				nCurY = nDetailDrawY;
				nCurY += s_nEachLineHeight * nDetailLine;
				//AfxGetPrinter()->SetColor(ARGB_WHITE);
				if(stOriItem.hide != 0)
				{
					sprintf(szText, "��ܣ�+%d(+%d)",
						m_stShowItem.hide, stOriItem.hide);
					nCurWidth = GetTextWidth(szText, 12);
					TEST_MAXWIDTH(nCurWidth);
					AddStringItem(szText, nCurX, nCurY, ARGB_GREEN);
				}
				else
				{
					sprintf(szText, "��ܣ�+%d",
						m_stShowItem.hide);
					nCurWidth = GetTextWidth(szText, 12);
					TEST_MAXWIDTH(nCurWidth);
					AddStringItem(szText, nCurX, nCurY);
				}

				++nDetailLine;
			}
		}
	}
	else if(m_stShowItem.type == ITEM_COST)
	{
		//	[��Ʒ��Ϣ]
		{
			nCurX = nDetailDrawX;
			nCurY = nDetailDrawY;
			nCurY += s_nEachLineHeight * nDetailLine;
			sprintf(szText, "[��Ʒ��Ϣ]");
			nCurWidth = 12 * 5;
			TEST_MAXWIDTH(nCurWidth);
			AddStringItem(szText, nCurX, nCurY);
			++nDetailLine;
		}
		//	Count
		{
			nCurX = nDetailDrawX;
			nCurY = nDetailDrawY;
			nCurY += s_nEachLineHeight * nDetailLine;
			sprintf(szText, "������%d",
				m_stShowItem.atkSpeed);
			nCurWidth = GetTextWidth(szText, 12);
			TEST_MAXWIDTH(nCurWidth);
			AddStringItem(szText, nCurX, nCurY);
			++nDetailLine;
		}

		//	Drug
		if(m_stShowItem.curse == 0)
		{
			//	HP
			if(m_stShowItem.HP != 0)
			{
				nCurX = nDetailDrawX;
				nCurY = nDetailDrawY;
				nCurY += s_nEachLineHeight * nDetailLine;
				if(m_stShowItem.lucky == 0)
				{
					sprintf(szText, "��������HP��%d",
						m_stShowItem.HP);
					nCurWidth = GetTextWidth(szText, 12);
					TEST_MAXWIDTH(nCurWidth);
					AddStringItem(szText, nCurX, nCurY);
				}
				else if(m_stShowItem.lucky == 1)
				{
					sprintf(szText, "���ٲ���HP��%d",
						m_stShowItem.HP);
					nCurWidth = GetTextWidth(szText, 12);
					TEST_MAXWIDTH(nCurWidth);
					AddStringItem(szText, nCurX, nCurY);
				}
				else if(m_stShowItem.lucky == 2)
				{
					sprintf(szText, "���ٲ���HP��%d%%%%",
						m_stShowItem.HP);
					nCurWidth = GetTextWidth(szText, 12);
					TEST_MAXWIDTH(nCurWidth);
					AddStringItem(szText, nCurX, nCurY);
				}
				++nDetailLine;
			}
			//	MP
			if(m_stShowItem.MP != 0)
			{
				nCurX = nDetailDrawX;
				nCurY = nDetailDrawY;
				nCurY += s_nEachLineHeight * nDetailLine;
				if(m_stShowItem.lucky == 0)
				{
					sprintf(szText, "��������MP��%d",
						m_stShowItem.MP);
					nCurWidth = GetTextWidth(szText, 12);
					TEST_MAXWIDTH(nCurWidth);
					AddStringItem(szText, nCurX, nCurY);
				}
				else if(m_stShowItem.lucky == 1)
				{
					sprintf(szText, "���ٲ���MP��%d",
						m_stShowItem.MP);
					nCurWidth = GetTextWidth(szText, 12);
					TEST_MAXWIDTH(nCurWidth);
					AddStringItem(szText, nCurX, nCurY);
				}
				else if(m_stShowItem.lucky == 2)
				{
					sprintf(szText, "���ٲ���MP��%d%%%%",
						m_stShowItem.MP);
					nCurWidth = GetTextWidth(szText, 12);
					TEST_MAXWIDTH(nCurWidth);
					AddStringItem(szText, nCurX, nCurY);
				}
				++nDetailLine;
			}
			//	Cooldown time
			if(m_stShowItem.hide != 0)
			{
				nCurX = nDetailDrawX;
				nCurY = nDetailDrawY;
				nCurY += s_nEachLineHeight * nDetailLine;
				sprintf(szText, "��ȴʱ�䣺%d��",
					m_stShowItem.hide);
				nCurWidth = GetTextWidth(szText, 12);
				TEST_MAXWIDTH(nCurWidth);
				AddStringItem(szText, nCurX, nCurY);
				++nDetailLine;
			}
		}
		else if(m_stShowItem.curse == 10)
		{
			nCurX = nDetailDrawX;
			nCurY = nDetailDrawY;
			nCurY += s_nEachLineHeight * nDetailLine;
			sprintf(szText, "��������Ǳ�ܣ�%d",
				m_stShowItem.hide);
			nCurWidth = GetTextWidth(szText, 12);
			TEST_MAXWIDTH(nCurWidth);
			AddStringItem(szText, nCurX, nCurY);
			++nDetailLine;
		}
	}
	else if(m_stShowItem.type == ITEM_BOOK)
	{
		//	[�ؼ���Ϣ]
		nCurX = nDetailDrawX;
		nCurY = nDetailDrawY;
		nCurY += s_nEachLineHeight * nDetailLine;
		nCurWidth = 12 * 5;
		TEST_MAXWIDTH(nCurWidth);
		AddStringItem("[�ؼ���Ϣ]", nCurX, nCurY);
		++nDetailLine;

		//	Job
		nCurX = nDetailDrawX;
		nCurY = nDetailDrawY;
		nCurY += s_nEachLineHeight * nDetailLine;
		//AfxGetPrinter()->SetColor(ARGB_WHITE);
		if(m_stShowItem.lucky == 1)
		{
			nCurWidth = 12 * 4;
			TEST_MAXWIDTH(nCurWidth);
			AddStringItem("սʿ�ؼ�", nCurX, nCurY);
		}
		else if(m_stShowItem.lucky == 2)
		{
			nCurWidth = 12 * 4;
			TEST_MAXWIDTH(nCurWidth);
			AddStringItem("��ʦ�ؼ�", nCurX, nCurY);
		}
		else if(m_stShowItem.lucky == 3)
		{
			nCurWidth = 12 * 4;
			TEST_MAXWIDTH(nCurWidth);
			AddStringItem("��ʿ�ؼ�", nCurX, nCurY);
		}
		else
		{
			nCurWidth = 12 * 4;
			TEST_MAXWIDTH(nCurWidth);
			AddStringItem("������", nCurX, nCurY);
		}

		++nDetailLine;
	}
	else if(m_stShowItem.type == ITEM_OTHER)
	{
		//	������
		if(m_stShowItem.curse >= 1 &&
			m_stShowItem.curse <= 8)
		{
			//	[������Ϣ]
			nCurX = nDetailDrawX;
			nCurY = nDetailDrawY;
			nCurY += s_nEachLineHeight * nDetailLine;
			nCurWidth = 12 * 5;
			TEST_MAXWIDTH(nCurWidth);
			AddStringItem("[������Ϣ]", nCurX, nCurY);
			++nDetailLine;

			//	Ʒ��
			nCurX = nDetailDrawX;
			nCurY = nDetailDrawY;
			nCurY += s_nEachLineHeight * nDetailLine;
			nCurWidth = 12 * 3 + 6;
			DWORD dwQualityColor = ARGB_WHITE;
			if(m_stShowItem.lucky >= 10)
			{
				nCurWidth += 6;
				dwQualityColor = ARGB_YELLOW;
			}
			sprintf(szText, "Ʒ�ʣ�%d", m_stShowItem.lucky);
			TEST_MAXWIDTH(nCurWidth);
			AddStringItem(szText, nCurX, nCurY, dwQualityColor);
			++nDetailLine;

			//	��Ҫ��ұ���ȼ�
			if(m_stShowItem.curse % 2 == 1)
			{
				nCurX = nDetailDrawX;
				nCurY = nDetailDrawY;
				nCurY += s_nEachLineHeight * nDetailLine;
				nCurWidth = 9 * 12 + 6;
				sprintf(szText, "��Ҫ����ܵȼ���%d", m_stShowItem.hide);
				TEST_MAXWIDTH(nCurWidth);
				AddStringItem(szText, nCurX, nCurY, ARGB_WHITE);
				++nDetailLine;
			}
		}
	}

	//	��������
	if(IsEquipItem(&m_stShowItem) &&
		m_stShowItem.maxMP != 0)
	{
		//	a line blank
		++nDetailLine;

		//	hide attrib describe
		nCurX = nDetailDrawX;
		nCurY = nDetailDrawY;
		nCurY += s_nEachLineHeight * nDetailLine;
		AddStringItem("[��������]", nCurX, nCurY);
		++nDetailLine;

		int nHideAttribCount = HideAttribHelper::GetAllAttribCount(m_stShowItem.maxMP);
		int nActiveAttribCount = HideAttribHelper::GetActiveAttribCount(m_stShowItem.maxMP);

#ifdef _DEBUG
		nActiveAttribCount = nHideAttribCount;
#endif

		for(int i = 0; i < nHideAttribCount; ++i)
		{
			if(i < nActiveAttribCount)
			{
				//	��ʾ����
				int nHideAttribType = HideAttribHelper::GetActiveAttribType(i, m_stShowItem.maxMP);
				int nHideAttribValue = HideAttribHelper::GetActiveAttribValue(i, m_stShowItem.maxMP);
				const char* pszHideAttribName = GetHideAttribString((HideAttribType)nHideAttribType);

				nCurX = nDetailDrawX;
				nCurY = nDetailDrawY;
				nCurY += s_nEachLineHeight * nDetailLine;
				sprintf(szText, "%s��+%d", pszHideAttribName, nHideAttribValue);
				nCurWidth = GetTextWidth(szText, 12);
				TEST_MAXWIDTH(nCurWidth);
				AddStringItem(szText, nCurX, nCurY, ARGB_YELLOW);
				++nDetailLine;
			}
			else
			{
				//	��������
				nCurX = nDetailDrawX;
				nCurY = nDetailDrawY;
				nCurY += s_nEachLineHeight * nDetailLine;
				AddStringItem("*δ����", nCurX, nCurY, ARGB_RED);
				++nDetailLine;
			}
		}
	}

	//	weapon grow up
	if(m_stShowItem.type == ITEM_WEAPON)
	{
		//	a line blank
		++nDetailLine;

		//	suit effect describe
		nCurX = nDetailDrawX;
		nCurY = nDetailDrawY;
		nCurY += s_nEachLineHeight * nDetailLine;
		AddStringItem("[�����ɳ�]", nCurX, nCurY);
		++nDetailLine;

		int nLevel = GetGrowLevelFromDWORD(m_stShowItem.HP);

		//	grow level
		nCurX = nDetailDrawX;
		nCurY = nDetailDrawY;
		nCurY += s_nEachLineHeight * nDetailLine;
		sprintf(szText, "�ɳ�ֵ  ��%d",
			nLevel);
		nCurWidth = GetTextWidth(szText, 12);
		TEST_MAXWIDTH(nCurWidth);
		AddStringItem(szText, nCurX, nCurY);
		++nDetailLine;

		//	lucky
		nCurX = nDetailDrawX;
		nCurY = nDetailDrawY;
		nCurY += s_nEachLineHeight * nDetailLine;
		sprintf(szText, "����    ��+%d",
			nLevel / 3);
		nCurWidth = GetTextWidth(szText, 12);
		TEST_MAXWIDTH(nCurWidth);
		AddStringItem(szText, nCurX, nCurY);
		++nDetailLine;

		//	super attack
		nCurX = nDetailDrawX;
		nCurY = nDetailDrawY;
		nCurY += s_nEachLineHeight * nDetailLine;
		sprintf(szText, "��������+%d",
			nLevel * 2);
		nCurWidth = GetTextWidth(szText, 12);
		TEST_MAXWIDTH(nCurWidth);
		AddStringItem(szText, nCurX, nCurY);
		++nDetailLine;
	}

	//	Ǳ��
	if(IsEquipItem(&m_stShowItem) &&
		m_stShowItem.MP != 0)
	{
		//	a line blank
		++nDetailLine;

		//	read it
		int nPotentialTime = PotentialAttribHelper::GetPotentialTime(m_stShowItem.MP);
		int nPotentialIndex = PotentialAttribHelper::GetPotentialIndex(m_stShowItem.MP);
		int nPotentialOffset = PotentialAttribHelper::GetPotentialOffset(m_stShowItem.MP);

		//	potential attrib describe
		nCurX = nDetailDrawX;
		nCurY = nDetailDrawY;
		nCurY += s_nEachLineHeight * nDetailLine;
		sprintf(szText, "[����Ǳ�ܴ��� %d/%d]", nPotentialTime, MAX_POTENTIAL_TIME);
		nCurWidth = GetTextWidth(szText, 12);
		TEST_MAXWIDTH(nCurWidth);
		AddStringItem(szText, nCurX, nCurY);
		++nDetailLine;

		//	�ɸ��ӵ�Ǳ�ܵȼ�
		nCurX = nDetailDrawX;
		nCurY = nDetailDrawY;
		nCurY += s_nEachLineHeight * nDetailLine;
		sprintf(szText, "���Ǳ�ܼ���%d", nPotentialOffset);
		nCurWidth = GetTextWidth(szText, 12);
		TEST_MAXWIDTH(nCurWidth);
		AddStringItem(szText, nCurX, nCurY);
		++nDetailLine;

		if(nPotentialIndex != 0)
		{
			if(NULL != pStoveAttribInfo &&
				0 != pStoveAttribInfo->nAttribCount)
			{
				//	��ǰ��Ǳ�ܵȼ�
				nCurX = nDetailDrawX;
				nCurY = nDetailDrawY;
				nCurY += s_nEachLineHeight * nDetailLine;
				sprintf(szText, "��ǰǱ�ܼ���%d", pStoveAttribInfo->nLevel);
				nCurWidth = GetTextWidth(szText, 12);
				TEST_MAXWIDTH(nCurWidth);
				AddStringItem(szText, nCurX, nCurY);
				++nDetailLine;

				WORD wAttribType = 0;
				WORD wAttribValue = 0;

				for(int i = 0; i < pStoveAttribInfo->nAttribCount; ++i)
				{
					nCurX = nDetailDrawX;
					nCurY = nDetailDrawY;
					nCurY += s_nEachLineHeight * nDetailLine;
					wAttribType = LOWORD(pStoveAttribInfo->dwAttribs[i]);
					wAttribValue = HIWORD(pStoveAttribInfo->dwAttribs[i]);
					sprintf(szText, "%s��+%d", GetStoveAttribName(StoveAttribType(wAttribType)), wAttribValue);
					nCurWidth = GetTextWidth(szText, 12);
					TEST_MAXWIDTH(nCurWidth);
					AddStringItem(szText, nCurX, nCurY, ARGB_YELLOW);
					++nDetailLine;
				}
			}
		}
	}

	if(m_stShowItem.type == ITEM_BOOK ||
		IsEquipItem(&m_stShowItem))
	{
		//	A line blank
		++nDetailLine;

		nCurX = nDetailDrawX;
		nCurY = nDetailDrawY;
		nCurY += s_nEachLineHeight * nDetailLine;
		//AfxGetPrinter()->SetColor(ARGB_WHITE);

		DWORD dwErr = 0;
		DWORD dwEquipColor = ARGB_WHITE;
		if(!GamePlayer::GetInstance()->CanEquip(&m_stShowItem, &dwErr))
		{
			//AfxGetPrinter()->SetColor(ARGB_RED);
			dwEquipColor = ARGB_RED;
		}

		if(m_stShowItem.reqType == REQ_LEVEL)
		{
			sprintf(szText, "��Ҫ�ȼ���%d",
				m_stShowItem.reqValue);
			nCurWidth = GetTextWidth(szText, 12);
			TEST_MAXWIDTH(nCurWidth);
			AddStringItem(szText, nCurX, nCurY, dwEquipColor);
		}
		else if(m_stShowItem.reqType == REQ_DC)
		{
			sprintf(szText, "��Ҫ������%d",
				m_stShowItem.reqValue);
			nCurWidth = GetTextWidth(szText, 12);
			TEST_MAXWIDTH(nCurWidth);
			AddStringItem(szText, nCurX, nCurY, dwEquipColor);
		}
		else if(m_stShowItem.reqType == REQ_MC)
		{
			sprintf(szText, "��Ҫħ����%d",
				m_stShowItem.reqValue);
			nCurWidth = GetTextWidth(szText, 12);
			TEST_MAXWIDTH(nCurWidth);
			AddStringItem(szText, nCurX, nCurY, dwEquipColor);
		}
		else if(m_stShowItem.reqType == REQ_SC)
		{
			sprintf(szText, "��Ҫ������%d",
				m_stShowItem.reqValue);
			nCurWidth = GetTextWidth(szText, 12);
			TEST_MAXWIDTH(nCurWidth);
			AddStringItem(szText, nCurX, nCurY, dwEquipColor);
		}
		else
		{
			//AfxGetPrinter()->PrintWithoutStroke(nCurX, nCurY, "��ʹ������");
			nCurWidth = 5 * 12;
			TEST_MAXWIDTH(nCurWidth);
			AddStringItem("��ʹ������", nCurX, nCurY);
		}
		++nDetailLine;
	}

	//	suit attribute
	if(IsEquipItem(&m_stShowItem) &&
		m_stShowItem.atkPalsy != 0)
	{
		const ItemExtraAttribList* pExtraAttribList = GetGlobalSuitExtraAttrib(m_stShowItem.atkPalsy);

		if(pExtraAttribList)
		{
			//	a line blank
			++nDetailLine;

			//	Suit name
			const ItemAttrib* pPlayerItem = NULL;
			bool bActived[10];
			ZeroMemory(bActived, sizeof(bActived));
			int nActiveItemSum = 0;
			int nActiveItemAll = 0;

			for(int i = 0; i < 10; ++i)
			{
				if(pExtraAttribList->nSuitEquipID[i] != 0)
				{
					++nActiveItemAll;
				}
			}

			for(int i = PLAYER_ITEM_WEAPON; i < PLAYER_ITEM_TOTAL; ++i)
			{
				if(-1 == m_nOwner)
				{
					pPlayerItem = GamePlayer::GetInstance()->GetPlayerItem((PLAYER_ITEM_TYPE)i);
				}
				else
				{
					pPlayerItem = &m_stItems[i];
				}

				if(pPlayerItem->atkPalsy == pExtraAttribList->nSuitID)
				{
					//	����Ƿ���ڹ�
					for(int j = 0; j < 10; ++j)
					{
						if(pPlayerItem->id == pExtraAttribList->nSuitEquipID[j] &&
							!bActived[j] &&
							pExtraAttribList->nSuitEquipID[j] != 0)
						{
							bActived[j] = true;
							++nActiveItemSum;
							break;
						}
					}
				}
			}

			nCurX = nDetailDrawX;
			nCurY = nDetailDrawY;
			nCurY += s_nEachLineHeight * nDetailLine;
			sprintf(szText, "[%s](%d/%d)",
				pExtraAttribList->szSuitChName,
				nActiveItemSum,
				nActiveItemAll);
			nCurWidth = GetTextWidth(szText, 12);
			TEST_MAXWIDTH(nCurWidth);
			AddStringItem(szText, nCurX, nCurY, ARGB_GREEN);
			++nDetailLine;
			
			//	suit item name
			for(int i = 0; i < 10; ++i)
			{
				if(pExtraAttribList->nSuitEquipID[i] != 0)
				{
					ItemAttrib item;
					ZeroMemory(&item, sizeof(ItemAttrib));

					if(GameInfoManager::GetInstance()->GetItemAttrib(pExtraAttribList->nSuitEquipID[i], &item))
					{
						nCurX = nDetailDrawX;
						nCurY = nDetailDrawY;
						nCurY += s_nEachLineHeight * nDetailLine;
						nCurWidth = GetTextWidth(item.name, 12);
						TEST_MAXWIDTH(nCurWidth);
						if(bActived[i])
						{
							AddStringItem(item.name, nCurX, nCurY, COLOR_SUITNAMEITEM_ACTIVE);
						}
						else
						{
							AddStringItem(item.name, nCurX, nCurY, COLOR_SUITNAMEITEM_UNACTIVE);
						}
						
						++nDetailLine;
					}
				}
			}

			//	a line blank
			++nDetailLine;

			//	suit effect describe
			nCurX = nDetailDrawX;
			nCurY = nDetailDrawY;
			nCurY += s_nEachLineHeight * nDetailLine;
			//AfxGetPrinter()->SetColor(ARGB_GREEN);
			if(pExtraAttribList->nSuitShowType == 1)
			{
				AddStringItem("[����Ч��]", nCurX, nCurY, ARGB_GREEN);
			}
			else
			{
				AddStringItem("[��װЧ��]", nCurX, nCurY, ARGB_GREEN);
			}
			++nDetailLine;

			//	suit effect
			int nActiveMax = 0;
			int nActiveAttribSum = 0;

			for(int i = 0; i < MAX_EXTRAATTIRB; ++i)
			{
				if(nActiveItemSum >= pExtraAttribList->nActiveSum[i] &&
					pExtraAttribList->nActiveSum[i] > nActiveMax)
				{
					nActiveMax = pExtraAttribList->nActiveSum[i];
					nActiveAttribSum = pExtraAttribList->nActiveAttribSum[i];
				}
			}

			ItemExtraAttribItem extraAttrib;

			for(int i = 0; i < 10; ++i)
			{
				if(pExtraAttribList->stExtraAttrib[i].nAttribID == 0)
				{
					break;
				}

				DWORD dwItemColor = COLOR_EXTRAATTRIB_UNACTIVE;
				if(nActiveAttribSum > i)
				{
					dwItemColor = COLOR_EXTRAATRRIB_ACTIVE;
				}

				extraAttrib = pExtraAttribList->stExtraAttrib[i];
				if(extraAttrib.nAttribID == EAID_AC ||
					extraAttrib.nAttribID == EAID_MAC ||
					extraAttrib.nAttribID == EAID_DC ||
					extraAttrib.nAttribID == EAID_MC ||
					extraAttrib.nAttribID == EAID_SC)
				{
					sprintf(szText, "%s%d-%d",
						g_szExtraAttribDescriptor[extraAttrib.nAttribID], HIWORD(extraAttrib.nAttribValue), LOWORD(extraAttrib.nAttribValue));
				}
				else
				{
					sprintf(szText, "%s+%d",
						g_szExtraAttribDescriptor[extraAttrib.nAttribID], extraAttrib.nAttribValue);
				}

				nCurX = nDetailDrawX;
				nCurY = nDetailDrawY;
				nCurY += s_nEachLineHeight * nDetailLine;
				nCurWidth = GetTextWidth(szText, 12);
				TEST_MAXWIDTH(nCurWidth);
				AddStringItem(szText, nCurX, nCurY, dwItemColor);
				//AfxGetPrinter()->PrintWithoutStroke(nCurX, nCurY, szText, dwItemColor);
				++nDetailLine;
			}
		}
	}

	//	addition information
	if(TEST_FLAG_BOOL(m_stShowItem.EXPR, /*EXPR_MASK_NOSAVE*/1) ||
		TEST_FLAG_BOOL(m_stShowItem.EXPR, /*EXPR_MASK_NOSELL*/2))
	{
		//	A line blank
		++nDetailLine;

		//	No save
		if(TEST_FLAG_BOOL(m_stShowItem.EXPR, /*EXPR_MASK_NOSAVE*/1))
		{
			nCurX = nDetailDrawX;
			nCurY = nDetailDrawY;
			nCurY += s_nEachLineHeight * nDetailLine;
			nCurWidth = 12 * 6;
			TEST_MAXWIDTH(nCurWidth);
			AddStringItem("���޷����棩", nCurX, nCurY, ARGB_RED);
			++nDetailLine;
		}
		if(TEST_FLAG_BOOL(m_stShowItem.EXPR, /*EXPR_MASK_NOSELL*/2))
		{
			nCurX = nDetailDrawX;
			nCurY = nDetailDrawY;
			nCurY += s_nEachLineHeight * nDetailLine;
			nCurWidth = 12 * 6;
			TEST_MAXWIDTH(nCurWidth);
			AddStringItem("���޷����ۣ�", nCurX, nCurY, ARGB_RED);
			++nDetailLine;
		}
	}

	//	item describe
	int nItemDescHeight = 0;
	ItemDesc desc;
	GameInfoManager::GetInstance()->GetItemDesc(m_stShowItem.id, &desc);
	if(desc.nTextWidth != 0)
	{
		//	a blank line
		++nDetailLine;

		nCurX = nDetailDrawX;
		nCurY = nDetailDrawY;
		nCurY += s_nEachLineHeight * nDetailLine;
		nCurWidth = 12 * 6;
		TEST_MAXWIDTH(nCurWidth);
		DisplayItem* pStringItem = AddStringItem(desc.pszDesc, nCurX, nCurY, ARGB(255, 118, 215, 65));
		pStringItem->nTextLineFeedWidth = ITEM_DESC_TEXT_WIDTH;
		pStringItem->nTextKerningHeight = 2;

		nCurWidth = ITEM_DESC_TEXT_WIDTH;
		if(desc.nTextLine == 1)
		{
			nCurWidth = GetTextWidth(desc.pszDesc, 12);
		}
		TEST_MAXWIDTH(nCurWidth);

		//	�������������ָ߶�
		nItemDescHeight = 12 * desc.nTextLine + (desc.nTextLine - 1) * pStringItem->nTextKerningHeight + 2;
	}

	//	end, add border width
	//m_rcClient.left = m_rcClient.top = 0;
	m_rcClient.right = m_rcClient.left + nMaxWidth + 2 * s_nLeftBoundary;
	m_rcClient.bottom = m_rcClient.top + s_nTitleHeight + s_nItemIconHeight + s_nItemIntroGapX + s_nEachLineHeight * nDetailLine + 2 * s_nTopBoundary + nStarHeight + nItemDescHeight;

	//	adjust name rect
	if(pNameItem)
	{
		/*int nNameCenterX = GetTextWidth(pNameItem->xText.c_str(), 14);
		nNameCenterX = (m_rcClient.right - m_rcClient.left - nNameCenterX) / 2;
		pNameItem->nPosX = nNameCenterX;*/

		int nNameLength = GetTextWidth(pNameItem->xText.c_str(), 14);
		if(nNameLength > RECT_WIDTH(m_rcClient))
		{
			m_rcClient.right = m_rcClient.left + nNameLength + 2 * s_nLeftBoundary;
		}
		else
		{
			int nNameCenterX = (m_rcClient.right - m_rcClient.left - nNameLength) / 2;
			pNameItem->nPosX = nNameCenterX;
		}
	}
}