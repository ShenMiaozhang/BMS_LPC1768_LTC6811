#include "BAT.h"
/*	BAT_Work_Status��λ��������
//bit24:FCCF					(Full_CAP_Cali_Flag)������У׼��־
//bit17:CHG_Complete	�����ɱ�־
//bit16:CHG_Plugged		���������
//bit10:Heating				����״̬ ������
//bit9:CHGing					����״̬ �����		������С��CHG_Stop_Current_Threshold����1min����Ϊ�����ɣ������������ӡ�
//bit8:DSGing					����״̬ �ŵ���
//bit3:SelfPWR_mos	  �Թ��翪��״̬	1��ͨ 0�Ͽ�
//bit2:Heat_mos				���ȿ���״̬	1��ͨ 0�Ͽ�
//bit1:CHG_mos				��翪��״̬	1��ͨ 0�Ͽ�	
//bit0:DSG_mos				�ŵ翪��״̬	1��ͨ 0�Ͽ�					*/
uint32_t BAT_Work_Status = 0;									//﮵�ع���״̬

/*	BAT_Protect_Status��λ��������
//bit20:MTA						(Maintain Alarm)ά������
//bit19:HOT						���ȳ���
//bit18:HOC						���ȹ���
//bit17:Freeze				0�����¸澯
//bit16:CapLow				�����͸澯
//bit10:TUB						�¶Ȳ�����澯
//bit9:UT							����	���ڲ�Ʒ��ʹ���¶ȷ�Χ
//bit8:OT							����	���ڲ�Ʒ��ʹ���¶ȷ�Χ
//bit7:PkUV	  				������ܵ�ѹǷѹ
//bit6:PkOV						������ܵ�ѹ��ѹ
//bit5:CeUV	  				����Ƿѹ
//bit4:CeOV						���峬ѹ
//bit3:COC						������
//bit2:DOC0						�ŵ����0��
//bit1:DOC1						�ŵ����1��
//bit0:SC							��·					*/
uint32_t BAT_Protect_Status = 0;						//﮵�ر���״̬
uint32_t BAT_Protect_Alarm = 0;							//﮵�ر����澯	�ڴﵽ����ֵ��Ϊ�ﵽ��ʱֵʱ��λ�ñ�����Ӧλ���ڴﵽ��ʱֵʱ����ñ�������λBAT_Protect_Status��
																						//��BAT_Protect_StatusΪ1ʱ��BAT_Protect_AlarmҲΪ1��ʾ�ָ������ﵽ����ʱֵδ�ﵽ
//mos_Status����Protection_deal(void)������Ӧ�ã�Ϊ1��ʾ��Ҫ��mos��Ϊ0��ʾ��Ҫ�ر�mos
uint8_t CHG_mos_Status = 0;
uint8_t DSG_mos_Status = 0;
uint8_t Heat_mos_Status = 0;

//���ݵ�ѹ�жϵ���ֵʱ�ĵ����ѹ�ο�(0.1mV)
uint16_t 	BatVol_Ref[20] = {27000,29000,30000,30300,30700,31000,31200,31400,31600,31800,32000,32100,32150,32200,32300,32450,32600,32800,33000,33500};
//����ѹ�ο�ֵ��Ӧ��ʣ�����ֵ(%)
uint8_t BatCapPer_Ref[20] = {0,0,0,3,10,17,27,35,43,50,55,60,65,70,75,80,85,90,95,100};
uint8_t _Remain_CAP_Percent;		//���ݵ����ѹ�������ʣ�������ٷֱ�
	
uint8_t BatTemp_Ref[20] = 						{60,65,70,75,80,85,90,95,100,	105,110,115,120,125,130,135,140,145,150,155};	//100��Ӧ0��
uint8_t BatTemp_CapAttenuation[20] = 	{60,63,67,72,80,84,87,90,92,	95,	98,	100,100,100,100,99,	98,	98,	98,	98};
uint8_t CAP_Temp_para = 100;			//�������¶ȵ�˥��ϵ�����ٷ��ƣ��������仯ʱ������������ʣ������
uint32_t CAP_Temp_para_times = 0;	//
uint8_t CAP_Temp_para_old = 100;	//��ֵ �������¶ȵ�˥��ϵ�����ٷ���
	
//�������EEPROM�ж�ȡ����ֵ
uint32_t EE_BAT_Voltage;
uint32_t EE_Rem_CAP_mAh;
uint8_t EE_Remain_CAP_Percent;
uint32_t EE_Full_CAP;
uint32_t EE_DSG_CAP_Lifetime_mAh;	
uint32_t EE_CHG_CAP_Lifetime_mAh;	
uint16_t EE_Cycle_Times;
uint16_t EE_Flag;		//EE�洢��β��־ EE_Flag_def
	
uint32_t CHGStop_Timer = 0;						//������С��CHG_Stop_Current_Thresholdʱ��ʼ��ʱ������ʱ����	
uint32_t DOC1_Timer = 0;							//�ŵ����1������ʱ�ͻָ���ʱ��ʱ
uint32_t DOC0_Timer = 0;							//�ŵ����0������ʱ�ͻָ���ʱ��ʱ
uint32_t COC_Timer = 0;								//������������ʱ�ͻָ���ʱ��ʱ																						
uint32_t CeOV_Timer = 0;							//���峬ѹ������ʱ�ͻָ���ʱ��ʱ
uint32_t CeUV_Timer = 0;							//����Ƿѹ������ʱ�ͻָ���ʱ��ʱ
uint32_t PkOV_Timer = 0;							//�ܵ�ѹ��ѹ������ʱ�ͻָ���ʱ��ʱ
uint32_t PkUV_Timer = 0;							//�ܵ�ѹǷѹ������ʱ�ͻָ���ʱ��ʱ
uint32_t OT_Timer = 0;								//���´�����ʱ�ͻָ���ʱ��ʱ
uint32_t UT_Timer = 0;								//���´�����ʱ�ͻָ���ʱ��ʱ
uint32_t BAT_Timer = 0;								//ͬSYSTimer_ms_Counter��Ϊ����ʱ�Ӽ���
uint32_t CAP_Timer = 0;								//�ϴ������������ʱ�ĺ���ʱ����
uint32_t CANSend_Timer = 0;						//ͬSYSTimer_ms_Counter��CAN���ͼ�ʱ
uint32_t DOC_Delay_Timer = 0;					//DOC��ʱ�ָ���ʱ
uint32_t COC_Delay_Timer = 0;					//COC��ʱ�ָ���ʱ
uint32_t CeUV_Delay_Timer = 0;				//CeUV��ʱ�ָ���ʱ
uint32_t CeOV_Delay_Timer = 0;				//CeOV��ʱ�ָ���ʱ
uint32_t PkUV_Delay_Timer = 0;				//PkUV��ʱ�ָ���ʱ
uint32_t PkOV_Delay_Timer = 0;				//PkOV��ʱ�ָ���ʱ

uint32_t CeUV_Counter = 0;							
uint32_t CeOV_Counter = 0;
uint32_t PkUV_Counter = 0;							
uint32_t PkOV_Counter = 0;
uint32_t OT_Counter = 0;							
uint32_t UT_Counter = 0;							
uint32_t Freeze_Counter = 0;
uint32_t TUB_Counter = 0;

uint32_t Full_CAP = 20000;										//������			(mAh)
uint32_t Full_CAP_RT = 20000;									//ʵʱ������	(mAh)	RT��RealTime
uint32_t Remain_CAP_mAms;						//ʣ������		(mAms)	������Remain_CAP_mAms���ۼӺ��ۼ����ۼӺ����1mAhʱ��Rem_CAP_mAh��1���ۼ�����ʱ��Rem_CAP_mAh��1
uint32_t Rem_CAP_mAh = 20000;									//ʣ������		(mAh)
//uint32_t Remain_CAP_RT_mAh;						//ʣ������		(mAh)	RT��RealTime
uint32_t Designed_CAP = DFC;					//�������		(mAh)
uint8_t Remain_CAP_Percent;						//ʣ������		(%)		(ʣ������/�������)*100%
uint32_t Current = 0;									//����				(mA)	�ŵ�ʱΪ�������ʱΪ�������λΪ1��ʾΪ�������λΪ0��ʾΪ��
uint16_t DSG_Rate = 0;								//�ŵ籶��		����100�ı�ֵ��Ϊ����ֵ��100��Ӧ1C��50��Ӧ0.5C��200��Ӧ2C
uint16_t CHG_Rate = 0;								//��籶��		����100�ı�ֵ��Ϊ����ֵ��100��Ӧ1C��50��Ӧ0.5C��200��Ӧ2C
uint32_t Heat_Current = 0;						//���ȵ���		(mA)	����ʱ����Ϊ��
uint32_t BAT_Voltage = 0;							//������ѹ	(mV)
uint32_t Port_Voltage = 0;						//����ڵ�ѹ	(mV)

uint32_t DSG_CAP_Lifetime_mAh;				//����ۼƷŵ���
uint32_t DSG_CAP_Lifetime_mAms;				//����ۼƷŵ���	��1mAh��DSG_CAP_Lifetime_mAh��1
uint32_t CHG_CAP_Lifetime_mAh;				//����ۼƳ����
uint32_t CHG_CAP_Lifetime_mAms;				//����ۼƳ����	��1mAh��CHG_CAP_Lifetime_mAh��1
uint16_t Cycle_Times;									//ѭ������		DSG_CAP_Lifetime_mAh/DFC �ܷŵ��������������

uint8_t Full_CAP_Cali_Flag = 0;				//������У׼��־��У׼��ʽ���ڳ����½��У��ȷŵ���Ƿѹ���ڷŵ�Ƿѹ��Full_CAP_Cali_Flag��1����ʱ����ʣ������
																			//Ȼ�󲻹ػ�ֱ�ӽ��г�磬�������������Full_CAP_Cali_Current_Thresholdʱ��������ʼʼ�յ���ʣ������
																			//������С��CHG_Stop_Current_Threshold����1min����Ϊ�����ɣ������������ӡ�
uint8_t AlarmLED_Flash = 0;															
uint8_t AlarmLED_Flash_Flag = 0;			//��������˸��־��Ϊ1ʱ��ʼ��˸��Ϊ0ʱֹͣ
uint8_t AlarmLED_Light_Flag = 0;			//�����Ƴ�����־
uint8_t BATUartSendData[500];

uint8_t BATUartSendbuff[30];

uint8_t EE_DATE_Send[50] = {0xA0,0x00,0x01};
uint8_t EE_DATE_Receive[50] = {0xA0,0x00,0x01,0xA1};
uint8_t EE_DATE_Receive_Check[50] = {0xA0,0x00,0x01,0xA1};

//// CAP������
uint32_t buff_mAh = 0,buff_mAms = 0;
//uint32_t buff = 0,buff1 = 0;

uint8_t Array_Check(uint8_t a[],uint8_t b[],uint8_t num){
	uint8_t i,c;
	c = 1;
	for(i=0;i<num;i++){
		if(a[i] != b[i])	c = 0;
	}
	return c; 
}
void EE_Date_Save(void){
	EE_DATE_Send[0] = 0xA0;
	EE_DATE_Send[1] = 0x01;
	EE_DATE_Send[2] = 0x01;	
	EE_DATE_Send[3] = (BAT_Voltage&0xFF000000)>>24;
	EE_DATE_Send[4] = (BAT_Voltage&0x00FF0000)>>16;	
	EE_DATE_Send[5] = (BAT_Voltage&0x0000FF00)>>8;	
	EE_DATE_Send[6] = (BAT_Voltage&0x000000FF)>>0;	
	EE_DATE_Send[7] = (Rem_CAP_mAh&0xFF000000)>>24;
	EE_DATE_Send[8] = (Rem_CAP_mAh&0x00FF0000)>>16;	
	EE_DATE_Send[9] = (Rem_CAP_mAh&0x0000FF00)>>8;	
	EE_DATE_Send[10] = (Rem_CAP_mAh&0x000000FF)>>0;	
	EE_DATE_Send[11] = Remain_CAP_Percent;		
	EE_DATE_Send[12] = (Full_CAP&0xFF000000)>>24;
	EE_DATE_Send[13] = (Full_CAP&0x00FF0000)>>16;	
	EE_DATE_Send[14] = (Full_CAP&0x0000FF00)>>8;	
	EE_DATE_Send[15] = (Full_CAP&0x000000FF)>>0;
	EE_DATE_Send[16] = (DSG_CAP_Lifetime_mAh&0xFF000000)>>24;
	EE_DATE_Send[17] = (DSG_CAP_Lifetime_mAh&0x00FF0000)>>16;	
	EE_DATE_Send[18] = (DSG_CAP_Lifetime_mAh&0x0000FF00)>>8;	
	EE_DATE_Send[19] = (DSG_CAP_Lifetime_mAh&0x000000FF)>>0;	
	EE_DATE_Send[20] = (CHG_CAP_Lifetime_mAh&0xFF000000)>>24;
	EE_DATE_Send[21] = (CHG_CAP_Lifetime_mAh&0x00FF0000)>>16;	
	EE_DATE_Send[22] = (CHG_CAP_Lifetime_mAh&0x0000FF00)>>8;	
	EE_DATE_Send[23] = (CHG_CAP_Lifetime_mAh&0x000000FF)>>0;	
	EE_DATE_Send[24] = (Cycle_Times&0xFF00)>>8;	
	EE_DATE_Send[25] = (Cycle_Times&0x00FF)>>0;	
	
	EE_DATE_Send[26] = EE_Flag_def>>8;	
	EE_DATE_Send[27] = EE_Flag_def&0x00FF;	
}

