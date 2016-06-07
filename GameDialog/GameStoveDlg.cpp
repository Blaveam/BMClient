#include "GameStoveDlg.h"
#include "../GameScene/GameResourceManager.h"
#include "GameDlgBase.h"
#include "../../CommonModule/StoveManager.h"
#include "GameDisplayDlg.h"
#include "GameInfoBoardDlg.h"
#include "../BackMir/BackMir.h"
#include "../../CommonModule/PotentialAttribHelper.h"
#include "../Common/gfx_utils.h"
//////////////////////////////////////////////////////////////////////////
static const int s_nDlgWidth = 358;
static const int s_nDlgHeight = 420;
//////////////////////////////////////////////////////////////////////////
#define BID_LIFESKILL	1
#define BID_SMELT		2
#define BID_MAKEEQUIP	3
#define BID_OPENPOTENTIAL 4

#define BID_OK		10
#define BID_MAKEEQUIP_DO	11
#define BTN_COLOR		ARGB(255, 181, 206, 186)

const int s_nButtonID[] =
{
	BID_LIFESKILL,
	BID_SMELT,
	BID_MAKEEQUIP,
	BID_OPENPOTENTIAL
};
//////////////////////////////////////////////////////////////////////////
GameStoveDlg::GameStoveDlg()
{
	ResetData();

	SetWindowTitle("�ֹ�����");
	m_pRender = new hgeSprite(0, 0, 0, 0, 0);

	m_rcClient.right = s_nDlgWidth;
	m_rcClient.bottom = s_nDlgHeight;
	SetBkStype(1);
	CalcCloseButtonPosition();
	CenterWindow();
	Move(0, -30);
	SetVisible(false);

	AddCommonButton(CBTN_OPTION, BID_LIFESKILL, 28, 53, "�ֹ�����", 27, BTN_COLOR);
	AddCommonButton(CBTN_OPTION, BID_SMELT, 28 + 71 * 1, 53, "��������", 27, BTN_COLOR);
	AddCommonButton(CBTN_OPTION, BID_MAKEEQUIP, 28 + 71 * 2, 53, "װ������", 27, BTN_COLOR);
	AddCommonButton(CBTN_OPTION, BID_OPENPOTENTIAL, 28 + 71 * 3, 53, "����Ǳ��", 27, BTN_COLOR);

	AddCommonButton(CBTN_NORMAL, BID_OK, 134, 370, "����");

	GetCommonButtonData(BID_LIFESKILL)->bState = BTN_STATE_CHECKED;
	SwitchTab(kStoveDlgPage_LifeSkill);

	const MakeEquipInfoMap& refMap = StoveManager::GetInstance()->GetMakeEquipInfoMap();
	MakeEquipInfoMap::const_iterator begIter = refMap.begin();
	MakeEquipInfoMap::const_iterator endIter = refMap.end();

	if(!refMap.empty())
	{
		int* pItems = new int[refMap.size()];
		int nIndex = 0;

		for(begIter;
			begIter != endIter;
			++begIter)
		{
			const MakeEquipInfo& refInfo = begIter->second;
			pItems[nIndex++] = refInfo.nItemId;
		}
		SetMakeCategory(pItems, nIndex);
		SAFE_DELETE_ARRAY(pItems);

		OnMakeListSelectChanged();
	}
}

void GameStoveDlg::SetMakeCategory(const int* _pArray, size_t _uSize)
{
	m_xMakeItemVector.resize(_uSize);
	int nValidCount = 0;

	for(size_t i = 0; i < _uSize; ++i)
	{
		ItemAttrib item;
		if(GameInfoManager::GetInstance()->GetItemAttrib(_pArray[i], &item))
		{
			m_xMakeItemVector[nValidCount].nItemId = _pArray[i];
			m_xMakeItemVector[nValidCount].xName = item.name;
			++nValidCount;
		}
		else
		{
			AfxGetHge()->System_Log("��Ч�Ķ���Ŀ¼��ƷID[^d]", _pArray[i]);
		}
	}
}

void GameStoveDlg::ResetData()
{
	m_nMakeTotalCount = 0;
	m_nMakeShowIndex = 0;
	m_nMakeCursorIndex = 0;

	ZeroMemory(m_aySmeltItems, sizeof(m_aySmeltItems));
	ZeroMemory(&m_stSmeltedItem, sizeof(m_stSmeltedItem));
	m_nSmeltPrice = 0;

	ZeroMemory(m_ayMakeItems, sizeof(m_ayMakeItems));
	ZeroMemory(&m_stMadeItem, sizeof(m_stMadeItem));
	ZeroMemory(m_nMakeNeedCount, sizeof(m_nMakeNeedCount));
	m_nMakePrice = 0;
	m_nMakeNeedLevel = 0;
	m_nMakeExp = 0;

	ZeroMemory(&m_stOpenStone, sizeof(m_stOpenStone));
	ZeroMemory(&m_stSourceItem, sizeof(m_stSourceItem));
	m_nPotentialPrice = 0;

	OnMakeListSelectChanged();
}

void GameStoveDlg::Render()
{
	__super::Render();

	HTEXTURE tex = 0;
	GameTextureManager* pPngMgr = GameResourceManager::GetInstance()->GetTexs(RES_CUSTOM);

	//	ѡ�е��Ǹ� ������
	CommonButton* pCheckedButton = NULL;
	
	for(int i = 0; i < sizeof(s_nButtonID) / sizeof(s_nButtonID[0]); ++i)
	{
		pCheckedButton = GetCommonButtonData(s_nButtonID[i]);
		if(pCheckedButton->bState != BTN_STATE_CHECKED)
		{
			pCheckedButton = NULL;
		}
		else
		{
			break;
		}
	}

	if(NULL != pCheckedButton)
	{
		tex = pPngMgr->GetTexture(28);
		if(tex)
		{
			m_pRender->SetTexture(tex);
			m_pRender->SetTextureRect(0, 0, 70, 29);
			m_pRender->Render(RELATIVE_X(pCheckedButton->stPos.left), RELATIVE_Y(pCheckedButton->stPos.top - 6));

			int nWidth = GetTextWidth(pCheckedButton->szText, 12);
			int nPrintX = RELATIVE_X(pCheckedButton->stPos.left + (RECT_WIDTH(pCheckedButton->stPos) - nWidth) / 2);
			int nPrintY = RELATIVE_Y(pCheckedButton->stPos.top + (RECT_HEIGHT(pCheckedButton->stPos) - 12) / 2);
			AfxGetPrinter()->SetColor(BTN_COLOR);
			AfxGetPrinter()->Print(nPrintX, nPrintY,
				pCheckedButton->szText);
		}
	}

	if(m_ePage == kStoveDlgPage_LifeSkill)
	{
		RenderTabLifeSkill();
	}
	else if(m_ePage == kStoveDlgPage_Smelt)
	{
		RenderTabSmelt();
	}
	else if(m_ePage == kStoveDlgPage_MakeEquip)
	{
		RenderTabMakeEquip();
	}
	else if(m_ePage == kStoveDlgPage_OpenPotential)
	{
		RenderTabOpenPotential();
	}
}

