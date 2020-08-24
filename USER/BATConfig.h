/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "LPC17xx.h"

#define COC_Delay_Recovery	//COC��ʱ�ָ����ܿ���	ע�͵���ر�
#define DOC0_Delay_Recovery	//DOC0��ʱ�ָ����ܿ���	ע�͵���ر�
#define CeUV_Delay_Recovery	//CeUV��ʱ�ָ����ܿ���	ע�͵���ر�
#define CeOV_Delay_Recovery	//CeUV��ʱ�ָ����ܿ���	ע�͵���ر�
#define PkUV_Delay_Recovery	//PkUV��ʱ�ָ����ܿ���	ע�͵���ر�
#define PkOV_Delay_Recovery	//PkUV��ʱ�ָ����ܿ���	ע�͵���ر�

#define COC_Delay_Recovery_Time		(30)			//COC��ʱ�ָ� (s)
#define DOC0_Delay_Recovery_Time	(30)			//DOC0��ʱ�ָ� (s)
#define CeUV_Delay_Recovery_Time	(30)			//CeUV��ʱ�ָ� (s)
#define CeOV_Delay_Recovery_Time	(30)			//CeOV��ʱ�ָ� (s)
#define PkUV_Delay_Recovery_Time	(30)			//PkUV��ʱ�ָ� (s)
#define PkOV_Delay_Recovery_Time	(30)			//PkOV��ʱ�ָ� (s)

#define BMS_Cur 									(150)			//BMS���ĵ��� (mA)

//#define SC_Threshold 							(10000)		//short current (mA)	ͨ��Ӳ���Ƚϵ�·ʵ�֣�ͨ����λ������

//#define DOC1_Threshold 						(4000)		//Discharge over current 1 Threshold (mA)	ͨ��Ӳ���Ƚϵ�·ʵ�֣�ͨ����λ������
//#define DOC1_Delay_Time 					(500)			//Discharge over current 1 Delay Time (us)

#define DOC0_Threshold 						(10000)		//Discharge over current 0 Threshold (mA)	ͨ��Ӳ���Ƚϵ�·ʵ�֣�ͨ����λ������
#define DOC0_Delay_Time 					(2000)		//Discharge over current 0 Delay Time (us)

#define COC_Threshold 						(10000)		//Charge over current Threshold (mA)	ͨ��Ӳ���Ƚϵ�·ʵ�֣�ͨ����λ������
#define COC_Delay_Time 						(3000)		//Charge over current Delay Time (us)

#define HOC_Threshold 						(15000)		//Heat over current Threshold (mA)	ͨ�������õ����ȵ���ֵ

#define CeOV_Threshold 						(36500)		//Cell over Voltage Threshold (0.1mV)
#define CeOV_Delay_Time 					(55*10)		//Cell over Voltage Delay Time (ms) ��ѹ��ֵÿ55ms����һ��
#define CeOV_Recovery_Threshold 	(34000)		//Cell over Voltage Recovery Threshold (0.1mV)

#define CeUV_Threshold 						(25500)		//Cell under Voltage Threshold (0.1mV)
#define CeUV_Delay_Time 					(55*10)		//Cell under Voltage Delay Time (ms) ��ѹ��ֵÿ55ms����һ��
#define CeUV_Recovery_Threshold 	(30000)		//Cell under Voltage Recovery Threshold (0.1mV)

#define PkOV_Threshold 						(3650*15)	//Pack over Voltage Threshold (mV)
#define PkOV_Delay_Time 					(55*10)		//Pack over Voltage Delay Time (ms) ��ѹ��ֵÿ55ms����һ��
#define PkOV_Recovery_Threshold 	(3400*15)	//Pack over Voltage Recovery Threshold (mV)

#define PkUV_Threshold 						(2550*15)	//Pack under Voltage Threshold (mV)
#define PkUV_Delay_Time 					(50*10)		//Pack under Voltage Delay Time (ms) ��ѹ��ֵÿ50ms����һ��
#define PkUV_Recovery_Threshold 	(3000*15)	//Pack under Voltage Recovery Threshold (mV)

#define OT_Threshold 							(60+100)	//Over Temperature Threshold (��)				100��Ϊ0�ȵ�
#define OT_Delay_Time 						(10)				//Over Temperature Delay Time (s)
#define OT_Recovery_Threshold 		(50+100)	//Over Temperature Recovery Threshold (��)			100��Ϊ0�ȵ�
#define OT_Recovery_Time 					(10)				//Over Temperature Recovery Delay Time (s)

#define UT_Threshold 							(100-40)	//Over Temperature Threshold (��)			100��Ϊ0�ȵ�
#define UT_Delay_Time 						(10)				//Over Temperature Delay Time (s)
#define UT_Recovery_Threshold 		(100-35)	//Over Temperature Recovery Threshold (��)		100��Ϊ0�ȵ�
#define UT_Recovery_Time 					(10)				//Over Temperature Recovery Delay Time (s)

#define CapLow_Threshold 					(10)				//Capacity Low Threshold (%)		ʣ������ٷֱȵ��ڴ�ֵʱ�澯				
#define Freeze_Threshold 					(0+100)		//Freeze Point Threshold (��)		�¶ȴﵽ5������ʱ�澯
#define Freeze_Recovery_Threshold (2+100)		//Freeze Protect Recovery Threshold (��)		������±����Ļָ���ֵ
#define HOT_Threshold 						(50+100)	//Heat Over Temperature Threshold (��)		���ȹ����У�����¶ȳ�����ֵ

#define TUB_Threshold 						(15)				//Temperature Unbalance (��)		�Ǽ���״̬�£��������̽ͷ������¶����ֵ����Сֵ֮����ڴ�ֵʱ�澯

#define DFC 											(20000)		//design full capacity (mAh)
#define MAX_DFC 									(21000)		//design full capacity (mAh)
#define MTA_Alarm_days						(180)			//ά���澯���������ϴιػ�����ο������ʱ����ڸ�������ά���澯λ���ػ�����

#define Full_CAP_Cali_Current_Threshold (3000)	//(mA)������У׼��������ֵ����������У׼��־��λʱ�����������͵���ֵʱ����������ʼ����ʣ������ֱ����ѹ�������ֹͣ

#define CHG_Stop_Current_Threshold (1000)		//mA	������С�ڴ�ֵ����1min��Ϊ������
#define CHG_Stop_Timer_Threshold 	(60)				//s		������С�ڴ�ֵ����1min��Ϊ������
/*
	END OF FILE
*/
