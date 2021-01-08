#ifndef _TOUCHSCREEN_H_
#define _TOUCHSCREEN_H_

void touchscreen_init(void);
void ts_read_raw(int *px, int *py, int *ppressure);
void ts_read_raw_test(int *px, int *py, int *ppressure);
int ts_read_raw_for_sensor_button(int *px, int *py, int *ppressure);



#endif /* _TOUCHSCREEN_H_ */
