#include "../GameMonster.h"
#include "../../GameScene/GameScene.h"
//////////////////////////////////////////////////////////////////////////
IceSoldierMonster::IceSoldierMonster()
{
	m_fLastUpdateSkill = 0.0f;
}

void IceSoldierMonster::Render()
{
	if(GetStatus() == PST_EXT1)
	{
		HGE* hge = AfxGetHge();
		GamePlayer* pPlayer = GamePlayer::GetInstance();

		if(!IsVisible())
		{
			return;
		}

		if(m_texs)
		{
			GameTextureManager* res = m_texs;
			//	������
			int nTextIndex = 340 + GetDirection() * 10 + m_bCurFrame;
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

				if(!m_pRender)
				{
					//m_pSpr = new hgeSprite(tex, 0, 0, res->GetTextureWidth(tex), res->GetTextureHeight(tex));
					m_pRender = new hgeSprite(tex, 0, 0, res->GetTextureWidth(nTextIndex),
						res->GetTextureHeight(nTextIndex));
				}
				else
				{
					m_pRender->SetTexture(tex);
					m_pRender->SetTextureRect(0, 0, res->GetTextureWidth(nTextIndex), 
						res->GetTextureHeight(nTextIndex));
				}
				sOffsetX = res->GetTextureOffsetX(nTextIndex);
				sOffsetY = res->GetTextureOffsetY(nTextIndex);
				//	�������λ�� ����������������ε�λ��
				nDrawOffsetX = CENTRAL_X + (int)m_fPosx - (int)GamePlayer::GetInstance()->GetObjx();
				nDrawOffsetY = CENTRAL_Y + (int)m_fPosy - (int)GamePlayer::GetInstance()->GetObjy();
				m_pRender->Render(nDrawOffsetX + sOffsetX,
					nDrawOffsetY + sOffsetY);
				//	��¼��ǰ����
				m_nCurrentTextureIndex = nTextIndex;
			}
		}
	}
	else
	{
		__super::Render();
	}
}

void IceSoldierMonster::Update(float _dt)
{
	if(GetStatus() == PST_EXT1)
	{
		if(GetStatus() == PST_EXT1 &&
			!IsLastFrame())
		{
			m_fLastUpdateSkill += _dt;
			if(m_fLastUpdateSkill > /*0.1f*/0.1f)
			{
				SetCurFrame(GetCurFrame() + 1);
				m_fLastUpdateSkill = 0.0f;
			}
		}
		else if(GetStatus() == PST_EXT1 &&
			IsLastFrame())
		{
			m_fLastUpdateSkill += _dt;
			if(m_fLastUpdateSkill > /*0.2f*/0.1f)
			{
				SetStatus(PST_STAND);
				MagicElement* pAni = new MagicMonsPosAni;
				if(pAni->Create(MAKELONG(GetObjx(), GetObjy()), this, NULL, 420 + GetDirection() * 10, 10, 80))
				{
					pAni->SetActive();
					GameScene::sThis->InsertNewMagic(pAni);
					GameSoundManager::GetInstancePtr()->PlayObjectSoundDirect(GetSoundIndex(), 6);
				}
				else
				{
					SAFE_DELETE(pAni);
				}
			}
		}
	}
	else
	{
		__super::Update(_dt);
	}
}

bool IceSoldierMonster::IsLastFrame()
{
	if(GetStatus() == PST_EXT1)
	{
		return (GetCurFrame() == (10 - 1));
	}
	else
	{
		return __super::IsLastFrame();
	}
}

/*void IceSoldierMonster::OnPacket(const PacketHeader* _pPkt)
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

void IceSoldierMonster::DoPacket(const PkgObjectActionNot& _not)
{
	if(_not.uAction == ACTION_EXT1)
	{
		m_fLastUpdateSkill = 0.0f;
		SetStatus(PST_EXT1);
		SetReallyCoord(LOWORD(_not.uParam0), HIWORD(_not.uParam0));
		int nDrt = HIWORD(_not.uParam1);
		if(nDrt >= 0 &&
			nDrt < 8)
		{
			SetDirection((PLAYER_DIRECTION)nDrt);
		}
	}

	__super::DoPacket(_not);
}*/
void IceSoldierMonster::OnObjectAction(const PkgObjectActionNot& _not)
{
	if(_not.uAction == ACTION_EXT1)
	{
		m_fLastUpdateSkill = 0.0f;
		SetStatus(PST_EXT1);
		SetReallyCoord(LOWORD(_not.uParam0), HIWORD(_not.uParam0));
		int nDrt = HIWORD(_not.uParam1);
		if(nDrt >= 0 &&
			nDrt < 8)
		{
			SetDirection((PLAYER_DIRECTION)nDrt);
		}
	}

	__super::OnObjectAction(_not);
}