#ifndef _RTC_time_H
#define _RTC_time_H
#include <stm32f10x_lib.h>

//#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */

extern const u8 *weekday[];

struct tm Time_ConvUnixToCalendar(time_t t);
time_t Time_ConvCalendarToUnix(struct tm t);
time_t Time_GetUnixTime(void);
struct tm Time_GetCalendarTime(void);//
void Time_SetUnixTime(time_t);
void Time_SetCalendarTime(struct tm t);//

void RTC_Configuration(void);
void Setup_RTC(void);
u8 str_cmpx(uc8 *s1,uc8 *s2,u8 len);

#endif
