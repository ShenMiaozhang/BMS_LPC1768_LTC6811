#ifndef __BAT_H__
#define __BAT_H__
#include "LPC17xx.h"
#include "uart.h"
#include "6811.h"
#include "I2C.h"
#include "ad.h"
#include "BATConfig.h"
#include "IO.h" 
#include "AD7124.h"
#include "CAN.h"

#define CAN_Addr_Base	0x1909F000


#define Port_voltage_CHN 					CH_Volt[1]				//AD7124 ͨ��0��ѹ	��AIN4��
#define Current_CHN 							CH_Volt[0]				//AD7124 ͨ��1��ѹ	��AIN6��
#define Heat_Current_CHN 					1250							//CH_Volt[1]				//AD7124 ͨ��1��ѹ	��AIN6��
#define Current_offset 						1279							//С��Ϊ�ŵ磬����Ϊ���
//#define Current_offset 						CH_OffSet[0]							//С��Ϊ�ŵ磬����Ϊ���
#define Heat_Current_offset 			1250							//
//#define Heat_Current_offset 			CH_OffSet[1]							//
#define CellVolt_Max_Data 				(DEVICE[0].CellVolt_Max)			
#define CellVolt_Min_Data 				(DEVICE[0].CellVolt_Min)			
#define TEMP_Max_Data 						(DEVICE[0].LT68_NTC_Temp_MAX)						//����¶�ֵ	100��Ϊ0�ȵ�
#define TEMP_Min_Data 						(DEVICE[0].LT68_NTC_Temp_MIN)						//����¶�ֵ	100��Ϊ0�ȵ�

#define Cell_Volt_Ave							((DEVICE[0].CellVolt_Max + DEVICE[0].CellVolt_Min)/2)		//�����ѹ���ֵ����Сֵ��ƽ��ֵ��Ϊ��ʼ��ʱ���ݵ�ѹ��������������
#define Temp_Ave									((TEMP_Max_Data+TEMP_Min_Data)/2)		//�¶�������Сֵ��ƽ��ֵ����Ϊ������������¶�˥����ϵ�����Ի��ʵʱ����

//DOC��COC�ж�ͨ���˷ŶԵ�ѹ�ıȽϣ�GPIO��ȡ�Ƚ�������߼�ֵ��XXX_StatusΪ��ȡ�����߼�ֵ��
//XXX_PassΪ�ж��߼�����Чֵ(1:��ʾXXX_StatusΪ1ʱ����������0����ʾXXX_StatusΪ0ʱ��������)
#define SC_Status									SC_Read
#define DOC0_Status								DOC0_Read
#define DOC1_Status								DOC1_Read
#define COC_Status								COC_Read
#define SC_Pass										0
#define DOC_Pass									0
#define COC_Pass									0

#define Current_Lowest_threshold	300													//(mA) ��⵽�ĵ������ڸ�ֵʱ������������

#define SC_bit						(1<<0)
#define DOC1_bit					(1<<1)
#define DOC0_bit					(1<<2)
#define COC_bit						(1<<3)
#define CeOV_bit					(1<<4)
#define CeUV_bit					(1<<5)
#define PkOV_bit					(1<<6)
#define PkUV_bit					(1<<7)
#define OT_bit						(1<<8)
#define UT_bit						(1<<9)
#define TUB_bit						(1<<10)
#define CapLow_bit				(1<<16)
#define Freeze_bit				(1<<17)
#define HOC_bit						(1<<18)
#define HOT_bit						(1<<19)
#define MTA_bit						(1<<20)


#define FCCF_bit					(1<<24)
#define CHG_Complete_bit	(1<<17)
#define CHG_Plugged_bit		(1<<16)
#define Heating_bit				(1<<10)
#define CHGing_bit				(1<<9)
#define DSGing_bit				(1<<8)
#define SelfPWR_mos_bit		(1<<3)
#define Heat_mos_bit			(1<<2)
#define CHG_mos_bit				(1<<1)
#define DSG_mos_bit				(1<<0)

#define BAT_Work_Status_DSG_mos_ON		BAT_Work_Status |= DSG_mos_bit
#define BAT_Work_Status_DSG_mos_OFF		BAT_Work_Status &= ~DSG_mos_bit
#define BAT_Work_Status_CHG_mos_ON		BAT_Work_Status |= CHG_mos_bit
#define BAT_Work_Status_CHG_mos_OFF		BAT_Work_Status &= ~CHG_mos_bit
#define BAT_Work_Status_Heat_mos_ON		BAT_Work_Status |= Heat_mos_bit
#define BAT_Work_Status_Heat_mos_OFF	BAT_Work_Status &= ~Heat_mos_bit
#define BAT_Work_Status_SelfPWR_mos_ON		BAT_Work_Status |= SelfPWR_mos_bit
#define BAT_Work_Status_SelfPWR_mos_OFF		BAT_Work_Status &= ~SelfPWR_mos_bit



#define EE_Flag_def	0x5AA5

extern uint8_t EE_DATE_Send[50];

