#include "LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "GPIO_LPC17xx.h"
#include "uart.h"
#include "spi.h"
#include "6811.h"
#include "AD.h"
#include "I2C.h"
#include "CAN.h"
#include "BAT.h" 
#include "IO.h" 
#include "Timer.h"
#include "ssp0.h"
#include "ssp1.h"
#include "AD7124.h"
#include "rtc.h"

#define LED1_DIR_OUTPUT GPIO_SetDir(1,22,GPIO_DIR_OUTPUT)
#define LED2_DIR_OUTPUT GPIO_SetDir(1,23,GPIO_DIR_OUTPUT)

#define LED1_SET_1 GPIO_PinWrite(1,22,1)
#define LED1_SET_0 GPIO_PinWrite(1,22,0)
#define LED2_SET_1 GPIO_PinWrite(1,23,1)
#define LED2_SET_0 GPIO_PinWrite(1,23,0)

uint8_t ms_interrupt_status = 0;	//SYS�����жϱ�־�������жϺ���1��

uint8_t KEY_Status = 0;	//0��δ����		1��������		2�������º���
uint8_t RUN_Status = 0;	//0x00����ʼ��	0x01������״̬	0xFF������ػ�����
uint32_t SYSTimer_ms_Counter = 0;
uint32_t SYSTimer_s_Counter = 0;
uint32_t SYSRun_Seconds = 0;

uint16_t i=0,j=0,k=0;

uint8_t buff[30] = {'E','E','P','R','O','M',' ','O','K','\n','\r'};
RTCTime local_time, alarm_time, current_time;

//�ⲿ�ж�I/O��ʼ��
void EINT_GPIO_Init(void){
//	LPC_GPIOINT->IO2IntEnF |= 0x00000002;	//P2.1�½����ж�ʹ��
//	LPC_GPIOINT->IO2IntEnF |= 0x00000004;	//P2.2�½����ж�ʹ��	
//	LPC_GPIOINT->IO2IntEnF |= 0x00000008;	//P2.3�½����ж�ʹ��
//	LPC_GPIOINT->IO2IntEnF |= 0x00000010;	//P2.4�½����ж�ʹ��
//	LPC_GPIOINT->IO2IntEnF |= 0x00000020;	//P2.5�½����ж�ʹ��
}

//��·����		��ֹ��ŵ磬���¿����ָ�
//�ŵ����		��ֹ�ŵ磬�����磬���¿����ָ�
//������		��ֹ��磬����ŵ磬���¿����ָ�
void EINT3_IRQHandler(void){
//	if((LPC_GPIOINT->IO2IntStatF & 0x00000002)>0){	//�ж�P2.1�ж�	�ػ��ж�
//		LPC_GPIOINT->IO2IntClr |= 0x00000002;	//����жϱ�־
//		j++;	
//	}
//	if((LPC_GPIOINT->IO2IntStatF & 0x00000004)>0){	//�ж�P2.2�ж�	DOC0�ж�
//		LPC_GPIOINT->IO2IntClr |= 0x00000004;	//����жϱ�־
//		timer3_start();
//		DOC0_Timer = timer3_get_TC();
//		while(DOC0_Status == DOC_Pass){
//			if(timer3_get_TC() > DOC0_Timer+DOC0_Delay_Time){
//				DSG_OFF;
//				CHG_ON;
//				Light_AlarmLED;	//����������
//				BAT_Protect_Status |= DOC0_bit;			//�ñ���λ
//			}
//		}
//		timer3_TC_Reset();
//		timer3_stop();
//	}	
//	if((LPC_GPIOINT->IO2IntStatF & 0x00000008)>0){	//�ж�P2.3�ж�	COC�ж�
//		LPC_GPIOINT->IO2IntClr |= 0x00000008;	//����жϱ�־
//		timer3_start();
//		COC_Timer = timer3_get_TC();
//		while(COC_Status == COC_Pass){
//			if(timer3_get_TC() > COC_Timer+COC_Delay_Time){
//				DSG_ON;
//				CHG_OFF;
//				Light_AlarmLED;	//����������
//				BAT_Protect_Status |= COC_bit;			//�ñ���λ
//			}
//		}
//		timer3_TC_Reset();
//		timer3_stop();				
//	}	
//	if((LPC_GPIOINT->IO2IntStatF & 0x00000010)>0){	//�ж�P2.4�ж�	DOC1�ж�
//		LPC_GPIOINT->IO2IntClr |= 0x00000010;	//����жϱ�־
//		timer3_start();
//		DOC1_Timer = timer3_get_TC();
//		while(DOC1_Status == DOC_Pass){
//			if(timer3_get_TC() > DOC1_Timer+DOC1_Delay_Time){
//				DSG_OFF;
//				CHG_ON;
//				Light_AlarmLED;	//����������
//				BAT_Protect_Status |= DOC1_bit;			//�ñ���λ
//			}
//		}
//		timer3_TC_Reset();
//		timer3_stop();		
//	}	
//	if((LPC_GPIOINT->IO2IntStatF & 0x00000020)>0){	//�ж�P2.5�ж�	SC�ж�
//		LPC_GPIOINT->IO2IntClr |= 0x00000020;	//����жϱ�־
//		DSG_OFF;
//		CHG_OFF;
//		Light_AlarmLED;	//����������
//		BAT_Protect_Status |= SC_bit;			//�ñ���λ
//	}	
}

