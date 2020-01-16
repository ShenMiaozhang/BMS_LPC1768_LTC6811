#include "ssp0.h"

uint8_t *ssp0_sendpt;								//��Ҫ���͵����ݿ�ָ��
uint16_t ssp0_sendnum;							//��Ҫ���͵����ݸ���
uint8_t ssp0_receivebuff[100];			//�������ݻ�����
uint16_t ssp0_receivenum;						//���յ���Ч���ݵĸ���
uint16_t ssp0_readstep;							//��һ�ζ�ȡʱ��spi_receivebuff���

void SSP0_init(void){
	LPC_SC->PCLKSEL1 |= 0x00000800;		//SystemCoreClock/2��Ƶ
	LPC_SC->PCONP |= 0x00200000;			//ʹ�����蹩��
	//���Ź�������
	PIN_Configure (0,15,PIN_FUNC_2 ,PIN_PINMODE_PULLUP ,PIN_PINMODE_NORMAL );
//	PIN_Configure (0,16,PIN_FUNC_2 ,PIN_PINMODE_PULLUP ,PIN_PINMODE_NORMAL );	
	PIN_Configure (0,17,PIN_FUNC_2 ,PIN_PINMODE_PULLUP ,PIN_PINMODE_NORMAL );	
	PIN_Configure (0,18,PIN_FUNC_2 ,PIN_PINMODE_PULLUP ,PIN_PINMODE_NORMAL );	
	
	SSEL0_DIR_OUTPUT;
	SSEL0_SET_1;
	//SPI����
	LPC_SSP0 ->CR0 = 0x0AC7;	//8λ���ݣ�SPI,CPHA=1��CPOL=1��1MHz
	LPC_SSP0 ->CPSR = 5;			//1MHz
}

uint16_t SSP0_GetRxBufferSize(void){
	return ssp0_receivenum;
}
void SSP0_PutArray(uint8_t buffer[],uint16_t byteCount){
	ssp0_sendpt = buffer;
	ssp0_sendnum = byteCount;
	ssp0_receivenum = 0;
	ssp0_readstep = 0;
	LPC_SSP0 ->CR1 = 0x02;		//ʹ��SSP1	
	SSEL0_SET_0;		
	while(ssp0_sendnum > 0){
		while((LPC_SSP0->SR & 0x02) == 0){};	// Wait Tx FIFO empty 
			LPC_SSP0 ->DR = *ssp0_sendpt;
			ssp0_sendpt++;
			ssp0_sendnum--;
		while((LPC_SSP0->SR& 0x01) == 0){}; 	// Send out all data in the FIFO		
		while((LPC_SSP0->SR& 0x04) == 0){}; 	// Wait for the Rx data
			ssp0_receivebuff[ssp0_receivenum] = LPC_SSP0 ->DR;		//�����������
			ssp0_receivenum++;		
	}
	SSEL0_SET_1;
	LPC_SSP0 ->CR1 = 0x00;		//�ر�SSP0
}

uint8_t SSP0_ReadRxData(void){
	if(ssp0_readstep < ssp0_receivenum){
		ssp0_readstep ++;
		if(ssp0_readstep == (ssp0_receivenum-1)){
			ssp0_readstep = 0;
			return ssp0_receivebuff[ssp0_receivenum-1];
		}else {
			return ssp0_receivebuff[ssp0_readstep];
		}
	}else return 0;
}









