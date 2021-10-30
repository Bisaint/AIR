#include <WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

//巴法云服务器地址默认即可
#define TCP_SERVER_ADDR "bemfa.com"
//服务器端口，tcp创客云端口8344
#define TCP_SERVER_PORT "8344"

//********************需要修改的部分*******************//

//WIFI名称，区分大小写，不要写错
#define DEFAULT_STASSID  "毕圣的iPhone"
//WIFI密码
#define DEFAULT_STAPSW   "bisheng1579"
//用户私钥，可在控制台获取,修改为自己的UID
String UID = "22ce0563f226aaac9cf0a7db0ff7cef1";
//主题名字，可在控制台新建
String TOPIC = "3DPRINTA";
String TOPICSTA = "3DPRINTSTA";
//**************************************************//

//设置上传速率2s（1s<=upDataTime<=60s）
//下面的2代表上传间隔是2秒
#define upDataTime 2*1000
//最大字节数
#define MAX_PACKETSIZE 512
//设置心跳值30s
#define KEEPALIVEATIME 30*1000

struct DEL //JSON上传报告结构体
{
int XXA = 0;  
int YYA = 0; 
int ZZA = 0; 
}DEL;

struct GET //JSON获取控制结构体
{
int XS = 0; 
int XK = 0;
int YS = 0; 
int YK = 0; 
int xx = 0; 
int yy = 0;  
int MC = 0; 
int MS = 0;  
}GET;

struct RD //步进电机引脚控制结构体
{
const int stepPinX = 14; 
const int dirPinX = 27; //x轴步进电机的两个控制引脚

const int stepPinY = 26; 
const int dirPinY = 25; //y轴步进电机的两个控制引脚

const int stepPinZ = 33; 
const int dirPinZ = 32; //z轴步进电机的两个控制引脚

const int LIMIT_X = 16; 
const int LIMIT_Y = 4; //限位开关
const int LIMIT_Z = 15; 

const int STAUTS_LED = 13; //系统状态LED

const int SERVO = 17; //光耦12v输出EM（LASER（12v）/SERVO（5v）公用）
}RD;
//风扇直接12v供电，无控制单元

//tcp客户端相关初始化，默认即可
WiFiClient TCPclient;
String TcpClient_Buff = "";
unsigned int TcpClient_BuffIndex = 0;
unsigned long TcpClient_preTick = 0;
unsigned long preHeartTick = 0;//心跳
unsigned long preTCPStartTick = 0;//连接
bool preTCPConnected = false;


//相关函数初始化
//连接WIFI
void doWiFiTick();
void startSTA();

//TCP初始化连接
void doTCPClientTick();
void startTCPClient();
void sendtoTCPServer(String p);

//led 控制函数
void ST(const int R,const int T);
void turnOnLed();
void turnOffLed();

/*
  *发送数据到TCP服务器
 */
void sendtoTCPServer(String p){
  
  if (!TCPclient.connected()) 
  {
    Serial.println("Client is not readly");
    return;
  }
  TCPclient.print(p);
  Serial.println("[Send to TCPServer]:String");
  Serial.println(p);
}


/*
  *初始化和服务器建立连接
*/
void startTCPClient(){
  if(TCPclient.connect(TCP_SERVER_ADDR, atoi(TCP_SERVER_PORT))){
    Serial.print("\nConnected to server:");
    Serial.printf("%s:%d\r\n",TCP_SERVER_ADDR,atoi(TCP_SERVER_PORT));
    String tcpTemp = "";
    tcpTemp = "cmd=1&uid="+UID+"&topic="+TOPIC+"\r\n";

    sendtoTCPServer(tcpTemp);
    tcpTemp = "";
    preTCPConnected = true;
    preHeartTick = millis();
    TCPclient.setNoDelay(true);
  }
  else{
    Serial.print("Failed connected to server:");
    Serial.println(TCP_SERVER_ADDR);
    TCPclient.stop();
    preTCPConnected = false;
  }
  preTCPStartTick = millis();
}