extern uint8_t EE_DATE_Receive[50];
extern uint8_t EE_DATE_Receive_Check[50];
extern uint32_t EE_BAT_Voltage;
extern uint32_t EE_Rem_CAP_mAh;
extern uint8_t EE_Remain_CAP_Percent;
extern uint32_t EE_Full_CAP;
extern uint32_t EE_DSG_CAP_Lifetime_mAh;	
extern uint32_t EE_CHG_CAP_Lifetime_mAh;
extern uint16_t EE_Cycle_Times;
extern uint16_t EE_Flag;

extern uint8_t _Remain_CAP_Percent;

extern uint8_t AlarmLED_Flash_Flag;
extern uint32_t BAT_Timer;
extern uint8_t Remain_CAP_Percent;

extern uint32_t BAT_Work_Status;								//﮵�ع���״̬
extern uint32_t BAT_Protect_Status;							//﮵�ر���״̬
extern uint32_t BAT_Protect_Alarm;


extern uint32_t Full_CAP;											//������			(mAh)
extern uint32_t Full_CAP_RT;
extern uint32_t Remain_CAP_mAms;							//ʣ������		(mAms)	������Remain_CAP_mAms���ۼӺ��ۼ����ۼӺ����1mAhʱ��Rem_CAP_mAh��1���ۼ�����ʱ��Rem_CAP_mAh��1
extern uint32_t Rem_CAP_mAh;									//ʣ������		(mAh)
extern uint32_t Designed_CAP;									//�������		(mAh)
extern uint8_t Remain_CAP_Percent;						//ʣ������		(%)		(ʣ������/�������)*100%
extern uint32_t Current;											//����				(mA)	�ŵ�ʱΪ�������ʱΪ�������λΪ1��ʾΪ�������λΪ0��ʾΪ��
extern uint32_t Heat_Current;									//���ȵ���		(mA)	����ʱ����Ϊ��
extern uint32_t BAT_Voltage;									//������ѹ	(mV)
extern uint32_t Port_Voltage;									//����ڵ�ѹ	(mV)
extern uint32_t BAT_Timer;										//ͬSYSTimer_ms_Counter��Ϊ����ʱ�Ӽ���
extern uint32_t CAP_Timer;										//�ϴ������������ʱ�ĺ���ʱ����
extern uint32_t CANSend_Timer;
extern uint16_t Cycle_Times;
extern uint32_t DSG_CAP_Lifetime_mAh;
extern uint32_t CHG_CAP_Lifetime_mAh;

extern uint32_t CHGStop_Timer;
extern uint32_t DOC1_Timer;							//�ŵ����1������ʱ�ͻָ���ʱ��ʱ
extern uint32_t DOC0_Timer;							//�ŵ����0������ʱ�ͻָ���ʱ��ʱ
extern uint32_t COC_Timer;								//������������ʱ�ͻָ���ʱ��ʱ
extern uint32_t CeOV_Timer;								//���峬ѹ������ʱ�ͻָ���ʱ��ʱ
extern uint32_t CeUV_Timer;								//����Ƿѹ������ʱ�ͻָ���ʱ��ʱ
extern uint32_t PkOV_Timer;								//�ܵ�ѹ��ѹ������ʱ�ͻָ���ʱ��ʱ
extern uint32_t PkUV_Timer;								//�ܵ�ѹǷѹ������ʱ�ͻָ���ʱ��ʱ
extern uint32_t OT_Timer;									//���´�����ʱ�ͻָ���ʱ��ʱ
extern uint32_t UT_Timer;									//���´�����ʱ�ͻָ���ʱ��ʱ

extern uint32_t DOC_Delay_Timer;					//DOC��ʱ�ָ���ʱ
extern uint32_t COC_Delay_Timer;					//COC��ʱ�ָ���ʱ
extern uint32_t CeUV_Delay_Timer;					//CeUV��ʱ�ָ���ʱ
extern uint32_t CeOV_Delay_Timer;					//CeOV��ʱ�ָ���ʱ
extern uint32_t PkUV_Delay_Timer;					//PkUV��ʱ�ָ���ʱ
extern uint32_t PkOV_Delay_Timer;					//PkOV��ʱ�ָ���ʱ

extern uint8_t CAP_Temp_para;			//�������¶ȵ�˥��ϵ�����ٷ��ƣ��������仯ʱ������������ʣ������
extern uint8_t CAP_Temp_para_old;	//��ֵ �������¶ȵ�˥��ϵ�����ٷ���

uint8_t Array_Check(uint8_t a[],uint8_t b[],uint8_t num);
uint8_t EE_Date_Read_Check(void);
void EE_Date_Save(void);
void EE_Date_Read(void);
void CHG_MOS(uint8_t data);
void DSG_MOS(uint8_t data);
void Heat_MOS(uint8_t data);
void From_Temp_Get_RTCap(void);
void From_CellVolt_Get_RemCapPer(void);
void BAT_Voltage_get(void);
void Port_Voltage_get(void);
void Current_get(void);
void CHG_Plugged_get(void);
void Heat_Current_get(void);
void CAP(void);
void Remain_CAP_Percent_get(void);
void Protection_deal(void);
void BAT_Protect_Status_get_slow(void);
void BAT(void);
void Heat(void);
void BAT_CANSend(void);
void BAT_UartSend(void);












#endif


/*
	END OF FILE
*/