uint8_t EE_Date_Read_Check(void){
	uint8_t i;
	EE_DATE_Receive[0] = 0xA0;
	EE_DATE_Receive[1] = 0x00;
	EE_DATE_Receive[2] = 0x01;
	EE_DATE_Receive[3] = 0xA1;
	EE_BAT_Voltage = ((uint32_t)EE_DATE_Receive[4]<<24)+((uint32_t)EE_DATE_Receive[5]<<16)+((uint32_t)EE_DATE_Receive[6]<<8)+((uint32_t)EE_DATE_Receive[7]<<0);
	EE_Rem_CAP_mAh = ((uint32_t)EE_DATE_Receive[8]<<24)+((uint32_t)EE_DATE_Receive[9]<<16)+((uint32_t)EE_DATE_Receive[10]<<8)+((uint32_t)EE_DATE_Receive[11]<<0);
	EE_Remain_CAP_Percent = EE_DATE_Receive[12];
	EE_Full_CAP = ((uint32_t)EE_DATE_Receive[13]<<24)+((uint32_t)EE_DATE_Receive[14]<<16)+((uint32_t)EE_DATE_Receive[15]<<8)+((uint32_t)EE_DATE_Receive[16]<<0);
	EE_DSG_CAP_Lifetime_mAh = ((uint32_t)EE_DATE_Receive[17]<<24)+((uint32_t)EE_DATE_Receive[18]<<16)+((uint32_t)EE_DATE_Receive[19]<<8)+((uint32_t)EE_DATE_Receive[20]<<0);
	EE_CHG_CAP_Lifetime_mAh = ((uint32_t)EE_DATE_Receive[21]<<24)+((uint32_t)EE_DATE_Receive[22]<<16)+((uint32_t)EE_DATE_Receive[23]<<8)+((uint32_t)EE_DATE_Receive[24]<<0);
	EE_Cycle_Times = ((uint16_t)EE_DATE_Receive[25]<<8)+((uint16_t)EE_DATE_Receive[26]<<0);
	EE_Flag = ((uint16_t)EE_DATE_Receive[27]<<8)+((uint16_t)EE_DATE_Receive[28]<<0);

	i = 1;
	if(EE_BAT_Voltage != BAT_Voltage) i = 0;
	if(EE_Rem_CAP_mAh != Rem_CAP_mAh) i = 0;
	if(EE_Remain_CAP_Percent != Remain_CAP_Percent) i = 0;
	if(EE_Full_CAP != Full_CAP) i = 0;	
	if(EE_DSG_CAP_Lifetime_mAh != DSG_CAP_Lifetime_mAh) i = 0;
	if(EE_CHG_CAP_Lifetime_mAh != CHG_CAP_Lifetime_mAh) i = 0;	
	if(EE_Cycle_Times != Cycle_Times) i = 0;
	if(EE_Flag != EE_Flag_def) i = 0;
	return i;
}

void EE_Date_Read(void){
	EE_DATE_Receive[0] = 0xA0;
	EE_DATE_Receive[1] = 0x01;
	EE_DATE_Receive[2] = 0x01;
	EE_DATE_Receive[3] = 0xA1;
	EE_BAT_Voltage = ((uint32_t)EE_DATE_Receive[4]<<24)+((uint32_t)EE_DATE_Receive[5]<<16)+((uint32_t)EE_DATE_Receive[6]<<8)+((uint32_t)EE_DATE_Receive[7]<<0);
	EE_Rem_CAP_mAh = ((uint32_t)EE_DATE_Receive[8]<<24)+((uint32_t)EE_DATE_Receive[9]<<16)+((uint32_t)EE_DATE_Receive[10]<<8)+((uint32_t)EE_DATE_Receive[11]<<0);
	EE_Remain_CAP_Percent = EE_DATE_Receive[12];
	EE_Full_CAP = ((uint32_t)EE_DATE_Receive[13]<<24)+((uint32_t)EE_DATE_Receive[14]<<16)+((uint32_t)EE_DATE_Receive[15]<<8)+((uint32_t)EE_DATE_Receive[16]<<0);
	EE_DSG_CAP_Lifetime_mAh = ((uint32_t)EE_DATE_Receive[17]<<24)+((uint32_t)EE_DATE_Receive[18]<<16)+((uint32_t)EE_DATE_Receive[19]<<8)+((uint32_t)EE_DATE_Receive[20]<<0);
	EE_CHG_CAP_Lifetime_mAh = ((uint32_t)EE_DATE_Receive[21]<<24)+((uint32_t)EE_DATE_Receive[22]<<16)+((uint32_t)EE_DATE_Receive[23]<<8)+((uint32_t)EE_DATE_Receive[24]<<0);
	EE_Cycle_Times = ((uint16_t)EE_DATE_Receive[25]<<8)+((uint16_t)EE_DATE_Receive[26]<<0);
	EE_Flag = ((uint16_t)EE_DATE_Receive[27]<<8)+((uint16_t)EE_DATE_Receive[28]<<0);
}

void CHG_MOS(uint8_t data){
	if(data == 1){
		CHG_ON;
		BAT_Work_Status_CHG_mos_ON;
	}else{
		CHG_OFF;
		BAT_Work_Status_CHG_mos_OFF;
	}
}
void DSG_MOS(uint8_t data){
	if(data == 1){
		DSG_ON;
		BAT_Work_Status_DSG_mos_ON;
	}else{
		DSG_OFF;
		BAT_Work_Status_DSG_mos_OFF;
	}
}
void Heat_MOS(uint8_t data){
	if(data == 1){
		Heat_ON;
		BAT_Work_Status_Heat_mos_ON;
	}else{
		Heat_OFF;
		BAT_Work_Status_Heat_mos_OFF;
	}
}

void From_Temp_Get_RTCap(void){	//�����������¶�ϵ����ͨ���¶Ȳ�������˥��ϵ�������ʵʱ����
	uint8_t	i;
	uint16_t buff;
	buff = TEMP_Min_Data;
	if(buff < BatTemp_Ref[0])		CAP_Temp_para = BatTemp_CapAttenuation[0];
	if(buff > BatTemp_Ref[19])	CAP_Temp_para = BatTemp_CapAttenuation[19];
	for(i=0;i<19;i++){
		if((buff > BatTemp_Ref[i]) && (buff < BatTemp_Ref[i+1]))	CAP_Temp_para = BatTemp_CapAttenuation[i];
	}
}

void From_CellVolt_Get_RemCapPer(void){	//���ݵ����ѹ����ʣ�������ٷֱ� ������Ϊ_Remain_CAP_Percent
	uint8_t	i;
	if(Cell_Volt_Ave < BatVol_Ref[0])	_Remain_CAP_Percent = 0;
	if(Cell_Volt_Ave > BatVol_Ref[19])	_Remain_CAP_Percent = 100;
	for(i=0;i<19;i++){
		if((Cell_Volt_Ave > BatVol_Ref[i]) && (Cell_Volt_Ave < BatVol_Ref[i+1]))	_Remain_CAP_Percent = BatCapPer_Ref[i];
	}
}

void BAT_Voltage_get(void){
	uint8_t i;
	uint32_t buff = 0;
	for(i=0;i<LTC6811_DeviceNUM;i++){
		buff += (DEVICE[i].SCVolt * 2);
	}
	BAT_Voltage = buff;
}

void Port_Voltage_get(void){
	Port_Voltage = 200*Port_voltage_CHN;
}

void Current_get(void){
	uint32_t cur_buff = 0;
	if(Current_CHN <= Current_offset){											//�ŵ�
		cur_buff = (20*(Current_offset - Current_CHN));	//buffΪ����ֵ����λΪmA
		DSG_Rate = (100*cur_buff)/Designed_CAP;										//����ŵ籶��
		if(cur_buff > Current_Lowest_threshold){									//�������������ֵ�������ŵ�
			Current = 0x80000000|cur_buff;
			BAT_Work_Status |= DSGing_bit;											//��λ �ŵ��� ��־
			BAT_Work_Status &= ~CHGing_bit;											//��λ ����� ��־
		}else{																								//����С�������ֵ���������ŵ�
			Current = 0;
			BAT_Work_Status &= ~CHGing_bit;											//��λ ����� ��־
			BAT_Work_Status &= ~DSGing_bit;											//��λ �ŵ��� ��־
		}
	}else{																									//���
		cur_buff = (20*(Current_CHN - Current_offset));	//buffΪ����ֵ����λΪmA
		CHG_Rate = (100*cur_buff)/Designed_CAP;										//�����籶��
		if(cur_buff > Current_Lowest_threshold){
			Current = cur_buff;
			BAT_Work_Status |= CHGing_bit;											//��λ ����� ��־
			BAT_Work_Status &= ~DSGing_bit;											//��λ �ŵ��� ��־
		}else{																								//����С�������ֵ���������ŵ�
			Current = 0;
			BAT_Work_Status &= ~CHGing_bit;											//��λ ����� ��־
			BAT_Work_Status &= ~DSGing_bit;											//��λ �ŵ��� ��־
		}
	}
}

void CHG_Plugged_get(void){
	if((BAT_Work_Status&CHGing_bit) > 0)	BAT_Work_Status |= CHG_Plugged_bit;	//����������־��λ
	else																	BAT_Work_Status &= ~CHG_Plugged_bit;
	if((Port_Voltage-10000) > BAT_Voltage)				BAT_Work_Status |= CHG_Plugged_bit;	//����������־��λ
	else																	BAT_Work_Status &= ~CHG_Plugged_bit;	
}
void Heat_Current_get(void){
	uint32_t buff = 0;
	if(Heat_Current_CHN > Heat_Current_offset){	//����
		buff = 0;
		if(buff > Current_Lowest_threshold){
			Heat_Current = buff;
			BAT_Work_Status |= Heating_bit;	//�ü���λ
		}else{
			BAT_Work_Status &= ~Heating_bit;	//�����λ
		}
	}
}