/*
  *检查数据，发送心跳
*/
void doTCPClientTick(){
 //检查是否断开，断开后重连
   if(WiFi.status() != WL_CONNECTED) return;

  if (!TCPclient.connected()) {//断开重连

  if(preTCPConnected == true){

    preTCPConnected = false;
    preTCPStartTick = millis();
    Serial.println();
    Serial.println("TCP Client disconnected.");
    TCPclient.stop();
  }
  else if(millis() - preTCPStartTick > 1*1000)//重新连接
    startTCPClient();
  }
  else
  {
    if (TCPclient.available()) {//收数据
      char c =TCPclient.read();
      TcpClient_Buff +=c;
      TcpClient_BuffIndex++;
      TcpClient_preTick = millis();
      
      if(TcpClient_BuffIndex>=MAX_PACKETSIZE - 1){
        TcpClient_BuffIndex = MAX_PACKETSIZE-2;
        TcpClient_preTick = TcpClient_preTick - 200;
      }
      preHeartTick = millis();
    }
    if(millis() - preHeartTick >= KEEPALIVEATIME){//保持心跳
      preHeartTick = millis();
      Serial.println("--Keep alive:");
      sendtoTCPServer("cmd=0&msg=keep\r\n");
    }
  }
  if(millis() - preHeartTick >= upDataTime){//上传数据
      preHeartTick = millis();
/*****************获取数据*****************/

      /*********************数据上传*******************/
      /*
        数据用#号包裹，以便app分割出来数据，&msg=#23#80#on#\r\n，即#温度#湿度#按钮状态#，app端会根据#号分割字符串进行取值，以便显示
        如果上传的数据不止温湿度，可在#号后面继续添加&msg=#23#80#data1#data2#data3#data4#\r\n,app字符串分割的时候，要根据上传的数据进行分割
      */
      String upstr = "";
      upstr = "cmd=2&uid="+UID+"&topic="+TOPICSTA+"&msg=#"+DEL.XXA+"#"+DEL.YYA+"#"+DEL.ZZA+"#"+"\r\n";
      sendtoTCPServer(upstr);
      upstr = "";
    }
  if((TcpClient_Buff.length() >= 1) && (millis() - TcpClient_preTick>=200))
  {//data ready
    TCPclient.flush();
    Serial.println("Buff");
    Serial.println(TcpClient_Buff);
    if((TcpClient_Buff.indexOf("&msg=on") > 0)) {
      turnOnLed();
    }else if((TcpClient_Buff.indexOf("&msg=off") > 0)) {
      turnOffLed();
    }else{

      int msgIndex = TcpClient_Buff.indexOf("msg=");//检测字符串中的msg=位置
      if(msgIndex>=0){
              String myjson = TcpClient_Buff.substring(msgIndex+4);//字符串截取，msg=位置后面第四位开始截取
              
              Serial.print("Get json:");
              Serial.println(myjson);//串口调试
              
              //json数据解析
              StaticJsonDocument<200> doc;
              DeserializationError error = deserializeJson(doc, myjson);
              if (error) {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.c_str());
              }
              JsonVariant XS = doc["XS"]; //获取解析值
              JsonVariant XK = doc["XK"];//获取解析值
              JsonVariant YS = doc["YS"];//获取解析值
              JsonVariant YK= doc["YK"];//获取解析值
              JsonVariant xx= doc["xx"];//获取解析值
              JsonVariant yy= doc["yy"];//获取解析值
              JsonVariant MC= doc["MC"];//获取解析值
              JsonVariant MS= doc["MS"];//获取解析值

              GET.XS = XS.as<int>();  //转换成int类型
              GET.XK = XK.as<int>(); //转换成int类型
              GET.YS = YS.as<int>(); //转换成int类型
              GET.YK = YK.as<int>(); //转换成int类型
              GET.xx = xx.as<int>(); //转换成int类型
              GET.yy = yy.as<int>(); //转换成int类型
              GET.MC = MC.as<int>(); //转换成int类型
              GET.MS = MS.as<int>(); //转换成int类型
              
              Serial.print("Get XS:"); //串口打印
              Serial.println(GET.XS);//串口打印
              Serial.print("Get XK:");//串口打印
              Serial.println(GET.XK);//串口打印
              Serial.print("Get YS:");//串口打印
              Serial.println(GET.YS);//串口打印
              Serial.print("Get YK:");//串口打印
              Serial.println(GET.YK);//串口打印
              Serial.print("Get xx:");//串口打印
              Serial.println(GET.xx);//串口打印
              Serial.print("Get yy:");//串口打印
              Serial.println(GET.yy);//串口打印
              Serial.print("Get MC:");//串口打印
              Serial.println(GET.MC);//串口打印
              Serial.print("Get MS:");//串口打印
              Serial.println(GET.MS);//串口打印
                
              ST(GET.MC,GET.MS);
              Serial.print("DEL XXA:");//串口打印
              Serial.println(DEL.XXA);//串口打印
              Serial.print("DEL YYA:");//串口打印
              Serial.println(DEL.YYA);//串口打印
        }
     

        
      }
   TcpClient_Buff="";
   TcpClient_BuffIndex = 0;
  }
}

