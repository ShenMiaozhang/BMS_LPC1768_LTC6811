/****************************************Copyright (c)****************************************************
**                                 http://www.openmcu.com
**--------------File Info---------------------------------------------------------------------------------
** File name:           can.c
** Last modified Date:  2014-04-18
** Last Version:        V1.1
** Descriptions:        
**
**--------------------------------------------------------------------------------------------------------
** Created by:          openmcu
** Created date:        2012-07-23
** Version:             V1.00
** Descriptions:        ��дʾ������
**
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "CAN.h"                                 

CAN_FRAME        CAN_TxMsg[2];                                          /* ���ͻ�����                   */
CAN_FRAME        CAN_RxMsg[2];                                          /* ���ջ�����                   */
unsigned int     CAN_TxRdy[2] = {0,0};                                  /* ���Ϳ��ñ�ʶ                 */
unsigned int     CAN_RxRdy[2] = {0,0};                                  /* ������ɱ�ʶ                 */

/*********************************************************************************************************
**      ����������  
**      NT = Nominal bit time = TSEG1 + TSEG2 + 3
**      SP = Sample point     = ((TSEG2 +1) / (TSEG1 + TSEG2 + 3)) * 100%
**      SAM,  SJW, TSEG1, TSEG2, NT,  SP             
*********************************************************************************************************/
const uint32_t CAN_BIT_TIME[] = {          0,                           /*  not used                    */
                                           0,                           /*  not used                    */
                                           0,                           /*  not used                    */
                                           0,                           /*  not used                    */
                                  0x0001C000,                           /*  0+1,  3+1, 1+1, 0+1, 4, 75% */
                                           0,                           /*  not used                    */
                                  0x0012C000,                           /*  0+1,  3+1, 2+1,1+1, 6, 67%  */
                                           0,                           /*  not used             */
                                  0x0023C000,                           /*  0+1,  3+1, 3+1,2+1,  8, 63% */
                                           0,                           /*  not used             */
                                  0x0025C000,                           /*  0+1,  3+1, 5+1, 2+1, 10,70% */
                                           0,                           /*  not used             */
                                  0x0036C000,                           /*  0+1,  3+1, 6+1, 3+1,12, 67% */
                                           0,                           /*  not used                    */
                                           0,                           /*  not used                    */
                                  0x0048C000,                           /*  0+1,  3+1, 8+1,4+1, 15, 67% */
                                  0x0049C000,                           /*  0+1,  3+1, 9+1,4+1, 16, 69% */
                                };

/*********************************************************************************************************
** Functoin name:       CAN_cfgBaudrate
** Descriptions:        ���ò�����
** input paraments:     ctrl:CAN�������ţ���ѡ�� CAN1/CAN2
**                      baudrate: ������     
** output paraments:    ��    
** Returned values:     ��
*********************************************************************************************************/
static void CAN_cfgBaudrate(uint32_t ctrl, uint32_t baudrate){
    LPC_CAN_TypeDef *pCAN = (ctrl == 0) ? LPC_CAN1 : LPC_CAN2;
    uint32_t result = 0;
    uint32_t nominal_time;

    if (((PCLK / 1000000) % 6) == 0) 
		{
      nominal_time = 12;                                                /* CCLK Ϊ72MHz                 */
    } 
		else 
		{
      nominal_time = 10;                                                /* CCLK Ϊ100MHz                */
    }

    result  = (PCLK / nominal_time) / baudrate - 1;				//������Ԥ��Ƶֵ BRP=2.5M/baudrate-1
    result &= 0x000003FF;
    result |= CAN_BIT_TIME[nominal_time];

    pCAN->BTR  = result;                                  //���ò����� 25M/(BRP+1)/(3+5+2)

}

