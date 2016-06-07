#ifndef _SASTART_H_
#define _SASTART_H_
#include <Windows.h>
#include <stack>
#include <vector>
#include <list>

#define SEARCH_OFFSET_X	5
#define SEARCH_OFFSET_Y	5

namespace SASTAR
{
	enum ASTAR_DIRECTION
	{
		ADT_TOPLEFT,
		ADT_TOP,
		ADT_TOPRIGHT,
		ADT_LEFT,
		ADT_RIGHT,
		ADT_BOTTOMLEFT,
		ADT_BOTTOM,
		ADT_BOTTOMRIGHT,
		ADT_TOTAL,
	};
	struct SAstarElement
	{
		int nX;
		int nY;
		BOOL bOpen;
		//	������Ϣ
		//	��
		int nFn;
		//	��ʼ�㵽��ǰλ���ƶ�����
		UINT nGn;
		//	��ǰλ�õ�Ŀ������ľ���
		UINT nHn;
		SAstarElement* pParent;
	};

	typedef std::list<SAstarElement>								SAstarElements;
	typedef std::list<SAstarElement*>								SAstarPelements;
	typedef std::list<POINT>										SAstarPoints;

	class SAstarDataAdaptor
	{
	public:
		SAstarDataAdaptor()
		{
			m_pSrc = NULL;
		}
		virtual ~SAstarDataAdaptor(){}

	public:
		void SetDataSource(void* _psrc)
		{
			m_pSrc = _psrc;
		}
		//	����Ԫ��nFnΪ-1��ʾԪ�ز�����
		virtual void GetData(int _x, int _y, SAstarElement* _pele) const = 0;

	protected:
		void* m_pSrc;
	};

	//	Element
	BOOL Calculate8DirectionNormal(const SAstarDataAdaptor* _psrc, SAstarElements* _pret, const SAstarElement* _pfrom, const SAstarElement* _pto);
	BOOL Calculate8DirectionNoDiag(const SAstarDataAdaptor* _psrc, SAstarElements* _pret, const SAstarElement* _pfrom, const SAstarElement* _pto);
	//	POINT
	BOOL Calculate8DirectionNormal(const SAstarDataAdaptor* _psrc, SAstarPoints* _pret, const POINT* _pfrom, const POINT* _pto);
	BOOL Calculate8DirectionNoDiag(const SAstarDataAdaptor* _psrc, SAstarPoints* _pret, const POINT* _pfrom, const POINT* _pto);

	//	����ʽ����Hn
	int GetElementHn(const SAstarElement* _pcur, const SAstarElement* _pdest);
	int GetElementHn(const SAstarElement* _pcur, const POINT* _pdest);
	bool operator== (const SAstarElement& ele1, const SAstarElement& ele2);

	//	�ͷ���Դ
	void FreeMemory();

	struct SAstarCompGreater
	{
		bool operator()(const SAstarElement* e1, const SAstarElement* e2)
		{
 			if(e1->nFn < e2->nFn)
 			{
 				return true;
 			}
			
			return false;
		}
	};

	struct SAstarEqual
	{
		bool operator()(const SAstarElement& e1, const SAstarElement& e2)
		{
			if(e1.nX == e2.nX &&
				e2.nY == e1.nY)
			{
				return true;
			}
			return false;
		}
	};

};

#endif