void CAP(void){

	//�ŵ�
	if((BAT_Work_Status&DSGing_bit) == DSGing_bit){				//�ж�Ϊ�ŵ���
		//������ʷ�ŵ���
		DSG_CAP_Lifetime_mAms += (Current&0x7FFFFFFF);
		if(DSG_CAP_Lifetime_mAms > (3600*1000)){
			DSG_CAP_Lifetime_mAh += DSG_CAP_Lifetime_mAms / (3600*1000);
			DSG_CAP_Lifetime_mAms = DSG_CAP_Lifetime_mAms % (3600*1000);
		}
		
		//����ʣ������ ����
		if(Remain_CAP_mAms > (Current&0x7FFFFFFF)){
			Remain_CAP_mAms -= (Current&0x7FFFFFFF);
		}else{
			if(Rem_CAP_mAh > 0){
				Rem_CAP_mAh--;		
				Remain_CAP_mAms += 3600*1000;
				Remain_CAP_mAms -= (Current&0x7FFFFFFF);
			}else{
				Rem_CAP_mAh = 0;
				Remain_CAP_mAms = 0;			
			}
		}
//		BAT_Work_Status &= ~CHG_Complete_bit;	//������ɱ�־
//		BAT_Work_Status &= ~FCCF_bit;					//��������У׼��־
	}
	
	//���
	if((BAT_Work_Status&CHGing_bit) == CHGing_bit){
		if((BAT_Work_Status&CHG_Complete_bit) == 0){	//���δ���
			
			//������ʷ�����
			CHG_CAP_Lifetime_mAms += (Current&0x7FFFFFFF);						//��mAms
			if(CHG_CAP_Lifetime_mAms > (3600*1000)){									//�жϼ�mAms��ʣ��mAms�Ƿ����1mAh������ӵ�ʣ��mAh��
				CHG_CAP_Lifetime_mAh += CHG_CAP_Lifetime_mAms/(3600*1000);
				CHG_CAP_Lifetime_mAms = CHG_CAP_Lifetime_mAms%(3600*1000);
			}				
			
			//����ʣ������ ����
			Remain_CAP_mAms += (Current&0x7FFFFFFF);						//��mAms
			if(Remain_CAP_mAms > (3600*1000)){									//�жϼ�mAms��ʣ��mAms�Ƿ����1mAh������ӵ�ʣ��mAh��
				Rem_CAP_mAh++;
				Remain_CAP_mAms = Remain_CAP_mAms - (3600*1000);
				if(Rem_CAP_mAh >= MAX_DFC){
					Rem_CAP_mAh = MAX_DFC;
					Remain_CAP_mAms = 0;
				}
			}
			
			if(CellVolt_Max_Data>35500){
				Full_CAP = Full_CAP_RT = Rem_CAP_mAh;
			}
			
//			if((BAT_Work_Status&FCCF_bit) == FCCF_bit){
//				if(CellVolt_Max_Data>35500){
//					Full_CAP = Full_CAP_RT = Rem_CAP_mAh;
//				}
//			}else{
//				if(Rem_CAP_mAh > Full_CAP_RT) Full_CAP_RT = Rem_CAP_mAh;	//ʣ����������������ʱ����������
//				if(Full_CAP_RT > Full_CAP) Full_CAP = Full_CAP_RT;
//			}
		}
	}

	//�����Ժĵ�
	DSG_CAP_Lifetime_mAms += BMS_Cur;											//���Ժĵ���뵽��ʷ�ŵ�������
	if(Remain_CAP_mAms > BMS_Cur){
		Remain_CAP_mAms -= BMS_Cur;
	}else{
		if(Rem_CAP_mAh > 0){
			Rem_CAP_mAh--;
			Remain_CAP_mAms = Remain_CAP_mAms+3600*1000-BMS_Cur;
		}else{
			Rem_CAP_mAh = 0;
			Remain_CAP_mAms = 0;			
		}
	}
	
	Cycle_Times = DSG_CAP_Lifetime_mAh/((DFC*80)/100);					//����ѭ������	��ʷ�ŵ����������������80%
}

void Remain_CAP_Percent_get(void){	
	Remain_CAP_Percent = (100*Rem_CAP_mAh)/Full_CAP;
	if(((100*Rem_CAP_mAh)%Full_CAP)>(Full_CAP/2))	Remain_CAP_Percent += 1;
	if(Remain_CAP_Percent > 100){
		Remain_CAP_Percent = (100*Rem_CAP_mAh)/Full_CAP;
		if(((100*Rem_CAP_mAh)%Full_CAP)>(Full_CAP/2))	Remain_CAP_Percent += 1;
		if(Remain_CAP_Percent > 100){
			Remain_CAP_Percent = (100*Rem_CAP_mAh)/Full_CAP;
			if(((100*Rem_CAP_mAh)%Full_CAP)>(Full_CAP/2))	Remain_CAP_Percent += 1;
			if(Remain_CAP_Percent > 100) Remain_CAP_Percent = 100;
		}
	}	
	
//	Remain_CAP_Percent = (100*Rem_CAP_mAh)/Full_CAP_RT;
//	if(((100*Rem_CAP_mAh)%Full_CAP_RT)>(Full_CAP_RT/2))	Remain_CAP_Percent += 1;
//	if(Remain_CAP_Percent > 100){
//		Remain_CAP_Percent = (100*Rem_CAP_mAh)/Full_CAP_RT;
//		if(((100*Rem_CAP_mAh)%Full_CAP_RT)>(Full_CAP_RT/2))	Remain_CAP_Percent += 1;
//		if(Remain_CAP_Percent > 100){
//			Remain_CAP_Percent = (100*Rem_CAP_mAh)/Full_CAP_RT;
//			if(((100*Rem_CAP_mAh)%Full_CAP_RT)>(Full_CAP_RT/2))	Remain_CAP_Percent += 1;
//			if(Remain_CAP_Percent > 100) Remain_CAP_Percent = 100;
//		}
//	}
	
}


//���ȹ���		�澯�Ƴ������رռ��ȣ����¿����ָ�
//�ޱ���״̬ 	��ŵ�����
//�����͸澯	��˸�澯��
//�������		�ڷŵ�״̬�������ŵ磬�ڳ��״̬�½�ֹ��硢����ŵ磬��û�е���������������ŵ磬�ﵽ�������ʱ�ָ�
//���峬ѹ		�ڷŵ�״̬�������ŵ磬�ڳ��״̬�½�ֹ��硢����ŵ磬��û�е���������������ŵ磬�ﵽ�ָ�����ʱ�ָ�
//���峬ѹ		�ڷŵ�״̬�������ŵ磬�ڳ��״̬�½�ֹ��硢����ŵ磬��û�е���������������ŵ磬�ﵽ�ָ�����ʱ�ָ�
//����Ƿѹ		�ڷŵ�״̬�½�ֹ�ŵ硢�����磬�ڳ��״̬�������ŵ磬��û�е���������������ŵ磬�ﵽ�ָ�����ʱ�ָ�
//����Ƿѹ		�ڷŵ�״̬�½�ֹ�ŵ硢�����磬�ڳ��״̬�������ŵ磬��û�е���������������ŵ磬�ﵽ�ָ�����ʱ�ָ�
//���º͵���	��ֹ�ŵ磬��ֹ��磬�ﵽ�ָ�����ʱ�ָ�
void Protection_deal(void){
	AlarmLED_Light_Flag = 0;
	AlarmLED_Flash = 0;
	CHG_mos_Status = 1;
	DSG_mos_Status = 1;
//////ά���澯//////////////////////////////////////////////////////////////////////////////////////////////////
	if((BAT_Protect_Status&MTA_bit)==MTA_bit){		//ά���澯
		AlarmLED_Flash = 1;								//�澯����˸��־��λ
	}		
//////�͵����澯//////////////////////////////////////////////////////////////////////////////////////////////////
	if((BAT_Protect_Status&CapLow_bit)==CapLow_bit){		//�͵����澯
		AlarmLED_Flash = 1;								//�澯����˸��־��λ
	}
//////�¶Ȳ�����//////////////////////////////////////////////////////////////////////////////////////////////////
	if((BAT_Protect_Status&TUB_bit)==TUB_bit){				//�¶Ȳ�����
		AlarmLED_Flash = 1;								//�澯����˸��־��λ
	}		
//////�������////////////////////////////////////////////////////////////////////////////////////////////////////
	if(((BAT_Protect_Status&Freeze_bit)==Freeze_bit)){		//�������
		CHG_mos_Status = 0;
		BAT_Work_Status &= ~FCCF_bit;
	}
//////���ȳ��¡����ȹ���//////////////////////////////////////////////////////////////////////////////////////////
		if(((BAT_Protect_Status&HOT_bit)>0)||((BAT_Protect_Status&HOC_bit)>0)){				//���ȳ��¡����ȹ���
//			CHG_mos_Status = 0;
//			DSG_mos_Status = 0;
//			Heat_mos_Status = 0;
			AlarmLED_Light_Flag = 1;	//����������
		}		
//////���峬ѹ�����峬ѹ//////////////////////////////////////////////////////////////////////////////////////////		
		if((BAT_Protect_Status&CeOV_bit)==CeOV_bit){		//���峬ѹ�����峬ѹ
			CHG_mos_Status = 0;
			AlarmLED_Light_Flag = 1;	//����������
		}
		if((BAT_Protect_Status&PkOV_bit)==PkOV_bit){		//���峬ѹ�����峬ѹ
//			CHG_mos_Status = 0;
			AlarmLED_Light_Flag = 1;	//����������
		}		
		
//////����Ƿѹ������Ƿѹ//////////////////////////////////////////////////////////////////////////////////////////			
		if((BAT_Protect_Status&CeUV_bit)==CeUV_bit){//����Ƿѹ
			BAT_Work_Status |= FCCF_bit;	//��������У׼��־
			Rem_CAP_mAh = 0;
			Remain_CAP_mAms = 0;
			Remain_CAP_Percent = 0;
			DSG_mos_Status = 0;
			AlarmLED_Light_Flag = 1;	//����������
		}
		if((BAT_Protect_Status&PkUV_bit)==PkUV_bit){//����Ƿѹ
//			DSG_mos_Status = 0;
			AlarmLED_Light_Flag = 1;	//����������
		}		
		
///////���¡�����/////////////////////////////////////////////////////////////////////////////////////////////////		
		if(((BAT_Protect_Status&OT_bit)==OT_bit)||((BAT_Protect_Status&UT_bit)==UT_bit)){//���¡�����
			CHG_mos_Status = 0;
			DSG_mos_Status = 0;
			AlarmLED_Light_Flag = 1;	//����������		
		}	
///////������/////////////////////////////////////////////////////////////////////////////////////////////////		
		if((BAT_Protect_Status&COC_bit)==COC_bit){
			CHG_mos_Status = 0;
			AlarmLED_Light_Flag = 1;	//����������		
		}
///////�ŵ����/////////////////////////////////////////////////////////////////////////////////////////////////		
		if((BAT_Protect_Status&DOC0_bit)==DOC0_bit){
			DSG_mos_Status = 0;
			AlarmLED_Light_Flag = 1;	//����������		
		}
///////////////////////////////////////////////////////////////////////////////////////////////
			
	if(CHG_mos_Status > 0)	CHG_MOS(1);
	else{
		CHG_MOS(0);		
	}
	if(DSG_mos_Status > 0)	DSG_MOS(1);
	else{										
		DSG_MOS(0);	
	}
	if(Heat_mos_Status > 0)	Heat_MOS(1);
	else										Heat_MOS(0);
		
	if(AlarmLED_Light_Flag > 0){
		Light_AlarmLED;
		AlarmLED_Flash_Flag = 0;
	}else{
		if(AlarmLED_Flash >0){
			AlarmLED_Flash_Flag = 1;
		}else{
			UnLight_AlarmLED;
			AlarmLED_Flash_Flag = 0;		
		}
	}	
}

