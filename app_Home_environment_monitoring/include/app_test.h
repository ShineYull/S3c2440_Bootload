#ifndef _APP_TEST_H_
#define _APP_TEST_H_

/* �û�����Ĳ˵�ѡ��˴��������ѡ������
 * ��Ӧgo_where��Ա
 */
enum user_desktop_menu {
	sensor_enum = 1111,  /* ������ѡ�� */
	drawing_board_enum,  /* ����ѡ��, ��δʵ��...[YLL] */ 
};

/* ѡ�񷵻���һ������
 * ��Ӧgo_back_to��Ա
 */
enum go_back_desktop {
	user_desktop_enum = 1,
	login_desktop_enum,
};

/* ������� */
typedef struct desktop{
	char *name;
	int  go_back_to;
	int  go_where;
	void (*into_desktop)(void);
}desktop, *pdesktop;


extern desktop sensor_desktop;
extern desktop user_desktop;
extern desktop login_desktop;


#endif
