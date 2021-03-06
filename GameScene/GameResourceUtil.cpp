#include "GameResourceUtil.h"
#include "../Common/GlobalFunction.h"

#ifdef _DEBUG
const char* g_szResFile[] =
{
	"Tiles.sib",
	"SmTiles.sib",
	"Hum.sib",
	"Hum2.sib",
	"Objects.sib",
	"Objects2.sib",
	"Objects3.sib",
	"Objects4.sib",
	"Objects5.sib",
	"Objects6.sib",
	"Objects7.sib",
	"Objects8.sib",
	"Objects9.sib",
	"Objects10.sib",
	"Objects11.sib",
	"Objects12.sib",
	"Objects13.sib",
	"Objects14.sib",
	"Hair.sib",
	"Hair2.sib",
	"Weapon.sib",
	"Weapon2.sib",
	"Weapon_Effect.sib",
	//"Prguse.wil",
	//"Prguse2.wil",
	"OpUI.sib",
	"Items.sib",
	"DnItems.sib",
	"Mon1.sib",
	"Mon2.sib",	//	just for evil dragon
	"Mon3.sib",
	"Mon4.sib",
	"Mon5.sib",
	"Mon6.sib",
	"Mon7.sib",
	"Mon8.sib",
	"Mon9.sib",
	"Mon10.sib",
	"Mon11.sib",
	"Mon12.sib",
	"Mon13.sib",
	"Mon14.sib",
	"Mon15.sib",
	"Mon16.sib",
	"Mon17.sib",
	"Mon18.sib",
	"Mon19.sib",
	"Mon20.sib",
	"Mon21.sib",
	"Mon22.sib",
	"Mon23.sib",
	"Mon24.sib",
	"Mon25.sib",
	"Mon26.sib",
	"Mon27.sib",
	"Mon28.sib",
	"Mon29.sib",
	"Mon30.sib",
	"Mon31.sib",
	"Mon32.sib",
	"Mon33.sib",
	"mmap.sib",
	"bmap.sib",
	"Magic.sib",
	"Magic2.sib",
	"Magic3.sib",
	"Magic4.sib",
	"Magic5.sib",
	"Magic6.sib",
	"Magic7.sib",
	"Magic8.sib",
	"Magic9.sib",
	"Magic10.sib",
	"npc.sib",
	"StateItem.sib",
	"HumEffect.sib",
	"HumEffect2.sib",
	"MagicIcon.sib",
	"Status.sib",
	"Effect.sib",
	"Custom.spb",
	"Ani.sib"
	//"ChrSel.wil"
};
#else
const char* g_szResFile[] =
{
	"Tiles.sib",
	"SmTiles.sib",
	"Hum.sib",
	"Hum2.sib",
	"Objects.sib",
	"Objects2.sib",
	"Objects3.sib",
	"Objects4.sib",
	"Objects5.sib",
	"Objects6.sib",
	"Objects7.sib",
	"Objects8.sib",
	"Objects9.sib",
	"Objects10.sib",
	"Objects11.sib",
	"Objects12.sib",
	"Objects13.sib",
	"Objects14.sib",
	"Hair.sib",
	"Hair2.sib",
	"Weapon.sib",
	"Weapon2.sib",
	"Weapon_Effect.sib",
	//"Prguse.wil",
	//"Prguse2.wil",
	"OpUI.sib",
	"Items.sib",
	"DnItems.sib",
	"Mon1.sib",
	"Mon2.sib",		//	just for evil dragon
	"Mon3.sib",
	"Mon4.sib",
	"Mon5.sib",
	"Mon6.sib",
	"Mon7.sib",
	"Mon8.sib",
	"Mon9.sib",
	"Mon10.sib",
	"Mon11.sib",
	"Mon12.sib",
	"Mon13.sib",
	"Mon14.sib",
	"Mon15.sib",
	"Mon16.sib",
	"Mon17.sib",
	"Mon18.sib",
	"Mon19.sib",
	"Mon20.sib",
	"Mon21.sib",
	"Mon22.sib",
	"Mon23.sib",
	"Mon24.sib",
	"Mon25.sib",
	"Mon26.sib",
	"Mon27.sib",
	"Mon28.sib",
	"Mon29.sib",
	"Mon30.sib",
	"Mon31.sib",
	"Mon32.sib",
	"Mon33.sib",
	"mmap.sib",
	"bmap.sib",
	"Magic.sib",
	"Magic2.sib",
	"Magic3.sib",
	"Magic4.sib",
	"Magic5.sib",
	"Magic6.sib",
	"Magic7.sib",
	"Magic8.sib",
	"Magic9.sib",
	"Magic10.sib",
	"npc.sib",
	"StateItem.sib",
	"HumEffect.sib",
	"HumEffect2.sib",
	"MagicIcon.sib",
	"Status.sib",
	"Effect.sib",
	"Custom.spb",
	"Ani.sib"
	//"ChrSel.wil"
};
#endif

const char* GetGameResourceDir() {
	// 1. Check the RES_DATA_DIR environment variables
	// 2. Use GetRootPath as default path
	const char *pszResDataDir = getenv("RES_DATA_DIR");
	if (nullptr == pszResDataDir) {
		pszResDataDir = GetRootPath();
	}
	return pszResDataDir;
}
