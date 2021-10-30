#include"stm32f10x_conf.h"

#define ADS1256_CMD_WREG     0x50 
#define ADS1256_MUX          0x01 

#define ADS1256_DRATE_30000SPS   0xF0 
#define ADS1256_DRATE_15000SPS   0xE0 
#define ADS1256_DRATE_7500SPS   0xD0 
#define ADS1256_DRATE_3750SPS   0xC0 
#define ADS1256_DRATE_2000SPS   0xB0 
#define ADS1256_DRATE_1000SPS   0xA1 
#define ADS1256_DRATE_500SPS    0x92 
#define ADS1256_DRATE_100SPS    0x82 
#define ADS1256_DRATE_60SPS     0x72 
#define ADS1256_DRATE_50SPS     0x63 
#define ADS1256_DRATE_30SPS     0x53 
#define ADS1256_DRATE_25SPS     0x43 
#define ADS1256_DRATE_15SPS     0x33 
#define ADS1256_DRATE_10SPS     0x23 
#define ADS1256_DRATE_5SPS      0x13 
#define ADS1256_DRATE_2_5SPS    0x03

#define ADS1256_DIN_H GPIO_SetBits(GPIOC,GPIO_Pin_0)
#define ADS1256_DIN_L GPIO_ResetBits(GPIOC,GPIO_Pin_0)
#define ADS1256_SCLK_H GPIO_SetBits(GPIOC,GPIO_Pin_1)
#define ADS1256_SCLK_L GPIO_ResetBits(GPIOC,GPIO_Pin_1)
#define ADS1256_RESET_H GPIO_SetBits(GPIOC,GPIO_Pin_2)
#define ADS1256_RESET_L GPIO_ResetBits(GPIOC,GPIO_Pin_2)
#define ADS1256_CS_H GPIO_SetBits(GPIOC,GPIO_Pin_3)
#define ADS1256_CS_L GPIO_ResetBits(GPIOC,GPIO_Pin_3)
#define ADS1256_DRDY GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)
#define ADS1256_DOUT GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)
#define ADS1256_SYNC_H GPIO_SetBits(GPIOC,GPIO_Pin_6)
#define ADS1256_SYNC_L GPIO_ResetBits(GPIOC,GPIO_Pin_6)

void RCC_Configuration(void); 
void GPIO_Configuration(void);
void NVIC_Configuration(void); 
void Delay(vu32 nCount);
void SPI_WriteByte(uint8_t TxData);
uint8_t SPI_ReadByte(void);
void ADS1256WREG(uint8_t regaddr,uint8_t databyte);
void ADS1256_Init(void);
uint32_t ADS1256ReadData(uint8_t channel);

/*****************************************************/
uint32_t ADS1256_ReadData(void);
void Channal_sel(uint8_t cha);
float Volts[8];



uint8_t R_register[11];
int main(void) 
{ 

  RCC_Configuration();
  GPIO_Configuration();
  NVIC_Configuration();

  ADS1256_Init();

  /******** read register data
  ADS1256_CS_L;
  SPI_WriteByte(0x10);
  SPI_WriteByte(0x0A);
  for(uint8_t i=0;i<11;i++)
  {
    R_register[i]=SPI_ReadByte();
  }
  ADS1256_CS_H;
  */
  
  uint32_t test[8];
  uint8_t sss;
  
  while (1) 
  { 
    for(sss=0;sss<8;sss++)
    {
      Channal_sel(sss);
      Delay(0x3FFFF);//通道切换需要等待大于一到两个ADC转换周期，否则会出现通道间数据错误。
      test[sss]=ADS1256_ReadData();
    } 
  } 
} 

void ADS1256_Init(void)
{
  ADS1256_SYNC_H;
  ADS1256_CS_L;
  ADS1256_RESET_L;
  Delay(0x10);
  ADS1256_RESET_H;//reset to power-up values
  
  SPI_WriteByte(0x00);//completes sync and exits standby mode
  while(ADS1256_DRDY);
  
  SPI_WriteByte(0x50);//WREG COM for write reg 0x00
  SPI_WriteByte(0x04);//write 6 register
  SPI_WriteByte(0x06);//00000110---status register
  SPI_WriteByte(0x1F);//00000001--mux register(default values)
  SPI_WriteByte(0x00);//00000000
  SPI_WriteByte(ADS1256_DRATE_50SPS);//01100011
  SPI_WriteByte(0xE0);//default values
  while(ADS1256_DRDY);
  
  SPI_WriteByte(0xF2);//gain self-calibration
  while(ADS1256_DRDY);
  SPI_WriteByte(0xF0);//offset self-calibration
  while(ADS1256_DRDY);

  //SPI_WriteByte(0xF3);//system offset calibration
  //SPI_WriteByte(0xF4);//system gain calibration
}  


