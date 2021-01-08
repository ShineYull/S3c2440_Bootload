#ifndef _APP_TEST_H_
#define _APP_TEST_H_

/* 用户桌面的菜单选项，此处定义的是选项索引
 * 对应go_where成员
 */
enum user_desktop_menu {
	sensor_enum = 1111,  /* 传感器选项 */
	drawing_board_enum,  /* 画板选项, 还未实现...[YLL] */ 
};

/* 选择返回哪一个桌面
 * 对应go_back_to成员
 */
enum go_back_desktop {
	user_desktop_enum = 1,
	login_desktop_enum,
};

/* 桌面对象 */
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