bool GameStoveDlg::ProcUserCmd(const POINT& _mp)
{
	bool bRet = __super::ProcUserCmd(_mp);

	if(m_ePage == kStoveDlgPage_LifeSkill)
	{
		ProcTabLifeSkill(_mp);
	}
	else if(m_ePage == kStoveDlgPage_Smelt)
	{
		ProcTabSmelt(_mp);
	}
	else if(m_ePage == kStoveDlgPage_MakeEquip)
	{
		ProcTabMakeEquip(_mp);
	}
	else if(m_ePage == kStoveDlgPage_OpenPotential)
	{
		ProcTabOpenPotential(_mp);
	}

	return bRet;
}

bool GameStoveDlg::OnCommonButtonClick(int _id)
{
	if(_id == BID_LIFESKILL)
	{
		SwitchTab(kStoveDlgPage_LifeSkill);
	}
	else if(_id == BID_SMELT)
	{
		SwitchTab(kStoveDlgPage_Smelt);
	}
	else if(_id == BID_MAKEEQUIP)
	{
		SwitchTab(kStoveDlgPage_MakeEquip);
	}
	else if(_id == BID_OPENPOTENTIAL)
	{
		SwitchTab(kStoveDlgPage_OpenPotential);
	}
	else if(_id == BID_OK)
	{
		OnButtonOK();
	}

	return true;
}

void GameStoveDlg::OnButtonOK()
{
	GamePlayer* pSelf = GamePlayer::GetInstance();

	if(m_ePage == kStoveDlgPage_Smelt)
	{
		//	�����ʯ
		int nCurrentCount = 0;
		ItemAttrib stMaterial = {0};
		for(int i = 0; i < sizeof(m_aySmeltItems) / sizeof(m_aySmeltItems[0]); ++i)
		{
			if(m_aySmeltItems[i].type == ITEM_OTHER)
			{
				++nCurrentCount;
				
				if(ITEM_NO == stMaterial.type)
				{
					stMaterial = m_aySmeltItems[i];
				}
			}
		}

		if(0 == nCurrentCount)
		{
			GameInfoBoardDlg::GetInstance()->InsertBoardMsg(ARGB_RED, "��������");
			return;
		}

		//	�жϵȼ�
		bool bLevelOk = true;
		int nCurrentLevel = 0;
		if(stMaterial.curse == 1)
		{
			nCurrentLevel = pSelf->GetExtendAttrib()->uSmeltOreLevel;
		}
		else if(stMaterial.curse == 3)
		{
			nCurrentLevel = pSelf->GetExtendAttrib()->uSmeltWoodLevel;
		}
		else if(stMaterial.curse == 5)
		{
			nCurrentLevel = pSelf->GetExtendAttrib()->uSmeltClothLevel;
		}
		else if(stMaterial.curse == 7)
		{
			nCurrentLevel = pSelf->GetExtendAttrib()->uSmeltGemLevel;
		}
		else
		{
			GameInfoBoardDlg::GetInstance()->InsertBoardMsg(ARGB_RED, "�Ƿ��Ĳ���");
		}

		if(0 == nCurrentLevel ||
			nCurrentLevel < stMaterial.hide)
		{
			bLevelOk = false;
		}

		if(!bLevelOk)
		{
			GameInfoBoardDlg::GetInstance()->InsertBoardMsg(ARGB_RED, "���ļ��ܲ��㣬�޷��ӹ��ò���");
			return;
		}

		//	���
		if(m_nSmeltPrice > pSelf->GetPlayerBag()->GetMoney())
		{
			GameInfoBoardDlg::GetInstance()->InsertBoardMsg(ARGB_RED, "���Ľ�Ҳ��㣬�޷��ӹ��ò���");
			return;
		}

		PkgPlayerSmeltMaterialsReq req;
		req.uUserId = pSelf->GetHandlerID();
		req.xItemsTag.resize(nCurrentCount, 0);
		int nIndex = 0;

		for(int i = 0; i < sizeof(m_aySmeltItems) / sizeof(m_aySmeltItems[0]); ++i)
		{
			if(m_aySmeltItems[i].type == ITEM_OTHER)
			{
				req.xItemsTag[nIndex++] = m_aySmeltItems[i].tag;
			}
		}

		g_xBuffer.Reset();
		g_xBuffer << req;
		SendBufferToGS(g_xBuffer);
	}
	else if(m_ePage == kStoveDlgPage_MakeEquip)
	{
		if(ITEM_NO == m_stMadeItem.type)
		{
			GameInfoBoardDlg::GetInstance()->InsertBoardMsg(ARGB_RED, "��ѡ��Ҫ������װ��");
			return;
		}

		//	�鿴�Ƿ�������
		const MakeEquipInfo* pInfo = StoveManager::GetInstance()->GetMakeEquipInfo(m_stMadeItem.id);
		if(NULL == pInfo)
		{
			GameInfoBoardDlg::GetInstance()->InsertBoardMsg(ARGB_RED, "�޷���ȡ������Ϣ");
			return;
		}

		//	��鼼�ܵȼ��Ƿ񳬹���
		if(pInfo->nNeedLevel > pSelf->GetExtendAttrib()->uMakeEquipLevel ||
			0 == pSelf->GetExtendAttrib()->uMakeEquipLevel)
		{
			GameInfoBoardDlg::GetInstance()->InsertBoardMsg(ARGB_RED, "���ļ��ܵȼ�����");
			return;
		}

		//	��鱳���Ƿ��иò���
		for(int i = 0; i < pInfo->nMaterialCount; ++i)
		{
			int nNeedCount = pInfo->nMaterialsCount[i];
			int nItemId = pInfo->nMaterialsId[i];
			int nCount = pSelf->GetPlayerBag()->CountItem(nItemId);

			if(nCount < nNeedCount)
			{
				GameInfoBoardDlg::GetInstance()->InsertBoardMsg(ARGB_RED, "���Ĳ��ϲ���");
				return;
			}
		}

		PkgPlayerHandMakeItemReq req;
		req.uUserId = pSelf->GetHandlerID();
		req.nItemId = pInfo->nItemId;
		g_xBuffer.Reset();
		g_xBuffer << req;
		SendBufferToGS(g_xBuffer);
	}
	else if(m_ePage == kStoveDlgPage_OpenPotential)
	{
		if(m_stOpenStone.type == ITEM_NO ||
			m_stSourceItem.type == ITEM_NO)
		{
			GameInfoBoardDlg::GetInstance()->InsertBoardMsg(ARGB_RED, "����߸��ӷ���Ǳ��ʯ���ұ߷����ֹ�װ��");
			return;
		}

		PkgPlayerOpenPotentialReq req;
		req.uUserId = pSelf->GetHandlerID();
		req.nItemId = m_stSourceItem.tag;
		req.nStoneId = m_stOpenStone.tag;
		g_xBuffer.Reset();
		g_xBuffer << req;
		SendBufferToGS(g_xBuffer);
	}
}

