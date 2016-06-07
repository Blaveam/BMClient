//////////////////////////////////////////////////////////////////////////
//
// �༭�������
//
// ���ߣ�΢���ƽ��(BOGY)
//
// �汾��For HGE v1.6
//
//////////////////////////////////////////////////////////////////////////
#ifndef MY_HGE_GFXEDIT_H
#define MY_HGE_GFXEDIT_H

class MyGfxEdit;
typedef int(_stdcall *PressEnterCallback)(void* _pEdit);
typedef int(_stdcall *OnLoseFocusCallback)(void* _pEdit);
typedef int(_stdcall *OnCharInput)(char _cKey);
typedef int(_stdcall *CanLoseFocus)(void* _pEdit);

//#include "GfxFont.h"
#include "../Common/TextPrinter.h"

#define EXTEND_FONT

class MyGfxEdit
{
public:

	///
	/// nEditWidth		�༭����
	///	nFontColor		������ɫ
	/// lpFontName		��������
	///	nFontSize		�����С
	/// bBold			�Ƿ����
	/// bItalic			�Ƿ�б��
	/// bAntialias		�Ƿ�ƽ��
	///
	MyGfxEdit(UINT nEditWidth, DWORD nFontColor, CHAR* lpFontName = "����", UINT nFontSize = 12, BOOL bBold = FALSE, BOOL bItalic = FALSE, BOOL bAntialias = TRUE);
	~MyGfxEdit(void);

	typedef enum tagDeletePos
	{
		DP_LEFT = 0,
		DP_RIGHT
	}TDP;

public:
	void			SetFocus(); // ���ñ༭�򽹵㣬ֻ��ӵ�б༭�򽹵������Ӧ����
	void			KillFocus(); // ����༭�򽹵㣬ʧȥ�༭�򽹵�󽫲�����Ӧ����
	bool			IsFocused(); // �жϱ༭���Ƿ�ӵ�н���

	void			ClearCookie(); // ����༭������

	void			InsertCookie(const char* lpCookie); // �����ַ������༭��ǰ��괦
	const char*		GetCookie(); // ��ȡ�༭��ǰ����

	void			InsertCookieW(const wchar_t* lpCookie); // �����ַ������༭��ǰ��괦(���ַ�ģʽ)
	const wchar_t*	GetCookieW(); // ��ȡ�༭��ǰ����(���ַ�ģʽ)

	void			Updata();
	void			Render(float x, float y); //

	inline void		SetRenderBorder(bool _br)		{m_bRenderBorder = _br;}

private:
	void			OnKey( UINT nKey, UINT nRepCnt, UINT nFlags );
	void			OnChar( UINT nChar, UINT nRepCnt, UINT nFlags );
	void			InsertChar(wchar_t aChar);
	void			DeleteChar(TDP aPos);
	void			OnCtrlPaste();

private:
	static const int COOKIE_SIZE = 1024;
	wchar_t			m_sCookie[COOKIE_SIZE];
	char			m_sBuffer[COOKIE_SIZE];

	FLOAT			m_nEditW;
	FLOAT			m_nEditH;

	UINT			m_nCharPos;		// �ַ�λ��

	hgeRect			m_rcWorld;
    
	HGE*			m_pHGE;			// HGEָ��
	hgeSprite*		m_pSprite;		// ��꾫��
#ifdef EXTEND_FONT
public:
	TextPrinter*    m_pFont;
	PressEnterCallback m_pEnterCallback;
	inline PressEnterCallback SetEnterKeyCallback(PressEnterCallback _pFunc)
	{
		PressEnterCallback pPre = m_pEnterCallback;
		m_pEnterCallback = _pFunc;
		return pPre;
	}
	OnLoseFocusCallback m_pOnLoseFocus;
	inline OnLoseFocusCallback SetOnLoseFocusCallback(OnLoseFocusCallback _pFunc)
	{
		OnLoseFocusCallback pPre = m_pOnLoseFocus;
		m_pOnLoseFocus = _pFunc;
		return pPre;
	}
	OnCharInput m_pOnCharInput;
	inline OnCharInput SetOnCharInputCallback(OnCharInput _pFunc)
	{
		OnCharInput pPre = m_pOnCharInput;
		m_pOnCharInput = _pFunc;
		return pPre;
	}
	CanLoseFocus m_pCanLoseFocus;
	inline CanLoseFocus SetCanLoseFocus(CanLoseFocus _pFunc)
	{
		CanLoseFocus pPre = m_pCanLoseFocus;
		m_pCanLoseFocus = _pFunc;
		return pPre;
	}
#else
	GfxFont*		m_pFont;		// ���徫��
#endif

	static LRESULT CALLBACK MyGfxEditWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

	int         m_CursorShowCount;

	//////////////////////////////////////////////////////////////////////////
	bool m_bRenderBorder;
};
#endif//MY_HGE_GFXEDIT_H