void BAT_Protect_Status_get_slow(void){
//COC get
	if((BAT_Work_Status&CHGing_bit) > 0){								//�жϵ���Ϊ������
//		BAT_Protect_Status &= ~COC_bit;						//�屣��λ
		if((Current&0x7FFFFFFF) > COC_Threshold){
			BAT_Protect_Status |= COC_bit;						//�ø澯λ
		}
	}
	
//DOC get
	if((BAT_Work_Status&DSGing_bit) > 0){								//�жϵ���Ϊ�ŵ����
//		BAT_Protect_Status &= ~DOC0_bit;						//�屣��λ
		if((Current&0x7FFFFFFF) > DOC0_Threshold){
			BAT_Protect_Status |= DOC0_bit;						//�ø澯λ
		}
	}

//CapLow get
	if(Remain_CAP_Percent < CapLow_Threshold){
		BAT_Protect_Status |= CapLow_bit;
	}else{
		BAT_Protect_Status &= ~CapLow_bit;
	}


	if(LTC68_Data_Ready == 1){
		LTC68_Data_Ready = 0;
		//CeUV get
		if(CellVolt_Min_Data < CeUV_Threshold){
			CeUV_Counter++;
			if(CeUV_Counter > 60)
				BAT_Protect_Status |= CeUV_bit;						//�ø澯λ
		}else CeUV_Counter = 0;
		
	//CeOV get
		if(CellVolt_Max_Data > CeOV_Threshold){
			CeOV_Counter++;
			if(CeOV_Counter > 40) BAT_Protect_Status |= CeOV_bit;						//�ø澯λ
		}else CeOV_Counter = 0;
		
	//PkUV get
		if(BAT_Voltage < PkUV_Threshold){
			PkUV_Counter++;
			if(PkUV_Counter > 40)	BAT_Protect_Status |= PkUV_bit;						//�ø澯λ
		}else	PkUV_Counter = 0;
	//PkOV get
		if(BAT_Voltage > PkOV_Threshold){
			PkOV_Counter++;
			if(PkOV_Counter > 40)	BAT_Protect_Status |= PkOV_bit;						//�ø澯λ
		}else	PkOV_Counter = 0;
	//OT get
		if(TEMP_Max_Data > OT_Threshold){
			OT_Counter++;
			if(OT_Counter > 100) BAT_Protect_Status |= OT_bit;	//�ñ���λ
		}
		if(TEMP_Max_Data < OT_Recovery_Threshold){
			OT_Counter = 0;
			BAT_Protect_Status &= ~OT_bit;	//�屣��λ
		}	
	//UT get
		if(TEMP_Min_Data < UT_Threshold){
			UT_Counter++;
			if(UT_Counter > 100) BAT_Protect_Status |= UT_bit;	//�ñ���λ
		}
		if(TEMP_Min_Data > UT_Recovery_Threshold){
			UT_Counter = 0;
			BAT_Protect_Status &= ~UT_bit;	//�屣��λ
		}
		
	//Freeze get
		if(TEMP_Min_Data < Freeze_Threshold){
			Freeze_Counter++;
			if(Freeze_Counter > 100) BAT_Protect_Status |= Freeze_bit;
		}
		if(TEMP_Min_Data > Freeze_Recovery_Threshold){
			Freeze_Counter = 0;
			BAT_Protect_Status &= ~Freeze_bit;	
		}
		
	//TUB get	�������̽ͷ������¶����ֵ����Сֵ֮����ڴ�ֵʱ�澯
		if((TEMP_Max_Data-TEMP_Min_Data) > TUB_Threshold){
			TUB_Counter++;
			if(TUB_Counter > 100) BAT_Protect_Status |= TUB_bit;	
		}else{
			TUB_Counter = 0;
			BAT_Protect_Status &= ~TUB_bit;
		}
	}
/*	
////CeOV get
//	if(CellVolt_Max_Data < CeOV_Threshold){	//δ�ﵽ�������� ������Ӧ����
//		BAT_Protect_Status &= ~CeOV_bit;	//�屣��λ
//		BAT_Protect_Alarm &= ~CeOV_bit;		//��澯λ
//		CeOV_Timer = 0;
//	}
//	//���ֵ����CeOV_Threshold��CeOV_Recovery_Threshold���ұ���λ�͸澯λδ��λ���򴥷��澯��ʼ��ʱ��ʱ
//	if((CellVolt_Max_Data > CeOV_Threshold) && (CellVolt_Max_Data > CeOV_Recovery_Threshold) && ((BAT_Protect_Status&CeOV_bit)==0) && ((BAT_Protect_Alarm&CeOV_bit)==0)){	
//		CeOV_Timer = 0;												//�����ʱ
//		BAT_Protect_Status &= ~CeOV_bit;			//�屣��λ
//		BAT_Protect_Alarm |= CeOV_bit;				//�ø澯λ
//	}
//	//���ֵ����CeOV_Threshold��CeOV_Recovery_Threshold���ұ���λδ��λ���澯λ��λ, ��ʾ���ڴ�������ʱ��ʱ
//	if((CellVolt_Max_Data > CeOV_Threshold) && (CellVolt_Max_Data > CeOV_Recovery_Threshold) && ((BAT_Protect_Status&CeOV_bit)==0) && ((BAT_Protect_Alarm&CeOV_bit)>0)){		
//		if(CeOV_Timer > CeOV_Delay_Time){	//�����ʱ��ʱ�����趨ֵ ���ñ���״̬λ	��澯λ
//			CeOV_Timer = 0;												//�����ʱ
//			BAT_Protect_Status |= CeOV_bit;				//�ñ���λ
//			BAT_Protect_Alarm &= ~CeOV_bit;				//��澯λ
//		}
//	}
////	//����λ��λ���澯λδ��λ, ��ʾ�Ѿ���������
////	if(((BAT_Protect_Status&CeOV_bit)>0) && ((BAT_Protect_Alarm&CeOV_bit)==0)){		
////			BAT_Protect_Status |= CeOV_bit;
////			BAT_Protect_Alarm &= ~CeOV_bit;
////	}		
//	//���ֵС��CeOV_Threshold��CeOV_Recovery_Threshold���ұ���λ��λ���澯λδ��λ, ��ﵽ�ָ����� ��ʼ�ָ���ʱ
//	if((CellVolt_Max_Data < CeOV_Threshold) && (CellVolt_Max_Data < CeOV_Recovery_Threshold) && ((BAT_Protect_Status&CeOV_bit)>0) && ((BAT_Protect_Alarm&CeOV_bit)==0)){
//		CeOV_Timer = 0;													//�����ʱ
//		BAT_Protect_Status |= CeOV_bit;					//�ñ���λ
//		BAT_Protect_Alarm |= CeOV_bit;					//�ø澯λ
//	}	
//	//���ֵС��CeOV_Threshold��CeOV_Recovery_Threshold���ұ���λ��λ���澯λ��λ, ��ʾ���ڻָ�����ʱ��ʱ
//	if((CellVolt_Max_Data < CeOV_Threshold) && (CellVolt_Max_Data < CeOV_Recovery_Threshold) && ((BAT_Protect_Status&CeOV_bit)>0) && ((BAT_Protect_Alarm&CeOV_bit)>0)){		
//		if(CeOV_Timer > CeOV_Recovery_Time){	//�����ʱ��ʱ�����趨ֵ ���屣��λ	��澯λ
//			CeOV_Timer = 0;												//�����ʱ
//			BAT_Protect_Status &= ~CeOV_bit;			//�屣��λ
//			BAT_Protect_Alarm &= ~CeOV_bit;				//��澯λ
//		}
//	}
	
////CeUV get
//	if(CellVolt_Min_Data > CeUV_Threshold){	//δ�ﵽ�������� ������Ӧ����
//		BAT_Protect_Status &= ~CeUV_bit;	//�屣��λ
//		BAT_Protect_Alarm &= ~CeUV_bit;		//��澯λ
//		CeUV_Timer = 0;
//	}
//	//���ֵС����CeUV_Threshold��CeUV_Recovery_Threshold���ұ���λ�͸澯λδ��λ���򴥷��澯��ʼ��ʱ��ʱ
//	if((CellVolt_Min_Data < CeUV_Threshold) && (CellVolt_Min_Data < CeUV_Recovery_Threshold) && ((BAT_Protect_Status&CeUV_bit)==0) && ((BAT_Protect_Alarm&CeUV_bit)==0)){	
//		CeUV_Timer = 0;												//�����ʱ
//		BAT_Protect_Status &= ~CeUV_bit;			//�屣��λ
//		BAT_Protect_Alarm |= CeUV_bit;				//�ø澯λ	
//	}
//	//���ֵ����CeUV_Threshold��CeUV_Recovery_Threshold���ұ���λδ��λ���澯λ��λ, ��ʾ���ڴ�������ʱ��ʱ
//	if((CellVolt_Min_Data < CeUV_Threshold) && (CellVolt_Min_Data < CeUV_Recovery_Threshold) && ((BAT_Protect_Status&CeUV_bit)==0) && ((BAT_Protect_Alarm&CeUV_bit)>0)){		
//		if(CeUV_Timer > CeUV_Delay_Time){	//�����ʱ��ʱ�����趨ֵ ���ñ���״̬λ	��澯λ
//			CeUV_Timer = 0;												//�����ʱ
//			BAT_Protect_Status |= CeUV_bit;				//�ñ���λ
//			BAT_Protect_Alarm &= ~CeUV_bit;				//��澯λ
//		}
//	}
//	//����λ��λ���澯λδ��λ, ��ʾ�Ѿ���������
//	if(((BAT_Protect_Status&CeUV_bit)>0) && ((BAT_Protect_Alarm&CeUV_bit)==0)){		
//			BAT_Protect_Status |= CeUV_bit;
//			BAT_Protect_Alarm &= ~CeUV_bit;
//	}		
//	//���ֵ����CeUV_Threshold��CeUV_Recovery_Threshold���ұ���λ��λ���澯λδ��λ, ��ﵽ�ָ����� ��ʼ�ָ���ʱ
//	if((CellVolt_Min_Data > CeUV_Threshold) && (CellVolt_Min_Data > CeUV_Recovery_Threshold) && ((BAT_Protect_Status&CeUV_bit)>0) && ((BAT_Protect_Alarm&CeUV_bit)==0)){
//		if(CeUV_Timer > CeUV_Recovery_Time){
//			CeUV_Timer = 0;													//�����ʱ
//			BAT_Protect_Status |= CeUV_bit;					//�ñ���λ
//			BAT_Protect_Alarm |= CeUV_bit;					//�ø澯λ
//		}
//	}	
//	//���ֵС��CeUV_Threshold��CeUV_Recovery_Threshold���ұ���λ��λ���澯λ��λ, ��ʾ���ڻָ�����ʱ��ʱ
//	if((CellVolt_Min_Data > CeUV_Threshold) && (CellVolt_Min_Data > CeUV_Recovery_Threshold) && ((BAT_Protect_Status&CeUV_bit)>0) && ((BAT_Protect_Alarm&CeUV_bit)>0)){		
//		if(CeUV_Timer > CeUV_Recovery_Time){	//�����ʱ��ʱ�����趨ֵ ���屣��λ	��澯λ
//			CeUV_Timer = 0;												//�����ʱ
//			BAT_Protect_Status &= ~CeUV_bit;			//�屣��λ
//			BAT_Protect_Alarm &= ~CeUV_bit;				//��澯λ
//		}
//	}

////PkOV get
//	if(BAT_Voltage < PkOV_Threshold){	//δ�ﵽ�������� ������Ӧ����
//		BAT_Protect_Status &= ~PkOV_bit;	//�屣��λ
//		BAT_Protect_Alarm &= ~PkOV_bit;		//��澯λ
//		PkOV_Timer = 0;
//	}
//	//���ֵ����PkOV_Threshold��PkOV_Recovery_Threshold���ұ���λ�͸澯λδ��λ���򴥷��澯��ʼ��ʱ��ʱ
//	if((BAT_Voltage > PkOV_Threshold) && (BAT_Voltage > PkOV_Recovery_Threshold) && ((BAT_Protect_Status&PkOV_bit)==0) && ((BAT_Protect_Alarm&PkOV_bit)==0)){	
//		PkOV_Timer = 0;												//�����ʱ
//		BAT_Protect_Status &= ~PkOV_bit;			//�屣��λ
//		BAT_Protect_Alarm |= PkOV_bit;				//�ø澯λ	
//	}
//	//���ֵ����PkOV_Threshold��PkOV_Recovery_Threshold���ұ���λδ��λ���澯λ��λ, ��ʾ���ڴ�������ʱ��ʱ
//	if((BAT_Voltage > PkOV_Threshold) && (BAT_Voltage > PkOV_Recovery_Threshold) && ((BAT_Protect_Status&PkOV_bit)==0) && ((BAT_Protect_Alarm&PkOV_bit)>0)){		
//		if(PkOV_Timer > PkOV_Delay_Time){	//�����ʱ��ʱ�����趨ֵ ���ñ���״̬λ	��澯λ
//			PkOV_Timer = 0;												//�����ʱ
//			BAT_Protect_Status |= PkOV_bit;				//�ñ���λ
//			BAT_Protect_Alarm &= ~PkOV_bit;				//��澯λ
//		}
//	}
//	//����λ��λ���澯λδ��λ, ��ʾ�Ѿ���������
//	if(((BAT_Protect_Status&PkOV_bit)>0) && ((BAT_Protect_Alarm&PkOV_bit)==0)){		
//			BAT_Protect_Status |= PkOV_bit;
//			BAT_Protect_Alarm &= ~PkOV_bit;
//	}		
//	//���ֵС��PkOV_Threshold��PkOV_Recovery_Threshold���ұ���λ��λ���澯λδ��λ, ��ﵽ�ָ����� ��ʼ�ָ���ʱ
//	if((BAT_Voltage < PkOV_Threshold) && (BAT_Voltage < PkOV_Recovery_Threshold) && ((BAT_Protect_Status&PkOV_bit)>0) && ((BAT_Protect_Alarm&PkOV_bit)==0)){
//		PkOV_Timer = 0;													//�����ʱ
//		BAT_Protect_Status |= PkOV_bit;					//�ñ���λ
//		BAT_Protect_Alarm |= PkOV_bit;					//�ø澯λ
//	}	
//	//���ֵС��CeOV_Threshold��CeOV_Recovery_Threshold���ұ���λ��λ���澯λ��λ, ��ʾ���ڻָ�����ʱ��ʱ
//	if((BAT_Voltage < PkOV_Threshold) && (BAT_Voltage < PkOV_Recovery_Threshold) && ((BAT_Protect_Status&PkOV_bit)>0) && ((BAT_Protect_Alarm&PkOV_bit)>0)){		
//		if(PkOV_Timer > PkOV_Recovery_Time){	//�����ʱ��ʱ�����趨ֵ ���屣��λ	��澯λ
//			PkOV_Timer = 0;									//�����ʱ
//			BAT_Protect_Status &= ~PkOV_bit;			//�屣��λ
//			BAT_Protect_Alarm &= ~PkOV_bit;				//��澯λ
//		}
//	}
	
////PkUV get
//	if(BAT_Voltage > PkUV_Threshold){	//δ�ﵽ�������� ������Ӧ����
//		BAT_Protect_Status &= ~PkUV_bit;	//�屣��λ
//		BAT_Protect_Alarm &= ~PkUV_bit;		//��澯λ
//		PkUV_Timer = 0;
//	}
//	//���ֵС����CeUV_Threshold��CeUV_Recovery_Threshold���ұ���λ�͸澯λδ��λ���򴥷��澯��ʼ��ʱ��ʱ
//	if((BAT_Voltage < PkUV_Threshold) && (BAT_Voltage < PkUV_Recovery_Threshold) && ((BAT_Protect_Status&PkUV_bit)==0) && ((BAT_Protect_Alarm&PkUV_bit)==0)){	
//		PkUV_Timer = 0;												//�����ʱ
//		BAT_Protect_Status &= ~PkUV_bit;			//�屣��λ
//		BAT_Protect_Alarm |= PkUV_bit;				//�ø澯λ
//	}
//	//���ֵ����PkUV_Threshold��PkUV_Recovery_Threshold���ұ���λδ��λ���澯λ��λ, ��ʾ���ڴ�������ʱ��ʱ
//	if((BAT_Voltage < PkUV_Threshold) && (BAT_Voltage < PkUV_Recovery_Threshold) && ((BAT_Protect_Status&PkUV_bit)==0) && ((BAT_Protect_Alarm&PkUV_bit)>0)){		
//		if(PkUV_Timer > PkUV_Delay_Time){	//�����ʱ��ʱ�����趨ֵ ���ñ���״̬λ	��澯λ
//			PkUV_Timer = 0;												//�����ʱ
//			BAT_Protect_Status |= PkUV_bit;				//�ñ���λ
//			BAT_Protect_Alarm &= ~PkUV_bit;				//��澯λ
//		}
//	}
//	//����λ��λ���澯λδ��λ, ��ʾ�Ѿ���������
//	if(((BAT_Protect_Status&PkUV_bit)>0) && ((BAT_Protect_Alarm&PkUV_bit)==0)){		
//			BAT_Protect_Status |= PkUV_bit;
//			BAT_Protect_Alarm &= ~PkUV_bit;
//	}		
//	//���ֵ����PkUV_Threshold��PkUV_Recovery_Threshold���ұ���λ��λ���澯λδ��λ, ��ﵽ�ָ����� ��ʼ�ָ���ʱ
//	if((BAT_Voltage > PkUV_Threshold) && (BAT_Voltage > PkUV_Recovery_Threshold) && ((BAT_Protect_Status&PkUV_bit)>0) && ((BAT_Protect_Alarm&PkUV_bit)==0)){
//		PkUV_Timer = 0;													//�����ʱ
//		BAT_Protect_Status |= PkUV_bit;					//�ñ���λ
//		BAT_Protect_Alarm |= PkUV_bit;					//�ø澯λ
//	}	
//	//���ֵС��CeUV_Threshold��CeUV_Recovery_Threshold���ұ���λ��λ���澯λ��λ, ��ʾ���ڻָ�����ʱ��ʱ
//	if((BAT_Voltage > PkUV_Threshold) && (BAT_Voltage > PkUV_Recovery_Threshold) && ((BAT_Protect_Status&PkUV_bit)>0) && ((BAT_Protect_Alarm&PkUV_bit)>0)){		
//		if(PkUV_Timer > PkUV_Recovery_Time){	//�����ʱ��ʱ�����趨ֵ ���屣��λ	��澯λ
//			PkUV_Timer = 0;												//�����ʱ
//			BAT_Protect_Status &= ~PkUV_bit;			//�屣��λ
//			BAT_Protect_Alarm &= ~PkUV_bit;				//��澯λ
//		}
//	}	
	
////OT get
//	if(TEMP_Max_Data < OT_Threshold){	//δ�ﵽ�������� ������Ӧ����
//		BAT_Protect_Status &= ~OT_bit;	//�屣��λ
//		BAT_Protect_Alarm &= ~OT_bit;		//��澯λ
//		OT_Timer = 0;
//	}
//	//���ֵ����OT_Threshold��OT_Recovery_Threshold���ұ���λ�͸澯λδ��λ���򴥷��澯��ʼ��ʱ��ʱ
//	if((TEMP_Max_Data > OT_Threshold) && (TEMP_Max_Data > OT_Recovery_Threshold) && ((BAT_Protect_Status&OT_bit)==0) && ((BAT_Protect_Alarm&OT_bit)==0)){	
//		OT_Timer = 0;												//�����ʱ
//		BAT_Protect_Status &= ~OT_bit;			//�屣��λ
//		BAT_Protect_Alarm |= OT_bit;				//�ø澯λ	
//	}
//	//���ֵ����OT_Threshold��OT_Recovery_Threshold���ұ���λδ��λ���澯λ��λ, ��ʾ���ڴ�������ʱ��ʱ
//	if((TEMP_Max_Data > OT_Threshold) && (TEMP_Max_Data > OT_Recovery_Threshold) && ((BAT_Protect_Status&OT_bit)==0) && ((BAT_Protect_Alarm&OT_bit)>0)){		
//		if(OT_Timer > OT_Delay_Time){	//�����ʱ��ʱ�����趨ֵ ���ñ���״̬λ	��澯λ
//			OT_Timer = 0;												//�����ʱ
//			BAT_Protect_Status |= OT_bit;				//�ñ���λ
//			BAT_Protect_Alarm &= ~OT_bit;				//��澯λ
//		}
//	}
//	//����λ��λ���澯λδ��λ, ��ʾ�Ѿ���������
//	if(((BAT_Protect_Status&OT_bit)>0) && ((BAT_Protect_Alarm&OT_bit)==0)){		
//			BAT_Protect_Status |= OT_bit;
//			BAT_Protect_Alarm &= ~OT_bit;
//	}		
//	//���ֵС��OT_Threshold��OT_Recovery_Threshold���ұ���λ��λ���澯λδ��λ, ��ﵽ�ָ����� ��ʼ�ָ���ʱ
//	if((TEMP_Max_Data < OT_Threshold) && (TEMP_Max_Data < OT_Recovery_Threshold) && ((BAT_Protect_Status&OT_bit)>0) && ((BAT_Protect_Alarm&OT_bit)==0)){
//		OT_Timer = 0;													//�����ʱ
//		BAT_Protect_Status |= OT_bit;					//�ñ���λ
//		BAT_Protect_Alarm |= OT_bit;					//�ø澯λ
//	}	
//	//���ֵС��OT_Threshold��OT_Recovery_Threshold���ұ���λ��λ���澯λ��λ, ��ʾ���ڻָ�����ʱ��ʱ
//	if((TEMP_Max_Data < OT_Threshold) && (TEMP_Max_Data < OT_Recovery_Threshold) && ((BAT_Protect_Status&OT_bit)>0) && ((BAT_Protect_Alarm&OT_bit)>0)){		
//		if(OT_Timer > OT_Recovery_Time){	//�����ʱ��ʱ�����趨ֵ ���屣��λ	��澯λ
//			OT_Timer = 0;									//�����ʱ
//			BAT_Protect_Status &= ~OT_bit;			//�屣��λ
//			BAT_Protect_Alarm &= ~OT_bit;				//��澯λ
//		}
//	}
	
////UT get
//	if(TEMP_Min_Data < UT_Threshold){	//δ�ﵽ�������� ������Ӧ����
//		BAT_Protect_Status &= ~UT_bit;	//�屣��λ
//		BAT_Protect_Alarm &= ~UT_bit;		//��澯λ
//		UT_Timer = 0;
//	}
//	//���ֵС����UT_Threshold��UT_Recovery_Threshold���ұ���λ�͸澯λδ��λ���򴥷��澯��ʼ��ʱ��ʱ
//	if((TEMP_Min_Data < UT_Threshold) && (TEMP_Min_Data < PkUV_Recovery_Threshold) && ((BAT_Protect_Status&UT_bit)==0) && ((BAT_Protect_Alarm&UT_bit)==0)){	
//		UT_Timer = 0;												//�����ʱ
//		BAT_Protect_Status &= ~UT_bit;			//�屣��λ
//		BAT_Protect_Alarm |= UT_bit;				//�ø澯λ
//	}
//	//���ֵ����UT_Threshold��UT_Recovery_Threshold���ұ���λδ��λ���澯λ��λ, ��ʾ���ڴ�������ʱ��ʱ
//	if((TEMP_Min_Data < UT_Threshold) && (TEMP_Min_Data < UT_Recovery_Threshold) && ((BAT_Protect_Status&UT_bit)==0) && ((BAT_Protect_Alarm&UT_bit)>0)){		
//		if(UT_Timer > UT_Delay_Time){	//�����ʱ��ʱ�����趨ֵ ���ñ���״̬λ	��澯λ
//			UT_Timer = 0;												//�����ʱ
//			BAT_Protect_Status |= UT_bit;				//�ñ���λ
//			BAT_Protect_Alarm &= ~UT_bit;				//��澯λ
//		}
//	}
//	//����λ��λ���澯λδ��λ, ��ʾ�Ѿ���������
//	if(((BAT_Protect_Status&UT_bit)>0) && ((BAT_Protect_Alarm&UT_bit)==0)){		
//			BAT_Protect_Status |= UT_bit;
//			BAT_Protect_Alarm &= ~UT_bit;
//	}		
//	//���ֵ����UT_Threshold��UT_Recovery_Threshold���ұ���λ��λ���澯λδ��λ, ��ﵽ�ָ����� ��ʼ�ָ���ʱ
//	if((TEMP_Min_Data > UT_Threshold) && (TEMP_Min_Data > UT_Recovery_Threshold) && ((BAT_Protect_Status&UT_bit)>0) && ((BAT_Protect_Alarm&UT_bit)==0)){
//		UT_Timer = 0;													//�����ʱ
//		BAT_Protect_Status |= UT_bit;					//�ñ���λ
//		BAT_Protect_Alarm |= UT_bit;					//�ø澯λ
//	}	
//	//���ֵС��UT_Threshold��UT_Recovery_Threshold���ұ���λ��λ���澯λ��λ, ��ʾ���ڻָ�����ʱ��ʱ
//	if((TEMP_Min_Data > UT_Threshold) && (TEMP_Min_Data > UT_Recovery_Threshold) && ((BAT_Protect_Status&UT_bit)>0) && ((BAT_Protect_Alarm&UT_bit)>0)){		
//		if(UT_Timer > UT_Recovery_Time){	//�����ʱ��ʱ�����趨ֵ ���屣��λ	��澯λ
//			UT_Timer = 0;												//�����ʱ
//			BAT_Protect_Status &= ~UT_bit;			//�屣��λ
//			BAT_Protect_Alarm &= ~UT_bit;				//��澯λ
//		}
//	}
*/

////HOT get	�ڼ��ȹ����� ����¶ȸ���HOT_Threshold�澯
//	if((BAT_Work_Status&Heating_bit) > 0){			//�ж�Ϊ����״̬
//		if((TEMP_Max_Data+100) > HOC_Threshold)	BAT_Protect_Status |= HOT_bit;
//		else																		BAT_Protect_Status &= ~HOT_bit;
//	}else{
//		BAT_Protect_Status &= ~HOT_bit;
//	}	

		
	
////HOC get
//	if(Heat_Current > HOC_Threshold){
//		BAT_Protect_Status |= HOC_bit;
//	}else{
//		BAT_Protect_Status &= ~HOC_bit;
//	}
}

