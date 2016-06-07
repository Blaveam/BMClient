#ifndef GAMEINFOMANAGER_H_
#define GAMEINFOMANAGER_H_
//////////////////////////////////////////////////////////////////////////
#include "../../CommonModule/ObjectData.h"
#include <sqlite3.h>
#include <map>
//////////////////////////////////////////////////////////////////////////
#define ITEM_ID_MAX	2000
#define ITEM_DESC_TEXT_WIDTH	(12 * 12)
//////////////////////////////////////////////////////////////////////////
struct MagicInfo;
struct MagicRenderInfo;
struct EffectHumInfo;

struct ItemDesc
{
	const char* pszDesc;
	int nTextLength;
	int nTextWidth;
	int nTextLine;
};
//////////////////////////////////////////////////////////////////////////

class GameInfoManager
{
	//SINGLETON_DECLARE(GameInfoManager);
protected:
	GameInfoManager();
public:
	~GameInfoManager();

public:
	//	��Ʒ��Ϣ
	bool GetItemAttrib(int _id, ItemAttrib* _pitem);
	bool GetItemAttribINI(int _id, ItemAttrib* _pitem);
	bool GetItemAttribSQL(int _id, ItemAttrib* _pitem);
	//	����NPC������Ϣ
	bool GetMonsterRenderInfo(int _id, RenderInfo* _info);
	bool GetMonsterRenderInfo(int _id, const RenderInfo** _info);
	//	��������
	bool GetMonsterAttrib(int _id, ItemAttrib* _pitem);
	bool GetMonsterAttribINI(int _id, ItemAttrib* _pitem);
	bool GetMonsterAttribSQL(int _id, ItemAttrib* _pitem);
	//	���������Ʒ
	bool GetMonsterItems(int _id, int* _sum, ItemAttrib** _items);
	//	С��ͼ����
	bool GetMiniMapIndex(const char* lpMapName, int* _pvalue);
	//	ħ����Ϣ
	//bool GetMgcInfo(int _id, MagicInfo* _mgc);
	bool GetMagicRenderInfo(int _id, const MagicRenderInfo** _info);
	bool GetMagicRenderInfo(int _id, MagicRenderInfo* _info);
	//	����Ч����Ϣ
	bool GetEffectHumInfo(int _id, EffectHumInfo** _ppEffectHumInfo);
	bool GetEffectHumInfo(int _id, EffectHumInfo* _pEffectHumInfo);
	//	��Ʒ������Ϣ
	bool GetItemDesc(int _id, ItemDesc* _pDesc);
	bool GetItemDescINI(int _id, ItemDesc* _pDesc);
	bool GetItemDescSQL(int _id, ItemDesc* _pDesc);

	//	NPC�ű���Ϣ

	//	�̵���Ʒ��Ϣ
	bool GetShopSellItems(ItemList* _list, BYTE _type, int _minlevel, int _maxlevel);
	//	��Ʒ�۸���Ϣ
	int GetItemPrice(ItemAttrib* _item);
	//	ħ������
	bool InitMgcCostTable();

	void ReloadRenderInfo();
	void ReloadMagicRenderInfo();

public:
	bool CanUseSQL() const;

 public:
 	static GameInfoManager* GetInstance()
 	{
 		static GameInfoManager* pIns = NULL;
 		if(!pIns)
 		{
 			pIns = new GameInfoManager;
 		}
 		return pIns;
 	}

private:
	static int DBCallbackItemAttrib(void* _param, int _count, char** _value,char** _name);
	static int DBCallbackItemDesc(void* _param, int _count, char** _value,char** _name);
	static int DBCallbackShopItems(void* _param, int _count, char** _value,char** _name);
	static int DBCallbackMagicCost(void* _param, int _count, char** _value,char** _name);

private:
	sqlite3* m_sql;
	std::map<int, RenderInfo*> m_xMonsterRenderInfo;
	std::map<int, MagicRenderInfo*> m_xMagicRenderInfo;
	std::map<int, EffectHumInfo*> m_xEffectHumInfo;
	const ItemDesc* m_pItemDescCache[ITEM_ID_MAX];
};

#endif