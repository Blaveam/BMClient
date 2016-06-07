#include "CTextureBuffer.h"

//////////////////////////////////////////////////////////////////////////
CTextureBuffer::CTextureBuffer()
{
	if(!s_hge)
	{
		s_hge = hgeCreate(HGE_VERSION);
	}
	m_tex = 0;
	ReleaseBuffer();
}

CTextureBuffer::~CTextureBuffer()
{
	ReleaseBuffer();
	if(s_hge)
	{
		s_hge->Release();
		s_hge = NULL;
	}
}

HGE* CTextureBuffer::s_hge = NULL;



//////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* ��ʼ������
/************************************************************************/
bool CTextureBuffer::InitBuffer(DWORD _dwWidth, DWORD _dwHeight)
{
	if(_dwHeight < 0 ||
		_dwWidth < 0)
	{
		return false;
	}
	ReleaseBuffer();

	m_tex = s_hge->Texture_Create(_dwWidth, _dwHeight);
	if(!m_tex)
	{
		return false;
	}

	m_stRect.x = _dwWidth;
	m_stRect.y = _dwHeight;
	m_dwRealWidth = s_hge->Texture_GetWidth(m_tex);
	m_dwRealHeight = s_hge->Texture_GetHeight(m_tex);
	if(m_dwRealWidth == _dwWidth &&
		m_dwRealHeight == _dwHeight)
	{
		m_bIsStandardRect = true;
	}

	DWORD* pData = s_hge->Texture_Lock(m_tex, false);
	ZeroMemory(pData, m_dwRealHeight * m_dwRealWidth);
	s_hge->Texture_Unlock(m_tex);

	//	����1��ƫ�ƾ���ʹ�ÿ���д��
	m_stRcOffset[1].left = 0;
	m_stRcOffset[1].right = _dwWidth;
	m_stRcOffset[1].top = 0;
	m_stRcOffset[1].bottom = _dwHeight;
	m_bCanDraw = true;
}

/************************************************************************/
/* �ͷ�����
/************************************************************************/
void CTextureBuffer::ReleaseBuffer()
{
	if(m_tex)
	{
		s_hge->Texture_Free(m_tex);
		m_tex = 0;
	}

	ZeroMemory(&m_stRect, sizeof(m_stRect));
	//	��ֹд��
	m_bCanDraw = false;
	ZeroMemory(m_stRcOffset, sizeof(m_stRcOffset));
	m_bIsStandardRect = false;
	m_dwRealWidth = m_dwRealHeight = 0;
}

/************************************************************************/
/* ��������(����Ч��������ڲŻ���� ʹ��ǰ��Ҫ����Offset)
/************************************************************************/
bool CTextureBuffer::DrawTexture(HTEXTURE _tex, int _nDestX, int _nDestY, int _nWidth, int _nHeight)
{
	if(!m_bCanDraw)
	{
		return false;
	}

	RECT stRcDraw;
	stRcDraw.left = _nDestX;
	stRcDraw.right = _nDestX + _nWidth;
	stRcDraw.top = _nDestY;
	stRcDraw.bottom = _nDestY + _nHeight;

	RECT stRcCanDraw;
	POINT stDrawPos;
	DWORD dwRealWidth = s_hge->Texture_GetWidth(_tex);
	int nInsertDest = 0;
	int nFromSrc = 0;
	DWORD* pDest = s_hge->Texture_Lock(m_tex, false);
	DWORD* pSrc = s_hge->Texture_Lock(_tex);
	DWORD dwPixel = 0;
	if(!pDest ||
		!pSrc)
	{
		return false;
	}

	for(int i = 0; i < 2; ++i)
	{
		if(::IntersectRect(&stRcCanDraw, &stRcDraw, &m_stRcOffset[i]))
		{
			//	ƫ��1 ����
			for(int j = stRcCanDraw.left; j <= stRcCanDraw.right; ++j)
			{
				for(int k = stRcCanDraw.top; k <= stRcCanDraw.bottom; ++k)
				{
					if(j < 0 ||
						j >= m_stRect.x ||
						k < 0 ||
						k >= m_stRect.y)
					{
						continue;
					}

					nFromSrc = (k - _nDestY) * dwRealWidth + (j - _nDestX);
					stDrawPos.x = j;
					stDrawPos.y = k;
					if(GetRealPoint(&stDrawPos, &nInsertDest))
					{
						dwPixel = pSrc[nFromSrc];
						if(dwPixel & 0xFF000000)
						{
							//	alphaͨ����Ϊ0 д��
							pDest[nInsertDest] = dwPixel;
						}
					}
				}
			}
		}
	}

	s_hge->Texture_Unlock(m_tex);
	s_hge->Texture_Unlock(_tex);
	return true;
}

