#include "pn532_iic.h"

/**
  * @brief  PN532 GPIO ��ʼ��
  * @param  void
  * @return void
  */
void PN532_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
      
    RCC_APB2PeriphClockCmd(PN532_RESET_RCC | PN532_IRQ_RCC, ENABLE);
    
    PN532_IIC_Init();//����IIC��ʼ��
    
    GPIO_InitStructure.GPIO_Pin = PN532_RESET_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(PN532_RESET_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = PN532_IRQ_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(PN532_IRQ_PORT, &GPIO_InitStructure);
}

/**
  * @brief  PN532����֡���
  * @param  in_buf : ���ݸ���ָ��
  * @param  in_len : ���ݸ����ֽڳ���
  * @param  out_buf: ������������������ָ֡��
  * @return �������ݰ�����
  */

static int PN532_Package(uint8_t *in_buf, int in_len, uint8_t* out_buf)
{
    int i = 0;
    uint8_t checksum = 0;
    
    /*PREAMBLE*/
    out_buf[0] = PN532_PREAMBLE;
    /*START CODE*/
    out_buf[1] = PN532_STARTCODE1;
    out_buf[2] = PN532_STARTCODE2;
    /*LEN*/
    out_buf[3] = in_len + 1;
    /*LCS*/
    out_buf[4] = (uint8_t)(0x100 - out_buf[3]);
    /*TFI*/
    out_buf[5] = Master_TO_PN532;
    
    checksum += out_buf[5];
    /*Data*/
    for(i = 0; i < in_len; i++)
    {
        out_buf[i + 6] = in_buf[i];
        checksum += in_buf[i];
    }
    
    /*DCS*/
    out_buf[in_len + 6] = (uint8_t)(0x100 - checksum);
   
    /*Tail*/
    out_buf[in_len + 7] = 0x00;
    
    return in_len + 8;
}

/**
  *
  * @brief  PN532����֡����
  * @param  buf    : ����������ָ֡��
  * @param  length : ����������֡����
  * @param  out_buf: ���������ݸ���ָ��
  * @return ���ݸ��س���
  * @return -1, ���ݲ���Ӧ
  */
static int PN532_Parse(uint8_t *buf, int length, uint8_t *out_buf)
{
    int ret = -1;
    int i;
    uint8_t len;
    
    if(len < 7){//
        goto EXIT;
    }
    
    if((buf[0] != 0x00) || (buf[1] != 0x00) || (buf[2] != 0xff)){//֡ͷ����
        goto EXIT;
    }
    
    if(buf[3] != (0x100 - buf[4])){//LCS�ֳ�У�鲻��
        goto EXIT;
    }
    
    if(buf[3] > len - 7){//ʵ�����ݸ��س�����궨���Ȳ���
        goto EXIT;
    }  
    
    len = buf[3];//ʵ�����ݳ����ɱ궨���ȸ���
    
    if(buf[len + 7 - 1] != 0x00){//֡β����
        goto EXIT;
    }
    
    if(buf[5] != PN532_TO_Master)//����PN532��������������
    {
        goto EXIT;
    }    
    
    for(i = 0; i < buf[3]; i++)//�����ݸ��ض���
    {
        out_buf[i] = buf[6 + i];
    }
    return i;
EXIT:    
    return ret;
}

/**
  *
  * @brief  ��λPN532
  * @param  void
  * @return void
  */
void PN532_Reset(void)
{
    GPIO_SetBits(PN532_RESET_PORT, PN532_RESET_PIN);
    GPIO_ResetBits(PN532_RESET_PORT, PN532_RESET_PIN);
    delay_ms(40);
    GPIO_SetBits(PN532_RESET_PORT, PN532_RESET_PIN);
    delay_ms(10);
}

/**
  *
  * @brief  Wait IIC bus ready.
  * @param  timeout, ��ʱʱ�䣬��λus.
  * @return Result. 
  */
PN532_RES PN532_WaitReady(int timeout)
{
    while(GPIO_ReadInputDataBit(PN532_IRQ_PORT, PN532_IRQ_PIN))
    {
        delay_us(1);

        if(timeout == -1)
        {
            continue;
        }
        else if(!timeout--)
        {
            return PN532_TIMEOUT;
        }
    }
    return PN532_OK;
}

/**
  ******************************************************************************
  * @brief  IIC data buffer, and wait pn532 ack.
  * @param  pbuf, data buffer.
  * @param  len, data length.
  * @param  timeout, wait time, unit us.
  * @return Result. 
  ******************************************************************************/
