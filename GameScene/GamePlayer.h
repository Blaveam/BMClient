#ifndef _GAMEPLAYER_H_
#define _GAMEPLAYER_H_
#include "../Common/GameRenderObject.h"
#include "../Common/PlayerBag.h"
//#include "../GameScene/GameMagicManager.h"
#include "../Common/GlobalDefine.h"
#include "../Common/PlayerStatusControl.h"
#include "../Net/GamePacketHandler.h"
#include "GameOtherPlayer.h"
#include "../../CommonModule/ShareData.h"
#include "../../CommonModule/QuestContext.h"
#include "../../CommonModule/RandGenerator.h"
//////////////////////////////////////////////////////////////////////////
#define WAITSERVERRSP_NONE			0
#define WAITSERVERRSP_NORMALATK		1
//////////////////////////////////////////////////////////////////////////

class GameMonster;

typedef std::map<DWORD, DWORD> MGCHISTORYMAP;

class GamePlayer : public GameOtherPlayer/*, public MyHeroPacketHandler*/
{
	//SINGLETON_DECLARE(GamePlayer);
public:
	static GamePlayer* GetInstance()
	{
		static GamePlayer* pIns = NULL;
		if(!pIns)
		{
			pIns = new GamePlayer;
		}
		return pIns;
	}
protected:
	GamePlayer();
public:
	virtual ~GamePlayer();

public:
	virtual bool LoadProperty(int _id);
	virtual void Render();
	virtual int OnLButtonDown(){return 0;}
	virtual int OnRButtonDown(){return 0;}
	virtual void Update(float _dt);
	virtual bool ProcUserCmd(const POINT& _mp);
	virtual int GetSkillLevel(DWORD _dwMgcID);

public:
	bool Save();
	void Reset();

public:
	//	��Ѫ��
	void RenderHP();
	bool Run(PLAYER_DIRECTION _pd);
	bool Walk(PLAYER_DIRECTION _pd);
	bool AttackMonster(GameObject* _mons);
	bool NPCClick(GameObject* _pObj);
	bool OtherPlayerClick(GameObject* _pObj);
	bool HandleMagicInput(const POINT& _mp);
	bool HandleNoneSpellMagic(const POINT& _mp);
	bool HandleSpellMagic(const POINT& _mp);

public:
// 	int CalTextureIndex(int* _ptexsum = NULL);
// 	int CalHairTexIndex(int* _ptexsum = NULL);
// 	int CalWeaponTexIndex(int* _ptexsum = NULL);
	int GetScreenOffsetX();
	int GetScreenOffsetY();

	PlayerEquipAttrib* GetEquip()
	{
		return &m_equipAttrib;
	}
	PlayerBag* GetPlayerBag()
	{
		return &m_bag;
	}
	inline void Unlock()
	{
		m_bIsLocked = false;
		m_dwLockType = PST_NONE;
	}
	inline bool IsUsingPreLock()
	{
		return m_bUsingPreLock;
	}
	inline void Lock(DWORD _dwType)
	{
		m_dwLockType = _dwType;
		m_bIsLocked = true;
		m_dwLastLockedTime = GetTickCount();
	}
	inline DWORD GetLockType()
	{
		return m_dwLockType;
	}
	inline bool IsLocked()
	{
		return m_bIsLocked;
	}
	//	��ȡְҵ�ַ���
	const char* GetHref()
	{
		return "";
	}
	int GetMapID()
	{
		return m_nMapID;
	}
	void SetMapID(int _map)
	{
		m_nMapID = _map;
	}
	inline void SetWalkLocked()
	{
		m_bIsWalkLocked = true;
		m_dwLastLockedWalkTime = GetTickCount();
	}
	int GetMagicCost(const MagicDetail* _pDetail);
	int GetMagicDelay(const MagicDetail* _pDetail);
	inline int GetWanLi()
	{
		if(m_attrib.level > MAX_LEVEL)
		{
			return 0;
		}
		if(GetHeroJob() < 0 ||
			GetHeroJob() > 2)
		{
			return 0;
		}
		return g_nWanLiTable[m_attrib.level - 1][GetHeroJob()];
	}
	inline int GetDonateLeft()			{return m_nDonateLeft;}
	inline void SetDonateLeft(int _nLeft){m_nDonateLeft = _nLeft;}

	inline bool IsRequestSmallQuit()	{return m_bRequestSmallQuit;}
	inline void SetRequestSmallQuit(bool _b)
	{
		m_bRequestSmallQuit = _b;
	}

