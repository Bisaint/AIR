#include <WiFi.h>
#include <WiFiClient.h>

#define TCP_SERVER_ADDR "bemfa.com"
#define TCP_SERVER_PORT "8344"
String UID = "22ce0563f226aaac9cf0a7db0ff7cef1";
String TOPIC = "rout";

#define ssid = "***********"
#define password = "***********"

struct RD //步进电机引脚控制结构体
{
const int stepPinX = 14; 
const int dirPinX = 27; //x轴步进电机的两个控制引脚

const int stepPinY = 26; 
const int dirPinY = 25; //y轴步进电机的两个控制引脚

const int stepPinZ = 33; 
const int dirPinZ = 32; //z轴步进电机的两个控制引脚
}RD;

#define upDataTime 2*1000
#define MAX_PACKETSIZE 512 //最大字节数


//tcp客户端相关初始化
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

//步进电机初始化
void RUNL(const int S,const int D,const int STEP,const int DELAY);
void RUNR(const int S,const int D,const int STEP,const int DELAY);
void getin();
void getout();
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
    String tcpTemp="";
    tcpTemp = "cmd=1&uid="+UID+"&topic="+TOPIC2+"\r\n";

    sendtoTCPServer(tcpTemp);
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
  *检查数据，发送数据
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
    if(millis() - preHeartTick >= upDataTime){//上传数据
      preHeartTick = millis();
            return;
   }   

   void startSTA(){
  WiFi.disconnect();
    WiFi.mode(WIFI_STA);
        //固定连接
     WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
    /*   //smartconfig
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
  */
 
    Serial.println("Connected to AP");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

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
 /************************************************************************/
void RUNL(const int S,const int D,const int STEP,const int DELAY){
 digitalWrite(D,HIGH); //步进电机dir脚控制正反转，HIGH为正转
  // Makes 200 pulses for making one full cycle rotation
  for(int x = 0; x < STEP; x++) {
    digitalWrite(S,HIGH); 
    delayMicroseconds(DELAY); 
    digitalWrite(S,LOW); 
    delayMicroseconds(DELAY); 
  }
}
void RUNR(const int S,const int D,const int STEP,const int DELAY){
  digitalWrite(D,LOW); //Changes the rotations direction
  // Makes 400 pulses for making two full cycle rotation
  for(int x = 0; x < STEP; x++) {
    digitalWrite(S,HIGH);
    delayMicroseconds(DELAY);
    digitalWrite(S,LOW);
    delayMicroseconds(DELAY);
  }
}
  /************************************************************************/
void getin(){ //注入液体
  RUNL(RD.stepPinX,RD.dirPinX,1000,500);
   delay(500); 
  RUNL(RD.stepPinY,RD.dirPinY,1000,500);
   delay(500); 
}
void getout(){ //转出液体
  RUNR(RD.stepPinX,RD.dirPinX,1000,500);
   delay(500);
  RUNR(RD.stepPinY,RD.dirPinY,1000,500);
   delay(500);
}
/**************************************************************************/
void setup() {
    Serial.begin(115200);
  pinMode(RD,OUTPUT); //将步进电机引脚设置为输出状态
}
void loop() {
    //doWiFiTick();
 // doTCPClientTick();
getin();
}