PN532_RES PN532_Write_WaitAck(uint8_t *pbuf, uint16_t len, uint32_t timeout)
{
    uint8_t ack[] = {0x01, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00};
    uint8_t res[7];
    
    IIC_Write(pbuf, len, PN532_IIC_ADDR);
    
    if(PN532_WaitReady(timeout) != 0)
    {
        return PN532_TIMEOUT;
    }
    
    IIC_Read(res, 7, PN532_IIC_ADDR | 0x01);
    
    if(memcmp(ack, res, 7) == 0)
    {
        return PN532_OK;
    }
   
    return PN532_DATA_ERROR;
}

/**
  ******************************************************************************
  * @brief  Wait PN532 response.
  * @param  pbuf, data buffer.
  * @param  len, data length.
  * @param  timeout, wait time, unit us.
  * @return Result. 
  ******************************************************************************/
PN532_RES PN532_Response(uint8_t *pbuf, uint16_t len, int timeout)
{
    if(PN532_WaitReady(timeout) != 0)
    {
        return PN532_TIMEOUT;
    }
    
    IIC_Read(pbuf, len, PN532_IIC_ADDR | 0x01);  

    return PN532_OK;    
}

/**
  ******************************************************************************
  * @brief  Get PN532 version.
  * @param  ver, version 2 bytes.
  * @return Result. 
  ******************************************************************************/
PN532_RES PN532_Get_Version(uint8_t *ver)
{
    //uint8_t buf[] = {0x00, 0x00, 0xff, 0x02, 0xfe, 0xd4, 0x02, 0x2a, 0x00};  
    uint8_t pbuf[] = {CMD_GET_FW_VERSION};
    uint8_t pack[16];
    uint8_t res[16];
    int ret;
    
    ret = PN532_Package(pbuf, 1, pack);
    
    if(PN532_Write_WaitAck(pack, ret, 2000) != 0)
    {
        return PN532_TIMEOUT;
    }
    
    if(PN532_Response(res, 16, 1000) != 0)
    {
        return PN532_TIMEOUT;
    }
    
    ver[0] = res[9];
    ver[1] = res[10];
    
    sw_delay_us(1000);
    
    return PN532_OK;
}

/**
  ******************************************************************************
  * @brief  Config SAM.
  * @return Result. 
  ******************************************************************************/
PN532_RES PN532_SAMConfig(void)
{
    //uint8_t buf[] = {0x00, 0x00, 0xff, 0x05, 0xfb, 0xd4, 0x14, 0x01, 0x14, 0x01, 0x02, 0x00};   
    uint8_t pbuf[3];
    uint8_t pack[16];
    uint8_t res[16]; 
    int ret;
    
    pbuf[0] = CMD_SAM_CONFIG;
    pbuf[1] = FIND_NFCCARD_MAXNUM;
    pbuf[2] = NFC_106K_PROTOCOL;
    
    ret = PN532_Package(pbuf, 3, pack);
    
    if(PN532_Write_WaitAck(pack, ret, 2000) != 0)
    {
        return PN532_TIMEOUT;
    }
    
    if(PN532_Response(res, 8, 1000) != 0)
    {
        return PN532_TIMEOUT;
    }

    sw_delay_us(1000);
    
    if(res[7] == 0x15)
    {  
        return PN532_OK;
    }
    else
    {
        return PN532_DATA_ERROR;
    }
}

/**
  ******************************************************************************
  * @brief  Wait passive card.
  * @param  card_id, card id.
  * @param  id_len, card bytes count.
  * @return Result. 
  ******************************************************************************/
PN532_RES PN532_ReadPassiveTargetID(uint8_t *card_id, uint8_t *id_len)
{
    uint8_t buf[3];
    uint8_t pack[64];
    uint8_t res[32];
    uint8_t pdata[16];
    int ret;
    
    buf[0] = CMD_IN_LIST_PASSIVE_TARGET;
    buf[1] = FIND_NFCCARD_MAXNUM;
    buf[2] = NFC_106K_PROTOCOL;
    
    ret = PN532_Package(buf, 3, pack);    
    
    if(PN532_Write_WaitAck(pack, ret, 1000) != 0)
    {
        return PN532_TIMEOUT;
    } 
    
    if(PN532_Response(res, 23, -1) != 0)
    {
        return PN532_TIMEOUT;
    }
    
    if(res[0] == 0x01)
    {
        ret = PN532_Parse(&res[1], 22, pdata);
        if(ret > 0)
        {            
            *id_len = pdata[6];
            for(uint8_t i = 0; i < *id_len; i++)
            {
                card_id[i] = pdata[7+i];
            }
               
            return PN532_OK;
        }
    }
    else
    {
        return PN532_TIMEOUT;
    }
    sw_delay_us(1000);
    return PN532_OK;
}