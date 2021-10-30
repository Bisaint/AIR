int input1 = 14;
int input2 = 2; 
int input3 = 15;
int input4 = 13;
int xx = 0; 
   void pwm();
   void cc();
     void runx();
     
     void AnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255) {
  // calculate duty, 8191 from 2 ^ 13 - 1
  uint32_t duty = (8191 / valueMax) * min(value, valueMax);

  // write duty to LEDC
  ledcWrite(channel, duty);
}
void setup() {
  Serial.begin(115200);
pinMode(input1,OUTPUT);
pinMode(input2,OUTPUT);
pinMode(input3,OUTPUT);
pinMode(input4,OUTPUT);
pinMode(xx,OUTPUT);
 
}
 
void loop() {
cc();
}
void sss(uint8_t addr){
  digitalWrite(xx,HIGH);
  delay(1);
  digitalWrite(xx,LOW);
  delay(3);
  for(int i=0;i<8;i++){
     digitalWrite(xx,HIGH);
     if(addr & 0x01){
      delay(0.5);
      digitalWrite(xx,LOW);
      delay(0.21);
      }
      else{
        delay(0.21);
        digitalWrite(xx,LOW);
        delay(0.5);
        }
        addr>>=1;
    }
    digitalWrite(xx,HIGH);
  }
void cc(){
   AnalogWrite(input1,1024); //给指定引脚写入数据
     AnalogWrite(input3,1024); //给指定引脚写入数据
  }
void pwm(){
    for(int val=0;val<1024;val++){ 
    AnalogWrite(input1,val); //给指定引脚写入数据
    delay(2); 
    }
    for(int val=1023;val>=0;val--){ 
      AnalogWrite(input1,val); 
      delay(2); 
      }
      }
  void runx(){
      delay(20);
  //forward 向前转
  digitalWrite(input1,HIGH); //给高电平
  digitalWrite(input2,LOW);  //给低电平
  digitalWrite(input3,HIGH); //给高电平
  digitalWrite(input4,LOW);  //给低电平
  delay(1000);   //延时1秒
 
 //stop 停止
 digitalWrite(input1,LOW);
 digitalWrite(input2,LOW);  
 digitalWrite(input3,LOW);
 digitalWrite(input4,LOW);  
 delay(500);  //延时0.5秒
 
  
  //back 向后转
  digitalWrite(input1,LOW);
  digitalWrite(input2,HIGH);  
  digitalWrite(input3,LOW);
  digitalWrite(input4,HIGH);  
  delay(1000);    
 
    }
