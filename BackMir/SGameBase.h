#ifndef _SGAMEBASE_H
#define _SGAMEBASE_H

#include <hge.h>
#include <hgegui.h>  
#include <hgeSprite.h>   
#include <hgefont.h>  
#include <Windows.h>

//////////////////////////////////////////////////////   
//////////////////////////////////////////////////////   
//  
// 
//
//
//	SGameBase Ver 1.1
//				2012.3.9
//
//
//
//
//////////////////////////////////////////////////////  
//////////////////////////////////////////////////////  


//��   
#define S_DEBUG\
	{MessageBoxA(NULL,\
	m_pHGE->System_GetErrorMessage(),\
	"HGE Error Code",\
	MB_OK|MB_ICONERROR);abort();}  
#define S_DEBUG_(M)\
	{char buf[200];\
	sprintf(buf,"Line:%d    Function:%s   Run;",__LINE__,__FUNCTION__);\
	MessageBoxA(NULL,\
	M,\
	buf,\
	MB_OK);abort();}     


#define S_INITIAL_TEST(O)\
	if(O==NULL)\
	{MessageBoxA(NULL,\
	m_pHGE->System_GetErrorMessage(),\
	"HGE Error Code",\
	MB_OK|MB_ICONERROR);return false;}   

#define S_MSG(msg)\
	MessageBoxA(NULL,\
	msg,"SPTGAMEBASE MESSAGE",\
	MB_OK|MB_ICONINFORMATION);

////////////////////////////////  


class SGameBase  
{  
	//////////////////////////////////////////////////////  
	//  
	//  ��Ҫ(����)����ʵ�ֵĺ���
	//  
	///////////////////////////////////////////////////////  
protected:
	//
	//	�¼����˺���
	//
	virtual LRESULT                 WinEventFilter(HWND h, UINT u, WPARAM w, LPARAM l);  
	//
	//	��ý��� ʧȥ���� ����ͼ��
	//
	virtual bool                    RealFocusGainFunc();  
	virtual bool                    RealFocusLostFunc();  
	virtual bool                    RealGfxRestoreFunc();  
	//
	//	�����û���Ϸ��Դ
	//
	virtual bool                    UserInitial();  
	//
	//	�ͷ��û���Ϸ��Դ
	//
	virtual void                    UserUninitial();  
	//
	//	�û�ͼ����㺯��
	//
	virtual void                    UserFrameFunc();  
	//
	//	�û����ƺ���
	//
	virtual void                    UserRenderFunc(); 
	virtual bool                    UserProcCommand(); 

public:  
	SGameBase(const char* szTitle=NULL,int nViewWidth=800,int nViewHeight=600);  
	virtual ~SGameBase();  

public:  
	//
	//	��̬������������ʵ���ĳ�Ա����  ��ϵͳ�Զ�����
	//
	static bool                     FrameFunc();  
	static bool                     RenderFunc();  
	static bool                     FocusGainFunc();  
	static bool                     FocusLostFunc();  
	static bool                     GfxRestoreFrameFunc();  
	//
	//���ڹ���  
	//
	static LRESULT                  MainWinProc(HWND h, UINT u, WPARAM w, LPARAM l);  


public:  
	//
	//�ⲿ�ӿ�
	//
	bool						    SetGameCursor(const char* szCursorPath);  
	bool							SetGameCursor(HTEXTURE _texCursor);
	bool						    SetGameTitle(const char* szTitle);  
	void							SetGameFps(int nFps);
	void							SetGameLogFile(const char* szFile);
	bool							SetGameTimer(UINT nEventID,UINT nElapse);

	void							AddLog(const char* szLog);
	void							KillGameTimer(UINT nEventID);

	char*                           GetGameTitle(){return m_szGameTitle;}  
	const char*                     GetGameTitle()const{return m_szGameTitle;}  
	RECT&							GetGameViewRectNew();
	RECT&							GetGameViewRect();
	int								GetGameViewWidth(){return m_nViewWidth;}
	int								GetGameViewHeight(){return m_nViewHeight;}

	bool							Init();
	void				            EnableSound(bool);  
	void                            Exec();  

	virtual bool					IsWindowMode()	{return true;}

protected:  
	//
	//��ȡʵ��ָ��  
	//
	static SGameBase *              InitialInstance(SGameBase * p=NULL);  
	//
	//	��ȡʵ�����ڹ���  
	//
	static WNDPROC                  InitialProc(WNDPROC p=NULL);  

private:  
	bool                            HgeInitial(const char* szTitle);  
	void                            HgeUninitial();  

protected:   
	//
	//	��ʵ�Ĵ�����	һ�㲻��Ҫ����ʵ��	��UserFrameFunc UserRenderFunc�д���	��ProcCommand�д���GUI��Ϣ
	//
	virtual bool                    RealFrameFunc();  
	virtual bool                    RealRenderFunc();   

	//
	//	GUI����������  
	//
	virtual void                    UpdateCommand();  
	//
	//	��������
	//
	virtual bool					IsCaptionBar(int x,int y);
	virtual void					OnGameTimer(UINT nEventID);

protected:  
	HGE *                           m_pHGE;  
	hgeGUI *                        m_pGUI;  
	int                             m_nCommandID;  
	HWND							m_hWin;

protected:  
	HTEXTURE                        m_hCursor;  
	char                            m_szGameTitle[50];  
	hgeSprite*                      m_pSprCursor;  
	bool                            m_bHgeInitial;  
	float                           m_fTimeDelta;  
	int								m_nViewWidth;
	int								m_nViewHeight;
	RECT							m_rcView;
};  


#endif