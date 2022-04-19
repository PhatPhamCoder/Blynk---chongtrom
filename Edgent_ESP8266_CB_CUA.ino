#define BLYNK_TEMPLATE_ID "TMPLPiFms-Q4"
#define BLYNK_DEVICE_NAME "Chống trộm"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"
#define BLYNK_PRINT Serial
#define APP_DEBUG
#define USE_NODE_MCU_BOARD
#include "BlynkEdgent.h"
const int cbcua=5;  //D1 //cảm biến vật cản
const int buzzer=4; //D2
const int button=0; //D3
int ledr = D5;
boolean buttonState=HIGH;
boolean runMode=0; //Bật tắt chế độ cảnh báo
boolean canhbao=0;
boolean canhbaoState=0;
BlynkTimer timer;
int timerID1,timerID2;
WidgetLED led(V0); //Khai báo đèn hiện thị trạng thái kết nối trên Blynk
unsigned long times=millis();

void setup()
{
  Serial.begin(115200);
  delay(100);
  pinMode(cbcua,INPUT_PULLUP);
  pinMode(button,INPUT_PULLUP);
  pinMode(buzzer,OUTPUT);
  pinMode(ledr, OUTPUT);
  digitalWrite(buzzer,LOW); //Tắt buzzer
  Blynk.syncVirtual(V3); //Đồng bộ dữ liệu chân ảo V3 trên Blynk
  BlynkEdgent.begin();
  timerID1 = timer.setInterval(1000L,handleTimerID1);
  timerID2 = timer.setInterval(10000L,handleTimerID2);
  timer.disable(timerID2);
}

void loop() {
  BlynkEdgent.run();
  timer.run();
  if(digitalRead(button)==LOW){
    if(buttonState==HIGH){
      buttonState=LOW;
      digitalWrite(buzzer,!digitalRead(buzzer));
      Serial.println("Buzzer: "+String(digitalRead(buzzer)));
      delay(200);
    }
    if(millis()-times>3000){
      runMode=!runMode;
      Serial.println("Runmode: " + String(runMode));
      if(runMode==0){
        timer.disable(timerID2);
      }else{
        canhbao=0;
      }
    }
  }else{
    buttonState=HIGH;
    times=millis();
  }
}
void handleTimerID1(){
  String s;
  if(digitalRead(cbcua)==0){
    s = "Có trộm!";
  }else{
    s="Bình Thường!";
  }
  Blynk.virtualWrite(V1,s);
  Serial.println("Trạng thái cửa: " +String(digitalRead(cbcua)));
  if(runMode==1){
    if(digitalRead(cbcua)==0){
      if(canhbao==0){
        canhbaoState=!canhbaoState;
        Blynk.virtualWrite(V4,canhbaoState);
        digitalWrite(buzzer, HIGH);
        digitalWrite(ledr, HIGH);
        timer.enable(timerID2);
        timer.restartTimer(timerID2);
        canhbao=1;
      }
    }
  }
  //--------------------------------------
  if(led.getValue()) {
    led.off();
  } else {
    led.on();
  }
  //-------------------------------------
}
void handleTimerID2(){
  canhbaoState=!canhbaoState;
  Blynk.virtualWrite(V4,canhbaoState);
}
BLYNK_CONNECTED() {
  Blynk.syncVirtual(V2,V4);
}
BLYNK_WRITE(V2) {
  runMode = param.asInt();
  if(runMode==0){
    timer.disable(timerID2);
  }else{
    canhbao=0;
  }
}
BLYNK_WRITE(V3) {
  int p = param.asInt();
  digitalWrite(buzzer,p);
}
BLYNK_WRITE(V4) {
  canhbaoState = param.asInt();
}
