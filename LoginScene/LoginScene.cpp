#include "../LoginScene/LoginScene.h"
#include "../GameScene/GameScene.h"
#include "../Common/GlobalFunction.h"
#include "../Common/GlobalDefine.h"
#include "../Net/ClientSocket.h"
#include "../../CommonModule/ByteBuffer.h"
#include "../../CommonModule/GamePacket.h"
#include "../BackMir/BackMir.h"
#include "../GameDialog/GameCommonDlg.h"
#include <process.h>
#include <hgeresource.h>
#include <Windows.h>
#include <ZipArchive.h>
#include "../BackMir/BMPreConnWnd.h"
#include "../GameScene/GameResourceManager.h"
#include "../Common/gfx_utils.h"
#include "../../CommonModule/BMHttpManager.h"
#include "../../CommonModule/cJSON.h"
#include "../../CommonModule/CommandLineHelper.h"
#include "../../CommonModule/version.h"
#include "../BackMir/GlobalLuaConfig.h"
#include "../GameScene/GameResourceUtil.h"

//////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* Varible
/************************************************************************/
static const char* g_szButtonText[] = 
{
	"ս����Ϸ",
	"��ʼ��Ϸ",
	"������",
	"��    ��",
	"�˳���Ϸ"
};

extern ByteBuffer g_xBuffer;

#define FIREWORK_UPDATE_TIME		100

//////////////////////////////////////////////////////////////////////////
LoginScene::LoginScene()
{
	m_pxResMgr = NULL;
	ZeroMemory(m_stRect, sizeof(m_stRect));
	ZeroMemory(m_stHero, sizeof(m_stHero));

//#ifdef _DEBUG
	m_stRect[RECT_BATTLE].left = 329;
	m_stRect[RECT_BATTLE].right = 478;
	m_stRect[RECT_BATTLE].top = 213;
	m_stRect[RECT_BATTLE].bottom = 238;

	m_stRect[RECT_NET].left = 329;
	m_stRect[RECT_NET].right = 478;
	m_stRect[RECT_NET].top = 249;
	m_stRect[RECT_NET].bottom = 275;

	m_stRect[RECT_SETTING].left = 329;
	m_stRect[RECT_SETTING].right = 478;
	m_stRect[RECT_SETTING].top = 285;
	m_stRect[RECT_SETTING].bottom = 311;

	m_stRect[RECT_MADE].left = 329;
	m_stRect[RECT_MADE].right = 478;
	m_stRect[RECT_MADE].top = 341;
	m_stRect[RECT_MADE].bottom = 366;

	m_stRect[RECT_QUIT].left = 329;
	m_stRect[RECT_QUIT].right = 478;
	m_stRect[RECT_QUIT].top = 377;
	m_stRect[RECT_QUIT].bottom = 403;
/*#else
	m_stRect[RECT_SINGLE + 1].left = 329;
	m_stRect[RECT_SINGLE + 1].right = 478;
	m_stRect[RECT_SINGLE + 1].top = 213;
	m_stRect[RECT_SINGLE + 1].bottom = 238;

	m_stRect[RECT_NET + 1].left = 329;
	m_stRect[RECT_NET + 1].right = 478;
	m_stRect[RECT_NET + 1].top = 249;
	m_stRect[RECT_NET + 1].bottom = 275;

	m_stRect[RECT_SETTING + 1].left = 329;
	m_stRect[RECT_SETTING + 1].right = 478;
	m_stRect[RECT_SETTING + 1].top = 285;
	m_stRect[RECT_SETTING + 1].bottom = 311;

	m_stRect[RECT_MADE + 1].left = 329;
	m_stRect[RECT_MADE + 1].right = 478;
	m_stRect[RECT_MADE + 1].top = 341;
	m_stRect[RECT_MADE + 1].bottom = 366;
#endif*/

	m_wCurProg = m_wTotalProg = 0;
	ResetMouseState();
	//m_wCurPage = PAGE_LOGIN;
	m_wCurPage = PAGE_PREFACE;
	m_nHumCount = 0;
	m_pSelChrDlg = NULL;

	m_szIP = NULL;
	m_wPort = 0;
	m_bIsConnectiong = false;

	m_pxSelChrScene = new SelChrScene;

	m_pRender = new hgeSprite(0, 0, 0, 0, 0);

	m_pEffRender = new hgeSprite(0, 0, 0, 0, 0);
	m_pEffRender->SetBlendMode(BLEND_ALPHAADD | BLEND_NOZWRITE);

	m_pTexMgr = NULL;
}

LoginScene::~LoginScene()
{
	//SAFE_DELETE(m_pxResMgr);
	SAFE_DELETE(m_pxSelChrScene);
	SAFE_DELETE(m_pSelChrDlg);
	SAFE_DELETE(m_pRender);
	SAFE_DELETE(m_pEffRender);
	SAFE_DELETE(m_pTexMgr);

	if(!m_xFireworks.empty())
	{
		FireworksRenderList::const_iterator begIter = m_xFireworks.begin();
		FireworksRenderList::const_iterator endIter = m_xFireworks.end();

		for(begIter;
			begIter != endIter;
			++begIter)
		{
			delete *begIter;
		}

		m_xFireworks.clear();
	}
}


//////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* ��ʼ������
/************************************************************************/
bool LoginScene::Init(hgeResourceManager* _pRes)
{
	m_pxResMgr = _pRes;
	m_hThread = 0;

	m_pxSelChrScene->Init(_pRes);

	m_pTexMgr = new GameTextureManagerSib;
	char szPath[MAX_PATH];
	sprintf(szPath, "%s\\Data\\Ani.sib",
		GetGameResourceDir());
	if(!m_pTexMgr->LoadPackage(szPath))
	{
		AfxGetHge()->System_Log("can't load package [Ani.sib]");
	}

	return true;
}