bool GameStoveDlg::OnShowDialog()
{
	SetNeedTopLevel(true);
	return __super::OnShowDialog();
}

void GameStoveDlg::SwitchTab(StoveDlgPageType _eType)
{
	m_ePage = _eType;
	ResetData();

	GetCommonButtonData(BID_OK)->bVisible = false;

	if(_eType == kStoveDlgPage_Smelt ||
		_eType == kStoveDlgPage_MakeEquip ||
		_eType == kStoveDlgPage_OpenPotential)
	{
		GetCommonButtonData(BID_OK)->bVisible = true;
	}

	if(_eType == kStoveDlgPage_MakeEquip)
	{
		SetWindowTip("[��ʾ]�����װ��ѡ�����й�������м����Կ��ٸı���ѡװ��\n����װ��ǰ���Ƚ�������Ϸ���ֿ�");
	}
	else
	{
		SetWindowTip("");
	}
}

#define OFFSET_X_LIFESKILL_BEGIN	35
#define OFFSET_Y_LIFESKILL_BEGIN	110
#define OFFSET_Y_LIFESKILL_GAP		40
#define OFFSET_X_LIFESKILL_EXP		(9*12)

#define OFFSET_X_LIFESKILL_EXPLINE	22
#define OFFSET_Y_LIFESKILL_EXPLINE	3

#define LENGTH_X_LIFESKILL_EXPLINE	138
#define LENGTH_X_LIFESKILL_EXPLINEFULL 181