void SysTick_Handler(void){
	ms_interrupt_status = 1;
	//UART1���ճ�ʱ�ж�
	if(uart1_receivestatus == Receive_Doing) uart1_received_OT++;
	if(uart1_received_OT>Receive_OT_Threshold){
		LPC_UART1->IER &= 0xFFFFFFFE;
		uart1_receivestatus = Receive_Done;
		uart1_received_OT = 0;
	}
	//UART2���ճ�ʱ�ж�
	if(uart2_receivestatus == Receive_Doing) uart2_received_OT++;
	if(uart2_received_OT>Receive_OT_Threshold){
		LPC_UART2->IER &= 0xFFFFFFFE;
		uart2_receivestatus = Receive_Done;
		uart2_received_OT = 0;
	}	
	//UART3���ճ�ʱ�ж�
	if(uart3_receivestatus == Receive_Doing) uart3_received_OT++;
	if(uart3_received_OT>Receive_OT_Threshold){
		LPC_UART3->IER &= 0xFFFFFFFE;
		uart3_receivestatus = Receive_Done;
		uart3_received_OT = 0;
	}
}

void Can_ReceiveData_Deal(void){
	if(CAN_RxRdy[0] > 0){
		CAN_RxRdy[0] = 0;
		if(CAN_RxMsg[0].id == (CAN_Addr_Base|0x00000FFF)){
			if(
				(CAN_RxMsg[0].data[0] == 0xEE) &&
				(CAN_RxMsg[0].data[1] == 0xEE) &&
				(CAN_RxMsg[0].data[2] == 0xDD) &&
				(CAN_RxMsg[0].data[3] == 0xDD) &&
				(CAN_RxMsg[0].data[4] == 0xED) &&
				(CAN_RxMsg[0].data[5] == 0xED) &&
				(CAN_RxMsg[0].data[6] == 0xDE) &&
				(CAN_RxMsg[0].data[7] == 0xDE)		
			){
				RUN_Status = 0xFF;
			}				
		}	
	}
}