/*********************************************************************************************************
** Functoin name:       CAN_setup
** Descriptions:        ���ÿ�����
** input paraments:     ctrl:CAN�������ţ���ѡ�� CAN1/CAN2    
** output paraments:    ��    
** Returned values:     ��
*********************************************************************************************************/
void CAN_setup(uint32_t ctrl)  
{
  LPC_CAN_TypeDef *pCAN = (ctrl == 0) ? LPC_CAN1 : LPC_CAN2;
//	LPC_SC->PCLKSEL0 &= 0x03FFFFFF;
	
  if(ctrl == 0) {
      LPC_SC->PCONP |= (1 << 13);                                //CAN������1�ϵ�               */
			PIN_Configure(0,0,PIN_FUNC_1,PIN_PINMODE_PULLUP,PIN_PINMODE_NORMAL);
			PIN_Configure(0,1,PIN_FUNC_1,PIN_PINMODE_PULLUP,PIN_PINMODE_NORMAL);
  }else{
      LPC_SC->PCONP |= (1 << 14);                                //CAN������2�ϵ�               */
			PIN_Configure(2,7,PIN_FUNC_1,PIN_PINMODE_PULLUP,PIN_PINMODE_NORMAL);
			PIN_Configure(2,8,PIN_FUNC_1,PIN_PINMODE_PULLUP,PIN_PINMODE_NORMAL);
  }
	
	NVIC_EnableIRQ(CAN_IRQn);                                           //����CAN�ж�                  */
	NVIC_SetPriority(CAN_IRQn,2);
	
	LPC_CANAF->AFMR = 2;                                                /* ��ʹ��Ӳ���˲�               */
	pCAN->MOD   = 1;                                                    /* ���븴λģʽ                 */
	pCAN->IER   = 0;                                                    /* �رս��շ����ж�             */
	pCAN->GSR   = 0;                                                    /* ���״̬�Ĵ���               */
	CAN_cfgBaudrate(ctrl, 500000);                                      /* ���ò�����                   */
	pCAN->IER   = 0x0003;                                               /* ������շ����ж�             */
}

/*********************************************************************************************************
** Functoin name:       CAN_start
** Descriptions:        ����������
** input paraments:     ctrl:CAN�������ţ���ѡ�� CAN1/CAN2    
** output paraments:    ��    
** Returned values:     ��
*********************************************************************************************************/
void CAN_start(uint32_t ctrl)  
{
  LPC_CAN_TypeDef *pCAN = (ctrl == 0) ? LPC_CAN1 : LPC_CAN2;
  pCAN->MOD = 0;                                 //������������ģʽ
}
void CAN_reset (uint32_t ctrl)  
{
  LPC_CAN_TypeDef *pCAN = (ctrl == 0) ? LPC_CAN1 : LPC_CAN2;
  pCAN->MOD = 1;                                 //������������ģʽ
}
/*********************************************************************************************************
** Functoin name:       CAN_waitReady
** Descriptions:        �ȴ����Ϳ���
** input paraments:     ctrl:CAN�������ţ���ѡ�� CAN1/CAN2    
** output paraments:    ��    
** Returned values:     ��
*********************************************************************************************************/
void CAN_waitReady(uint32_t ctrl)  
{
  LPC_CAN_TypeDef *pCAN = (ctrl == 0) ? LPC_CAN1 : LPC_CAN2;

  while ((pCAN->SR & (1<<2)) == 0);             //�ȴ����ͻ�����1�ͷ� ����
	CAN_TxRdy[ctrl] = 1;												//��λ���ͱ�־ 
}

/*********************************************************************************************************
** Functoin name:       CAN_send
** Descriptions:        ���ͳ���
** input paraments:     ctrl:CAN�������ţ���ѡ�� CAN1/CAN2
**                      msg: �����͵�֡    
** output paraments:    ��    
** Returned values:     ��
*********************************************************************************************************/
void CAN_send(uint32_t ctrl,CAN_FRAME *msg)  
{
  uint32_t CANData;
  LPC_CAN_TypeDef *pCAN = (ctrl == 0) ? LPC_CAN1 : LPC_CAN2;
	CAN_reset (ctrl);
	CAN_start (ctrl);
	CAN_waitReady(ctrl);
  CANData = ((((uint32_t) msg->len) << 16)     & 0x000F0000) |  			//���ݳ���
            ((msg->format == EXTENDED_FORMAT ) * 0x80000000) |			//0��׼֡��1��չ֡
            ((msg->type   == REMOTE_FRAME)     * 0x40000000);				//0����֡��1Զ��֡

  if (pCAN->SR & (1<<2))  
	{                                 
    pCAN->TFI1  = CANData;                                        //���֡��Ϣ 
    pCAN->TID1 = msg->id;                                         //���֡id                     
    pCAN->TDA1 = *(uint32_t *) &msg->data[0];                     //������ݳ�ǰ4�ֽ� 
    pCAN->TDB1 = *(uint32_t *) &msg->data[4];                     //������ݳ���4�ֽ�   
//    pCAN->CMR  = 0x31;                                          //ʹ���Է�����ģʽ      
    pCAN->CMR  = 0x21;                                            //����                         
  }
}