void GameStoveDlg::RenderTabLifeSkill()
{
	HTEXTURE tex = 0;
	GamePlayer* pPlayer = GamePlayer::GetInstance();
	ExtendHeroAttrib* pAttrib = pPlayer->GetExtendAttrib();

	GameTextureManager* pPngMgr = GameResourceManager::GetInstance()->GetTexs(RES_CUSTOM);

	char szTextBuf[25] = {0};
	int nItemCount = 0;

	//	��ʯұ��
	int nLv = pAttrib->uSmeltOreLevel;
	int nExp = pAttrib->uSmeltOreExp;
	int nNeedExp = 0;
	sprintf(szTextBuf, "Lv%d", nLv);
	
	AfxGetPrinter()->SetColor(ARGB_WHITE);
	AfxGetPrinter()->PrintWithoutStroke(RELATIVE_X(OFFSET_X_LIFESKILL_BEGIN), RELATIVE_Y(OFFSET_Y_LIFESKILL_BEGIN + OFFSET_Y_LIFESKILL_GAP * nItemCount), "��ʯұ����%s", szTextBuf);
	tex = pPngMgr->GetTexture(30);
	if(tex)
	{
		m_pRender->SetTexture(tex);
		m_pRender->SetTextureRect(0, 0, LENGTH_X_LIFESKILL_EXPLINEFULL, 15);
		m_pRender->Render(RELATIVE_X(OFFSET_X_LIFESKILL_BEGIN + OFFSET_X_LIFESKILL_EXP), RELATIVE_Y(OFFSET_Y_LIFESKILL_BEGIN + OFFSET_Y_LIFESKILL_GAP * nItemCount));
	}
	if(0 != nLv)
	{
		nNeedExp = StoveManager::GetInstance()->GetLifeSkillInfo(kLifeSkill_SmeltOre)->uExp[nLv - 1];
		tex = pPngMgr->GetTexture(31);
		if(tex)
		{
			m_pRender->SetTexture(tex);
			float fPercent = (float)nExp / (float)nNeedExp;
			m_pRender->SetTextureRect(0, 0, LENGTH_X_LIFESKILL_EXPLINE * fPercent, 9);
			m_pRender->Render(RELATIVE_X(OFFSET_X_LIFESKILL_BEGIN + OFFSET_X_LIFESKILL_EXP + OFFSET_X_LIFESKILL_EXPLINE),
				RELATIVE_Y(OFFSET_Y_LIFESKILL_BEGIN + OFFSET_Y_LIFESKILL_GAP * nItemCount + OFFSET_Y_LIFESKILL_EXPLINE));
		}
	}

	//	ľ��
	++nItemCount;
	nLv = pAttrib->uSmeltWoodLevel;
	nExp = pAttrib->uSmeltWoodExp;
	sprintf(szTextBuf, "Lv%d", nLv);
	AfxGetPrinter()->SetColor(ARGB_WHITE);
	AfxGetPrinter()->PrintWithoutStroke(RELATIVE_X(OFFSET_X_LIFESKILL_BEGIN), RELATIVE_Y(OFFSET_Y_LIFESKILL_BEGIN + OFFSET_Y_LIFESKILL_GAP * nItemCount), "ľ�ļӹ���%s", szTextBuf);
	tex = pPngMgr->GetTexture(30);
	if(tex)
	{
		m_pRender->SetTexture(tex);
		m_pRender->SetTextureRect(0, 0, LENGTH_X_LIFESKILL_EXPLINEFULL, 15);
		m_pRender->Render(RELATIVE_X(OFFSET_X_LIFESKILL_BEGIN + OFFSET_X_LIFESKILL_EXP), RELATIVE_Y(OFFSET_Y_LIFESKILL_BEGIN + OFFSET_Y_LIFESKILL_GAP * nItemCount));
	}
	if(0 != nLv)
	{
		nNeedExp = StoveManager::GetInstance()->GetLifeSkillInfo(kLifeSkill_SmeltOre)->uExp[nLv - 1];
		tex = pPngMgr->GetTexture(31);
		if(tex)
		{
			m_pRender->SetTexture(tex);
			float fPercent = (float)nExp / (float)nNeedExp;
			m_pRender->SetTextureRect(0, 0, LENGTH_X_LIFESKILL_EXPLINE * fPercent, 9);
			m_pRender->Render(RELATIVE_X(OFFSET_X_LIFESKILL_BEGIN + OFFSET_X_LIFESKILL_EXP + OFFSET_X_LIFESKILL_EXPLINE),
				RELATIVE_Y(OFFSET_Y_LIFESKILL_BEGIN + OFFSET_Y_LIFESKILL_GAP * nItemCount + OFFSET_Y_LIFESKILL_EXPLINE));
		}
	}

	//	����
	++nItemCount;
	nLv = pAttrib->uSmeltClothLevel;
	nExp = pAttrib->uSmeltClothExp;
	sprintf(szTextBuf, "Lv%d", nLv);
	AfxGetPrinter()->SetColor(ARGB_WHITE);
	AfxGetPrinter()->PrintWithoutStroke(RELATIVE_X(OFFSET_X_LIFESKILL_BEGIN), RELATIVE_Y(OFFSET_Y_LIFESKILL_BEGIN + OFFSET_Y_LIFESKILL_GAP * nItemCount), "���ϼӹ���%s", szTextBuf);
	tex = pPngMgr->GetTexture(30);
	if(tex)
	{
		m_pRender->SetTexture(tex);
		m_pRender->SetTextureRect(0, 0, LENGTH_X_LIFESKILL_EXPLINEFULL, 15);
		m_pRender->Render(RELATIVE_X(OFFSET_X_LIFESKILL_BEGIN + OFFSET_X_LIFESKILL_EXP), RELATIVE_Y(OFFSET_Y_LIFESKILL_BEGIN + OFFSET_Y_LIFESKILL_GAP * nItemCount));
	}
	if(0 != nLv)
	{
		nNeedExp = StoveManager::GetInstance()->GetLifeSkillInfo(kLifeSkill_SmeltOre)->uExp[nLv - 1];
		tex = pPngMgr->GetTexture(31);
		if(tex)
		{
			m_pRender->SetTexture(tex);
			float fPercent = (float)nExp / (float)nNeedExp;
			m_pRender->SetTextureRect(0, 0, LENGTH_X_LIFESKILL_EXPLINE * fPercent, 9);
			m_pRender->Render(RELATIVE_X(OFFSET_X_LIFESKILL_BEGIN + OFFSET_X_LIFESKILL_EXP + OFFSET_X_LIFESKILL_EXPLINE),
				RELATIVE_Y(OFFSET_Y_LIFESKILL_BEGIN + OFFSET_Y_LIFESKILL_GAP * nItemCount + OFFSET_Y_LIFESKILL_EXPLINE));
		}
	}

	//	��ʯ
	++nItemCount;
	nLv = pAttrib->uSmeltGemLevel;
	nExp = pAttrib->uSmeltGemExp;
	sprintf(szTextBuf, "Lv%d", nLv);
	AfxGetPrinter()->SetColor(ARGB_WHITE);
	AfxGetPrinter()->PrintWithoutStroke(RELATIVE_X(OFFSET_X_LIFESKILL_BEGIN), RELATIVE_Y(OFFSET_Y_LIFESKILL_BEGIN + OFFSET_Y_LIFESKILL_GAP * nItemCount), "��ʯ�ӹ���%s", szTextBuf);
	tex = pPngMgr->GetTexture(30);
	if(tex)
	{
		m_pRender->SetTexture(tex);
		m_pRender->SetTextureRect(0, 0, LENGTH_X_LIFESKILL_EXPLINEFULL, 15);
		m_pRender->Render(RELATIVE_X(OFFSET_X_LIFESKILL_BEGIN + OFFSET_X_LIFESKILL_EXP), RELATIVE_Y(OFFSET_Y_LIFESKILL_BEGIN + OFFSET_Y_LIFESKILL_GAP * nItemCount));
	}
	if(0 != nLv)
	{
		nNeedExp = StoveManager::GetInstance()->GetLifeSkillInfo(kLifeSkill_SmeltOre)->uExp[nLv - 1];
		tex = pPngMgr->GetTexture(31);
		if(tex)
		{
			m_pRender->SetTexture(tex);
			float fPercent = (float)nExp / (float)nNeedExp;
			m_pRender->SetTextureRect(0, 0, LENGTH_X_LIFESKILL_EXPLINE * fPercent, 9);
			m_pRender->Render(RELATIVE_X(OFFSET_X_LIFESKILL_BEGIN + OFFSET_X_LIFESKILL_EXP + OFFSET_X_LIFESKILL_EXPLINE),
				RELATIVE_Y(OFFSET_Y_LIFESKILL_BEGIN + OFFSET_Y_LIFESKILL_GAP * nItemCount + OFFSET_Y_LIFESKILL_EXPLINE));
		}
	}

	//	װ��
	++nItemCount;
	nLv = pAttrib->uMakeEquipLevel;
	nExp = pAttrib->uMakeEquipExp;
	sprintf(szTextBuf, "Lv%d", nLv);
	AfxGetPrinter()->SetColor(ARGB_WHITE);
	AfxGetPrinter()->PrintWithoutStroke(RELATIVE_X(OFFSET_X_LIFESKILL_BEGIN), RELATIVE_Y(OFFSET_Y_LIFESKILL_BEGIN + OFFSET_Y_LIFESKILL_GAP * nItemCount), "װ�����죺%s", szTextBuf);
	tex = pPngMgr->GetTexture(30);
	if(tex)
	{
		m_pRender->SetTexture(tex);
		m_pRender->SetTextureRect(0, 0, LENGTH_X_LIFESKILL_EXPLINEFULL, 15);
		m_pRender->Render(RELATIVE_X(OFFSET_X_LIFESKILL_BEGIN + OFFSET_X_LIFESKILL_EXP), RELATIVE_Y(OFFSET_Y_LIFESKILL_BEGIN + OFFSET_Y_LIFESKILL_GAP * nItemCount));
	}
	if(0 != nLv)
	{
		nNeedExp = StoveManager::GetInstance()->GetLifeSkillInfo(kLifeSkill_SmeltEquip)->uExp[nLv - 1];
		tex = pPngMgr->GetTexture(31);
		if(tex)
		{
			m_pRender->SetTexture(tex);
			float fPercent = (float)nExp / (float)nNeedExp;
			m_pRender->SetTextureRect(0, 0, LENGTH_X_LIFESKILL_EXPLINE * fPercent, 9);
			m_pRender->Render(RELATIVE_X(OFFSET_X_LIFESKILL_BEGIN + OFFSET_X_LIFESKILL_EXP + OFFSET_X_LIFESKILL_EXPLINE),
				RELATIVE_Y(OFFSET_Y_LIFESKILL_BEGIN + OFFSET_Y_LIFESKILL_GAP * nItemCount + OFFSET_Y_LIFESKILL_EXPLINE));
		}
	}
}

bool GameStoveDlg::ProcTabLifeSkill(const POINT &_mp)
{
	return true;
}

#define OFFSET_X_SMELT_BEGIN	15
#define OFFSET_Y_SMELT_BEGIN	85

#define OFFSET_X_SMELT_ITEM		71
#define OFFSET_Y_SMELT_ITEM		163
#define OFFSET_X_SMELT_ITEM_GAP	39

#define OFFSET_X_SMELT_DONE		148
#define OFFSET_Y_SMELT_DONE		31

#define OFFSET_X_SMELT_PRICE	163
#define OFFSET_Y_SMELT_PRICE	228

