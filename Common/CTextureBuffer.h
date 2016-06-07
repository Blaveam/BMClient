#ifndef CTEXTUREBUFFER_H_
#define CTEXTUREBUFFER_H_

#include <hge.h>

/************************************************************************/
/* CTextureBuffer ��������
/************************************************************************/

class CTextureBuffer
{
public:
	CTextureBuffer();
	virtual ~CTextureBuffer();

public:
	//	��ʼ������
	bool InitBuffer(DWORD _dwWidth, DWORD _dwHeight);
	//	�ͷ�����
	void ReleaseBuffer();
	//	����
	bool DrawTexture(HTEXTURE _tex, int _nDestX, int _nDestY, int _nWidth, int _nHeight);
	//	��������
	void EndDraw();
	//	ֱ�ӻ���
	bool DrawTextureDirect(HTEXTURE _tex, int _nDestX, int _nDestY, int _nWidth, int _nHeight);
	//	ֱ��ƫ��
	void OffsetDirect(int _nX, int _nY);
	//	�õ���������
	inline HTEXTURE GetBuffer()	{return m_tex;}
	//	����ƫ��
	void Offset(int _nX, int _nY);
	//	��û���ߴ�
	inline DWORD GetBufferWidth() {return m_stRect.x;}
	inline DWORD GetBufferHeight() {return m_stRect.y;}

protected:
	//	��ȡ��������תΪʵ������
	bool GetRealPoint(const POINT* _pstSrc, int* _pnDest);
	bool GetRealPoint(HTEXTURE _tex, const POINT* _pstSrc, const POINT* _pstSrcRect, int* _pnDest);

protected:
	HTEXTURE m_tex;
	POINT m_stRect;
	bool m_bIsStandardRect;
	DWORD m_dwRealWidth;
	DWORD m_dwRealHeight;
	RECT m_stRcOffset[3];
	bool m_bCanDraw;

private:
	static HGE* s_hge;
};

#endif