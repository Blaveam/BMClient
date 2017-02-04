#ifndef GAMESCENE_H_
#define GAMESCENE_H_

#include "../GameScene/GamePlayer.h"
#include "../GameScene/GameTextureManager.h"
#include "../GameScene/GameMapManager.h"
#include <hge.h>
#include "../Common/GfxFont.h"
#include "../GameDialog/GameDlgBase.h"
#include "../GameScene/GameMonster.h"
#include "../GameDialog/GameMiniMapDlg.h"
#include "../GameDialog/GameStatusDlg.h"
#include "../GameScene/MagicEffect.h"
#include "../GameDialog/GameFloatDlg.h"
#include <assert.h>

class GameMonster;
class GameInfoBoardDlg;
struct PacketBase;
typedef std::list<MagicElement*> MAGICLIST;

#define MAPPED_OBJECT_X		(VIEW_WIDTH / UNIT_WIDTH + 2)
#define MAPPED_OBJECT_Y		(VIEW_HEIGHT / UNIT_HEIGHT + 2)

#define CHAT_BTNS			4
#define CHAT_RECORD_NUMBER	15

//	GameScene Ϊ��Ϸ���� ������Ϸ��Դ ��Ⱦ �����û����� ���²���
class GameScene : public RenderTarget
{
public:
	GameScene();
	~GameScene();

public:
	bool Init(HGE* _hge);
	bool SwitchScene(DWORD _id);
	bool SwitchScene(const char* lpszmapname);
	virtual bool LoadProperty(int _id);
	//	�������ĵ��֮���
	void ProcKeyCmd();
	void ProcKeyCmd_ChatRecord();
	void ProcMagicCmd(GameObject* _obj);

	bool ProcItemClick(const POINT& _ptMouse);
	bool ProcChatClick(const POINT& _ptMouse);
	GameObject* FindGameMonsterByArea(float _mx, float _my);
	inline GameMainOptUI* GetMainOpt()
	{
		return m_pMainOpt;
	}

	bool LoadMagicCost();

	inline int GetMouseCoordX()					{return m_nMouseCoordX;}
	inline int GetMouseCoordY()					{return m_nMouseCoordY;}

	void BringDlgToTop(RenderObject* _obj);
	void UpdateMapData(float _dt);
	void UpdateMouseCoord();
	bool CleanMapDataCell(int _x, int _y, GameObject* _pObj);

	//	����λ��
	inline const MemMapObject* GetMemMapObject(int _x, int _y)
	{
		if(_x < 0 ||
			_x >= MAPPED_OBJECT_X ||
			_y < 0 ||
			_y >= MAPPED_OBJECT_Y)
		{
			return NULL;
		}
		return &m_stMapObjects[_x][_y];
	}

	bool CreateMonster(int _x, int _y, OBJECT_TYPE _type, int _id);
	bool InsertNewObject(const PkgNewPlayerNot& not);
	bool InsertNewObject(const PkgNewNPCNot& not);
	bool InsertNewGroundItem(const GroundItem* _pItem);
	bool InsertNewMagic(MagicElement* _pMgc);
	bool InsertNewAttackNumber(int _nNumber, bool _bAdd, GameObject* _pObj, bool _bCritical=false);
	bool RemoveGroundItem(DWORD _dwID);
	bool RemoveNPC(DWORD _uHandleID);

	GameMonster* NewMonsterByID(int _nID);

	GameObject* GetMonster(int _x, int _y);
	//	���λ��
	const MemMapObject* GetMappedObjectsRelative(int _x, int _y);
	inline GameObject* GetTarget()
	{
		return m_pTarget;
	}
	bool SendChatMessage();
	void ShowGmNotification(const std::string& _xMsg);
	void ShowSystemNotify(const std::string& _xMsg, DWORD dwColor, DWORD _dwTimes);
	static int __stdcall OnEnterPressed(void* _pEdit);
	static int __stdcall OnLoseFocus(void* _pEdit);

	inline PLAYERLIST& GetNPCList()
	{
		return m_lstPlayers;
	}
	GameObject* GetPlayerByName(const char* _pszName);