void GameStoveDlg::RenderTabSmelt()
{
	HTEXTURE tex = 0;
	GamePlayer* pPlayer = GamePlayer::GetInstance();
	ExtendHeroAttrib* pAttrib = pPlayer->GetExtendAttrib();

	GameTextureManager* pPngMgr = GameResourceManager::GetInstance()->GetTexs(RES_CUSTOM);
	GameTextureManager* pItemsMgr = GameResourceManager::GetInstance()->GetTexs(RES_ITEMS);

	tex = pPngMgr->GetTexture(32);
	if(tex)
	{
		m_pRender->SetTexture(tex);
		m_pRender->SetTextureRect(0, 0, 326, 272);
		m_pRender->Render(RELATIVE_X(OFFSET_X_SMELT_BEGIN), RELATIVE_Y(OFFSET_Y_SMELT_BEGIN));
	}

	//	render items
	int nTextWidth = 0;
	int nTextHeight = 0;
	for(int i = 0; i < 5; ++i)
	{
		const ItemAttrib& refItem = m_aySmeltItems[i];

		if(refItem.type != ITEM_NO)
		{
			tex = pItemsMgr->GetTexture(refItem.tex);
			if(NULL == tex)
			{
				continue;
			}
			nTextWidth = pItemsMgr->GetTextureWidth(refItem.tex);
			nTextHeight = pItemsMgr->GetTextureHeight(refItem.tex);

			m_pRender->SetTexture(tex);
			m_pRender->SetTextureRect(0, 0, nTextWidth, nTextHeight);
			m_pRender->Render(RELATIVE_X(OFFSET_X_SMELT_BEGIN + OFFSET_X_SMELT_ITEM + (36 - nTextWidth) / 2 + i * OFFSET_X_SMELT_ITEM_GAP),
				RELATIVE_Y(OFFSET_Y_SMELT_BEGIN + OFFSET_Y_SMELT_ITEM + (36 - nTextHeight) / 2));
		}
	}

	//	render smelted item
	if(m_stSmeltedItem.type != ITEM_NO)
	{
		tex = pItemsMgr->GetTexture(m_stSmeltedItem.tex);
		if(tex)
		{
			nTextWidth = pItemsMgr->GetTextureWidth(m_stSmeltedItem.tex);
			nTextHeight = pItemsMgr->GetTextureHeight(m_stSmeltedItem.tex);

			m_pRender->SetTexture(tex);
			m_pRender->SetTextureRect(0, 0, nTextWidth, nTextHeight);
			m_pRender->Render(RELATIVE_X(OFFSET_X_SMELT_BEGIN + OFFSET_X_SMELT_DONE + (36 - nTextWidth) / 2),
				RELATIVE_Y(OFFSET_Y_SMELT_BEGIN + OFFSET_Y_SMELT_DONE + (36 - nTextHeight) / 2));
		}
	}

	//	render smelt price
	AfxGetPrinter()->SetColor(ARGB_WHITE);
	AfxGetPrinter()->PrintWithoutStroke(RELATIVE_X(OFFSET_X_SMELT_BEGIN + OFFSET_X_SMELT_PRICE),
		RELATIVE_Y(OFFSET_Y_SMELT_BEGIN + OFFSET_Y_SMELT_PRICE),
		"%d",
		m_nSmeltPrice);
}

bool GameStoveDlg::ProcTabSmelt(const POINT &_mp)
{
	RECT rcTestRect = {0};

	for(int i = 0; i < 5; ++i)
	{
		ItemAttrib& refItem = m_aySmeltItems[i];

		//if(refItem.type == ITEM_NO)
		{
			rcTestRect.left = RELATIVE_X(OFFSET_X_SMELT_BEGIN + OFFSET_X_SMELT_ITEM + i * OFFSET_X_SMELT_ITEM_GAP);
			rcTestRect.right = rcTestRect.left + 36;
			rcTestRect.top = RELATIVE_Y(OFFSET_Y_SMELT_BEGIN + OFFSET_Y_SMELT_ITEM);
			rcTestRect.bottom = rcTestRect.top + 36;
			
			if(PtInRect(&rcTestRect, _mp))
			{
				if(refItem.type != ITEM_NO)
				{
					GameMainOptUI::GetInstance()->GetDisplayDlg()->ShowItemAttrib(&refItem);
				}

				//	put down
				if(AfxGetHge()->Input_KeyDown(HGEK_LBUTTON))
				{
					GameBagDlg2* pBagDlg = GameMainOptUI::GetInstance()->GetBagDlg2();
					ItemAttrib* pSelItem = pBagDlg->GetSelItem();

					if(NULL != pSelItem)
					{
						if(pSelItem->type == ITEM_OTHER &&
							pSelItem->curse >= 1 &&
							pSelItem->curse <= 8 &&
							pSelItem->curse % 2 == 1)
						{
							//	ͬһ��tag����Ʒ�����ظ�����
							bool bSameTag = false;
							for(int j = 0; j < 5; ++j)
							{
								if(m_aySmeltItems[j].type != ITEM_NO)
								{
									if(m_aySmeltItems[j].tag == pSelItem->tag)
									{
										bSameTag = true;
										break;
									}
								}
							}

							if(!bSameTag)
							{
								//	�����Ƿ��Ѿ������˲����� ��ô�������ͬ��ID
								int nPrevItemId = 0;
								for(int i = 0; i < sizeof(m_aySmeltItems) / sizeof(m_aySmeltItems[0]); ++i)
								{
									if(m_aySmeltItems[i].id != 0)
									{
										nPrevItemId = m_aySmeltItems[i].id;
										break;
									}
								}

								if(0 != nPrevItemId &&
									nPrevItemId != pSelItem->id)
								{
									GameInfoBoardDlg::GetInstance()->InsertBoardMsg(ARGB_RED, "�������ͬ�Ĳ���");
								}
								else
								{
									memcpy(&refItem, pSelItem, sizeof(ItemAttrib));

									//	���¼���۸�
									m_nSmeltPrice = 0;
									for(int j = 0; j < 5; ++j)
									{
										if(m_aySmeltItems[j].type != ITEM_NO)
										{
											m_nSmeltPrice += m_aySmeltItems[j].hide * 3000;
										}
									}
								}
							}
						}
						else
						{
							GameInfoBoardDlg::GetInstance()->InsertBoardMsg(ARGB_RED, "��������");
						}
						
						pBagDlg->CancelSel();
					}
				}
			}
		}
	}

	//	smelted item
	if(m_stSmeltedItem.type != ITEM_NO)
	{
		rcTestRect.left = RELATIVE_X(OFFSET_X_SMELT_BEGIN + OFFSET_X_SMELT_DONE);
		rcTestRect.right = rcTestRect.left + 36;
		rcTestRect.top = RELATIVE_Y(OFFSET_Y_SMELT_BEGIN + OFFSET_Y_SMELT_DONE);
		rcTestRect.bottom = rcTestRect.top + 36;

		if(PtInRect(&rcTestRect, _mp))
		{
			GameMainOptUI::GetInstance()->GetDisplayDlg()->ShowItemAttrib(&m_stSmeltedItem);
		}
	}

	return true;
}


