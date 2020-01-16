#include "spi.h"

uint8_t *spi_sendpt;							//��Ҫ���͵����ݿ�ָ��
uint16_t spi_sendnum;							//��Ҫ���͵����ݸ���
uint8_t spi_receivebuff[500];			//�������ݻ�����
uint16_t spi_receivenum;					//���յ���Ч���ݵĸ���
uint16_t spi_readstep;						//��һ�ζ�ȡʱ��spi_receivebuff���

void SPI_init(void){
	LPC_SC->PCLKSEL0 |= 0x00020000;		//SystemCoreClock/2��Ƶ
	LPC_SC->PCONP |= 0x00000100;			//ʹ�����蹩��
	//���Ź�������
	PIN_Configure (0,15,PIN_FUNC_3 ,PIN_PINMODE_PULLUP ,PIN_PINMODE_NORMAL );
//	PIN_Configure (0,16,PIN_FUNC_3 ,PIN_PINMODE_PULLUP ,PIN_PINMODE_NORMAL );	
	PIN_Configure (0,17,PIN_FUNC_3 ,PIN_PINMODE_PULLUP ,PIN_PINMODE_NORMAL );	
	PIN_Configure (0,18,PIN_FUNC_3 ,PIN_PINMODE_PULLUP ,PIN_PINMODE_NORMAL );	
	
	SSEL_DIR_OUTPUT ;
	SSEL_SET_1 ;
	//SPI����
	LPC_SPI ->SPCR = 0x00B8;	//8λ���ݣ�CPHA=1��CPOL=1������ģʽ��MSB���ȣ�ʹ���ж�
	LPC_SPI ->SPCCR = 50;
	//ʹ��NVIC
	NVIC_EnableIRQ (SPI_IRQn);
}

uint16_t SPI_6811_GetRxBufferSize(void){
	return spi_receivenum ;
}
void SPI_6811_PutArray(uint8_t buffer[],uint16_t byteCount){
	spi_sendpt = buffer;
	spi_sendnum = byteCount;
	spi_receivenum = 0;
	spi_readstep = 0;
	SSEL_SET_0 ;
	LPC_SPI ->SPDR = *spi_sendpt;
	spi_sendpt++;
	spi_sendnum--;
}

uint8_t SPI_6811_ReadRxData(void){
	if(spi_readstep < spi_receivenum){
		spi_readstep ++;
		if(spi_readstep == (spi_receivenum-1)){
			spi_readstep = 0;
			return spi_receivebuff[spi_receivenum-1];
		}else {
			return spi_receivebuff[spi_readstep];
		}
	}else return 0;
}

void SPI_IRQHandler(void){
	LPC_SPI ->SPINT = 0x01;
	if((LPC_SPI->SPSR & 0x80) == 0x80){
		spi_receivebuff[spi_receivenum] = LPC_SPI ->SPDR;		//�����������
		spi_receivenum++;
		if(spi_sendnum > 0){
			LPC_SPI ->SPDR = *spi_sendpt;
			spi_sendpt++;
			spi_sendnum--;
		}else{
			SSEL_SET_1;
			spi_readstep = 0;
		}
	}		
}






