#include "I2C.h"

uint8_t *I2C0_pt;	//�����͵����ݿ�ָ��
uint8_t I2C0_num;	//�����͵�����������
uint8_t I2C0_cnt;	//Ŀǰ��Ҫ���͵��������
uint8_t I2C0_mode;	//ģʽ 0x00��������		0x01��������
uint8_t I2C0_Done = 0;	//��ɱ�־��0x00:���ͻ������ɣ�0x01��δ��ɡ�
uint8_t I2C0_SA_num = ADDRESS_Byte;

void i2c0_init(void){
	LPC_SC->PCLKSEL0 |= 0x00008000;		//SystemCoreClock/2��Ƶ
	LPC_SC->PCONP |= 0x00000080;			//ʹ�����蹩��
	//���Ź�������
	PIN_Configure (0,28,PIN_FUNC_1,PIN_PINMODE_TRISTATE,PIN_PINMODE_OPENDRAIN);
	PIN_Configure (0,27,PIN_FUNC_1,PIN_PINMODE_TRISTATE,PIN_PINMODE_OPENDRAIN);	
	PIN_ConfigureI2C0Pins (PIN_I2C_Normal_Mode, 1);
	//ģʽ����
	LPC_I2C0->I2SCLH = 250;		//���ò�����100kHz
	LPC_I2C0->I2SCLL = 250;		//���ò�����100kHz
	LPC_I2C0->I2CONSET = 0x00000040;	//ʹ��I2C 
	//ʹ��NVIC
	NVIC_EnableIRQ(I2C0_IRQn);
	
	WP_DIR_OUTPUT;
	WP_SET_0;
}

//	*pt:ָ����Ҫ���͵��������飻num����Ҫ���͵����ݸ���
//ע���������������2�����0������Ҫ��ͬ ���������ж�Ϊ��ȡ
void i2c0_start(uint8_t *pt,uint8_t num){
	I2C0_pt = pt;
	I2C0_num = num;
	I2C0_cnt = 0;
	I2C0_Done = 1;	
	I2C0_SA_num = ADDRESS_Byte;
	if(((*(I2C0_pt+3) & 0x01) == 0x01) &&(*(I2C0_pt) == (*(I2C0_pt+3) & 0xFE)))		I2C0_mode = 0x01;	//�ж�Ϊ������ģʽ
	else																																					I2C0_mode = 0x00;	//�ж�Ϊ������ģʽ
	LPC_I2C0->I2CONSET = 0x00000020;	//��STA
}

uint8_t Get_I2C0_Done(void){
	return I2C0_Done;
}

void I2C0_IRQHandler(void){
//	if((LPC_I2C0->I2CONSET & 0x00000008) == 0x00000008){
  uint8_t StatValue;
  StatValue = LPC_I2C0->I2STAT & 0xF8;
		switch(StatValue){				
			case 0x08:	//�ѷ�����ʼ����	���͵�ַ�Ͷ�дλ
				LPC_I2C0->I2DAT = *I2C0_pt;
				LPC_I2C0->I2CONSET = 0x00000004;	//��AA
				LPC_I2C0->I2CONCLR = 0x00000020;	//����ʼ��־
				I2C0_cnt++;
				break;
			case 0x10:	//�ѷ����ظ���ʼ����
				LPC_I2C0->I2DAT = *(I2C0_pt++);
				LPC_I2C0->I2CONSET = 0x00000004;	//��AA
				I2C0_cnt++;
				break;
			case 0x18:	//֮ǰ״̬Ϊ8 ��10 ��ʾ�ѷ��ʹӻ���ַ��д����λ����������Ӧ�𡣼������͵�һ�������ֽںͽ���ACK λ��
				LPC_I2C0->I2DAT = *(I2C0_pt++);
				LPC_I2C0->I2CONSET = 0x00000004;	//��AA
				I2C0_SA_num--;
				I2C0_cnt++;
				break;
			case 0x20:	//�ѷ��ʹӻ���ַ��д����λ�������˷�Ӧ�𡣼�������ֹͣ������
				LPC_I2C0->I2CONSET = 0x00000014;	//��STO��AA
				break;
			case 0x28:	//�ѷ������ݲ�������ACK��������͵����������һ�������ֽ�����һ��ֹͣ��������������һ�������ֽڡ�
				if(I2C0_mode == 0x00){	//������ģʽ
					if(I2C0_cnt > I2C0_num){
						LPC_I2C0->I2CONSET = 0x00000014;	//��STO��AA
						I2C0_Done = 0;
					}else{
						LPC_I2C0->I2DAT = *(I2C0_pt++);
						I2C0_cnt++;				
						LPC_I2C0->I2CONSET = 0x00000004;	//��AA
					}
				}else{	//������ģʽ �����ظ���ʼ����
					if(I2C0_SA_num > 0){	//����ʣ���ַλ
						I2C0_SA_num --;
						LPC_I2C0->I2DAT = *(I2C0_pt++);
						I2C0_cnt++;				
						LPC_I2C0->I2CONSET = 0x00000004;	//��AA						
					}else{
						LPC_I2C0->I2CONSET = 0x00000020;	//��STA
					}
				}
				break;						
			case 0x30:	//�ѷ������ݲ����յ���Ӧ�𡣼�������ֹͣ����
				LPC_I2C0->I2CONSET = 0x00000014;	//��STO��AA
				break;			
			case 0x38:	//�����������·���
				LPC_I2C0->I2CONSET = 0x00000024;	//��STA��AA
				break;			
			case 0x40:	//ǰ���״̬��08 ��10 ��ʾ�ѷ��ʹӻ���ַ�Ͷ�����λ�������յ�ACK�����������ݺͷ���ACK��
				LPC_I2C0->I2CONSET = 0x00000004;	//��AA
				break;			
			case 0x48:	//�ѷ��ʹӻ���ַ�Ͷ�����λ�������յ���Ӧ�𡣽�����ֹͣ������
				LPC_I2C0->I2CONSET = 0x00000014;	//��STO��AA
				break;			
			case 0x50:	//�ѽ��յ����ݣ�������ACK������I2DAT ��ȡ���ݡ����������������ݡ�����������һ�������ֽڣ��򷵻ط�Ӧ�𣬷��򷵻�ACK��
				if(I2C0_cnt > I2C0_num){
					LPC_I2C0->I2CONCLR = 0x00000004;	//��AA
					I2C0_Done = 0;
				}else{
					*(I2C0_pt++) = LPC_I2C0->I2DAT;
					I2C0_cnt++;					
					LPC_I2C0->I2CONSET = 0x00000004;	//��AA
				}
				break;			
			case 0x58:	//�ѽ��յ����ݣ��ѷ��ط�Ӧ�𡣽���I2DAT �ж�ȡ���ݺͷ���ֹͣ������
				*(I2C0_pt++) = LPC_I2C0->I2DAT;
				LPC_I2C0->I2CONSET = 0x00000014;	//��STO��AA
				break;			
		}
		LPC_I2C0->I2CONCLR = 0x00000008;	//���жϱ�־
}