#define OFFSET_X_MAKE_BEGIN		15
#define OFFSET_Y_MAKE_BEGIN	85

#define OFFSET_X_MAKE_ITEM		142
#define OFFSET_Y_MAKE_ITEM		33
#define OFFSET_Y_MAKE_ITEM_GAP	39

#define OFFSET_X_MAKE_DONE		249
#define OFFSET_Y_MAKE_DONE		110

#define OFFSET_X_MAKE_PRICE		200
#define OFFSET_Y_MAKE_PRICE		239

#define OFFSET_X_MAKE_LIST		(OFFSET_X_MAKE_BEGIN + 120)
#define OFFSET_Y_MAKE_LIST		(OFFSET_Y_MAKE_BEGIN - 5)
#define HEIGHT_MAKE_LIST		272

#define OFFSET_X_MAKE_LIST_ITEM	(OFFSET_X_MAKE_BEGIN + 5)
#define OFFSET_Y_MAKE_LIST_ITEM (OFFSET_Y_MAKE_BEGIN + 5)
#define MAX_LIST_ITEM			18

void GameStoveDlg::RenderTabMakeEquip()
{
	HTEXTURE tex = 0;
	GamePlayer* pPlayer = GamePlayer::GetInstance();
	ExtendHeroAttrib* pAttrib = pPlayer->GetExtendAttrib();

	GameTextureManager* pPngMgr = GameResourceManager::GetInstance()->GetTexs(RES_CUSTOM);
	GameTextureManager* pItemsMgr = GameResourceManager::GetInstance()->GetTexs(RES_ITEMS);

	tex = pPngMgr->GetTexture(33);
	if(tex)
	{
		m_pRender->SetTexture(tex);
		m_pRender->SetTextureRect(0, 0, 326, 272);
		m_pRender->Render(RELATIVE_X(OFFSET_X_MAKE_BEGIN), RELATIVE_Y(OFFSET_Y_MAKE_BEGIN));
	}

	//	render items
	int nTextWidth = 0;
	int nTextHeight = 0;
	for(int i = 0; i < 5; ++i)
	{
		const ItemAttrib& refItem = m_ayMakeItems[i];

		if(refItem.type != ITEM_NO)
		{
			tex = pItemsMgr->GetTexture(refItem.tex);
			if(NULL == tex)
			{
				continue;
			}
			nTextWidth = pItemsMgr->GetTextureWidth(refItem.tex);
			nTextHeight = pItemsMgr->GetTextureHeight(refItem.tex);

			m_pRender->SetTexture(tex);
			m_pRender->SetTextureRect(0, 0, nTextWidth, nTextHeight);
			m_pRender->Render(RELATIVE_X(OFFSET_X_MAKE_BEGIN + OFFSET_X_MAKE_ITEM + (36 - nTextWidth) / 2),
				RELATIVE_Y(OFFSET_Y_MAKE_BEGIN + OFFSET_Y_MAKE_ITEM + (36 - nTextHeight) / 2 + i * OFFSET_Y_MAKE_ITEM_GAP));

			AfxGetPrinter()->SetColor(ARGB_WHITE);
			AfxGetPrinter()->Print(RELATIVE_X(OFFSET_X_MAKE_BEGIN + OFFSET_X_MAKE_ITEM + (36 - nTextWidth) / 2 + 5),
				RELATIVE_Y(OFFSET_Y_MAKE_BEGIN + OFFSET_Y_MAKE_ITEM + (36 - nTextHeight) / 2 + i * OFFSET_Y_MAKE_ITEM_GAP + 18),
				"%d",
				m_nMakeNeedCount[i]);
		}
	}

	//	render smelted item
	if(m_stMadeItem.type != ITEM_NO)
	{
		tex = pItemsMgr->GetTexture(m_stMadeItem.tex);
		if(tex)
		{
			nTextWidth = pItemsMgr->GetTextureWidth(m_stMadeItem.tex);
			nTextHeight = pItemsMgr->GetTextureHeight(m_stMadeItem.tex);

			m_pRender->SetTexture(tex);
			m_pRender->SetTextureRect(0, 0, nTextWidth, nTextHeight);
			m_pRender->Render(RELATIVE_X(OFFSET_X_MAKE_BEGIN + OFFSET_X_MAKE_DONE + (36 - nTextWidth) / 2),
				RELATIVE_Y(OFFSET_Y_MAKE_BEGIN + OFFSET_Y_MAKE_DONE + (36 - nTextHeight) / 2));
		}
	}

	//	render smelt price
	AfxGetPrinter()->SetColor(ARGB_WHITE);
	/*AfxGetPrinter()->PrintWithoutStroke(RELATIVE_X(OFFSET_X_MAKE_BEGIN + OFFSET_X_MAKE_PRICE),
		RELATIVE_Y(OFFSET_Y_MAKE_BEGIN + OFFSET_Y_MAKE_PRICE),
		"%d",
		m_nMakePrice);*/
	Gfx_PrintNumberWithComma(AfxGetPrinter(), RELATIVE_X(OFFSET_X_MAKE_BEGIN + OFFSET_X_MAKE_PRICE),
		RELATIVE_Y(OFFSET_Y_MAKE_BEGIN + OFFSET_Y_MAKE_PRICE),
		m_nMakePrice);

	//	render list
	tex = pPngMgr->GetTexture(23);
	if(tex)
	{
		m_pRender->SetTexture(tex);
		m_pRender->SetTextureRect(0, 0, 8, HEIGHT_MAKE_LIST);
		m_pRender->Render(RELATIVE_X(OFFSET_X_MAKE_LIST), RELATIVE_Y(OFFSET_Y_MAKE_LIST));
	}

	//	render category item
	int nShowIndex = 0;
	for(int i = 0; i < MAX_LIST_ITEM; ++i)
	{
		nShowIndex = m_nMakeShowIndex + i;

		if(nShowIndex < 0 ||
			nShowIndex >= m_xMakeItemVector.size())
		{
			continue;
		}

		MakeListItem& refItem = m_xMakeItemVector[nShowIndex];

		if(i == m_nMakeCursorIndex)
		{
			//	������ʾ
			AfxGetPrinter()->SetColor(ARGB_WHITE);
		}
		else
		{
			AfxGetPrinter()->SetColor(ARGB(255, 100, 100, 100));
		}
		AfxGetPrinter()->PrintWithoutStroke(RELATIVE_X(OFFSET_X_MAKE_LIST_ITEM),
			RELATIVE_Y(OFFSET_Y_MAKE_LIST_ITEM + (i * 14)),
			refItem.xName.c_str());
	}

	if(ITEM_NO != m_stMadeItem.type)
	{
		const MakeEquipInfo* pItem = StoveManager::GetInstance()->GetMakeEquipInfo(m_stMadeItem.id);
		AfxGetPrinter()->SetColor(ARGB_WHITE);
		AfxGetPrinter()->PrintWithoutStroke(RELATIVE_X(OFFSET_X_MAKE_BEGIN + OFFSET_X_MAKE_PRICE),
			RELATIVE_Y(OFFSET_Y_MAKE_BEGIN + OFFSET_Y_MAKE_PRICE - 42),
			"����װ������+%d",
			m_nMakeExp);

		if(pAttrib->uMakeEquipLevel < m_nMakeNeedLevel)
		{
			AfxGetPrinter()->SetColor(ARGB_RED);
		}
		AfxGetPrinter()->PrintWithoutStroke(RELATIVE_X(OFFSET_X_MAKE_BEGIN + OFFSET_X_MAKE_PRICE),
			RELATIVE_Y(OFFSET_Y_MAKE_BEGIN + OFFSET_Y_MAKE_PRICE - 56),
			"��Ҫ�����ȼ�:%d",
			m_nMakeNeedLevel);
	}
	
	RenderWindowTip();
}