	bool CanAttack();

public:
	void SetPlayerItem(PLAYER_ITEM_TYPE _type, const ItemAttrib* _item);
	bool CanEquip(const ItemAttrib* _item, DWORD* _err = NULL);
	bool DressEquip(int _bagidx, DWORD* _err = NULL);
	bool DressEquip(int _bagidx, PLAYER_ITEM_TYPE _type, DWORD* _err = NULL);
	bool UnDressEquip(PLAYER_ITEM_TYPE _type);

	PLAYER_ITEM_TYPE Item2PlayerItem(ItemAttrib* _item);
	BYTE PlayerItem2Item(PLAYER_ITEM_TYPE _type);

	bool MoveBagItemToAssistItem(int _bagidx, int _astidx);
	bool IsCostItem(const ItemAttrib& _item);
	bool IsEquipItem(const ItemAttrib& _item);
	void UpdateAccelerate(int _idx = -1);

	bool UseItemFromBag(int _bagidx);
	bool UseItemFromAssist(int _astidx);
	bool UseItem(ItemAttrib* _item);

	int GetAssistItemsEmptySum();
	int GetBagItemsEmptySum();

	void WriteBagItemOrder();
	void ReadBagItemOrder();

public:
	//	true : ��û���� false : ������
	bool AddHP(int _hp, int _way = ADD_NORMAL, int _step = 10);
	bool AddMP(int _mp, int _way = ADD_NORMAL, int _step = 10);
	void DecHP(int _hp);
	void DecMP(int _mp);
	void AddExp(int _exp);
	void RandomPos();

private:
	void LevelUp();
	bool LoadSaveData_SaveFile(const char* _pszFile);
	bool LoadSaveData_ZipArchive(const char* _pszFile);

public:
	//	ħ�����

public:
	//	����װ�������������ֵ
	void UpdatePlayerAttrib();
	//	������װ������Ϣ
	void UpdateSuitAttrib();
	//	������ͬƷ�ʵļӳ���Ϣ
	void UpdateSuitSameLevel();
	//void UpdatePlayerTexIndex();
	//	�Ƿ������һ֡
	bool IsLastFrame()
	{
		int nTotal = 0;
		CalTextureIndex(&nTotal);
		return nTotal - 1 == GetCurFrame();
	}
public:
	void SetAtkMonster(GameObject* _mons)
	{
		m_atkTarget = _mons;
	}
	void SetMgcTarget(GameObject* _pMons)
	{
		m_pMgcTarget = _pMons;
	}
	const GameObject* GetAtkMonster() const
	{
		return m_atkTarget;
	}
	GameObject* GetMgcTarget() const
	{
		return m_pMgcTarget;
	}

public:
	//GameMagicManager m_mgcs;
	int GetMagicCount();
	bool AddToMagicTree(DWORD _dwMgcID);
	bool IsMagicExist(DWORD _dwMgcID);
	bool UpgradeMagicLevel(DWORD _dwMgcID, char _c = 1);
	bool SetMagicLevel(DWORD _dwMgcID, char _level);
	bool SetMagicKey(DWORD _dwMgcID, char _cKey);
	bool SetMagicAccKey(DWORD _dwMgcID, char _cKey);
	const MagicDetail* GetMagicByKey(char _cKey);
	const MagicDetail* GetMagicByAccKey(char _cKey);
	inline const MagicDetail* GetMagicByIndex(int _index)
	{
		if(_index < 0 ||
			_index >= USER_MAGIC_NUM)
		{
			return NULL;
		}
		return &m_stMagicDetail[_index];
	}
	const MagicDetail* GetMagicByID(DWORD _dwMgcID);
	inline void WriteMagicKeyCfg(ByteBuffer* _pBuf)
	{
		std::map<char, MagicDetail*>::const_iterator begiter = m_xInputMap.begin();

		for(begiter; begiter != m_xInputMap.end();
			++begiter)
		{
			*_pBuf << begiter->first << begiter->second->wID;
		}
	}
	void WriteAccMagicKeyCfg();
	void LoadAccMagicKeyCfg();
	inline int GetAccMagicID()			{return m_nAccMagicID;}
	inline void SetAccMagicID(int _nID)	{m_nAccMagicID = _nID;}
	inline void SetSaveIndex(int _idx)
	{
		m_nSaveIndex = _idx;
	}
	inline int GetSaveIndex()
	{
		return m_nSaveIndex;
	}
	inline const char* GetSaveName()
	{
		return m_szSaveName;
	}
	inline void SetSaveName(const char* _pszSaveName)
	{
		strcpy(m_szSaveName, _pszSaveName);
	}
	inline CoolDownController* GetDrugCoolDownController()
	{
		return &m_xDrugCoolDown;
	}
	inline QuestContext* GetQuest()
	{
		return &m_xQuest;
	}
	inline void SetHeroNameInMask(const char* _pszName)
	{
		//	Copy to name mask buffer
		if(_pszName[0] == 0 ||
			strlen(_pszName) > 19)
		{
			return;
		}

		ZeroMemory(m_szNameCopy, sizeof(m_szNameCopy));

		for(int i = 0; i < 20/*strlen(_pszName)*/; ++i)
		{
			char cVar = _pszName[i];
			cVar ^= g_szNameMask;
			m_szNameCopy[i] = cVar;
		}
	}
	inline void GetHeroNameInMask(char* _pszNameBuf)
	{
		_pszNameBuf[0] = 0;

		/*if(m_szNameCopy[0] == 0 ||
			strlen(m_szNameCopy) > 19)
		{
			return;
		}*/

		for(int i = 0; i < 20/*strlen(m_szNameCopy)*/; ++i)
		{
			_pszNameBuf[i] = m_szNameCopy[i] ^ g_szNameMask;

			/*if(i == strlen(m_szNameCopy) - 1)
			{
				_pszNameBuf[i + 1] = 0;
			}*/
		}
	}
	inline void SetUID(UINT _uUID)				{m_uUID = _uUID;}
	inline UINT GetUID()						{return m_uUID;}

public:
	void SetRandSeedNormalAtk(DWORD _dwSeed)	{m_xNormalAtkRand.SetSeed(_dwSeed);}