	inline GROUNDITEMS& GetGroundItems()
	{
		return m_lstGroundItems;
	}

	inline DWORD GetNetDelayMillSeconds()
	{
		return m_dwNetDelayMillSeconds;
	}
	inline void SetNetDelayMillSeconds(DWORD _dwDelay)
	{
		m_dwNetDelayMillSeconds = _dwDelay;
	}
	inline DWORD GetLastSendSequence()
	{
		return m_dwLastSendSequence;
	}
	inline void SetNetDelayReceived()
	{
		m_bReceiveReply = true;
	}
	void OnReceiveNetDelayReply(const PkgPlayerNetDelayAck& ack);

	void PlayTransAnimation(int _nId, int _nX, int _nY);

public:
	void OnMessage(PacketBase* _pPacket);
	void RequestCloseGame();
	void RenderMagic();
	void RenderAttackNumber();
	void RemoveMagicFireAndTarget(GameObject* _pObj);

public:
	void MousePtToScreenPt(const POINT& _src, POINT& _dest);
	inline GameMiniMapDlg* GetMiniMapDlg()
	{
		return m_pMiniMap;
	}
	inline GameStatusDlg* GetStatusDlg()
	{
		return m_pStatusDlg;
	}
	inline GameFloatDlg* GetFloatDlg()
	{
		return m_pFloatDlg;
	}

private:
	/*
	 *	TEST
	 */
#ifdef _DEBUG
	void InsertBoardMsg();
#endif

private:
	bool IsInClient();
	void ProcGameCmd(const POINT& _mp);

private:
	int ProcExcept(int nExpCode);

public:
	virtual bool ProcUserCmd(const POINT& _mp);
	virtual void Render();
	virtual void Update(float _dt);

private:
	//	��Ϸ���
	GamePlayer* m_pPlayer;
	//	����������
	GameMainOptUI* m_pMainOpt;
	//	С��ͼ
	GameMiniMapDlg* m_pMiniMap;
	//	��Ϣ�������
	GameInfoBoardDlg* m_pMsgBoard;
	//	������Ϣ����
	GameFloatDlg* m_pFloatDlg;

	//	��Ϸ�����б�
	PLAYERLIST m_lstPlayers;
	//	��Ϸ������Ʒ�б�
	GROUNDITEMS m_lstGroundItems;
	//	����װ��ӳ���
	MemMapObject m_stMapObjects[MAPPED_OBJECT_X][MAPPED_OBJECT_Y];
	//	��Ϸħ���б�
	MAGICLIST m_xMagics;
	//	ѡ�й���
	GameObject* m_pTarget;
	//	���������б�
	MAGICLIST m_xAttackNumber;
	//	���״̬����
	GameStatusDlg* m_pStatusDlg;


	//	�����ӳ�
	//	�ϴη��͵����к�
	DWORD m_dwLastSendSequence;
	//	�����ӳ�ʱ��
	DWORD m_dwNetDelayMillSeconds;
	//	�ϴη��͵�ʱ��
	DWORD m_dwDelaySendTime;
	//	�Ƿ��յ��˻�Ӧ
	bool m_bReceiveReply;

	//	GM������ʾ
	std::string m_xGmMsg;
	DWORD m_dwLastShowGmMsgTime;
	RECT m_rcGmMsg;

	//	����ļ��������¼
	StringList m_xRecentChat;
	StringList::iterator m_xRecentChatIterator;

	bool m_bInit;

	bool m_bShowDonate;

private:
	static HGE* s_hge;
	char m_szCurMapName[15];

	DWORD m_dwLastDetectTime;

public:
	static GameScene* sThis;
	//	ħ�����ı�
	DWORD m_dwMgcCostTable[MEFF_USERTOTAL];
	//	ħ���ӳٱ�
	DWORD m_dwMgcDelayTable[MEFF_USERTOTAL];

	int m_nMouseCoordX;
	int m_nMouseCoordY;
};


//////////////////////////////////////////////////////////////////////////
#endif