bool GameStoveDlg::ProcTabMakeEquip(const POINT &_mp)
{
	RECT rcTestRect = {0};

	for(int i = 0; i < 5; ++i)
	{
		ItemAttrib& refItem = m_ayMakeItems[i];

		if(refItem.type != ITEM_NO)
		{
			rcTestRect.left = RELATIVE_X(OFFSET_X_MAKE_BEGIN + OFFSET_X_MAKE_ITEM);
			rcTestRect.right = rcTestRect.left + 36;
			rcTestRect.top = RELATIVE_Y(OFFSET_Y_MAKE_BEGIN + OFFSET_Y_MAKE_ITEM + i * OFFSET_Y_MAKE_ITEM_GAP);
			rcTestRect.bottom = rcTestRect.top + 36;

			if(PtInRect(&rcTestRect, _mp))
			{
				GameMainOptUI::GetInstance()->GetDisplayDlg()->ShowItemAttrib(&refItem);
			}
		}
	}

	//	made items
	if(m_stMadeItem.type != ITEM_NO)
	{
		rcTestRect.left = RELATIVE_X(OFFSET_X_MAKE_BEGIN + OFFSET_X_MAKE_DONE);
		rcTestRect.right = rcTestRect.left + 36;
		rcTestRect.top = RELATIVE_Y(OFFSET_Y_MAKE_BEGIN + OFFSET_Y_MAKE_DONE);
		rcTestRect.bottom = rcTestRect.top + 36;

		if(PtInRect(&rcTestRect, _mp))
		{
			GameMainOptUI::GetInstance()->GetDisplayDlg()->ShowItemAttrib(&m_stMadeItem);
		}
	}

	//	category
	rcTestRect.left = RELATIVE_X(OFFSET_X_MAKE_LIST_ITEM);
	rcTestRect.right = rcTestRect.left + 120;
	rcTestRect.top = RELATIVE_Y(OFFSET_Y_MAKE_LIST_ITEM);
	rcTestRect.bottom = rcTestRect.top + 14 * 18;

	if(PtInRect(&rcTestRect, _mp))
	{
		if(AfxGetHge()->Input_KeyUp(HGEK_LBUTTON))
		{
			int nIndex = (_mp.y - rcTestRect.top) / 14;
			int nMaxCursorIndex = MAX_LIST_ITEM - 1;

			if(m_xMakeItemVector.size() < MAX_LIST_ITEM)
			{
				nMaxCursorIndex = m_xMakeItemVector.size() - 1;
			}

			if(nIndex >= 0 &&
				nIndex <= nMaxCursorIndex)
			{
				m_nMakeCursorIndex = nIndex;
				OnMakeListSelectChanged();
			}
		}
	}

	//	����м���ѡ��
	int nMButtonScroll = AfxGetHge()->Input_GetMouseWheel();
	if(0 != nMButtonScroll)
	{
		if(CursorScroll(nMButtonScroll))
		{
			OnMakeListSelectChanged();
		}
	}
	
	return true;
}

void GameStoveDlg::OnMakeListSelectChanged()
{
	//	�����ǰѡ�е���
	int nCurrentSelIndex = m_nMakeShowIndex + m_nMakeCursorIndex;

	if(nCurrentSelIndex < 0 ||
		nCurrentSelIndex >= m_xMakeItemVector.size())
	{
		return;
	}

	MakeListItem& refItem = m_xMakeItemVector[nCurrentSelIndex];
	GameInfoManager::GetInstance()->GetItemAttrib(refItem.nItemId, &m_stMadeItem);

	const MakeEquipInfo* pInfo = StoveManager::GetInstance()->GetMakeEquipInfo(refItem.nItemId);
	if(NULL != pInfo)
	{
		m_nMakeNeedLevel = pInfo->nNeedLevel;
		m_nMakeExp = pInfo->nExp;
		m_nMakePrice = pInfo->nNeedMoney;
		for(int i = 0; i < pInfo->nMaterialCount; ++i)
		{
			m_nMakeNeedCount[i] = pInfo->nMaterialsCount[i];
			GameInfoManager::GetInstance()->GetItemAttrib(pInfo->nMaterialsId[i], &m_ayMakeItems[i]);
		}

		for(int i = pInfo->nMaterialCount; i < 5; ++i)
		{
			ZeroMemory(&m_ayMakeItems[i], sizeof(ItemAttrib));
		}
	}
}

bool GameStoveDlg::CursorScrollUp()
{
	if(0 == m_nMakeCursorIndex)
	{
		if(m_nMakeShowIndex > 0)
		{
			--m_nMakeShowIndex;
		}
		else
		{
			//	������
			return false;
		}
	}
	else
	{
		//	�������
		--m_nMakeCursorIndex;
	}

	return true;
}

bool GameStoveDlg::CursorScrollDown()
{
	int nCurrentSel = m_nMakeShowIndex + m_nMakeCursorIndex;
	if(nCurrentSel >= m_xMakeItemVector.size() - 1)
	{
		//	���ײ���
		return false;
	}
	else
	{
		//	��������
		++m_nMakeCursorIndex;
		if(m_nMakeCursorIndex >= MAX_LIST_ITEM)
		{
			m_nMakeCursorIndex = MAX_LIST_ITEM - 1;
			++m_nMakeShowIndex;
		}
	}

	return true;
}

bool GameStoveDlg::CursorScroll(int _nTimes)
{
	bool bScrolled = false;

	if(_nTimes > 0)
	{
		for(int i = 0; i < _nTimes; ++i)
		{
			if(CursorScrollUp())
			{
				bScrolled = true;
			}
		}
	}
	else
	{
		for(int i = 0; i < -_nTimes; ++i)
		{
			if(CursorScrollDown())
			{
				bScrolled = true;
			}
		}
	}

	return bScrolled;
}