void BAT(void){
	
//	CHG_Plugged_get();
	Current_get();				//�����ŵ����ֵ
	CAP();								//������������	
	Remain_CAP_Percent_get();	//����ʣ�������ٷֱ�	(ʣ������/�������)*100%	

	if(Measure_Num > 25){
		BAT_Voltage_get();		//���������ܵ�ѹ
//		Port_Voltage_get();		//����P+��ѹ
//		Heat_Current_get();		//������ȵ���ֵ


//		From_Temp_Get_RTCap();
//		if(CAP_Temp_para_old != CAP_Temp_para){
//			CAP_Temp_para_times++;
//			if(CAP_Temp_para_times > 2000){
//				CAP_Temp_para_old = CAP_Temp_para;
//				Full_CAP_RT = (Full_CAP*CAP_Temp_para)/100;							//���� �¶�-����ϵ�� ���ʵʱ������
//				Rem_CAP_mAh = (Full_CAP_RT*Remain_CAP_Percent)/100;			//ʵʱ�������º�ʣ�������ٷֱȲ��䣬���ݰٷֱ����ʣ������
//			}
//		}else{
//			CAP_Temp_para_times = 0;
//		}
		
	}
}
void Heat(void){
	
}
/*
void BAT_CANSend(void){
	switch(CANSend_Timer){
		case 50:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000001;
			CAN_TxMsg[CAN1].data[0] = (BAT_Protect_Status&0xFF000000)>>24;
			CAN_TxMsg[CAN1].data[1] = (BAT_Protect_Status&0x00FF0000)>>16;
			CAN_TxMsg[CAN1].data[2] = (BAT_Protect_Status&0x0000FF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (BAT_Protect_Status&0x000000FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (BAT_Work_Status&0xFF000000)>>24;
			CAN_TxMsg[CAN1].data[5] = (BAT_Work_Status&0x00FF0000)>>16;
			CAN_TxMsg[CAN1].data[6] = (BAT_Work_Status&0x0000FF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (BAT_Work_Status&0x000000FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;
		case 100:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000002;
			CAN_TxMsg[CAN1].data[0] = (Full_CAP_RT&0xFF000000)>>24;
			CAN_TxMsg[CAN1].data[1] = (Full_CAP_RT&0x00FF0000)>>16;
			CAN_TxMsg[CAN1].data[2] = (Full_CAP_RT&0x0000FF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (Full_CAP_RT&0x000000FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (Rem_CAP_mAh&0xFF000000)>>24;
			CAN_TxMsg[CAN1].data[5] = (Rem_CAP_mAh&0x00FF0000)>>16;
			CAN_TxMsg[CAN1].data[6] = (Rem_CAP_mAh&0x0000FF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (Rem_CAP_mAh&0x000000FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break; 
		case 150:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000003;
			CAN_TxMsg[CAN1].data[0] = (Designed_CAP&0xFF000000)>>24;
			CAN_TxMsg[CAN1].data[1] = (Designed_CAP&0x00FF0000)>>16;
			CAN_TxMsg[CAN1].data[2] = (Designed_CAP&0x0000FF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (Designed_CAP&0x000000FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (Cycle_Times&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (Cycle_Times&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = 0;
			CAN_TxMsg[CAN1].data[7] = Remain_CAP_Percent;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;
		case 200:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000004;
			CAN_TxMsg[CAN1].data[0] = (Current&0xFF000000)>>24;
			CAN_TxMsg[CAN1].data[1] = (Current&0x00FF0000)>>16;
			CAN_TxMsg[CAN1].data[2] = (Current&0x0000FF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (Current&0x000000FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DSG_Rate&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DSG_Rate&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (CHG_Rate&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (CHG_Rate&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;
		case 250:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000005;
			CAN_TxMsg[CAN1].data[0] = (BAT_Voltage&0xFF000000)>>24;
			CAN_TxMsg[CAN1].data[1] = (BAT_Voltage&0x00FF0000)>>16;
			CAN_TxMsg[CAN1].data[2] = (BAT_Voltage&0x0000FF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (BAT_Voltage&0x000000FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (Port_Voltage&0xFF000000)>>24;
			CAN_TxMsg[CAN1].data[5] = (Port_Voltage&0x00FF0000)>>16;
			CAN_TxMsg[CAN1].data[6] = (Port_Voltage&0x0000FF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (Port_Voltage&0x000000FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;
		case 300:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000006;
			CAN_TxMsg[CAN1].data[0] = (DSG_CAP_Lifetime_mAh&0xFF000000)>>24;
			CAN_TxMsg[CAN1].data[1] = (DSG_CAP_Lifetime_mAh&0x00FF0000)>>16;
			CAN_TxMsg[CAN1].data[2] = (DSG_CAP_Lifetime_mAh&0x0000FF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DSG_CAP_Lifetime_mAh&0x000000FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (CHG_CAP_Lifetime_mAh&0xFF000000)>>24;
			CAN_TxMsg[CAN1].data[5] = (CHG_CAP_Lifetime_mAh&0x00FF0000)>>16;
			CAN_TxMsg[CAN1].data[6] = (CHG_CAP_Lifetime_mAh&0x0000FF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (CHG_CAP_Lifetime_mAh&0x000000FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;		
		case 500:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000100;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[5].CellVolt[0]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[5].CellVolt[0]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[5].CellVolt[1]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[5].CellVolt[1]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[5].CellVolt[2]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[5].CellVolt[2]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[5].CellVolt[3]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[5].CellVolt[3]&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;		
		case 550:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000101;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[5].CellVolt[4]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[5].CellVolt[4]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[5].CellVolt[5]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[5].CellVolt[5]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[5].CellVolt[6]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[5].CellVolt[6]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[5].CellVolt[7]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[5].CellVolt[7]&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;		
		case 600:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000102;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[5].CellVolt[8]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[5].CellVolt[8]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[5].CellVolt[9]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[5].CellVolt[9]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[5].CellVolt[10]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[5].CellVolt[10]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[5].CellVolt[11]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[5].CellVolt[11]&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;		
		case 650:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000103;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[4].CellVolt[0]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[4].CellVolt[0]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[4].CellVolt[1]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[4].CellVolt[1]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[4].CellVolt[2]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[4].CellVolt[2]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[4].CellVolt[3]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[4].CellVolt[3]&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;		
		case 700:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000104;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[4].CellVolt[4]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[4].CellVolt[4]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[4].CellVolt[5]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[4].CellVolt[5]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[4].CellVolt[6]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[4].CellVolt[6]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[4].CellVolt[7]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[4].CellVolt[7]&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;
		case 750:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000105;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[4].CellVolt[8]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[4].CellVolt[8]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[4].CellVolt[9]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[4].CellVolt[9]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[4].CellVolt[10]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[4].CellVolt[10]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[4].CellVolt[11]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[4].CellVolt[11]&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;		
		case 800:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000106;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[3].CellVolt[0]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[3].CellVolt[0]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[3].CellVolt[1]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[3].CellVolt[1]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[3].CellVolt[2]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[3].CellVolt[2]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[3].CellVolt[3]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[3].CellVolt[3]&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;		
		case 850:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000107;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[3].CellVolt[4]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[3].CellVolt[4]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[3].CellVolt[5]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[3].CellVolt[5]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[3].CellVolt[6]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[3].CellVolt[6]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[3].CellVolt[7]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[3].CellVolt[7]&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;		
		case 900:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000108;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[3].CellVolt[8]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[3].CellVolt[8]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[3].CellVolt[9]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[3].CellVolt[9]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[3].CellVolt[10]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[3].CellVolt[10]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[3].CellVolt[11]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[3].CellVolt[11]&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;		
		case 950:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000109;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[2].CellVolt[0]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[2].CellVolt[0]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[2].CellVolt[1]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[2].CellVolt[1]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[2].CellVolt[2]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[2].CellVolt[2]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[2].CellVolt[3]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[2].CellVolt[3]&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;		
		case 1000:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x0000010A;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[2].CellVolt[4]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[2].CellVolt[4]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[2].CellVolt[5]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[2].CellVolt[5]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[2].CellVolt[6]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[2].CellVolt[6]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[2].CellVolt[7]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[2].CellVolt[7]&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;		
		case 1050:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x0000010B;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[2].CellVolt[8]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[2].CellVolt[8]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[2].CellVolt[9]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[2].CellVolt[9]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[2].CellVolt[10]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[2].CellVolt[10]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[2].CellVolt[11]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[2].CellVolt[11]&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;			
		case 1100:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x0000010C;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[1].CellVolt[0]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[1].CellVolt[0]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[1].CellVolt[1]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[1].CellVolt[1]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[1].CellVolt[2]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[1].CellVolt[2]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[1].CellVolt[3]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[1].CellVolt[3]&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;	
		case 1150:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x0000010D;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[1].CellVolt[4]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[1].CellVolt[4]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[1].CellVolt[5]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[1].CellVolt[5]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[1].CellVolt[6]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[1].CellVolt[6]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[1].CellVolt[7]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[1].CellVolt[7]&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;
		case 1200:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x0000010E;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[1].CellVolt[8]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[1].CellVolt[8]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[1].CellVolt[9]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[1].CellVolt[9]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[1].CellVolt[10]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[1].CellVolt[10]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[1].CellVolt[11]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[1].CellVolt[11]&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;
		case 1250:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x0000010F;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[0].CellVolt[0]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[0].CellVolt[0]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[0].CellVolt[1]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[0].CellVolt[1]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[0].CellVolt[2]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[0].CellVolt[2]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[0].CellVolt[3]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[0].CellVolt[3]&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;
		case 1300:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000110;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[0].CellVolt[4]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[0].CellVolt[4]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[0].CellVolt[5]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[0].CellVolt[5]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[0].CellVolt[6]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[0].CellVolt[6]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[0].CellVolt[7]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[0].CellVolt[7]&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;
		case 1350:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000111;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[0].CellVolt[8]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[0].CellVolt[8]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[0].CellVolt[9]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[0].CellVolt[9]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[0].CellVolt[10]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[0].CellVolt[10]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[0].CellVolt[11]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[0].CellVolt[11]&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;
		case 1400:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x000001FE;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[0].CellVolt_DELTA&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[0].CellVolt_DELTA&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = 0;
			CAN_TxMsg[CAN1].data[3] = 0;
			CAN_TxMsg[CAN1].data[4] = 0;
			CAN_TxMsg[CAN1].data[5] = 0;
			CAN_TxMsg[CAN1].data[6] = 0;
			CAN_TxMsg[CAN1].data[7] = 0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;
		case 1450:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x000001FF;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[0].CellVolt_Max&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[0].CellVolt_Max&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[0].MAX_Cell_NUM&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[0].MAX_Cell_NUM&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[0].CellVolt_Min&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[0].CellVolt_Min&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[0].MIN_Cell_NUM&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[0].MIN_Cell_NUM&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;
		case 1500:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000200;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[5].GPIO_NTC_TEMP[0]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[5].GPIO_NTC_TEMP[0]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[5].GPIO_NTC_TEMP[1]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[5].GPIO_NTC_TEMP[1]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[5].GPIO_NTC_TEMP[2]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[5].GPIO_NTC_TEMP[2]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[5].GPIO_NTC_TEMP[3]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[5].GPIO_NTC_TEMP[3]&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;		
		case 1550:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000201;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[5].GPIO_NTC_TEMP[4]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[5].GPIO_NTC_TEMP[4]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[4].GPIO_NTC_TEMP[0]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[4].GPIO_NTC_TEMP[0]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[4].GPIO_NTC_TEMP[1]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[4].GPIO_NTC_TEMP[1]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[4].GPIO_NTC_TEMP[2]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[4].GPIO_NTC_TEMP[2]&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;
		case 1600:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000202;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[4].GPIO_NTC_TEMP[3]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[4].GPIO_NTC_TEMP[3]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[4].GPIO_NTC_TEMP[4]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[4].GPIO_NTC_TEMP[4]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[3].GPIO_NTC_TEMP[0]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[3].GPIO_NTC_TEMP[0]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[3].GPIO_NTC_TEMP[1]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[3].GPIO_NTC_TEMP[1]&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;
		case 1650:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000203;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[3].GPIO_NTC_TEMP[2]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[3].GPIO_NTC_TEMP[2]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[3].GPIO_NTC_TEMP[3]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[3].GPIO_NTC_TEMP[3]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[3].GPIO_NTC_TEMP[4]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[3].GPIO_NTC_TEMP[4]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[2].GPIO_NTC_TEMP[0]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[2].GPIO_NTC_TEMP[0]&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;
		case 1700:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000204;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[2].GPIO_NTC_TEMP[1]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[2].GPIO_NTC_TEMP[1]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[2].GPIO_NTC_TEMP[2]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[2].GPIO_NTC_TEMP[2]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[2].GPIO_NTC_TEMP[3]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[2].GPIO_NTC_TEMP[3]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[2].GPIO_NTC_TEMP[4]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[2].GPIO_NTC_TEMP[4]&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;
		case 1750:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000205;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[1].GPIO_NTC_TEMP[0]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[1].GPIO_NTC_TEMP[0]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[1].GPIO_NTC_TEMP[1]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[1].GPIO_NTC_TEMP[1]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[1].GPIO_NTC_TEMP[2]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[1].GPIO_NTC_TEMP[2]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[1].GPIO_NTC_TEMP[3]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[1].GPIO_NTC_TEMP[3]&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;
		case 1800:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000206;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[1].GPIO_NTC_TEMP[4]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[1].GPIO_NTC_TEMP[4]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[0].GPIO_NTC_TEMP[0]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[0].GPIO_NTC_TEMP[0]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[0].GPIO_NTC_TEMP[1]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[0].GPIO_NTC_TEMP[1]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[0].GPIO_NTC_TEMP[2]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[0].GPIO_NTC_TEMP[2]&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;
		case 1850:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x00000207;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[0].GPIO_NTC_TEMP[3]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[0].GPIO_NTC_TEMP[3]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[0].GPIO_NTC_TEMP[4]&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[0].GPIO_NTC_TEMP[4]&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = 0;
			CAN_TxMsg[CAN1].data[5] = 0;
			CAN_TxMsg[CAN1].data[6] = 0;
			CAN_TxMsg[CAN1].data[7] = 0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;
		case 1900:
			CAN_TxMsg[CAN1].id = CAN_Addr_Base|0x000002FF;
			CAN_TxMsg[CAN1].data[0] = (DEVICE[0].LT68_NTC_Temp_MAX&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[1] = (DEVICE[0].LT68_NTC_Temp_MAX&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[2] = (DEVICE[0].LT68_NTC_Temp_MAX_NUM&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[3] = (DEVICE[0].LT68_NTC_Temp_MAX_NUM&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[4] = (DEVICE[0].LT68_NTC_Temp_MIN&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[5] = (DEVICE[0].LT68_NTC_Temp_MIN&0x00FF)>>0;
			CAN_TxMsg[CAN1].data[6] = (DEVICE[0].LT68_NTC_Temp_MIN_NUM&0xFF00)>>8;
			CAN_TxMsg[CAN1].data[7] = (DEVICE[0].LT68_NTC_Temp_MIN_NUM&0x00FF)>>0;
			CAN_TxMsg[CAN1].len = 8;	
			CAN_TxMsg[CAN1].format = 1;			//��չ֡
			CAN_TxMsg[CAN1].type = 0;				//����֡
			CAN_waitReady(CAN1);
			CAN_send(CAN1,&CAN_TxMsg[CAN1]);
			break;
		
		case 2000:			
			CANSend_Timer = 0;
			break;
	}
}
*/