/************************************************************************/
/* ���ӳɹ�
/************************************************************************/
void LoginScene::OnConnect()
{
#ifdef _SELCHR_
	if(m_pSelChrDlg)
	{
		m_pSelChrDlg->OnConnect();
	}
#else
	if(!GamePlayer::GetInstance()->LoadProperty((int)m_szHero))
	{
		ALERT_MSGBOX("�޷���ȡ����浵");
	}
#endif
}

//////////////////////////////////////////////////////////////////////////
void LoginScene::OnDisconnect()
{
	ALERT_MSGBOX("��������Ͽ�����");
#ifdef _DEBUG
#else
	PostQuitMessage(0);
#endif
}

/************************************************************************/
/* ���ݰ�
/************************************************************************/
void LoginScene::OnMessage(const PacketBase* _pPacket)
{
	switch(_pPacket->wCmd)
	{
	case PC_SYSTEM_LOGIN_ACK:
		{
			UserLoginPacketAck* pPacket = (UserLoginPacketAck*)_pPacket;
			if(pPacket->bRet == true)
			{
				//	��½�ɹ�
				AfxGetHge()->System_Log("������Ϸ�ɹ�");
				m_hThread = (HANDLE)_beginthreadex(NULL, 0, &LoginScene::LoadGameScene, GameScene::sThis, 0, NULL);
				m_wCurPage = PAGE_LOAD;
			}
		}
	}
}

/************************************************************************/
/* EnterLoadingScene
/************************************************************************/
void LoginScene::EnterLoadingPage()
{
	AfxGetHge()->System_Log("������Ϸ�ɹ�");
	//m_hThread = (HANDLE)_beginthreadex(NULL, 0, &LoginScene::LoadGameScene, GameScene::sThis, 0, NULL);
	LoadGameScene(GameScene::sThis);
	m_wCurPage = PAGE_LOAD;
}

//////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* ����
/************************************************************************/
void LoginScene::Render()
{
	HTEXTURE tex = 0;
	bool bDrawVersion = true;
	//	����

	switch(m_wCurPage)
	{
	case PAGE_LOGIN:
	case PAGE_LOGIN_CONNECT:
		{
			RenderLogin();
		}break;
	case PAGE_LOAD:
		{
			RenderLoad();
		}break;
	case PAGE_SELCHR:
		{
			RenderSelChr();
		}break;
	case PAGE_ABOUT:
		{
			RenderAbout();
		}break;
	case PAGE_PREFACE:
		{
			RenderPreface();
			bDrawVersion = false;
		}break;
	case PAGE_THANKS:
		{
			RenderThanks();
		}break;
	}

	if(bDrawVersion)
	{
		AfxGetPrinter()->SetColor(ARGB_WHITE);
		AfxGetPrinter()->Print(10, VIEW_HEIGHT + VIEW_OPERATE - 20, "��Ϸ�汾: %s", BACKMIR_CURVERSION);
	}
}

/************************************************************************/
/* ����
/************************************************************************/
void LoginScene::Update(float _dt)
{

}

/************************************************************************/
/* ��������
/************************************************************************/
bool LoginScene::ProcUserCmd(const POINT &_mp)
{
	ResetMouseState();
	static DWORD s_dwGenerateTime = 0;

	if(GetTickCount() - s_dwGenerateTime > 3000)
	{
		if(m_xFireworks.size() < 4)
		{
			int nRandom = rand() % 3;

			if(nRandom == 0)
			{
				//	Create a firework
				FireworksInfo* pFirework = new FireworksInfo;
				ZeroMemory(pFirework, sizeof(FireworksInfo));

				pFirework->bType = rand() % 6;
				pFirework->dwPos = MAKELONG(rand() % 500 + 100, rand() % 300 + 100);
				pFirework->dwLastUpdateTime = GetTickCount();
				pFirework->bCurFrame = 0;
				if(pFirework->bType == 5)
				{
					pFirework->bTotalFrame = 20;
				}
				else
				{
					pFirework->bTotalFrame = 16;
				}

				m_xFireworks.push_back(pFirework);
			}
		}
	}

	if(!m_xFireworks.empty())
	{
		FireworksRenderList::iterator begIter = m_xFireworks.begin();
		FireworksRenderList::const_iterator endIter = m_xFireworks.end();
		FireworksInfo* pFirework = NULL;

		for(begIter;
			begIter != endIter;
			)
		{
			pFirework = *begIter;

			if(pFirework)
			{
				if(pFirework->bCurFrame == (pFirework->bTotalFrame - 1) &&
					GetTickCount() - pFirework->dwLastUpdateTime > FIREWORK_UPDATE_TIME)
				{
					delete pFirework;
					begIter = m_xFireworks.erase(begIter);
				}
				else
				{
					if(GetTickCount() - pFirework->dwLastUpdateTime > FIREWORK_UPDATE_TIME)
					{
						++pFirework->bCurFrame;
						pFirework->dwLastUpdateTime = GetTickCount();
					}

					++begIter;
				}
			}
			else
			{
				begIter = m_xFireworks.erase(begIter);
			}
		}
	}

	switch(m_wCurPage)
	{
	case PAGE_LOGIN:
	case PAGE_LOGIN_CONNECT:
		{
			return CommandLogin(_mp);
		}break;
	case PAGE_LOAD:
		{
			return CommandLoad(_mp);
		}break;
	case PAGE_SELCHR:
		{
			return CommandSelChr(_mp);
		}break;
	case PAGE_ABOUT:
		{
			return CommandAbout(_mp);
		}break;
	case PAGE_PREFACE:
		{
			return CommandPreface(_mp);
		}break;
	case PAGE_THANKS:
		{
			return CommandThanks(_mp);
		}break;
// 	case PAGE_SELCHR:
// 		{
// 			sgui::Event evt;
// 			sgui::Widget::MakeEvent(&evt);
// 			m_pxSelChrScene->Dispatcher(&evt);
// 			return true;
// 		}
	}

	return false;
}



