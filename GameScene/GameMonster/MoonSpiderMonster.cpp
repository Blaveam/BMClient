#include "../GameMonster.h"
#include "../../GameScene/GameScene.h"
//////////////////////////////////////////////////////////////////////////
MoonSpiderMonster::MoonSpiderMonster()
{
	m_fLastUpdateSpeAtk1 = 0.0f;
}

void MoonSpiderMonster::OnObjectAction(const PkgObjectActionNot& not)
{
	if(not.uAction == ACTION_EXT1)
	{
		m_fLastUpdateSpeAtk1 = 0.0f;
		SetStatus(PST_EXT1);
		SetReallyCoord(LOWORD(not.uParam0), HIWORD(not.uParam0));
		int nDrt = HIWORD(not.uParam1);
		if(nDrt >= 0 &&
			nDrt < 8)
		{
			SetDirection((PLAYER_DIRECTION)nDrt);
		}
		GameSoundManager::GetInstancePtr()->PlayObjectSoundDirect(GetSoundIndex(), 6);
	}

	__super::OnObjectAction(not);
}

void MoonSpiderMonster::Update(float _dt)
{
	if(GetStatus() == PST_EXT1)
	{
		if(GetStatus() == PST_EXT1 &&
			!IsLastFrame())
		{
			m_fLastUpdateSpeAtk1 += _dt;
			if(m_fLastUpdateSpeAtk1 > /*0.1f*/0.1f)
			{
				SetCurFrame(GetCurFrame() + 1);
				m_fLastUpdateSpeAtk1 = 0.0f;
			}
		}
		else if(GetStatus() == PST_EXT1 &&
			IsLastFrame())
		{
			m_fLastUpdateSpeAtk1 += _dt;
			if(m_fLastUpdateSpeAtk1 > /*0.2f*/0.1f)
			{
				SetStatus(PST_STAND);
				m_fLastUpdateSpeAtk1 = 0.0f;

				//	magic
				MagicElement* pAni = new MagicMonsPosAni;
				if(pAni->Create(MAKELONG(GetObjx(), GetObjy()), this, NULL, 2234 + 10 * GetDirection(), 6, 50))
				{
					pAni->SetActive();
					GameScene::sThis->InsertNewMagic(pAni);
					GameSoundManager::GetInstancePtr()->PlayObjectSoundDirect(GetSoundIndex(), 7);
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

bool MoonSpiderMonster::IsLastFrame()
{
	if(GetStatus() == PST_EXT1)
	{
		return (5 == GetCurFrame());
	}

	return __super::IsLastFrame();
}

void MoonSpiderMonster::Render()
{
	if(GetStatus() == PST_EXT1)
	{
		GamePlayer* pPlayer = GamePlayer::GetInstance();

		int nTexIndex = 2150 + GetDirection() * 10 + GetCurFrame();
		HTEXTURE tex = m_texs->GetTexture(nTexIndex);

		if(tex)
		{
			if(GameScene::sThis->GetTarget() == this)
			{
				HTEXTURE texSelected = SelectedTextureManager::GetInstance()->GetSelectedTextureMonster(tex, GetAttrib()->id, nTexIndex);
				if(texSelected != NULL)
				{
					tex = texSelected;
				}
			}

			short sOftX = m_texs->GetTextureOffsetX(nTexIndex);
			short sOftY = m_texs->GetTextureOffsetY(nTexIndex);

			m_pRender->SetTexture(tex);
			m_pRender->SetTextureRect(0, 0,
				m_texs->GetTextureWidth(nTexIndex),
				m_texs->GetTextureHeight(nTexIndex));

			int nDrawOffsetX = GetScreenPosX();
			int nDrawOffsetY = GetScreenPosY();

			m_pRender->Render(nDrawOffsetX + sOftX, nDrawOffsetY + sOftY);
		}
	}
	else
	{
		__super::Render();
	}

	if(GetRenderMode() == ORM_TRANSPARENT &&
		GetStatus() == PST_EXT1)
	{
		if(GetCurFrame() >= 2)
		{
			int nTexIndex = 2230 + GetDirection() * 10 + (GetCurFrame() - 2);

			//	effect
			GamePlayer* pPlayer = GamePlayer::GetInstance();
			HTEXTURE tex = m_texs->GetTexture(nTexIndex);

			if(tex)
			{
				short sOftX = m_texs->GetTextureOffsetX(nTexIndex);
				short sOftY = m_texs->GetTextureOffsetY(nTexIndex);

				MagicElement::pEffectRender->SetTexture(tex);
				MagicElement::pEffectRender->SetTextureRect(0, 0,
					m_texs->GetTextureWidth(nTexIndex),
					m_texs->GetTextureHeight(nTexIndex));

				int nDrawOffsetX = GetScreenPosX();
				int nDrawOffsetY = GetScreenPosY();

				MagicElement::pEffectRender->Render(nDrawOffsetX + sOftX, nDrawOffsetY + sOftY);
			}
		}
	}
}