void ADS1256WREG(uint8_t regaddr,uint8_t databyte)
{
  ADS1256_CS_L;
  while(ADS1256_DRDY);
  SPI_WriteByte(ADS1256_CMD_WREG | (regaddr & 0x0F));
  SPI_WriteByte(0x00);
  SPI_WriteByte(databyte);
  ADS1256_CS_H;
}


void Channal_sel(uint8_t cha)
{
  ADS1256_CS_L;
  uint8_t cha_temp;
  cha_temp=cha;
  while(ADS1256_DRDY);
  SPI_WriteByte(0x51);
  SPI_WriteByte(0x00);
  cha_temp<<=4;
  SPI_WriteByte(cha_temp|0x0f);
  ADS1256_CS_H;
}

uint32_t ADS1256ReadData(uint8_t channel)  
{
  uint32_t sum=0;
  ADS1256WREG(ADS1256_MUX,channel);
  ADS1256_CS_L;
  Delay(0x01);
  SPI_WriteByte(0x01);
  Delay(0x1);
  sum |= (SPI_ReadByte() << 16);
  sum |= (SPI_ReadByte() << 8);
  sum |= SPI_ReadByte();
  ADS1256_CS_H;
  return sum;
}


uint32_t ADS1256_ReadData(void)  
{
  while(ADS1256_DRDY);
  uint32_t sum=0;
  ADS1256_CS_L;
  SPI_WriteByte(0x01);
  Delay(0x01);
  sum |= (SPI_ReadByte() << 16);
  sum |= (SPI_ReadByte() << 8);
  sum |= SPI_ReadByte();
  ADS1256_CS_H;
  return sum;
}

void SPI_WriteByte(uint8_t TxData)
{
  Delay(0x20);
  for(uint8_t i = 0; i < 8; i++)
  {
    if (TxData & 0x80){ADS1256_DIN_H;}
    else {ADS1256_DIN_L;}
    ADS1256_SCLK_H;
    Delay(0x0f);
    TxData <<= 1;
    ADS1256_SCLK_L;
    Delay(0x0f);
  }
} 


uint8_t SPI_ReadByte(void)
{
  uint8_t read = 0;
  Delay(0x20);
  for (uint8_t i = 0; i < 8; i++)
  {
    ADS1256_SCLK_H;
    Delay(0x0f);
    read = read<<1;
    ADS1256_SCLK_L;
    if(ADS1256_DOUT)
    {
      read++;
    }
    Delay(0x0f);
  }
  return read;
}

void RCC_Configuration(void) 
{    
  ErrorStatus HSEStartUpStatus;
  RCC_DeInit(); 
  RCC_HSEConfig(RCC_HSE_ON); 
  HSEStartUpStatus = RCC_WaitForHSEStartUp(); 
  if(HSEStartUpStatus == SUCCESS) 
  { 
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); 
    FLASH_SetLatency(FLASH_Latency_2); 
    RCC_HCLKConfig(RCC_SYSCLK_Div1);  
    RCC_PCLK2Config(RCC_HCLK_Div1);  
    RCC_PCLK1Config(RCC_HCLK_Div2); 
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); 
    RCC_PLLCmd(ENABLE); 
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) { } 
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); 
    while(RCC_GetSYSCLKSource() != 0x08) { } 
  } 
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE); 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC, ENABLE); 
} 

void GPIO_Configuration(void) 
{ 
  GPIO_InitTypeDef GPIO_InitStructure; 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_6|GPIO_Pin_7; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOC, &GPIO_InitStructure); 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4|GPIO_Pin_5; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
  GPIO_Init(GPIOC, &GPIO_InitStructure); 
} 


void NVIC_Configuration(void) 
{ 
#ifdef  VECT_TAB_RAM   
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);  
#else
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);    
#endif 
} 

void Delay(vu32 nCount) 
{ 
  for(; nCount != 0; nCount--); 
} 

#ifdef  DEBUG 
void assert_failed(u8* file, u32 line) 
{  
  while (1) { } 
} 
#endif 