#define OFFSET_X_POTENTIAL_BEGIN	OFFSET_X_MAKE_BEGIN
#define OFFSET_Y_POTENTIAL_BEGIN	OFFSET_Y_MAKE_BEGIN
#define OFFSET_X_POTENTIAL_STONE	(OFFSET_X_POTENTIAL_BEGIN + 85)
#define OFFSET_Y_POTENTIAL_STONE	(OFFSET_Y_POTENTIAL_BEGIN + 45)
#define OFFSET_X_POTENTIAL_SOURCE	(OFFSET_X_POTENTIAL_BEGIN + 205)
#define OFFSET_Y_POTENTIAL_SOURCE	(OFFSET_Y_POTENTIAL_BEGIN + 45)
#define OFFSET_X_POTENTIAL_PRICE	(OFFSET_X_POTENTIAL_BEGIN + 156)
#define OFFSET_Y_POTENTIAL_PRICE	(OFFSET_Y_POTENTIAL_BEGIN + 207)

bool GameStoveDlg::ProcTabOpenPotential(const POINT &_mp)
{
	GameBagDlg2* pBagDlg = GameMainOptUI::GetInstance()->GetBagDlg2();

	RECT rcTest = {0};
	rcTest.left = RELATIVE_X(OFFSET_X_POTENTIAL_SOURCE);
	rcTest.right = rcTest.left + 36;
	rcTest.top = RELATIVE_Y(OFFSET_Y_POTENTIAL_SOURCE);
	rcTest.bottom = rcTest.top + 36;

	if(PtInRect(&rcTest, _mp))
	{
		if(m_stSourceItem.type != ITEM_NO)
		{
			GameMainOptUI::GetInstance()->GetDisplayDlg()->ShowItemAttrib(&m_stSourceItem);
		}

		if(AfxGetHge()->Input_KeyUp(HGEK_LBUTTON))
		{
			if(pBagDlg->IsItemSel())
			{
				const ItemAttrib* pSelItem = pBagDlg->GetSelItem();

				if(NULL != pSelItem)
				{
					if(!GamePlayer::GetInstance()->IsEquipItem(*pSelItem))
					{
						GameInfoBoardDlg::GetInstance()->InsertBoardMsg(ARGB_RED, "������ֹ�װ��");
					}
					else
					{
						if(pSelItem->MP == 0)
						{
							GameInfoBoardDlg::GetInstance()->InsertBoardMsg(ARGB_RED, "������ֹ�װ��");
						}
						else
						{
							if(PotentialAttribHelper::GetPotentialTime(pSelItem->MP) >= 5)
							{
								GameInfoBoardDlg::GetInstance()->InsertBoardMsg(ARGB_RED, "��װ�����޻��Ὺ��Ǳ��");
							}
							else
							{
								memcpy(&m_stSourceItem, pSelItem, sizeof(ItemAttrib));
								m_nPotentialPrice = PotentialAttribHelper::GetPotentialOffset(pSelItem->MP) * 50000;
							}
						}
					}
				}

				pBagDlg->CancelSel();
			}
		}
	}

	rcTest.left = RELATIVE_X(OFFSET_X_POTENTIAL_STONE);
	rcTest.right = rcTest.left + 36;
	rcTest.top = RELATIVE_Y(OFFSET_Y_POTENTIAL_STONE);
	rcTest.bottom = rcTest.top + 36;

	if(PtInRect(&rcTest, _mp))
	{
		if(m_stOpenStone.type != ITEM_NO)
		{
			GameMainOptUI::GetInstance()->GetDisplayDlg()->ShowItemAttrib(&m_stOpenStone);
		}

		if(AfxGetHge()->Input_KeyUp(HGEK_LBUTTON))
		{
			if(pBagDlg->IsItemSel())
			{
				const ItemAttrib* pSelItem = pBagDlg->GetSelItem();

				if(NULL != pSelItem)
				{
					if(pSelItem->type != ITEM_COST ||
						pSelItem->curse != 10)
					{
						GameInfoBoardDlg::GetInstance()->InsertBoardMsg(ARGB_RED, "�����Ǳ��ʯ");
					}
					else
					{
						memcpy(&m_stOpenStone, pSelItem, sizeof(ItemAttrib));
					}
				}

				pBagDlg->CancelSel();
			}
		}
	}
	
	return true;
}

void GameStoveDlg::RenderTabOpenPotential()
{
	HTEXTURE tex = 0;
	GamePlayer* pPlayer = GamePlayer::GetInstance();
	ExtendHeroAttrib* pAttrib = pPlayer->GetExtendAttrib();

	GameTextureManager* pPngMgr = GameResourceManager::GetInstance()->GetTexs(RES_CUSTOM);
	GameTextureManager* pItemsMgr = GameResourceManager::GetInstance()->GetTexs(RES_ITEMS);

	tex = pPngMgr->GetTexture(34);
	if(tex)
	{
		m_pRender->SetTexture(tex);
		m_pRender->SetTextureRect(0, 0, 326, 272);
		m_pRender->Render(RELATIVE_X(OFFSET_X_POTENTIAL_BEGIN), RELATIVE_Y(OFFSET_Y_POTENTIAL_BEGIN));
	}

	//	render items
	int nTextWidth = 0;
	int nTextHeight = 0;

	if(m_stOpenStone.type != ITEM_NO)
	{
		tex = pItemsMgr->GetTexture(m_stOpenStone.tex);
		if(tex)
		{
			nTextWidth = pItemsMgr->GetTextureWidth(m_stOpenStone.tex);
			nTextHeight = pItemsMgr->GetTextureHeight(m_stOpenStone.tex);

			m_pRender->SetTexture(tex);
			m_pRender->SetTextureRect(0, 0, nTextWidth, nTextHeight);
			m_pRender->Render(RELATIVE_X(OFFSET_X_POTENTIAL_STONE + (36 - nTextWidth) / 2),
				RELATIVE_Y(OFFSET_Y_POTENTIAL_STONE + (36 - nTextHeight) / 2));
		}
	}
	if(m_stSourceItem.type != ITEM_NO)
	{
		tex = pItemsMgr->GetTexture(m_stSourceItem.tex);
		if(tex)
		{
			nTextWidth = pItemsMgr->GetTextureWidth(m_stSourceItem.tex);
			nTextHeight = pItemsMgr->GetTextureHeight(m_stSourceItem.tex);

			m_pRender->SetTexture(tex);
			m_pRender->SetTextureRect(0, 0, nTextWidth, nTextHeight);
			m_pRender->Render(RELATIVE_X(OFFSET_X_POTENTIAL_SOURCE + (36 - nTextWidth) / 2),
				RELATIVE_Y(OFFSET_Y_POTENTIAL_SOURCE + (36 - nTextHeight) / 2));
		}
	}

	AfxGetPrinter()->SetColor(ARGB_WHITE);
	AfxGetPrinter()->PrintWithoutStroke(RELATIVE_X(OFFSET_X_POTENTIAL_PRICE),
		RELATIVE_Y(OFFSET_Y_POTENTIAL_PRICE),
		"%d",
		m_nPotentialPrice);
}