void BAT_UartSend(void){
	uint16_t k=0;
	BATUartSendData[k] = 'B';
	BATUartSendData[k+1] = 'A';
	BATUartSendData[k+2] = 'T';
	BATUartSendData[k+3] = '\r';
	BATUartSendData[k+4] = '\n';
	k = k+5;
	BATUartSendData[k] = 'B';
	BATUartSendData[k+1] = 'V';
	BATUartSendData[k+2] = ' ';
	BATUartSendData[k+3] = ' ';
	BATUartSendData[k+4] = 'm';
	BATUartSendData[k+5] = 'V';
	BATUartSendData[k+6] = ' ';	
	BATUartSendData[k+7] = 0x30+BAT_Voltage/100000;
	BATUartSendData[k+8] = 0x30+(BAT_Voltage%100000)/10000;
	BATUartSendData[k+9] = 0x30+(BAT_Voltage%10000)/1000;
	BATUartSendData[k+10] = 0x30+(BAT_Voltage%1000)/100;
	BATUartSendData[k+11] = 0x30+(BAT_Voltage%100)/10;
	BATUartSendData[k+12] = 0x30+(BAT_Voltage%10)/1;
	BATUartSendData[k+13] = '\r';
	BATUartSendData[k+14] = '\n';
	k = k+15;
	BATUartSendData[k] = 'P';
	BATUartSendData[k+1] = 'V';
	BATUartSendData[k+2] = ' ';
	BATUartSendData[k+3] = ' ';
	BATUartSendData[k+4] = 'm';
	BATUartSendData[k+5] = 'V';
	BATUartSendData[k+6] = ' ';	
	BATUartSendData[k+7] = 0x30+Port_Voltage/100000;
	BATUartSendData[k+8] = 0x30+(Port_Voltage%100000)/10000;
	BATUartSendData[k+9] = 0x30+(Port_Voltage%10000)/1000;
	BATUartSendData[k+10] = 0x30+(Port_Voltage%1000)/100;
	BATUartSendData[k+11] = 0x30+(Port_Voltage%100)/10;
	BATUartSendData[k+12] = 0x30+(Port_Voltage%10)/1;
	BATUartSendData[k+13] = '\r';
	BATUartSendData[k+14] = '\n';
	k = k+15;
	BATUartSendData[k] = 'C';
	BATUartSendData[k+1] = ' ';
	BATUartSendData[k+2] = ' ';
	BATUartSendData[k+3] = 'O';
	BATUartSendData[k+4] = 'F';
	BATUartSendData[k+5] = 'S';
	BATUartSendData[k+6] = ' ';	
	BATUartSendData[k+7] = 0x30+Current_offset/100000;
	BATUartSendData[k+8] = 0x30+(Current_offset%100000)/10000;
	BATUartSendData[k+9] = 0x30+(Current_offset%10000)/1000;
	BATUartSendData[k+10] = 0x30+(Current_offset%1000)/100;
	BATUartSendData[k+11] = 0x30+(Current_offset%100)/10;
	BATUartSendData[k+12] = 0x30+(Current_offset%10)/1;
	BATUartSendData[k+13] = '\r';
	BATUartSendData[k+14] = '\n';
	k = k+15;	
	BATUartSendData[k] = 'H';
	BATUartSendData[k+1] = 'C';
	BATUartSendData[k+2] = ' ';
	BATUartSendData[k+3] = 'O';
	BATUartSendData[k+4] = 'F';
	BATUartSendData[k+5] = 'S';
	BATUartSendData[k+6] = ' ';	
	BATUartSendData[k+7] = 0x30+Heat_Current_offset/100000;
	BATUartSendData[k+8] = 0x30+(Heat_Current_offset%100000)/10000;
	BATUartSendData[k+9] = 0x30+(Heat_Current_offset%10000)/1000;
	BATUartSendData[k+10] = 0x30+(Heat_Current_offset%1000)/100;
	BATUartSendData[k+11] = 0x30+(Heat_Current_offset%100)/10;
	BATUartSendData[k+12] = 0x30+(Heat_Current_offset%10)/1;
	BATUartSendData[k+13] = '\r';
	BATUartSendData[k+14] = '\n';
	k = k+15;	
	BATUartSendData[k] = 'C';
	BATUartSendData[k+1] = ' ';
	BATUartSendData[k+2] = ' ';
	BATUartSendData[k+3] = 'm';
	BATUartSendData[k+4] = 'A';
	BATUartSendData[k+5] = ' ';
	if((Current&0x80000000) == 0x80000000){
		BATUartSendData[k+6] = '-';	
	}else{
		BATUartSendData[k+6] = '+';
	}
	BATUartSendData[k+7] = 0x30+(Current&0x7FFFFFF)/100000;
	BATUartSendData[k+8] = 0x30+((Current&0x7FFFFFF)%100000)/10000;
	BATUartSendData[k+9] = 0x30+((Current&0x7FFFFFF)%10000)/1000;
	BATUartSendData[k+10] = 0x30+((Current&0x7FFFFFF)%1000)/100;
	BATUartSendData[k+11] = 0x30+((Current&0x7FFFFFF)%100)/10;
	BATUartSendData[k+12] = 0x30+((Current&0x7FFFFFF)%10)/1;
	BATUartSendData[k+13] = '\r';
	BATUartSendData[k+14] = '\n';
	k = k+15;	
	BATUartSendData[k] = 'D';
	BATUartSendData[k+1] = 'S';
	BATUartSendData[k+2] = 'G';
	BATUartSendData[k+3] = 'R';
	BATUartSendData[k+4] = 'A';
	BATUartSendData[k+5] = 'T';
	BATUartSendData[k+6] = 'E';	
	BATUartSendData[k+7] = ' ';
	BATUartSendData[k+8] = 0x30+(DSG_Rate%1000)/100;
	BATUartSendData[k+9] = 0x30+(DSG_Rate%100)/10;
	BATUartSendData[k+10] = 0x30+(DSG_Rate%10)/1;
	BATUartSendData[k+11] = '\r';
	BATUartSendData[k+12] = '\n';
	k = k+13;	
	BATUartSendData[k] = 'C';
	BATUartSendData[k+1] = 'H';
	BATUartSendData[k+2] = 'G';
	BATUartSendData[k+3] = 'R';
	BATUartSendData[k+4] = 'A';
	BATUartSendData[k+5] = 'T';
	BATUartSendData[k+6] = 'E';	
	BATUartSendData[k+7] = ' ';
	BATUartSendData[k+8] = 0x30+(CHG_Rate%1000)/100;
	BATUartSendData[k+9] = 0x30+(CHG_Rate%100)/10;
	BATUartSendData[k+10] = 0x30+(CHG_Rate%10)/1;
	BATUartSendData[k+11] = '\r';
	BATUartSendData[k+12] = '\n';
	k = k+13;
	BATUartSendData[k] = 'H';
	BATUartSendData[k+1] = 'C';
	BATUartSendData[k+2] = ' ';
	BATUartSendData[k+3] = ' ';
	BATUartSendData[k+4] = 'm';
	BATUartSendData[k+5] = 'A';
	BATUartSendData[k+6] = ' ';	
	BATUartSendData[k+7] = 0x30+Heat_Current/100000;
	BATUartSendData[k+8] = 0x30+(Heat_Current%100000)/10000;
	BATUartSendData[k+9] = 0x30+(Heat_Current%10000)/1000;
	BATUartSendData[k+10] = 0x30+(Heat_Current%1000)/100;
	BATUartSendData[k+11] = 0x30+(Heat_Current%100)/10;
	BATUartSendData[k+12] = 0x30+(Heat_Current%10)/1;
	BATUartSendData[k+13] = '\r';
	BATUartSendData[k+14] = '\n';
	k = k+15;	
	BATUartSendData[k] = 'F';
	BATUartSendData[k+1] = 'C';
	BATUartSendData[k+2] = ' ';
	BATUartSendData[k+3] = 'm';
	BATUartSendData[k+4] = 'A';
	BATUartSendData[k+5] = 'h';
	BATUartSendData[k+6] = ' ';	
	BATUartSendData[k+7] = 0x30+Full_CAP/100000;
	BATUartSendData[k+8] = 0x30+(Full_CAP%100000)/10000;
	BATUartSendData[k+9] = 0x30+(Full_CAP%10000)/1000;
	BATUartSendData[k+10] = 0x30+(Full_CAP%1000)/100;
	BATUartSendData[k+11] = 0x30+(Full_CAP%100)/10;
	BATUartSendData[k+12] = 0x30+(Full_CAP%10)/1;
	BATUartSendData[k+13] = '\r';
	BATUartSendData[k+14] = '\n';
	k = k+15;	
	BATUartSendData[k] = 'F';
	BATUartSendData[k+1] = 'C';
	BATUartSendData[k+2] = 'r';
	BATUartSendData[k+3] = 'm';
	BATUartSendData[k+4] = 'A';
	BATUartSendData[k+5] = 'h';
	BATUartSendData[k+6] = ' ';	
	BATUartSendData[k+7] = 0x30+Full_CAP_RT/100000;
	BATUartSendData[k+8] = 0x30+(Full_CAP_RT%100000)/10000;
	BATUartSendData[k+9] = 0x30+(Full_CAP_RT%10000)/1000;
	BATUartSendData[k+10] = 0x30+(Full_CAP_RT%1000)/100;
	BATUartSendData[k+11] = 0x30+(Full_CAP_RT%100)/10;
	BATUartSendData[k+12] = 0x30+(Full_CAP_RT%10)/1;
	BATUartSendData[k+13] = '\r';
	BATUartSendData[k+14] = '\n';
	k = k+15;	
	BATUartSendData[k] = 'R';
	BATUartSendData[k+1] = 'C';
	BATUartSendData[k+2] = ' ';
	BATUartSendData[k+3] = 'm';
	BATUartSendData[k+4] = 'A';
	BATUartSendData[k+5] = 'h';
	BATUartSendData[k+6] = ' ';	
	BATUartSendData[k+7] = 0x30+Rem_CAP_mAh/100000;
	BATUartSendData[k+8] = 0x30+(Rem_CAP_mAh%100000)/10000;
	BATUartSendData[k+9] = 0x30+(Rem_CAP_mAh%10000)/1000;
	BATUartSendData[k+10] = 0x30+(Rem_CAP_mAh%1000)/100;
	BATUartSendData[k+11] = 0x30+(Rem_CAP_mAh%100)/10;
	BATUartSendData[k+12] = 0x30+(Rem_CAP_mAh%10)/1;
	BATUartSendData[k+13] = '\r';
	BATUartSendData[k+14] = '\n';
	k = k+15;
	BATUartSendData[k] = 'R';
	BATUartSendData[k+1] = 'C';
	BATUartSendData[k+2] = ' ';
	BATUartSendData[k+3] = 'm';
	BATUartSendData[k+4] = 'A';
	BATUartSendData[k+5] = 'm';
	BATUartSendData[k+6] = 's';	
	BATUartSendData[k+7] = ' ';
	BATUartSendData[k+8] = 0x30+Remain_CAP_mAms/100000000;
	BATUartSendData[k+9] = 0x30+(Remain_CAP_mAms%100000000)/10000000;
	BATUartSendData[k+10] = 0x30+(Remain_CAP_mAms%10000000)/1000000;
	BATUartSendData[k+11] = 0x30+(Remain_CAP_mAms%1000000)/100000;
	BATUartSendData[k+12] = 0x30+(Remain_CAP_mAms%100000)/10000;
	BATUartSendData[k+13] = 0x30+(Remain_CAP_mAms%10000)/1000;
	BATUartSendData[k+14] = 0x30+(Remain_CAP_mAms%1000)/100;
	BATUartSendData[k+15] = 0x30+(Remain_CAP_mAms%100)/10;
	BATUartSendData[k+16] = 0x30+(Remain_CAP_mAms%10)/1;
	BATUartSendData[k+17] = '\r';
	BATUartSendData[k+18] = '\n';
	k = k+19;	
	BATUartSendData[k] = 'R';
	BATUartSendData[k+1] = 'C';
	BATUartSendData[k+2] = ' ';
	BATUartSendData[k+3] = '%';
	BATUartSendData[k+4] = ' ';
	BATUartSendData[k+5] = ' ';
	BATUartSendData[k+6] = ' ';	
	BATUartSendData[k+7] = 0x30+Remain_CAP_Percent/100;
	BATUartSendData[k+8] = 0x30+(Remain_CAP_Percent%100)/10;
	BATUartSendData[k+9] = 0x30+(Remain_CAP_Percent%10)/1;
	BATUartSendData[k+10] = '\r';
	BATUartSendData[k+11] = '\n';
	k = k+12;	
	BATUartSendData[k] = 'L';
	BATUartSendData[k+1] = 'D';
	BATUartSendData[k+2] = 'C';
	BATUartSendData[k+3] = ' ';
	BATUartSendData[k+4] = 'm';
	BATUartSendData[k+5] = 'A';
	BATUartSendData[k+6] = 'h';	
	BATUartSendData[k+7] = ' ';
	BATUartSendData[k+8] = 0x30+DSG_CAP_Lifetime_mAh/100000000;
	BATUartSendData[k+9] = 0x30+(DSG_CAP_Lifetime_mAh%100000000)/10000000;
	BATUartSendData[k+10] = 0x30+(DSG_CAP_Lifetime_mAh%10000000)/1000000;
	BATUartSendData[k+11] = 0x30+(DSG_CAP_Lifetime_mAh%1000000)/100000;
	BATUartSendData[k+12] = 0x30+(DSG_CAP_Lifetime_mAh%100000)/10000;
	BATUartSendData[k+13] = 0x30+(DSG_CAP_Lifetime_mAh%10000)/1000;
	BATUartSendData[k+14] = 0x30+(DSG_CAP_Lifetime_mAh%1000)/100;
	BATUartSendData[k+15] = 0x30+(DSG_CAP_Lifetime_mAh%100)/10;
	BATUartSendData[k+16] = 0x30+(DSG_CAP_Lifetime_mAh%10)/1;
	BATUartSendData[k+17] = '\r';
	BATUartSendData[k+18] = '\n';
	k = k+19;	
	BATUartSendData[k] = 'L';
	BATUartSendData[k+1] = 'C';
	BATUartSendData[k+2] = 'C';
	BATUartSendData[k+3] = ' ';
	BATUartSendData[k+4] = 'm';
	BATUartSendData[k+5] = 'A';
	BATUartSendData[k+6] = 'h';	
	BATUartSendData[k+7] = ' ';
	BATUartSendData[k+8] = 0x30+CHG_CAP_Lifetime_mAh/100000000;
	BATUartSendData[k+9] = 0x30+(CHG_CAP_Lifetime_mAh%100000000)/10000000;
	BATUartSendData[k+10] = 0x30+(CHG_CAP_Lifetime_mAh%10000000)/1000000;
	BATUartSendData[k+11] = 0x30+(CHG_CAP_Lifetime_mAh%1000000)/100000;
	BATUartSendData[k+12] = 0x30+(CHG_CAP_Lifetime_mAh%100000)/10000;
	BATUartSendData[k+13] = 0x30+(CHG_CAP_Lifetime_mAh%10000)/1000;
	BATUartSendData[k+14] = 0x30+(CHG_CAP_Lifetime_mAh%1000)/100;
	BATUartSendData[k+15] = 0x30+(CHG_CAP_Lifetime_mAh%100)/10;
	BATUartSendData[k+16] = 0x30+(CHG_CAP_Lifetime_mAh%10)/1;
	BATUartSendData[k+17] = '\r';
	BATUartSendData[k+18] = '\n';
	k = k+19;	
	
	BATUartSendData[k] = 'B';
	BATUartSendData[k+1] = 'P';
	BATUartSendData[k+2] = ' ';
	BATUartSendData[k+3] = 'S';
	BATUartSendData[k+4] = 'T';
	BATUartSendData[k+5] = 'U';
	BATUartSendData[k+6] = ' ';	
	if(((BAT_Protect_Status&0xF0000000)>>28)>9)      	BATUartSendData[k+7] = 55+((BAT_Protect_Status&0xF0000000)>>28);
	else                                        			BATUartSendData[k+7] = 48+((BAT_Protect_Status&0xF0000000)>>28);
	if(((BAT_Protect_Status&0x0F000000)>>24)>9)      	BATUartSendData[k+8] = 55+((BAT_Protect_Status&0x0F000000)>>24);
	else                                        			BATUartSendData[k+8] = 48+((BAT_Protect_Status&0x0F000000)>>24);
	if(((BAT_Protect_Status&0x00F00000)>>20)>9)       BATUartSendData[k+9] = 55+((BAT_Protect_Status&0x00F00000)>>20);
	else                                        			BATUartSendData[k+9] = 48+((BAT_Protect_Status&0x00F00000)>>20); 
	if(((BAT_Protect_Status&0x000F0000)>>16)>9)      	BATUartSendData[k+10] = 55+((BAT_Protect_Status&0x000F0000)>>16);
	else                                        			BATUartSendData[k+10] = 48+((BAT_Protect_Status&0x000F0000)>>16);
	if(((BAT_Protect_Status&0x0000F000)>>12)>9)      	BATUartSendData[k+11] = 55+((BAT_Protect_Status&0x0000F000)>>12);
	else                                        			BATUartSendData[k+11] = 48+((BAT_Protect_Status&0x0000F000)>>12);
	if(((BAT_Protect_Status&0x00000F00)>>8)>9)       	BATUartSendData[k+12] = 55+((BAT_Protect_Status&0x00000F00)>>8);
	else                                        			BATUartSendData[k+12] = 48+((BAT_Protect_Status&0x00000F00)>>8);	
	if(((BAT_Protect_Status&0x000000F0)>>4)>9)      	BATUartSendData[k+13] = 55+((BAT_Protect_Status&0x000000F0)>>4);
	else                                        			BATUartSendData[k+13] = 48+((BAT_Protect_Status&0x000000F0)>>4);
	if(((BAT_Protect_Status&0x0000000F)>>0)>9)       	BATUartSendData[k+14] = 55+((BAT_Protect_Status&0x0000000F)>>0);
	else                                        			BATUartSendData[k+14] = 48+((BAT_Protect_Status&0x0000000F)>>0);		
	BATUartSendData[k+15] = '\r';
	BATUartSendData[k+16] = '\n';
	k = k+17;		
	
	BATUartSendData[k] = 'W';
	BATUartSendData[k+1] = 'K';
	BATUartSendData[k+2] = ' ';
	BATUartSendData[k+3] = 'S';
	BATUartSendData[k+4] = 'T';
	BATUartSendData[k+5] = 'U';
	BATUartSendData[k+6] = ' ';	
	if(((BAT_Protect_Status&0xF0000000)>>28)>9)      	BATUartSendData[k+7] = 55+((BAT_Work_Status&0xF0000000)>>28);
	else                                        			BATUartSendData[k+7] = 48+((BAT_Work_Status&0xF0000000)>>28);
	if(((BAT_Protect_Status&0x0F000000)>>24)>9)      	BATUartSendData[k+8] = 55+((BAT_Work_Status&0x0F000000)>>24);
	else                                        			BATUartSendData[k+8] = 48+((BAT_Work_Status&0x0F000000)>>24);
	if(((BAT_Protect_Status&0x00F00000)>>20)>9)       BATUartSendData[k+9] = 55+((BAT_Work_Status&0x00F00000)>>20);
	else                                        			BATUartSendData[k+9] = 48+((BAT_Work_Status&0x00F00000)>>20); 
	if(((BAT_Protect_Status&0x000F0000)>>16)>9)      	BATUartSendData[k+10] = 55+((BAT_Work_Status&0x000F0000)>>16);
	else                                        			BATUartSendData[k+10] = 48+((BAT_Work_Status&0x000F0000)>>16);
	if(((BAT_Protect_Status&0x0000F000)>>12)>9)      	BATUartSendData[k+11] = 55+((BAT_Work_Status&0x0000F000)>>12);
	else                                        			BATUartSendData[k+11] = 48+((BAT_Work_Status&0x0000F000)>>12);
	if(((BAT_Protect_Status&0x00000F00)>>8)>9)       	BATUartSendData[k+12] = 55+((BAT_Work_Status&0x00000F00)>>8);
	else                                        			BATUartSendData[k+12] = 48+((BAT_Work_Status&0x00000F00)>>8);	
	if(((BAT_Protect_Status&0x000000F0)>>4)>9)      	BATUartSendData[k+13] = 55+((BAT_Work_Status&0x000000F0)>>4);
	else                                        			BATUartSendData[k+13] = 48+((BAT_Work_Status&0x000000F0)>>4);
	if(((BAT_Protect_Status&0x0000000F)>>0)>9)       	BATUartSendData[k+14] = 55+((BAT_Work_Status&0x0000000F)>>0);
	else                                        			BATUartSendData[k+14] = 48+((BAT_Work_Status&0x0000000F)>>0);		
	BATUartSendData[k+15] = '\r';
	BATUartSendData[k+16] = '\n';
	k = k+17;
	
	BATUartSendData[k] = 'T';
	BATUartSendData[k+1] = 'M';
	BATUartSendData[k+2] = 'P';
	BATUartSendData[k+3] = 'p';
	BATUartSendData[k+4] = 'a';
	BATUartSendData[k+5] = 'r';
	BATUartSendData[k+6] = ' ';
	k = k+7;
	BATUartSendData[k] = '0'+CAP_Temp_para_old/100;
	BATUartSendData[k+1] = '0'+(CAP_Temp_para_old%100)/10;	
	BATUartSendData[k+2] = '0'+CAP_Temp_para_old%10;
	k = k+3;
	BATUartSendData[k] = '\r';
	BATUartSendData[k+1] = '\n';	
	k = k+2;	
	BATUartSendData[k] = '\r';
	BATUartSendData[k+1] = '\n';	
	k = k+2;	
	uart3_send(BATUartSendData, k);
	uart1_send(BATUartSendData, k);
}

/*
	END OF FILE
*/
