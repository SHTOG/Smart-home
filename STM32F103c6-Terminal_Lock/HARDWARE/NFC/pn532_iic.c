#include "pn532_iic.h"

/**
  * @brief  PN532 GPIO 初始化
  * @param  void
  * @return void
  */
void PN532_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
      
    RCC_APB2PeriphClockCmd(PN532_RESET_RCC | PN532_IRQ_RCC, ENABLE);
    
    PN532_IIC_Init();//调用IIC初始化
    
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
  * @brief  PN532数据帧打包
  * @param  in_buf : 数据负载指针
  * @param  in_len : 数据负载字节长度
  * @param  out_buf: 打包后输出的完整数据帧指针
  * @return 完整数据包长度
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
  * @brief  PN532数据帧解析
  * @param  buf    : 被解析数据帧指针
  * @param  length : 被解析数据帧长度
  * @param  out_buf: 解析后数据负载指针
  * @return 数据负载长度
  * @return -1, 数据不对应
  */
static int PN532_Parse(uint8_t *buf, int length, uint8_t *out_buf)
{
    int ret = -1;
    int i;
    uint8_t len;
    
    if(len < 7){//
        goto EXIT;
    }
    
    if((buf[0] != 0x00) || (buf[1] != 0x00) || (buf[2] != 0xff)){//帧头不对
        goto EXIT;
    }
    
    if(buf[3] != (0x100 - buf[4])){//LCS字长校验不对
        goto EXIT;
    }
    
    if(buf[3] > len - 7){//实际数据负载长度与标定长度不对
        goto EXIT;
    }  
    
    len = buf[3];//实际数据长度由标定长度覆盖
    
    if(buf[len + 7 - 1] != 0x00){//帧尾不对
        goto EXIT;
    }
    
    if(buf[5] != PN532_TO_Master)//不是PN532发向主机的数据
    {
        goto EXIT;
    }    
    
    for(i = 0; i < buf[3]; i++)//把数据负载读出
    {
        out_buf[i] = buf[6 + i];
    }
    return i;
EXIT:    
    return ret;
}

/**
  *
  * @brief  复位PN532
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
  * @param  timeout, 超时时间，单位us.
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