/************************************************************************/
/* ֱ�ӻ��ƣ��κ������ܻ��ƣ�
/************************************************************************/
bool CTextureBuffer::DrawTextureDirect(HTEXTURE _tex, int _nDestX, int _nDestY, int _nWidth, int _nHeight)
{
	DWORD* pDest = s_hge->Texture_Lock(m_tex, false);
	DWORD* pSrc = s_hge->Texture_Lock(_tex);
	DWORD dwPixel = 0;
	if(!pDest ||
		!pSrc)
	{
		return false;
	}

	RECT stRcCanDraw;
	stRcCanDraw.left = _nDestX;
	stRcCanDraw.right = _nDestX + _nWidth;
	stRcCanDraw.top = _nDestY;
	stRcCanDraw.bottom = _nDestY + _nHeight;

	DWORD dwRealWidth = s_hge->Texture_GetWidth(_tex);
	int nInsertDest = 0;
	int nFromSrc = 0;
	POINT stDrawPos;

	for(int j = stRcCanDraw.left; j <= stRcCanDraw.right; ++j)
	{
		for(int k = stRcCanDraw.top; k <= stRcCanDraw.bottom; ++k)
		{
			if(j < 0 ||
				j >= m_stRect.x ||
				k < 0 ||
				k >= m_stRect.y)
			{
				continue;
			}

			nFromSrc = (k - _nDestY) * dwRealWidth + (j - _nDestX);
			stDrawPos.x = j;
			stDrawPos.y = k;
			if(GetRealPoint(&stDrawPos, &nInsertDest))
			{
				dwPixel = pSrc[nFromSrc];
				if(dwPixel & 0xFF000000)
				{
					//	alphaͨ����Ϊ0 д��
					pDest[nInsertDest] = dwPixel;
				}
			}
		}
	}

	s_hge->Texture_Unlock(m_tex);
	s_hge->Texture_Unlock(_tex);
	return true;
}

/************************************************************************/
/* ��������
/************************************************************************/
void CTextureBuffer::EndDraw()
{
	ZeroMemory(m_stRcOffset, sizeof(m_stRcOffset));
}