	//	�ȴ�������ͬ������
	void SetWaitServerResponse(int _nWaitType, int _nTimeout);
	bool IsWaitServerResponse();
	void UpdateWaitServerResponse();
	inline int GetWaitServerResponseType()
	{
		return m_nWaitServerRspType;
	}

	void UpdateEnableSkill(int _nSkillType, bool _bEnabled);

private:
	void AutoAttackTarget();
	void PreNormalAttackTarget(GameObject* _pTarget);

private:
	//	���װ��
	PlayerEquipAttrib m_equipAttrib;
	hgeSprite* m_pSprHum;
	hgeSprite* m_pSprHair;
	hgeSprite* m_pSprWeapon;
	hgeSprite* m_pSprState;
	hgeSprite* m_pSprMgc;
	//	��ұ���
	PlayerBag m_bag;
	//	���װ��
	//ItemAttrib m_equip[PLAYER_ITEM_TOTAL];
	//	������Ϣ
	PlayerTexIndexTable m_indexs;
	//	�������
	GameObject* m_atkTarget;
	//	��ҵ�MapID
	int m_nMapID;
	//	���Ѫ��ħ������
	//PlayerStatusControl m_statusControl;
	MagicDetail m_stMagicDetail[USER_MAGIC_NUM];
	std::map<char, MagicDetail*> m_xInputMap;
	std::map<char, MagicDetail*> m_xAccInputMap;

	bool m_bIsLocked;
	DWORD m_dwLockType;
	DWORD m_dwLastLockedTime;
	bool m_bIsWalkLocked;
	DWORD m_dwLastLockedWalkTime;
	//	ħ������
	GameObject* m_pMgcTarget;
	DWORD m_dwLastUseMagicTime;
	//	��Ҵ浵����
	int m_nSaveIndex;
	//	��Ҵ浵����
	char m_szSaveName[20];
	//	ħ��ʹ����ʷ ����ʵ��ħ����ʱ ��ȴЧ��
	MGCHISTORYMAP m_xMgcHistory;

	//	ҩƷ��ȴ
	CoolDownController m_xDrugCoolDown;
	//	��������
	QuestContext m_xQuest;

	//	�������ֱ���
	char m_szNameCopy[20];

	UINT m_uUID;

	//	��ǰ���ħ��ID
	int m_nAccMagicID;

	//	�Ƿ�ʹ��Ԥ����
	bool m_bUsingPreLock;

	//	����������� �������ͬ��
	RandGenerator m_xNormalAtkRand;
	//	Ԥ���жϹ���Ч���Ĺ�������
	DWORD m_dwPreCalcNormalAttackEffMask;
	bool m_bEnableLieHuo;
	bool m_bEnableSuperLieHuo;
	bool m_bEnableBanYue;
	bool m_bEnableCiSha;
	bool m_bEnableKtSword;

	//	������ͬ������
	int m_nWaitServerRspType;
	DWORD m_dwWaitServerRspTimeoutTime;

	//	���ױ�
	int m_nDonateLeft;

	//	С����
	bool m_bRequestSmallQuit;
};

//////////////////////////////////////////////////////////////////////////
#endif