/*********************************************************************************************************
** Functoin name:       CAN_recive
** Descriptions:        ���ճ���
** input paraments:     ctrl:CAN�������ţ���ѡ�� CAN1/CAN2
**                      msg:  �洢������     
** output paraments:    ��    
** Returned values:     ��
*********************************************************************************************************/
void CAN_recive(uint32_t ctrl,CAN_FRAME *msg)  
{
  LPC_CAN_TypeDef *pCAN = (ctrl == 0) ? LPC_CAN1 : LPC_CAN2;
  uint32_t CANData;
                                                                        
  CANData = pCAN->RFS;																				//��ȡ֡��Ϣ 
  msg->format   = (CANData & 0x80000000) == 0x80000000;				//����֡ ����չ֡
  msg->type     = (CANData & 0x40000000) == 0x40000000;				//��׼֡ ��Զ��֡
  msg->len      = ((uint8_t)(CANData >> 16)) & 0x0F;					//���ݳ���
  msg->id = pCAN->RID;                                        //��ȡCAN��ʶ��                

  if (msg->type == DATA_FRAME)  
	{                             //��ȡ���ݳ�                   
    *(uint32_t *) &msg->data[0] = pCAN->RDA;
    *(uint32_t *) &msg->data[4] = pCAN->RDB;
  }
  pCAN->CMR = (1 << 2);                                       //�ͷŽ��ջ�����
}

void CAN_test(uint8_t chn){
	CAN_TxMsg[chn].id = 0x0001;
	CAN_TxMsg[chn].data[0] = 0xAA;
	CAN_TxMsg[chn].data[1] = 0xBB;
	CAN_TxMsg[chn].data[2] = 0xCC;
	CAN_TxMsg[chn].data[3] = 0xDD;
	CAN_TxMsg[chn].data[4] = 0xEE;
	CAN_TxMsg[chn].data[5] = 0xFF;
	CAN_TxMsg[chn].data[6] = 0x5A;
	CAN_TxMsg[chn].data[7] = 0xA5;
	if(CAN_RxRdy[chn] == 1){
		CAN_RxRdy[chn] = 0;
		if(CAN_RxMsg[chn].id == 0x0002){
			CAN_TxMsg[chn].data[0] = CAN_RxMsg[chn].data[0];
			CAN_TxMsg[chn].data[1] = CAN_RxMsg[chn].data[1];
			CAN_TxMsg[chn].data[2] = CAN_RxMsg[chn].data[2];
			CAN_TxMsg[chn].data[3] = CAN_RxMsg[chn].data[3];
			CAN_TxMsg[chn].data[4] = CAN_RxMsg[chn].data[4];
			CAN_TxMsg[chn].data[5] = CAN_RxMsg[chn].data[5];
			CAN_TxMsg[chn].data[6] = CAN_RxMsg[chn].data[6];
			CAN_TxMsg[chn].data[7] = CAN_RxMsg[chn].data[7];
		}
	}
	CAN_TxMsg[chn].len = 8;	
	CAN_TxMsg[chn].format = 0;
	CAN_TxMsg[chn].type = 0;	
	CAN_waitReady(chn);
	CAN_send(chn,&CAN_TxMsg[chn]);
}