//////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* ��������Ϣ
/************************************************************************/
void LoginScene::OnPressNetButton()
{
	if(pTheGame->GetGameMode() == GM_NORMAL)
	{
		char szFile[MAX_PATH];
		sprintf(szFile, "%s\\Save\\hum.sav",
			GetRootPath());
		HWND hWnd = AfxGetHge()->System_GetState(HGE_HWND);

		if(!PathFileExists(szFile))
		{
			CZipArchive xSave;
			xSave.Open(szFile, 0x0004);
			xSave.SetPassword(SaveFile::CalcInternalPassword());
			xSave.Close();
		}

		LoadHumData();

		//	����һ�ݴ浵�ĸ���
		char szCopyFile[MAX_PATH];
		strcpy(szCopyFile, szFile);
		PathRemoveFileSpec(szCopyFile);
		strcat(szCopyFile, "/hum.bak");
		CopyFile(szFile, szCopyFile, FALSE);
	}
	else if(pTheGame->GetGameMode() == GM_LOGIN)
	{
		for(int i = 0; i < 3; ++i)
		{
			memcpy(&m_stHero[i], &g_stHeroHeader[i], sizeof(HeroHeader));
		}
	}

	m_pSelChrDlg = new SelChrDlg;
	m_pSelChrDlg->SetData(m_stHero);
	//m_pDelCfmDlg = new DeleteConfirmDlg;
	//m_xDlgCtrl.InsertObject(m_pDelCfmDlg);
	SetPage(PAGE_SELCHR);
}

