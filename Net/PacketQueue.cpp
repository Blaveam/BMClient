#include "../Net/PacketQueue.h"

/************************************************************************/
/* �������ݰ�
/************************************************************************/
void PacketQueue::PushData(const char *_pData, int _len)
{
	PacketBase* pPacket = NULL;
	const char* pReadData = _pData;

	if(m_dwBufPointer == 0)
	{
		//	�������ǿյ�
		if(_len < 4)
		{
			//	�޷��������ݰ�����
			return;
		}

		//	�ȶ������ݳ���
		DWORD dwSize = ntohl(*(DWORD*)pReadData);
		DWORD dwLeft = _len;
// 		pReadData += 4;
// 		dwLeft -= 4;

		while(dwLeft > 0)
		{
			if(dwSize <= dwLeft)
			{
				//	���ݹ���
				pPacket = (PacketBase*)malloc(dwSize);
				pPacket->dwSize = dwSize;
				memcpy(pPacket, pReadData, dwSize);
				m_xPackets.push_back(pPacket);
				dwLeft -= dwSize;
				pReadData += dwSize;
			}
			else
			{
				//	���ݲ��� ���뻺����
				if(sizeof(m_szBuf) - m_dwBufPointer < dwLeft)
				{
					//	������������ ֱ����ջ���
					m_dwBufPointer = 0;
					return;
				}
				m_dwBufPointer += dwLeft;
				memcpy(m_szBuf + m_dwBufPointer, pReadData, dwLeft);
				return;
			}

			//	������
			if(dwLeft > 4)
			{
				dwSize = ntohl(*(DWORD*)pReadData);
// 				pReadData += 4;
// 				dwLeft -= 4;
			}
			else
			{
				//	���� ���뻺��
				if(sizeof(m_szBuf) - m_dwBufPointer < dwLeft)
				{
					//	������������ ֱ����ջ���
					m_dwBufPointer = 0;
					return;
				}
				m_dwBufPointer += dwLeft;
				memcpy(m_szBuf + m_dwBufPointer, pReadData, dwLeft);
				return;
			}
		}
	}
	else
	{
		//	������������ �ȴӻ�������ȡ����
		if(m_dwBufPointer < 4)
		{
			return;
		}
		else
		{
			//	�ȶ������ݳ��� ���Ƿ���Զ�����
			DWORD dwSize = ntohl(*(DWORD*)m_szBuf);
			if(dwSize > m_dwBufPointer + _len)
			{
				//	�������������� ������
				return;
			}
			else
			{
				//	���Զ��� �ȶ��껺����������
				DWORD dwLeft = _len;
				pReadData = _pData;
				DWORD dwNeed = dwSize - m_dwBufPointer;

				pPacket = (PacketBase*)malloc(dwSize);
				pPacket->dwSize = dwSize;
				memcpy(pPacket, m_szBuf, m_dwBufPointer);
				//	�ڶ�socket����
				memcpy(pPacket + m_dwBufPointer, pReadData, dwNeed);
				m_xPackets.push_back(pPacket);

				pReadData += dwNeed;
				dwLeft -= dwNeed;
				m_dwBufPointer = 0;

				//	�ٶ�����
				if(dwLeft > 4)
				{
					dwSize = ntohl(*(DWORD*)pReadData);

					while(dwLeft > 0)
					{
						if(dwSize <= dwLeft)
						{
							//	���ݹ���
							pPacket = (PacketBase*)malloc(dwSize);
							pPacket->dwSize = dwSize;
							memcpy(pPacket, pReadData, dwSize);
							m_xPackets.push_back(pPacket);
							dwLeft -= dwSize;
							pReadData += dwSize;
						}
						else
						{
							//	���ݲ��� ���뻺����
							if(sizeof(m_szBuf) - m_dwBufPointer < dwLeft)
							{
								//	������������ ֱ����ջ���
								m_dwBufPointer = 0;
								return;
							}
							m_dwBufPointer += dwLeft;
							memcpy(m_szBuf + m_dwBufPointer, pReadData, dwLeft);
							return;
						}
					}
				}
			}
		}
	}
}