int main(void){
	GPIO_PortClock(1);
	LED1_DIR_OUTPUT;
	LED2_DIR_OUTPUT;	
	KEY_DIR_INPUT;
	LED1_SET_1;
	LED2_SET_1;		
	IO_Init();
	
	LED_light(5);
	Light_AlarmLED;
	
	GPIO_SetDir(1,17,GPIO_DIR_OUTPUT);
	GPIO_PinWrite(1,17,0);
	GPIO_SetDir(1,16,GPIO_DIR_OUTPUT);
	GPIO_PinWrite(1,16,0);
	GPIO_SetDir(1,15,GPIO_DIR_OUTPUT);
	GPIO_PinWrite(1,15,0);
	GPIO_SetDir(1,14,GPIO_DIR_OUTPUT);
	GPIO_PinWrite(1,14,0);
	GPIO_SetDir(1,10,GPIO_DIR_OUTPUT);
	GPIO_PinWrite(1,10,0);
	GPIO_SetDir(0,19,GPIO_DIR_OUTPUT);
	GPIO_PinWrite(0,19,1);
	SelfPWR_Ctrl_SET_1;
	
	
	LED2_SET_0;
	LED1_SET_0;	
	
	while(KEY_Read == 0);
	uart1_init();
	uart2_init();
	uart3_init();
	SPI_init();
//	SSP0_init();
	SSP1_init();
	ad_init();	//��Ƭ��AD���� δʹ��
	i2c0_init();
	timer3_init();
	SysTick_Config(100000);

	init_PEC15_Table();
	LTC6811_CMD_init();	
	ad_start();
//	EINT_GPIO_Init();

	RTCInit();
//  local_time.RTC_Sec = 0;
//  local_time.RTC_Min = 30;
//  local_time.RTC_Hour = 15;
//  local_time.RTC_Mday = 26;
//  local_time.RTC_Wday = 0;
//  local_time.RTC_Yday = 26;		/* current date 05/12/2010 */
//  local_time.RTC_Mon = 1;
//  local_time.RTC_Year = 2020;
//  RTCSetTime( local_time );		/* Set local time */	
	RTCStart();
	
	CAN_setup(CAN1);
	CAN_setup(CAN2);	
//	CAN_SetACCF_Lookup();
	CAN_start(CAN1);	
	CAN_start(CAN2);
	CAN_waitReady(CAN1);	
	CAN_waitReady(CAN2);

	while(1){
		if(ms_interrupt_status == 1){
			ms_interrupt_status = 0;
			
			SYSTimer_ms_Counter++;
			BAT_Timer = SYSTimer_num = SYSTimer_ms_Counter;
			
//			Can_ReceiveData_Deal();
			
			if((SYSTimer_ms_Counter%1000) == 0){
				SYSRun_Seconds++;	
			}
			
			KEYSCAN();
			if(PRESSED > 0){
				PRESSED = 0;
				if(SYSRun_Seconds > 3) RUN_Status = 0xFF;
			}			
			if(RUN_Status == 0xFF) k++;
			
			AD7124_mission();
			
//			//��Ƭ��AD���� δʹ��
//			if(Measure_Num > 20){
//				ad();
//			}
			LTC6811_Mission();	

			switch(RUN_Status){
				case 0x00:		//�ж�Ϊ��ʼ����	
					if(LTC6811_Init_Status == 0x80){	//�ж�Ϊ6811�Լ����
						RUN_Status = 0x10;							//��ʼ��6811�Լ�δͨ��������
					}
					switch(SYSTimer_ms_Counter){
						case 700:
							EE_DATE_Receive[0] = 0xA0;
							EE_DATE_Receive[1] = 0x01;
							EE_DATE_Receive[2] = 0x01;
							EE_DATE_Receive[3] = 0xA1;
							i2c0_start(EE_DATE_Receive,30);							
							break;
						case 1200:
							EE_DATE_Receive_Check[0] = 0xA0;
							EE_DATE_Receive_Check[1] = 0x01;
							EE_DATE_Receive_Check[2] = 0x01;
							EE_DATE_Receive_Check[3] = 0xA1;
							i2c0_start(EE_DATE_Receive_Check,30);							
							break;						
						case 1650:
							if(Array_Check(EE_DATE_Receive,EE_DATE_Receive_Check,30) == 0) SYSTimer_ms_Counter = 650;					
							break;
						case 1700:
							EE_Date_Read();
							if(EE_Flag != EE_Flag_def){	//�ж�EE��ȡ����
								RUN_Status = 0x20;				//��ʼ��EE��ȡ���������
							}else{
								uart3_send(buff,11);
							}				
							break;
						case 1800:
//							From_CellVolt_Get_RemCapPer();//���ݵ����ѹ�ж�ʣ������ �رոù���
														
							DSG_CAP_Lifetime_mAh = EE_DSG_CAP_Lifetime_mAh;
							CHG_CAP_Lifetime_mAh = EE_CHG_CAP_Lifetime_mAh;
							Rem_CAP_mAh = EE_Rem_CAP_mAh;
							Remain_CAP_Percent = EE_Remain_CAP_Percent;
							Full_CAP = EE_Full_CAP;
							DSG_CAP_Lifetime_mAh = EE_DSG_CAP_Lifetime_mAh;
							CHG_CAP_Lifetime_mAh = EE_CHG_CAP_Lifetime_mAh;
							Cycle_Times = EE_Cycle_Times;						
//							Rem_CAP_mAh = 18700;	//��EEPROM��ʣ������ֵ����ȷʱ�������ֶ���������ֵ���򿪸��У�����ֵ�����أ����θ���������һ�Ρ�
						
							//��ȡ�ĵ����ٷֱȴ��ڸ��ݵ�ѹ�жϵģ��ҳ���5%��ֵ�����ж�Ϊ��ط���ʱ��ܾ������Էŵ��ԭ����������ƫ�
//							if(
//									((EE_Remain_CAP_Percent > _Remain_CAP_Percent) && ((EE_Remain_CAP_Percent - _Remain_CAP_Percent) > 20)) ||
//									((_Remain_CAP_Percent > EE_Remain_CAP_Percent) && ((_Remain_CAP_Percent - EE_Remain_CAP_Percent) > 20))
//								){	
//								Remain_CAP_Percent = _Remain_CAP_Percent;
//								Full_CAP = DFC;	
//								Rem_CAP_mAh = (DFC*Remain_CAP_Percent)/100;						
//								BAT_Protect_Status |= MTA_bit;			//��ά���澯
//							}else{
//								Rem_CAP_mAh = EE_Rem_CAP_mAh;
//								Remain_CAP_Percent = EE_Remain_CAP_Percent;
//								Full_CAP = EE_Full_CAP;
//								DSG_CAP_Lifetime_mAh = EE_DSG_CAP_Lifetime_mAh;
//								CHG_CAP_Lifetime_mAh = EE_CHG_CAP_Lifetime_mAh;
//								Cycle_Times = EE_Cycle_Times;
//								BAT_Protect_Status &= ~MTA_bit;			//��ά���澯
//							}

							if(Remain_CAP_Percent == 0) BAT_Work_Status |= FCCF_bit;	//��������У׼��־
							break;
						case 5000:
							From_Temp_Get_RTCap();
							CAP_Temp_para_old = CAP_Temp_para;
							Full_CAP_RT = (Full_CAP*CAP_Temp_para)/100;						
							RUN_Status = 0x01;				//��ʼ�����
							CHG_MOS(1);
							DSG_MOS(1);
							break;							
					}
					break;
				case 0x01:		//�ж�Ϊ�Լ�ͨ��
					//������ʱ��ʱ
					//CeOV�����ͻָ���ʱ
					if((BAT_Protect_Alarm&CeOV_bit)>0) 	CeOV_Timer++;
					//CeUV�����ͻָ���ʱ
					if((BAT_Protect_Alarm&CeUV_bit)>0) 	CeUV_Timer++;
					//PkOV�����ͻָ���ʱ
					if((BAT_Protect_Alarm&PkOV_bit)>0) 	PkOV_Timer++;	
					//PkUV�����ͻָ���ʱ
					if((BAT_Protect_Alarm&PkUV_bit)>0) 	PkUV_Timer++;

					if(CH_Volt[0] == 0) RUN_Status = 0x10;	//��⵽ADоƬͨ��0���������ͨ�����ĵ�ѹΪ0ʱ��ͨ����Current_offset����������ΪADоƬ�����쳣���Զ��ػ�
				
					if(Measure_Num > 52){
//						NVIC_EnableIRQ(EINT3_IRQn);

						BAT();
						BAT_Protect_Status_get_slow();
						Protection_deal();
//						Heat();						
						LED();						
//						BAT_CANSend();	//���������CAN���ͺ���
					}				
					
					//���ж�
					if((SYSTimer_ms_Counter%1000) == 0){
						SYSTimer_s_Counter++;
						
//						//�������ж�
//						if(((BAT_Work_Status&CHGing_bit)==CHGing_bit) && ((Current&0x7FFFFFFF)<CHG_Stop_Current_Threshold) && (BAT_Voltage>(3450*72))){
//							CHGStop_Timer++;
//							if(CHGStop_Timer > CHG_Stop_Timer_Threshold)	BAT_Work_Status |= CHG_Complete_bit;
//						}else CHGStop_Timer = 0;					
						
						//OT�����ͻָ���ʱ
						if((BAT_Protect_Alarm&OT_bit)>0) 	OT_Timer++;		
						//UT�����ͻָ���ʱ
						if((BAT_Protect_Alarm&UT_bit)>0) 	UT_Timer++;	
						
//						if((BAT_Protect_Status&COC_bit)>0){	//��ʱ30s�Իָ�
//							COC_Delay_Timer++;
//							if(COC_Delay_Timer > 30){
//								COC_Delay_Timer = 0;
//								BAT_Protect_Status &= ~COC_bit;
//							}
//						}
//						if((BAT_Protect_Status&DOC0_bit)>0){//��ʱ30s�Իָ�
//							DOC_Delay_Timer++;
//							if(DOC_Delay_Timer > 30){
//								DOC_Delay_Timer = 0;
//								BAT_Protect_Status &= ~DOC0_bit;
//							}							
//						}
//						if((BAT_Protect_Status&CeUV_bit)>0){//��ʱ30s�Իָ�
//							CeUV_Delay_Timer++;
//							if(CeUV_Delay_Timer > 30){
//								CeUV_Delay_Timer = 0;
//								BAT_Protect_Status &= ~CeUV_bit;
//							}							
//						}
//						if((BAT_Protect_Status&CeOV_bit)>0){//��ʱ30s�Իָ�
//							CeOV_Delay_Timer++;
//							if(CeOV_Delay_Timer > 30){
//								CeOV_Delay_Timer = 0;
//								BAT_Protect_Status &= ~CeOV_bit;
//							}							
//						}						
//						if((BAT_Protect_Status&PkOV_bit)>0){//��ʱ30s�Իָ�
//							PkOV_Delay_Timer++;
//							if(PkOV_Delay_Timer > 30){
//								PkOV_Delay_Timer = 0;
//								BAT_Protect_Status &= ~PkOV_bit;
//							}							
//						}
//						if((BAT_Protect_Status&PkUV_bit)>0){//��ʱ30s�Իָ�
//							PkUV_Delay_Timer++;
//							if(PkUV_Delay_Timer > 30){
//								PkUV_Delay_Timer = 0;
//								BAT_Protect_Status &= ~PkUV_bit;
//							}							
//						}						
						
						current_time = RTCGetTime();
						if(	(current_time.RTC_Year > 2020)	||
								(current_time.RTC_Yday > MTA_Alarm_days)
						){
							BAT_Protect_Status |= MTA_bit;			//��ά���澯
						}
						
						//2min�洢һ��EEPROM
						if((SYSTimer_s_Counter>10) && ((SYSTimer_s_Counter%120) == 0)){	
							EE_Date_Save();
							i2c0_start(EE_DATE_Send,30);
						}
						
						//AlarmLED��˸
						if(AlarmLED_Flash_Flag == 1){
							if((SYSTimer_s_Counter%2) == 0)	AlarmLed_SET_0;
							if((SYSTimer_s_Counter%2) == 1)	AlarmLed_SET_1;
						}
						
						//���ʹ�������
						if(SYSTimer_s_Counter > 3){
							i++;
							switch(i){
								case 1:
									LTC6811_UartSend();
									LED2_SET_0;
									break;
								case 2:
//									ad_UartSend();
									LED2_SET_1;
									break;				
								case 3:
									BAT_UartSend();
									break;
								case 4:
									AD7124_UartSend();
									break;
								case 5:
//									EE_Date_Save();
//									i2c0_start(EE_DATE_Send,30);
									break;
								case 6:
//									CAN_test(CAN1);
//									EE_DATE_Receive[0] = 0xA0;
//									EE_DATE_Receive[1] = 0x01;
//									EE_DATE_Receive[2] = 0x01;
//									EE_DATE_Receive[3] = 0xA1;
//									i2c0_start(EE_DATE_Receive,30);
									break;
								case 7:
									current_time = RTCGetTime();
									RTC_Send(current_time);
									break;
								case 10:
//									CAN_test(CAN2);						
									i = 0;
									break;						
							}
						}				
					}
					break;
				case 0xFF:		//�ж�Ϊ�ػ�
					switch(k){
						case 1:
							CHG_MOS(0);
							DSG_MOS(0);
							break;
						case 2:
							EE_Date_Save();
							i2c0_start(EE_DATE_Send,30);
							break;
						case 100:
							EE_DATE_Receive[0] = 0xA0;
							EE_DATE_Receive[1] = 0x01;
							EE_DATE_Receive[2] = 0x01;
							EE_DATE_Receive[3] = 0xA1;
							i2c0_start(EE_DATE_Receive,30);	
							break;
						case 200:
							if(EE_Date_Read_Check() == 0) k = 0;
							break;
						case 210://��2020��1��1��0ʱ0��0����Ϊ��׼ʱ�䣬�ڹػ�����RTC��ʱ���ڿ���ʱ��ȡRTCʱ�����׼ʱ��Ƚϣ�����
							local_time.RTC_Sec = 0;
							local_time.RTC_Min = 0;
							local_time.RTC_Hour = 0;
							local_time.RTC_Mday = 1;
							local_time.RTC_Wday = 3;
							local_time.RTC_Yday = 1;
							local_time.RTC_Mon = 1;
							local_time.RTC_Year = 2020;
							RTCSetTime( local_time );		/* Set local time */	
							RTCStart();
						case 250:
							SHUTDOWN();
							while(1)
							break;						
					}
					break;
				case 0x10:	//�ж�Ϊ��ʼ���Լ�ʧ��
					LED1_SET_0;
					SHUTDOWN();
					break;
				case 0x20:	//�ж�Ϊ��ʼ���Լ�ʧ��
					LED1_SET_0;
					SHUTDOWN();
					break;
			}
		}
	}
}

/*
	END OF FILE
*/

