#include "../GameMonster.h"
#include "../../GameScene/GameScene.h"
//////////////////////////////////////////////////////////////////////////
BlueGhastMonster::BlueGhastMonster()
{

}

/*void BlueGhastMonster::OnPacket(const PacketHeader* _pPkt)
{
	g_xBuffer.Reset();
	unsigned int uWriteLen = g_xBuffer.Write(_pPkt, _pPkt->uLen);
	if(uWriteLen != _pPkt->uLen)
	{
		ALERT_MSGBOX("д��ȫ��Bufferʧ��");
		::PostQuitMessage(0);
		return;
	}

	BEGIN_HANDLE_PACKET(_pPkt)
		HANDLE_PACKET(PKG_GAME_OBJECT_ACTION_NOT,		PkgObjectActionNot)
		END_HANDLE_PACKET(_pPkt)

		__super::OnPacket(_pPkt);
}

void BlueGhastMonster::DoPacket(const PkgObjectActionNot& _not)
{
	__super::DoPacket(_not);

	if(_not.uAction == ACTION_DEAD)
	{
		GameSoundManager::GetInstancePtr()->PlayObjectSoundDirect(GetSoundIndex(), 6);

		//	play magic
		MagicElement* pAni = new MagicMonsPosAni;
		if(pAni->Create(MAKELONG(GetObjx(), GetObjy()), this, NULL, 1860, 8, 80))
		{
			pAni->SetActive();
			GameScene::sThis->InsertNewMagic(pAni);
		}
		else
		{
			SAFE_DELETE(pAni);
		}
	}
}*/
void BlueGhastMonster::OnObjectAction(const PkgObjectActionNot& _not)
{
	if(_not.uAction == ACTION_DEAD)
	{
		GameSoundManager::GetInstancePtr()->PlayObjectSoundDirect(GetSoundIndex(), 6);

		//	play magic
		MagicElement* pAni = new MagicMonsPosAni;
		if(pAni->Create(MAKELONG(GetObjx(), GetObjy()), this, NULL, 1860, 8, 80))
		{
			pAni->SetActive();
			GameScene::sThis->InsertNewMagic(pAni);
		}
		else
		{
			SAFE_DELETE(pAni);
		}
	}

	__super::OnObjectAction(_not);
}

void BlueGhastMonster::Render()
{
	__super::Render();

	if(GetRenderMode() == ORM_TRANSPARENT)
	{
		HGE* hge = AfxGetHge();
		GamePlayer* pPlayer = GamePlayer::GetInstance();

		if(!IsVisible())
		{
			return;
		}

		//if(GetStatus() != PST_DEAD)
		{
			if(m_texs)
			{
				GameTextureManager* res = m_texs;
				//	������
				int nTextIndex = CalMonIndex() + 340 + GetCurFrame();
				HTEXTURE tex = res->GetTexture(nTextIndex);
				short sOffsetX = 0;
				short sOffsetY = 0;
				int nDrawOffsetX = -1;
				int nDrawOffsetY = -1;
				if(tex)
				{
					if(GameScene::sThis->GetTarget() == this)
					{
						HTEXTURE texSelected = SelectedTextureManager::GetInstance()->GetSelectedTextureMonster(tex, GetAttrib()->id, nTextIndex);
						if(texSelected != NULL)
						{
							tex = texSelected;
						}
					}

					sOffsetX = res->GetTextureOffsetX(nTextIndex);
					sOffsetY = res->GetTextureOffsetY(nTextIndex);
					//	�������λ�� ����������������ε�λ��
					nDrawOffsetX = CENTRAL_X + (int)m_fPosx - (int)GamePlayer::GetInstance()->GetObjx();
					nDrawOffsetY = CENTRAL_Y + (int)m_fPosy - (int)GamePlayer::GetInstance()->GetObjy();
					MagicElement::pEffectRender->SetTexture(tex);
					MagicElement::pEffectRender->SetTextureRect(0,
						0,
						m_texs->GetTextureWidth(nTextIndex),
						m_texs->GetTextureHeight(nTextIndex));
					MagicElement::pEffectRender->Render(nDrawOffsetX + sOffsetX,
						nDrawOffsetY + sOffsetY);
				}
			}
		}
	}
}