#ifndef SCRIPTCOMMAND_H_
#define SCRIPTCOMMAND_H_

enum SCRIPT_COMMAND
{
	SCRIPT_CMD_BEGIN	=	0,


	//	1	�����Ʒ ��һ������Ϊ��Ʒid
	SCRIPT_CMD_ADDITEM,
	//	2	����Ƿ�����Ʒ param0 ��Ʒid ����ֵ��Ʒ����
	//	��չ -1:��ȡ��ҽ�Ǯ -2:��ȡ��ҵȼ�
	SCRIPT_CMD_HAVEITEM,
	//	3	��ⱳ����������� ����ֵΪ������
	SCRIPT_CMD_EMPTY_ITEM,
	//	4	����Ҽ�Ѫ	param0 �ı��� ���������Ը�
	SCRIPT_CMD_CHANGEHP,
	//	5	�ı����MP param0 �ı��� ���������Ը�
	SCRIPT_CMD_CHANGEMP,
	//	6	�ı����EXP param0 �ı���
	SCRIPT_CMD_CHANGEEXP,
	//	7	������� �޸�������� param0 x���� param1 y����
	SCRIPT_CMD_MOVEPLAYER,
	//	8	�رնԻ��� param0 �ĸ��Ի���
	SCRIPT_CMD_CLOSEDLG,
	//	9	�ر����жԻ���
	SCRIPT_CMD_CLOSEALLDLG,
	//	10	��ʾװ�����״� param0 ���� param1��С�ȼ� param2 ���ȼ�
	SCRIPT_CMD_SHOWTRADEDLG,
	//	11	��������/NPC param0 ID param1 x���� param2 y���� ����ֵ �Ƿ�ɹ� 0/1
	SCRIPT_CMD_CREATENPC,
	//	12	������X����
	SCRIPT_CMD_GETPLAYERCOORDX,
	//	13	������Y����
	SCRIPT_CMD_GETPLAYERCOORDY,
	//	14	����ű�������Ϣ param0 �ļ� param1 ���� param2 ����
	SCRIPT_CMD_EXECUTEERROR,
	//	15	�õ���ҵ�����stage ������Ҫ�� ����ֵ����stage
	SCRIPT_CMD_GETQUESTSTAGE,
	//	16	�����������stage param0 ����ֵ
	SCRIPT_CMD_ADDQUESTSTAGE,


	SCRIPT_CMD_END,
};

#endif