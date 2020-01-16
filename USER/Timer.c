#include "Timer.h"

void rit_init(void){
	LPC_SC->PCLKSEL1 |= 0x08000000;	//SystemCoreClock/2��Ƶ
	LPC_SC->PCONP |= 0x00010000;		//ʹ������
	
	LPC_RIT->RICOMPVAL = 4999;	//100us����һ���ж�
	LPC_RIT->RIMASK = 0x00000000;
	LPC_RIT->RICTRL = 0x03;
	
	NVIC_EnableIRQ(RIT_IRQn);
}

void rit_start(void){
	LPC_RIT->RICTRL |= 0x08;
}

void rit_stop(void){
	LPC_RIT->RICTRL &= ~0x08;
}

void timer3_init(void){
	LPC_SC->PCLKSEL1 |= 0x00008000;	//SystemCoreClock/2��Ƶ
	LPC_SC->PCONP |= 0x00800000;		//ʹ������
	
	LPC_TIM3->CTCR = 0x00;
	LPC_TIM3->PR = 49;	//1us��ʱ
	LPC_TIM3->MR0 = 50;	//50us����һ���ж�
	LPC_TIM3->MCR = 0x0001;	//ƥ��ͨ��0��ƥ���ж�ʹ��
	
	NVIC_EnableIRQ(TIMER3_IRQn);
}

void timer3_start(void){
	LPC_TIM3->TCR |= 0x01;
}

void timer3_stop(void){
	LPC_TIM3->TCR &= ~0x01;
}

void timer3_TC_Reset(void){
	uint8_t i;
	LPC_TIM3->TCR |= 0x02;
	i = 10;
	while(i--);
	LPC_TIM3->TCR &= ~0x02;
}

uint32_t timer3_get_TC(void){
	return LPC_TIM3->TC;
}


/*
	END OF FILE
*/