/************************************************************************/
/* ƫ�ƽ���д�� >0 ����ƫ�ƣ����¿ɻ��ƾ��Σ�
/************************************************************************/
void CTextureBuffer::Offset(int _nX, int _nY)
{
	//	0
	m_stRcOffset[0].top = 0;
	m_stRcOffset[0].bottom = m_stRect.y;
	m_stRcOffset[0].right = _nX;
	m_stRcOffset[0].left = 0;

	if(m_stRcOffset[0].right < m_stRcOffset->left)
	{
		int nTmp = 0;
		m_stRcOffset[0].right += m_stRect.x;
		m_stRcOffset[0].left += m_stRect.x;
		nTmp = m_stRcOffset[0].left;
		m_stRcOffset[0].left = m_stRcOffset[0].right;
		m_stRcOffset[0].right = nTmp;
	}

	//	1
	m_stRcOffset[1].left = 0;
	m_stRcOffset[1].right = m_stRect.x;
	m_stRcOffset[1].top = 0;
	m_stRcOffset[1].bottom = _nY;

	if(m_stRcOffset[1].bottom < m_stRcOffset[1].top)
	{
		int nTmp = 0;
		m_stRcOffset[1].bottom += m_stRect.x;
		m_stRcOffset[1].top += m_stRect.x;
		nTmp = m_stRcOffset[1].top;
		m_stRcOffset[1].top = m_stRcOffset[1].bottom;
		m_stRcOffset[1].bottom = nTmp;
	}

	//	ƫ�ƾ������ ƫ������
	DWORD* pData = s_hge->Texture_Lock(m_tex, false);
	int nMoveIndexStart = 0;
	POINT stStart;
	int nMoveIndexNext = 0;
	POINT stNext;

	if(_nX > 0)
	{
		for(int i = 0; i < m_stRect.y; ++i)
		{
			//	ÿ��������
			stStart.x = 0;
			stStart.y = i;
			if(GetRealPoint(&stStart, &nMoveIndexStart))
			{
				memmove(&pData[nMoveIndexStart + _nX], &pData[nMoveIndexStart], (m_stRect.x - _nX) * sizeof(DWORD));
				memset(&pData[nMoveIndexStart], 0xFF000000, _nX * sizeof(DWORD));
			}
		}
	}
	else if(_nX < 0)
	{
		//	����
		_nX = -_nX;
		for(int i = 0; i < m_stRect.y; ++i)
		{
			stStart.x = _nX;
			stStart.y = i;
			if(GetRealPoint(&stStart, &nMoveIndexStart))
			{
				memmove(&pData[nMoveIndexStart - _nX], &pData[nMoveIndexStart], (m_stRect.x - _nX) * sizeof(DWORD));
				memset(&pData[nMoveIndexStart - _nX + m_stRect.x - _nX], 0xFF000000, _nX * sizeof(DWORD));
			}
		}
	}

	if(_nY > 0)
	{
		//	����
		for(int i = m_stRect.y - 1 - _nY; i >= 0; --i)
		{
			stStart.x = 0;
			stStart.y = i;
			if(GetRealPoint(&stStart, &nMoveIndexStart))
			{
				stNext.x = 0;
				stNext.y = stStart.y + _nY;
				if(GetRealPoint(&stNext, &nMoveIndexNext))
				{
					memmove(&pData[nMoveIndexNext], &pData[nMoveIndexStart], m_stRect.x * sizeof(DWORD));
				}
			}
		}
		//	�������
		for(int i = 0; i < _nY; ++i)
		{
			stStart.x = 0;
			stStart.y = i;
			if(GetRealPoint(&stStart, &nMoveIndexStart))
			{
				memset(&pData[nMoveIndexStart], 0xFF000000, m_stRect.x * sizeof(DWORD));
			}
		}
	}
	else if(_nY < 0)
	{
		//	����
		_nY = -_nY;
		for(int i = _nY; i < m_stRect.y; ++i)
		{
			stStart.x = 0;
			stStart.y = i;
			if(GetRealPoint(&stStart, &nMoveIndexStart))
			{
				stNext.x = 0;
				stNext.y = stStart.y - _nY;
				if(GetRealPoint(&stNext, &nMoveIndexNext))
				{
					memmove(&pData[nMoveIndexNext], &pData[nMoveIndexStart], m_stRect.x * sizeof(DWORD));
				}
			}
		}
		//	�������
		for(int i = m_stRect.y - _nY; i < m_stRect.y; ++i)
		{
			stStart.x = 0;
			stStart.y = i;
			if(GetRealPoint(&stStart, &nMoveIndexStart))
			{
				memset(&pData[nMoveIndexStart], 0xFF000000, m_stRect.x * sizeof(DWORD));
			}
		}
	}

	s_hge->Texture_Unlock(m_tex);
	m_bCanDraw = true;
}

/************************************************************************/
/* ֱ��ƫ�� �����¾���
/************************************************************************/
void CTextureBuffer::OffsetDirect(int _nX, int _nY)
{

}

/************************************************************************/
/* ��������
/************************************************************************/
// HTEXTURE CTextureBuffer::GetBuffer()
// {
// 	return m_tex;
// }


//////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* ��ȡʵ�����ص�λ��(��ȡ��������ʵ��д���)
/************************************************************************/
bool CTextureBuffer::GetRealPoint(const POINT* _pstSrc, int* _pnDest)
{
	if(!_pstSrc ||
		!_pnDest)
	{
		return false;
	}

	if(_pstSrc->x < 0 ||
		_pstSrc->x >= m_stRect.x ||
		_pstSrc->y < 0 ||
		_pstSrc->y >= m_stRect.y)
	{
		return false;
	}

	if(m_bIsStandardRect)
	{
		*_pnDest = _pstSrc->y * m_stRect.x + _pstSrc->x;
		return true;
	}
	else
	{
		//	����� �Կ���֧�ַ�32��������
		if(m_dwRealHeight == 0 ||
			m_dwRealWidth == 0)
		{
			return false;
		}

		*_pnDest = _pstSrc->y * m_dwRealWidth + _pstSrc->x;
	}

	return true;
}

/************************************************************************/
/* ��ȡʵ�����ص�λ��(��������д���)
/************************************************************************/
bool CTextureBuffer::GetRealPoint(HTEXTURE _tex, const POINT* _pstSrc, const POINT* _pstSrcRect, int* _pnDest)
{
	if(!_pstSrc ||
		!_pnDest ||
		!_tex ||
		!_pstSrcRect)
	{
		return false;
	}

	if(_pstSrc->x < 0 ||
		_pstSrc->x >= _pstSrcRect->x ||
		_pstSrc->y < 0 ||
		_pstSrc->y >= _pstSrcRect->y)
	{
		return false;
	}

	DWORD dwRealWidth = s_hge->Texture_GetWidth(_tex);
	*_pnDest = _pstSrc->y * dwRealWidth + _pstSrc->x;
	return true;
}