void startSTA(){
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);

  WiFi.begin(DEFAULT_STASSID, DEFAULT_STAPSW);
}
/*
    WiFi.beginSmartConfig();
    while (1)
  {
    Serial.print(".");
    delay(500);
    if (WiFi.smartConfigDone())
    {
      Serial.println("SmartConfig Success/连接成功");
      Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
      Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
      WiFi.setAutoConnect(true);  // 设置自动连接
      break;
    }
  }
    Serial.println("Connected to AP");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}
*/
/**************************************************************************
                                 WIFI
***************************************************************************/
/*
  WiFiTick
  检查是否需要初始化WiFi
  检查WiFi是否连接上，若连接成功启动TCP Client
  控制指示灯
*/
void doWiFiTick(){
  static bool startSTAFlag = false;
  static bool taskStarted = false;
  static uint32_t lastWiFiCheckTick = 0;

  if (!startSTAFlag) {
    startSTAFlag = true;
    startSTA();
    Serial.printf("Heap size:%d\r\n", ESP.getFreeHeap());
  }

  //未连接1s重连
  if ( WiFi.status() != WL_CONNECTED ) {
    if (millis() - lastWiFiCheckTick > 1000) {
      lastWiFiCheckTick = millis();
    }
  }
  //连接成功建立
  else {
    if (taskStarted == false) {
      taskStarted = true;
      Serial.print("\r\nGet IP Address: ");
      Serial.println(WiFi.localIP());
      startTCPClient();
    }
  }
}
//打开灯泡
void turnOnLed(){
  Serial.println("Turn ON");
  digitalWrite(RD.SERVO,LOW);
}
//关闭灯泡
void turnOffLed(){
  Serial.println("Turn OFF");
    digitalWrite(RD.SERVO,HIGH);
}

void RUNADS(const int AAA,const int AAB,const int ABA,const int ABB,const int STEP,const int DELAY,const int A){
   if(A==1){
   digitalWrite(AAA,HIGH); //步进电机dir脚控制正反转，HIGH为正转
   digitalWrite(AAB,HIGH); //步进电机dir脚控制正反转，HIGH为正转
     for(int x = 0; x < STEP; x++) {
    digitalWrite(ABA,HIGH); 
    digitalWrite(ABB,HIGH); 
    delayMicroseconds(DELAY); 
     digitalWrite(ABA,LOW); 
    digitalWrite(ABB,LOW); 
    delayMicroseconds(DELAY); 
  }
  }
 else if(A==0){
   digitalWrite(AAA,LOW); //步进电机dir脚控制正反转，HIGH为正转
   digitalWrite(AAB,LOW); //步进电机dir脚控制正反转，HIGH为正转
  // Makes 200 pulses for making one full cycle rotation
  for(int x = 0; x < STEP; x++) {
    digitalWrite(ABA,HIGH); 
    digitalWrite(ABB,HIGH); 
    delayMicroseconds(DELAY); 
     digitalWrite(ABA,LOW); 
    digitalWrite(ABB,LOW); 
    delayMicroseconds(DELAY); 
  }
  }
}


void RUN(const int S,const int D,const int STEP,const int DELAY,const int A){
  if(A==1){
 digitalWrite(D,HIGH); //步进电机dir脚控制正反转，HIGH为正转
  // Makes 200 pulses for making one full cycle rotation
  for(int x = 0; x < STEP; x++) {
    digitalWrite(S,HIGH); 
    delayMicroseconds(DELAY); 
    digitalWrite(S,LOW); 
    delayMicroseconds(DELAY); 
  }
  }
    else if(A==0){
 digitalWrite(D,LOW); //步进电机dir脚控制正反转，HIGH为正转
  // Makes 200 pulses for making one full cycle rotation
  for(int x = 0; x < STEP; x++) {
    digitalWrite(S,HIGH); 
    delayMicroseconds(DELAY); 
    digitalWrite(S,LOW); 
    delayMicroseconds(DELAY); 
  }
  }
}
  /************************************************************************/
void ST(const int R,const int T){
   for(int x = 0; x < R; x++) {
/*
    RUNADS(RD.dirPinX,RD.dirPinY,RD.stepPinX,RD.stepPinY,GET.XS,GET.XK,GET.xx);
*/
  RUN(RD.stepPinY,RD.dirPinY,GET.YS,GET.YK,GET.yy);//6400为一圈
  RUN(RD.stepPinX,RD.dirPinX,GET.XS,GET.XK,GET.xx);//6400为一圈
              if(GET.xx==1){
              DEL.XXA+=GET.XS;
              }
              else if(GET.xx==0){
                DEL.XXA-=GET.XS;
                }
              if(GET.yy==1){
              DEL.YYA+=GET.YS;
              }
              else if(GET.yy==0){
                DEL.YYA-=GET.YS;
                }
              delay(T);
              }
}
/**************************************************************************/
void setup() {
    Serial.begin(115200);
  pinMode(RD.stepPinX,OUTPUT); //将步进电机引脚设置为输出状态
    pinMode(RD.dirPinX,OUTPUT); //将步进电机引脚设置为输出状态
      pinMode(RD.stepPinY,OUTPUT); //将步进电机引脚设置为输出状态
        pinMode(RD.dirPinY,OUTPUT); //将步进电机引脚设置为输出状态
          pinMode(RD.stepPinZ,OUTPUT); //将步进电机引脚设置为输出状态
            pinMode(RD.dirPinZ,OUTPUT); //将步进电机引脚设置为输出状态
              pinMode(RD.SERVO,OUTPUT);
  digitalWrite(RD.SERVO,HIGH);
}

void loop() {
  doWiFiTick();
  doTCPClientTick();
}