void LoginScene::OnBattleNetHTTPGet(const char* _pData, size_t _uSize)
{
	if (NULL == _pData ||
		0 == _uSize)
	{
		::MessageBox(NULL, "�޷���ȡ�������б�", "����", MB_OK | MB_ICONERROR);
		return;
	}

	// parse json object
	cJSON* pRoot = cJSON_Parse(_pData);

	if (NULL == pRoot)
	{
		::MessageBox(NULL, "�޷���ȡ�������б�", "����", MB_OK | MB_ICONERROR);
		return;
	}

	bool bParseOk = false;
	const char* pszErrTip = "�����������б�ʧ��";
	std::string xServerList;
	int nCode = -1;

	do 
	{
		cJSON* pCode = cJSON_GetObjectItem(pRoot, "code");
		if (NULL == pCode)
		{
			break;
		}
		if (cJSON_Number != pCode->type)
		{
			break;
		}
		nCode = pCode->valueint;

		cJSON* pMsg = cJSON_GetObjectItem(pRoot, "message");
		if (NULL == pMsg)
		{
			break;
		}
		if (cJSON_String != pMsg->type)
		{
			break;
		}
		xServerList = pMsg->valuestring;
		bParseOk = true;
	} while (0);
	cJSON_Delete(pRoot);
	pRoot = NULL;

	if (!bParseOk)
	{
		::MessageBox(NULL, pszErrTip, "����", MB_OK | MB_ICONERROR);
		return;
	}
	if (0 != nCode ||
		0 == xServerList.size())
	{
		::MessageBox(NULL, xServerList.c_str(), "����", MB_OK | MB_ICONERROR);
		return;
	}

	CommandLineHelper xHelper;
	if (xHelper.InitParam())
	{
		const char* pszSvrIP = xHelper.GetParam("svrip");
		if (NULL != pszSvrIP)
		{
			if (0 == strcmp(xServerList.c_str(), pszSvrIP))
			{
				::MessageBox(NULL, "���ѵ�¼ս��������", "��ʾ", MB_OK | MB_ICONINFORMATION);
				return;
			}
		}
	}

	char szLaunchCmd[MAX_PATH];
#ifdef _DEBUG
	sprintf(szLaunchCmd, "%s/BinDebug/backmir.exe svrip=%s lk=lk0x", GetRootPath(), xServerList.c_str());
#else
	sprintf(szLaunchCmd, "%s/Bin/backmir.exe svrip=%s lk=lk0x", GetRootPath(), xServerList.c_str());
#endif
	
	PROCESS_INFORMATION pi;
	STARTUPINFO si = { sizeof(si) };
	si.wShowWindow = SW_NORMAL;

	BOOL bRet = CreateProcess(NULL, szLaunchCmd, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
	if (FALSE == bRet)
	{
		::MessageBox(NULL, "�޷�������Ϸ", "����", MB_OK | MB_ICONERROR);
		return;
	}

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	PostQuitMessage(0);
}

void LoginScene::OnAppVersionHTTPGet(const char* _pData, size_t _uSize)
{
	if (NULL == _pData ||
		0 == _uSize)
	{
		::MessageBox(NULL, "�޷���ð汾��Ϣ", "����", MB_OK | MB_ICONERROR);
		return;
	}

	// parse json object
	cJSON* pRoot = cJSON_Parse(_pData);

	if (NULL == pRoot)
	{
		::MessageBox(NULL, "�޷���ð汾��Ϣ", "����", MB_OK | MB_ICONERROR);
		return;
	}

	bool bParseOk = false;
	const char* pszErrTip = "�����汾��Ϣʧ��";
	std::string xAppVersionLatest;
	int nCode = -1;

	do
	{
		cJSON* pCode = cJSON_GetObjectItem(pRoot, "code");
		if (NULL == pCode)
		{
			break;
		}
		if (cJSON_Number != pCode->type)
		{
			break;
		}
		nCode = pCode->valueint;

		cJSON* pMsg = cJSON_GetObjectItem(pRoot, "message");
		if (NULL == pMsg)
		{
			break;
		}
		if (cJSON_String != pMsg->type)
		{
			break;
		}
		xAppVersionLatest = pMsg->valuestring;
		bParseOk = true;
	} while (0);
	cJSON_Delete(pRoot);
	pRoot = NULL;

	if (!bParseOk)
	{
		::MessageBox(NULL, pszErrTip, "����", MB_OK | MB_ICONERROR);
		return;
	}
	if (0 != nCode ||
		0 == xAppVersionLatest.size())
	{
		::MessageBox(NULL, xAppVersionLatest.c_str(), "����", MB_OK | MB_ICONERROR);
		return;
	}
	if (0 == strcmp(xAppVersionLatest.c_str(), BACKMIR_CURVERSION))
	{
		::MessageBox(NULL, "���Ѿ������°汾", "��ʾ", MB_OK | MB_ICONINFORMATION);
		return;
	}
	m_xLatestAppVersion = xAppVersionLatest;

	// get patch url
	const std::string& refURL = GetGlobalLuaConfig("gHTTPKVAddress");
	if (refURL.empty())
	{
		::MessageBox(NULL, "�޷���ȡ����", "����", MB_OK | MB_ICONERROR);
		return;
	}
	char szReqURL[MAX_PATH];
	sprintf(szReqURL, "%s/get?key=app_patch_url_%s", refURL.c_str(), xAppVersionLatest.c_str());
	// request for battle net address
	BMHttpManager::GetInstance()->DoGetRequestSync(szReqURL, std::bind(&LoginScene::OnPatchURLHTTPGet, this,
		std::placeholders::_1, std::placeholders::_2));
}

void LoginScene::OnPatchURLHTTPGet(const char* _pData, size_t _uSize)
{
	if (NULL == _pData ||
		0 == _uSize)
	{
		::MessageBox(NULL, "�޷���ò�����ַ", "����", MB_OK | MB_ICONERROR);
		return;
	}

	// parse json object
	cJSON* pRoot = cJSON_Parse(_pData);

	if (NULL == pRoot)
	{
		::MessageBox(NULL, "�޷���ò�����ַ", "����", MB_OK | MB_ICONERROR);
		return;
	}

	bool bParseOk = false;
	const char* pszErrTip = "����������ַʧ��";
	std::string xAppPatchURL;
	int nCode = -1;

	do
	{
		cJSON* pCode = cJSON_GetObjectItem(pRoot, "code");
		if (NULL == pCode)
		{
			break;
		}
		if (cJSON_Number != pCode->type)
		{
			break;
		}
		nCode = pCode->valueint;

		cJSON* pMsg = cJSON_GetObjectItem(pRoot, "message");
		if (NULL == pMsg)
		{
			break;
		}
		if (cJSON_String != pMsg->type)
		{
			break;
		}
		xAppPatchURL = pMsg->valuestring;
		bParseOk = true;
	} while (0);
	cJSON_Delete(pRoot);
	pRoot = NULL;

	if (!bParseOk)
	{
		::MessageBox(NULL, pszErrTip, "����", MB_OK | MB_ICONERROR);
		return;
	}
	if (0 != nCode ||
		0 == xAppPatchURL.size())
	{
		::MessageBox(NULL, xAppPatchURL.c_str(), "����", MB_OK | MB_ICONERROR);
		return;
	}

	// copy the clip
	char szTip[MAX_PATH];
	if (::OpenClipboard(NULL))
	{
		::EmptyClipboard();
		HGLOBAL clipbuffer;
		char *buffer;
		clipbuffer = ::GlobalAlloc(GMEM_DDESHARE, xAppPatchURL.size() + 1);
		buffer = (char *)::GlobalLock(clipbuffer);
		strcpy(buffer, xAppPatchURL.c_str());
		::GlobalUnlock(clipbuffer);
		::SetClipboardData(CF_TEXT, clipbuffer);
		::CloseClipboard();
	}
	else
	{
		sprintf(szTip, "���ʼ�����ʧ�ܣ�%s�������ص�ַΪ: %s", m_xLatestAppVersion.c_str(), xAppPatchURL.c_str());
		::MessageBox(NULL, szTip, "����", MB_OK | MB_ICONERROR);
		return;
	}
	
	sprintf(szTip, "���°汾(%s)�������ص�ַΪ��%s �� ��Ϊ�������������壬�븴�Ƶ�������з�������", m_xLatestAppVersion.c_str(), xAppPatchURL.c_str());
	::MessageBox(NULL, szTip, "��ʾ", MB_OK | MB_ICONINFORMATION);
}

void LoginScene::OnButton(RECT_RIGION _rc)
{
	bool bDown = AfxGetHge()->Input_GetKeyState(HGEK_LBUTTON);
	bool bUp = AfxGetHge()->Input_KeyUp(HGEK_LBUTTON);
	m_wCurOver = _rc;
	if(bDown)
	{
		m_wCurOver = 0xFF;
		m_wCurDown = _rc;
	}
	if (bUp)
	{
		GameSoundManager::GetInstancePtr()->PlayGameSound(SDGAME_CLICKBUTTON);
	}

	switch(_rc)
	{
	case RECT_MADE:
		{
			//
			if(bUp)
			{
				m_wCurPage = PAGE_ABOUT;
			}
		}break;
	case RECT_BATTLE:
		{
			//
			if(bUp)
			{
				//	ս����Ϸ
				// get kv http address
				const std::string& refURL = GetGlobalLuaConfig("gHTTPKVAddress");
				if (refURL.empty())
				{
					::MessageBox(NULL, "�޷���ȡ����", "����", MB_OK | MB_ICONERROR);
					return;
				}
				char szReqURL[MAX_PATH];
				sprintf(szReqURL, "%s/get?key=app_battlenet_addresses", refURL.c_str());
				// request for battle net address
				BMHttpManager::GetInstance()->DoGetRequestSync(szReqURL, std::bind(&LoginScene::OnBattleNetHTTPGet, this,
					std::placeholders::_1, std::placeholders::_2));
			}
		}break;
	case RECT_NET:
		{
			//
			//	socket
			if(bUp)
			{
				OnPressNetButton();
			}
		}break;
	case RECT_SETTING:
		{
			// check for update
			//
			if(bUp)
			{
				//m_wCurPage = PAGE_THANKS;
				const std::string& refURL = GetGlobalLuaConfig("gHTTPKVAddress");
				if (refURL.empty())
				{
					::MessageBox(NULL, "�޷���ȡ����", "����", MB_OK | MB_ICONERROR);
					return;
				}
				char szReqURL[MAX_PATH];
				sprintf(szReqURL, "%s/get?key=app_version", refURL.c_str());
				// request for battle net address
				BMHttpManager::GetInstance()->DoGetRequestSync(szReqURL, std::bind(&LoginScene::OnAppVersionHTTPGet, this,
					std::placeholders::_1, std::placeholders::_2));
			}
		}break;
	case RECT_QUIT:
		{
			//
			if(bUp)
			{
				::PostQuitMessage(0);
			}
		}break;
	default:
		{
			//	Error??
			ResetMouseState();
		}break;
	}
}


/************************************************************************/
/* ����������Ϣ
/************************************************************************/
void LoginScene::OnSceneProg(int _nTotal, int _nCur)
{
	m_wTotalProg = _nTotal;
	m_wCurProg = _nCur;
}


//////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* ���ð�ť״̬
/************************************************************************/
void LoginScene::ResetMouseState()
{
	m_wCurDown = 0xFF;
	m_wCurOver = 0xFF;
}

/************************************************************************/
/* ���Ʊ���
/************************************************************************/
void LoginScene::RenderBk()
{
	HTEXTURE tex = m_pxResMgr->GetTexture("login");
	if(tex)
	{
		if(!m_pRender)
		{
			m_pRender = new hgeSprite(tex, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		}
		else
		{
			m_pRender->SetTexture(tex);
			m_pRender->SetTextureRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		}
		m_pRender->Render(0, 0);

		//	Render animation effect
		FireworksRenderList::iterator begIter = m_xFireworks.begin();
		FireworksRenderList::const_iterator endIter = m_xFireworks.end();
		FireworksInfo* pFirework = NULL;
		int nTexIndex = 0;

		for(begIter;
			begIter != endIter;
			++begIter)
		{
			pFirework = *begIter;

			nTexIndex = 100 + pFirework->bType * 20 + pFirework->bCurFrame;
			tex = m_pTexMgr->GetTexture(nTexIndex);

			if(tex)
			{
				m_pEffRender->SetTexture(tex);
				m_pEffRender->SetTextureRect(0, 0, m_pTexMgr->GetTextureWidth(nTexIndex), m_pTexMgr->GetTextureHeight(nTexIndex));
				m_pEffRender->Render(LOWORD(pFirework->dwPos) + m_pTexMgr->GetTextureOffsetX(nTexIndex),
					HIWORD(pFirework->dwPos) + m_pTexMgr->GetTextureOffsetY(nTexIndex));
			}
		}
	}
}

/************************************************************************/
/* ���Ƶ�½����
/************************************************************************/
void LoginScene::RenderLogin()
{
	GameTextureManager* pPngMgr = GameResourceManager::GetInstance()->GetTexs(RES_CUSTOM);
	HTEXTURE tex = m_pxResMgr->GetTexture("login");
	if(tex)
	{
		/*if(!m_pRender)
		{
			m_pRender = new hgeSprite(tex, 0, 0, 816, 540);
		}
		else
		{
			m_pRender->SetTexture(tex);
			m_pRender->SetTextureRect(0, 0, 816, 540);
		}
		m_pRender->Render(0, 0);

		//	�������Ч��
		//static DWORD dwLastThunder1 = 0;
		if(rand() % 25 == 2)
		{
			tex = m_pxResMgr->GetTexture("thunder1");
			if(tex)
			{
				m_pRender->SetTexture(tex);
				m_pRender->SetTextureRect(0, 0, 266, 651);
				m_pRender->Render(rand() % 800, -10 + rand() % 15);
			}
		}
		else if(rand() % 25 == 3)
		{
			tex = m_pxResMgr->GetTexture("thunder2");
			if(tex)
			{
				m_pRender->SetTexture(tex);
				m_pRender->SetTextureRect(0, 0, 300, 651);
				m_pRender->Render(rand() % 800, -10 + rand() % 15);
			}
		}*/

		RenderBk();

		//	����ť
		tex = m_pxResMgr->GetTexture("buttonnormal");
		if(tex)
		{
			m_pRender->SetTexture(tex);
			m_pRender->SetTextureRect(0, 0, 149, 26);

			for (int i = RECT_BATTLE; i < RECT_TOTAL; ++i)
			{
				m_pRender->Render(m_stRect[i].left,
					m_stRect[i].top);
			}
		}

		//	��ť״̬
		if(m_wCurOver != 0xFF)
		{
			tex = m_pxResMgr->GetTexture("buttonover");
			if(tex)
			{
				m_pRender->SetTexture(tex);
				m_pRender->SetTextureRect(0, 0, 149,26);
				m_pRender->Render(m_stRect[m_wCurOver].left, m_stRect[m_wCurOver].top);
			};
		}

		if(m_wCurDown != 0xFF)
		{
			tex = m_pxResMgr->GetTexture("buttondown");
			if(tex)
			{
				m_pRender->SetTexture(tex);
				m_pRender->SetTextureRect(0, 0, 149,26);
				m_pRender->Render(m_stRect[m_wCurDown].left, m_stRect[m_wCurDown].top);
			}
		}
//#ifdef _DEBUG
		for (int i = RECT_BATTLE; i < RECT_TOTAL; ++i)
/*#else
		for(int i = RECT_SINGLE + 1; i < RECT_TOTAL; ++i)
#endif*/
		{
			if(m_wCurDown == i)
			{
				AfxGetPrinter()->Print(m_stRect[i].left + 75 - 2 * 12 + 2,
					m_stRect[i].top + 13 - 6 + 2, g_szButtonText[i]);
			}
			else
			{
				AfxGetPrinter()->Print(m_stRect[i].left + 75 - 2 * 12,
					m_stRect[i].top + 13 - 6, g_szButtonText[i]);
			}
		}

		//	��½����״̬
		if(m_bIsConnectiong)
		{
			//tex = m_pxResMgr->GetTexture("bmcolor");
			tex = pPngMgr->GetTexture(19);
			if(tex)
			{
				/*
				m_pRender->SetTexture(tex);
								m_pRender->SetTextureRect(0, 0, 250, 50);
								m_pRender->SetColor(ARGB(192, 255, 255, 255));
								m_pRender->Render(VIEW_WIDTH / 2 - 250 / 2, 250);
								m_pRender->SetColor(ARGB(255, 255, 255, 255));*/
				RECT rcScale9 = {9, 9, 9, 9};
				RECT rcTex = {0, 0, 32, 32};
				Gfx_SetRenderState(kGfx_9Path_CenterStretch, 1);
				Gfx_Render9Path(m_pRender, tex, &rcTex, &rcScale9, VIEW_WIDTH / 2 - 250 / 2, 250, 250, 250);
			}
			static float s_fTime = 0.0f;
			s_fTime += AfxGetHge()->Timer_GetDelta();
			char szBuf[50];

			if(s_fTime > 3.0f)
			{
				s_fTime = 0.0f;
				sprintf(szBuf, "�������ӷ�����.");
			}
			else if(s_fTime > 2.0f)
			{
				sprintf(szBuf, "�������ӷ�����...");
			}
			else if(s_fTime > 1.0f)
			{
				sprintf(szBuf, "�������ӷ�����..");
			}
			else
			{
				sprintf(szBuf, "�������ӷ�����.");
			}
			AfxGetPrinter()->SetColor(ARGB_WHITE);
			AfxGetPrinter()->Print(360, 250 + 25 - 6, szBuf);
		}
	}
}

/************************************************************************/
/* ��½�����û���Ϣ����
/************************************************************************/
bool LoginScene::CommandLogin(const POINT& _ptMouse)
{
//#ifdef _DEBUG
	for (int i = RECT_BATTLE; i < RECT_TOTAL; ++i)
/*#else
	for(int i = RECT_SINGLE + 1; i < RECT_TOTAL; ++i)
#endif*/
	{
		if(m_bIsConnectiong)
		{
			return false;
		}
		if(::PtInRect(&m_stRect[i], _ptMouse))
		{
			OnButton((RECT_RIGION)i);
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
void LoginScene::RenderAbout()
{
#define _TEXTABOUT_

#ifdef _TEXTABOUT_
	//HTEXTURE tex = m_pxResMgr->GetTexture("login");
	//if(tex)
	{
		/*if(!m_pRender)
		{
			m_pRender = new hgeSprite(tex, 0, 0, 816, 540);
		}
		else
		{
			m_pRender->SetTexture(tex);
			m_pRender->SetTextureRect(0, 0, 816, 540);
		}
		m_pRender->Render(0, 0);*/
		RenderBk();

		static const char* s_pszAbout = "����: sryan yuan\n"\
										"�߻�����:sryan yuan\n"
										"�ʼ���ϵ:553056120@qq.com\n\n"\
										"����:����Ϸ�زĲ�����ʢ�� ��Ѫ�����زģ��زİ�Ȩ��ʢ�����У�����Ϸ���κ���ҵĿ�������ʣ����е�λ���������޸ı���Ϸ���ַ��Ƿ������\n"
										"�����Ϊ�������߱����޹أ�����Ϸ�Ŀ���Ŀ����Ϊ��ѧϰ��\n"\
										"This title uses Fmodex non-commercial license.Audio engine : FMOD Sound System by Firelight Technologies\n"\
										"Other library used in this game:ZLIB, HGE, ZIPARCHIVE, GLOG, LUA, TOLUA, SQLITE, and they are all free for non-commercial purpose\n"\
										"����Ϸʹ���� ΢���ƽ�� ��������ʾ����������򷽰�(GfxFont and GfxEdit)������չ����ʵ�����Ч��������Ҫ��Ч��Դ�룬�뷢�ʼ���ȡ��\n\n"\
										"����Ϸ�������ڸ���ԭ���ݸ��µ���Ѫ����1.76�汾����������1.76����װ���Լ�������ȥ�ľ����ͼ�����������\"�λ�\"���档\n"\
										"δ�������������ɣ������öԱ�����������򹤳̡������롢������Լ��κ���ʽ���޸ġ�\n"\
										//"����ϲ������Ϸ��������������Ȥ������֧�����ߵĿ�����"
										"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nBackMIR��һ����ѵ���Ϸ�������Ϸ�ľ�������1����ҵ��ʱ�䣬��������ø���Ϸ���棬֧�����߿���������Կ��Ǿ���֧�����ߣ���Ȼ������\n"\
										"�������㽫ӵ�г���ʹ��BackMIR������Ϸ����κ�Ȩ����\n"
										//"������Ϣ: ֧����:sryan@qq.com\n"
										"���Ʋ�����Ϸ���ܾ�������Ϸ��ע����������������ƭ�ϵ����ʶ���Ϸ���ԣ�������Ϸ����������ʱ�䣬���ܽ������";

		AfxGetPrinter()->SetColor(ARGB_WHITE);
		//AfxGetPrinter()->LineFeedRender(20, 20, s_pszAbout, 64 * 12);
		float fPreKerning = AfxGetPrinter()->GetKerningHeight();
		AfxGetPrinter()->SetKerningHeight(2.0f);
		AfxGetPrinter()->Print(10, 20, s_pszAbout);
		AfxGetPrinter()->SetKerningHeight(fPreKerning);
	//}
	}
#else
	HTEXTURE tex = m_pxResMgr->GetTexture("about");
	if(tex)
	{
		if(!m_pRender)
		{
			m_pRender = new hgeSprite(tex, 0, 0, 816, 540);
		}
		else
		{
			m_pRender->SetTexture(tex);
			m_pRender->SetTextureRect(0, 0, 816, 540);
		}
		m_pRender->Render(0, 0);
	}
#endif
}
//////////////////////////////////////////////////////////////////////////
bool LoginScene::CommandAbout(const POINT& _ptMouse)
{
	if(AfxGetHge()->Input_KeyUp(HGEK_LBUTTON))
	{
		m_wCurPage = PAGE_LOGIN;
	}
	return true;
}

/************************************************************************/
/* ��ȡ�������
/************************************************************************/
void LoginScene::RenderLoad()
{
	HTEXTURE tex = 0;

	if(m_wCurProg != m_wTotalProg &&
		m_wTotalProg != 0)
	{
		tex = m_pxResMgr->GetTexture("load1");
		if(tex)
		{
			m_pRender->SetTexture(tex);
			m_pRender->SetTextureRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
			m_pRender->Render(0, 0);
		}

		//	����
		float fProg = (float)m_wCurProg / (float)m_wTotalProg;
		int nShow = fProg * 100;
		AfxGetPrinter()->Print(408, 500, "%d%%",
			nShow);

		tex = m_pxResMgr->GetTexture("progempty");
		if(tex)
		{
			m_pRender->SetTexture(tex);
			m_pRender->SetTextureRect(0, 0, 417, 24);
			m_pRender->Render(200, 510);
		}
		tex = m_pxResMgr->GetTexture("progfull");
		if(tex)
		{
			m_pRender->SetTexture(tex);
			m_pRender->SetTextureRect(0, 0, 364 * fProg, 11);
			m_pRender->Render(200 + 25, 510 + 5);
		}
	}
	else if(m_wTotalProg != 0)
	{
		tex = m_pxResMgr->GetTexture("load1");
		if(tex)
		{
			m_pRender->SetTexture(tex);
			m_pRender->SetTextureRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
			m_pRender->Render(0, 0);
		}

		tex = m_pxResMgr->GetTexture("progempty");
		if(tex)
		{
			m_pRender->SetTexture(tex);
			m_pRender->SetTextureRect(0, 0, 417, 24);
			m_pRender->Render(200, 510);
		}
		tex = m_pxResMgr->GetTexture("progfull");
		if(tex)
		{
			m_pRender->SetTexture(tex);
			m_pRender->SetTextureRect(0, 0, 364, 11);
			m_pRender->Render(200 + 25, 510 + 5);
		}

		static DWORD dwFlash = ::GetTickCount();
		DWORD dwCurCount = ::GetTickCount();
		static bool bFlash = true;
		if(dwCurCount - dwFlash > 500)
		{
			bFlash = !bFlash;
			dwFlash = dwCurCount;
		}

		if(bFlash)
		{
			AfxGetPrinter()->Print(408 - 5 * 12 + 6, 500, "����������������Ϸ");
		}
	}
}

void LoginScene::RenderSelChr()
{
	m_pSelChrDlg->Render();
	//m_xDlgCtrl.Render();
}

bool LoginScene::CommandSelChr(const POINT& _ptMouse)
{
	//if(!m_xDlgCtrl.ProcUserCmd(_ptMouse))
	{
		m_pSelChrDlg->ProcUserCmd(_ptMouse);
	}
	return true;
}


/************************************************************************/
/* ��ȡ������Ϣ����
/************************************************************************/
bool LoginScene::CommandLoad(const POINT& _ptMouse)
{
	if(m_wTotalProg != 0 &&
		m_wTotalProg == m_wCurProg)
	{
		if(AfxGetHge()->Input_KeyUp(HGEK_LBUTTON))
		{
			::PostMessage(AfxGetHge()->System_GetState(HGE_HWND), WM_ENTER_GAMESCENE, 0, 0);
			//pTheGame->IsTimerFunctionRepalaced();
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* ��Ϸ������ʼ���߳�
/************************************************************************/
unsigned __stdcall LoginScene::LoadGameScene(void * _arg)
{
	GameScene* pGameScene = (GameScene*)_arg;
	if(!pGameScene->Init(AfxGetHge()))
	{
		assert("Init gamescene error!");
	}
	
	pGameScene->SwitchScene(GamePlayer::GetInstance()->GetMapID());
	::PostMessage(AfxGetHge()->System_GetState(HGE_HWND), WM_ENTER_GAMESCENE, 0, 0);
	//::SendMessage(AfxGetHge()->System_GetState(HGE_HWND), WM_ENTER_GAMESCENE, 0, 0);
	//_endthreadex(0);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
int LoginScene::LoadHumData()
{
	char szFile[MAX_PATH];
	char* pBuf = NULL;
	int nValidHum = 0;

	sprintf(szFile, "%s\\Save\\hum.sav",
		GetRootPath());
	extern ByteBuffer g_xBuffer;

	if(!PathFileExists(szFile))
	{
		return 0;
	}
	else
	{
		CZipArchive xSave;
		CZipFileHeader* pFirstHeader = NULL;

		if(false == xSave.Open(szFile))
		{
			return 0;
		}
		xSave.SetPassword(SaveFile::CalcInternalPassword());

		ZIP_INDEX_TYPE zIndex = xSave.FindFile("head");
		if(zIndex != ZIP_FILE_INDEX_NOT_FOUND)
		{
			pFirstHeader = xSave.GetFileInfo(zIndex);
			if(NULL == pFirstHeader)
			{
				xSave.Close();
				return nValidHum;
			}

			xSave.OpenFile(zIndex);
			pBuf = new char[pFirstHeader->m_uUncomprSize];
			xSave.ReadFile(pBuf, pFirstHeader->m_uUncomprSize);
			g_xBuffer.Reset();
			g_xBuffer.Write(pBuf, pFirstHeader->m_uUncomprSize);
			SAFE_DELETE_ARRAY(pBuf);
			xSave.CloseFile();

			g_xBuffer >> m_stHero[0];
			++nValidHum;
		}

		//	Get second
		zIndex = xSave.FindFile("head1");
		if(zIndex != ZIP_FILE_INDEX_NOT_FOUND)
		{
			pFirstHeader = xSave.GetFileInfo(zIndex);
			if(NULL == pFirstHeader)
			{
				xSave.Close();
				return nValidHum;
			}

			xSave.OpenFile(zIndex);
			pBuf = new char[pFirstHeader->m_uUncomprSize];
			xSave.ReadFile(pBuf, pFirstHeader->m_uUncomprSize);
			g_xBuffer.Reset();
			g_xBuffer.Write(pBuf, pFirstHeader->m_uUncomprSize);
			SAFE_DELETE_ARRAY(pBuf);
			xSave.CloseFile();

			g_xBuffer >> m_stHero[1];
			++nValidHum;
		}

		//	Get third
		zIndex = xSave.FindFile("head2");
		if(zIndex != ZIP_FILE_INDEX_NOT_FOUND)
		{
			pFirstHeader = xSave.GetFileInfo(zIndex);
			if(NULL == pFirstHeader)
			{
				xSave.Close();
				return nValidHum;
			}

			xSave.OpenFile(zIndex);
			pBuf = new char[pFirstHeader->m_uUncomprSize];
			xSave.ReadFile(pBuf, pFirstHeader->m_uUncomprSize);
			g_xBuffer.Reset();
			g_xBuffer.Write(pBuf, pFirstHeader->m_uUncomprSize);
			SAFE_DELETE_ARRAY(pBuf);

			g_xBuffer >> m_stHero[2];
			++nValidHum;

			//	Get third
			xSave.CloseFile();
			xSave.Close();
		}
	}

	return nValidHum;
}

void LoginScene::RenderThanks()
{
	static const char* s_pszThanks[] = {
		"XXX ���� XXXԪ",
		"YYY ���� XXXԪ",
		"YYY ���� XXXԪ",
		"YYY ���� XXXԪ",
		"YYY ���� XXXԪ",
		"YYY ���� XXXԪ",
		"YYY ���� XXXԪ",
		"YYY ���� XXXԪ",
		"YYY ���� XXXԪ",
		"YYY ���� XXXԪ",
		"YYY ���� XXXԪ",
		"YYY ���� XXXԪ",
		"YYY ���� XXXԪ",
		"YYY ���� XXXԪ",
		"YYY ���� XXXԪ",
		"YYY ���� XXXԪ",
		"YYY ���� XXXԪ",
		"YYY ���� XXXԪ"
	};

	static float fTopRenderPosY = VIEW_HEIGHT + VIEW_OPERATE;
	const int nLiftSpeed = 40;
	float fDelta = AfxGetHge()->Timer_GetDelta();

	HTEXTURE tex = m_pxResMgr->GetTexture("login");
	if(tex)
	{
		/*if(!m_pRender)
		{
			m_pRender = new hgeSprite(tex, 0, 0, 816, 540);
		}
		else
		{
			m_pRender->SetTexture(tex);
			m_pRender->SetTextureRect(0, 0, 816, 540);
		}
		m_pRender->Render(0, 0);*/
		RenderBk();

		fTopRenderPosY -= nLiftSpeed * fDelta;

		for(int i = 0; i < sizeof(s_pszThanks) / sizeof(s_pszThanks[0]); ++i)
		{
			if(i == (sizeof(s_pszThanks) / sizeof(s_pszThanks[0]) - 1))
			{
				if(fTopRenderPosY + i * (12 + 5) <= -12)
				{
					fTopRenderPosY = VIEW_HEIGHT + VIEW_OPERATE;
					break;
				}
			}
			if(fTopRenderPosY + i * (12 + 5) < -12)
			{
				continue;
			}
			if(fTopRenderPosY + i * (12 + 5) + 12 > VIEW_HEIGHT + VIEW_OPERATE)
			{
				break;
			}

			AfxGetPrinter()->SetColor(ARGB_WHITE);
			int nDrawPos = VIEW_WIDTH / 2 - GetTextWidth(s_pszThanks[i], 12) / 2;
			AfxGetPrinter()->Print(nDrawPos, fTopRenderPosY + i * (12 + 5), s_pszThanks[i]);
		}
	}
}

bool LoginScene::CommandThanks(const POINT& _ptMouse)
{
	if(AfxGetHge()->Input_KeyUp(HGEK_LBUTTON))
	{
		m_wCurPage = PAGE_LOGIN;
	}

	return true;
}

bool LoginScene::CommandPreface(const POINT& _ptMouse)
{
	if(AfxGetHge()->Input_KeyUp(HGEK_LBUTTON))
	{
		m_wCurPage = PAGE_LOGIN;
		GameSoundManager::GetInstancePtr()->PlayBkSound(3);
	}
	return true;
}

void LoginScene::RenderPreface()
{
	static float fTime = 0.0f;
	static BYTE bAlpha = 0;
	static bool bRearchTop = false;

	if(bAlpha == 1)
	{
		m_wCurPage = PAGE_LOGIN;
		GameSoundManager::GetInstancePtr()->PlayBkSound(3);
	}

	HTEXTURE tex = m_pxResMgr->GetTexture("logo");
	if(tex)
	{
		m_pRender->SetTexture(tex);
		m_pRender->SetTextureRect(0, 0, 542, 291);

		fTime += AfxGetHge()->Timer_GetDelta();

#define ALPHA_SPEED		5

		if(fTime > 0.05f)
		{
			fTime = 0.0f;
			if(!bRearchTop)
			{
				if(bAlpha + ALPHA_SPEED > 0xFF)
				{
					bRearchTop = true;
					bAlpha = 0xFF;
				}
				else
				{
					bAlpha += ALPHA_SPEED;
				}
			}
			else
			{
				if(bAlpha < ALPHA_SPEED)
				{
					bAlpha = 1;
				}
				else
				{
					bAlpha -= ALPHA_SPEED;
				}
			}
		}

		m_pRender->SetColor(ARGB(bAlpha, 255, 255, 255));
		m_pRender->Render(VIEW_WIDTH / 2 - 542 / 2,
			(VIEW_HEIGHT + VIEW_OPERATE) / 2 - 290 / 2);
		m_pRender->SetColor(ARGB_WHITE);
	}
}