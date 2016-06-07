#include "../Common/PlayerStatusControl.h"
#include "../GameScene/GamePlayer.h"

PlayerStatusControl::PlayerStatusControl()
{
	//m_pPlayer = GamePlayer::GetInstance();
	m_pPlayer = NULL;
	/*
	 *	Note ����������ѭ����
	 */
}

PlayerStatusControl::~PlayerStatusControl()
{

}


void PlayerStatusControl::AddStatusItem(BYTE _type, int _total, int _step)
{
	if(_total <= 0)
	{
		return;
	}

	StatusItem item;
	item.type = _type;
	item.total = _total;
	item.step = _step;
	item.btime = ::GetTickCount();

	m_list.push_back(item);
}

void PlayerStatusControl::Update()
{
	if(!m_pPlayer)
	{
		return;
	}

	if(m_list.empty())
	{
		return;
	}

	DWORD dwCurTime = ::GetTickCount();

	for(STATUSLIST::iterator iter = m_list.begin();
		iter != m_list.end();
		)
	{
		if(dwCurTime - iter->btime > UPDATE_INTERVAL)
		{
			//	���Ը�����
			if(iter->type == STATUS_HP)
			{
				if(iter->total > iter->step)
				{
					//	ʣ��ֵ����
					iter->total -= iter->step;
					if(m_pPlayer->AddHP(iter->step))
					{
						iter->btime = dwCurTime;
						++iter;
						//	����Ѫ�� ������Ӽ�
						return;
					}
					else
					{
						iter = m_list.erase(iter);
					}
				}
				else
				{
					//	ʣ�಻������
					m_pPlayer->AddHP(iter->total);
					iter = m_list.erase(iter);
					//	����Ѫ�� ������Ӽ�
					return;
				}
			}
			else if(iter->type == STATUS_MP)
			{
				if(iter->total > iter->step)
				{
					//	ʣ��ֵ����
					iter->total -= iter->step;
					if(m_pPlayer->AddMP(iter->step))
					{
						iter->btime = dwCurTime;
						++iter;
						//	����Ѫ�� ������Ӽ�
						return;
					}
					else
					{
						iter = m_list.erase(iter);
					}
				}
				else
				{
					//	ʣ�಻������
					m_pPlayer->AddMP(iter->total);
					iter = m_list.erase(iter);
					//	����Ѫ�� ������Ӽ�
					return;
				}
			}
			else
			{
				++iter;
			}
		}
		else
		{
			//	����ʱ��δ��
			++iter;
		}
		
	}
}