unsigned int EXP_STD_ID[2]={0x0006,0x0008};			//���ձ�ʶ��Ϊ0x0001��0x0002�ı�׼��ʶ��֡
unsigned int GRP_STD_ID[2]={0x000,0x005};				//���ձ�ʶ����0x000��0x010֮��ı�׼֡��ʶ��
unsigned int EXP_EXT_ID[2]={0x100000,0x100001};	//���ձ�ʶ��Ϊ0x100000��0x100001����չ��ʶ��֡
unsigned int GRP_EXT_ID[2]={0x200000,0x200010};	//���ձ�ʶ����0x200000��0x200010֮�����չ֡��ʶ��
/*********************************************************************************************************
** Functoin name:       CAN_SetACCF_Lookup
** Descriptions:        �����˲���
** input paraments:     ��
** output paraments:    ��    
** Returned values:     ��
*********************************************************************************************************/
void CAN_SetACCF_Lookup(void)
{
  uint32_t address = 0;
//  uint32_t i;
  uint32_t ID_high, ID_low;

  LPC_CANAF->SFF_sa = address;				//���õ�����׼֡��ʶ������ַ
	ID_low = (0 << 29) | (EXP_STD_ID[0] << 16);
	ID_high = ((0) << 13) | (EXP_STD_ID[1] << 0);
	*((volatile uint32_t *)(LPC_CANAF_RAM_BASE + address)) = ID_low | ID_high;
	address += 4; 
		
  LPC_CANAF->SFF_GRP_sa = address;		//���ñ�׼֡���ʶ������ַ
	ID_low = (0 << 29) | (GRP_STD_ID[0] << 16);
	ID_high = ((0) << 13) | (GRP_STD_ID[1] << 0);
	*((volatile uint32_t *)(LPC_CANAF_RAM_BASE + address)) = ID_low | ID_high;
	address += 4; 
 
  LPC_CANAF->EFF_sa = address;				//���õ�����չ֡��ʶ������ַ
	ID_low = (0 << 29) | (EXP_EXT_ID[0] << 0);
	*((volatile uint32_t *)(LPC_CANAF_RAM_BASE + address)) = ID_low;
	address += 4; 
	ID_low = (0 << 29) | (EXP_EXT_ID[1] << 0);
	*((volatile uint32_t *)(LPC_CANAF_RAM_BASE + address)) = ID_low;
	address += 4; 

  LPC_CANAF->EFF_GRP_sa = address;		//������չ֡���ʶ������ַ
	ID_low = (0 << 29) | (GRP_EXT_ID[0] << 0);
	*((volatile uint32_t *)(LPC_CANAF_RAM_BASE + address)) = ID_low;
	address += 4; 
	ID_low = (0 << 29) | (GRP_EXT_ID[1] << 0);
	*((volatile uint32_t *)(LPC_CANAF_RAM_BASE + address)) = ID_low;
	address += 4; 

  LPC_CANAF->ENDofTable = address;		//����AF�������ַ
  return;
}

/*********************************************************************************************************
** Functoin name:       CAN_SetACCF
** Descriptions:        ����CANģʽ
** input paraments:     ACCFMode ģʽ����
** output paraments:    ��    
** Returned values:     ��
*********************************************************************************************************/
void CAN_SetACCF(uint32_t ACCFMode )
{
  switch(ACCFMode)
  {
		case ACCF_OFF:		//����������
			LPC_CANAF->AFMR = ACCFMode;
			LPC_CAN1->MOD = LPC_CAN2->MOD = 1;	//��λCAN
			LPC_CAN1->IER = LPC_CAN2->IER = 0;	//��ֹ�����ж�
			LPC_CAN1->GSR = LPC_CAN2->GSR = 0;	//��λ���������
		break;

		case ACCF_BYPASS:		//��ʹ���˲���������
			LPC_CANAF->AFMR = ACCFMode;
		break;

		case ACCF_ON:				//ʹ���˲�
		case ACCF_FULLCAN:
			LPC_CANAF->AFMR = ACCF_OFF;
			CAN_SetACCF_Lookup();
			LPC_CANAF->AFMR = ACCFMode;
		break;

		default:
		break;
  }
  return;
}

void CAN_IRQHandler(void)  
{
  volatile uint32_t icr;
                                                                      
  icr = LPC_CAN1->ICR;               	//��ȡ�ж���Ϣ  ��ȡ����
  if (icr & (1 << 0))  								//�����ж�
	{                          
    CAN_recive(CAN1, &CAN_RxMsg[CAN1]);  //��ȡ����
    CAN_RxRdy[CAN1] = 1;              //��λ���ձ�־
  }
  if (icr & (1 << 1)) 								//������1�������
	{                           				                      
		CAN_TxRdy[CAN1] = 1;  						//��λ���ͱ�־ 
  }

	icr = LPC_CAN2->ICR;                //��ȡ�ж���Ϣ  ��ȡ���� 
  if (icr & (1 << 0))  								//�����ж�
	{                          
    CAN_recive(CAN2, &CAN_RxMsg[CAN2]);  //��ȡ����   
    CAN_RxRdy[CAN2] = 1;              //��λ���ձ�־		
  }
  if (icr & (1 << 1)) 								//������1�������   
	{                           				                      
		CAN_TxRdy[CAN2] = 1;  						//��λ���ͱ�־ 
  }	
}

/*********************************************************************************************************
**                                        End Of File
*********************************************************************************************************/
