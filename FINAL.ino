/*

YF‐ S201 Water Flow Sensor

Water Flow Sensor output processed to read in litres/hour

*/
#include "FirebaseESP8266.h" 
#include <ESP8266WiFi.h>
#define flowsensor D4 // Sensor Input

#define FIREBASE_HOST "skku-hackerthon-default-rtdb.firebaseio.com" 
#define FIREBASE_AUTH "iUf56Am835QIRE1Q9fF7H3trGYrOidGcOcP7bFWh"
#define WIFI_SSID "Jj" // 연결 가능한 wifi의 ssid
#define WIFI_PASSWORD "01087194085" // wifi 비밀번호

FirebaseData firebaseData;
FirebaseJson json;

volatile int flow_frequency; // Measures flow sensor pulses

unsigned int l_hour; // Calculated litres/hour

unsigned long currentTime;
unsigned long cloopTime;
float percent;
float ans;
float total=0;
ICACHE_RAM_ATTR void flow() // Interrupt function
{
   flow_frequency++;
}


void setup()

{
   pinMode(flowsensor, INPUT);
   pinMode(A0,INPUT);
   digitalWrite(flowsensor, HIGH); // Optional Internal Pull-Up
   Serial.begin(9600);
   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
   Serial.println();
   Serial.print("Connecting to WiFi");
   while (WiFi.status() != WL_CONNECTED)
   {
     Serial.print(".");
     delay(500);
   }
   Serial.println();
   Serial.print("Connected with IP: ");
   Serial.println(WiFi.localIP());
   Serial.println();

   attachInterrupt(digitalPinToInterrupt(flowsensor), flow, RISING); // Setup Interrupt
   sei();  //Enable interrupts
   currentTime = millis();
   cloopTime = currentTime;

   Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
   Firebase.reconnectWiFi(true);
   firebaseData.setBSSLBufferSize(1024, 1024);
   firebaseData.setResponseSize(1024);
   Firebase.setReadTimeout(firebaseData, 1000 * 60);
   Firebase.setwriteSizeLimit(firebaseData, "tiny");
}

void loop ()
{
   currentTime = millis();
   // Every second, calculate and print litres/hour
   if(currentTime >= (cloopTime + 1000))
   { 
      l_hour = (flow_frequency * 60 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
      flow_frequency = 0; // Reset Counter
      float l_inthour=float(l_hour);
      total+=l_inthour/3600 ; 
      percent=total*50 ;
      cloopTime = currentTime;
      Firebase.setFloat(firebaseData,"Percent",percent);
      Firebase.setInt(firebaseData,"Turb", ans);
      Firebase.setFloat(firebaseData, "Water", total);
      Serial.println("-----------------------------");
      Serial.print("유속: ");
      Serial.print(l_hour, DEC); // Print litres/hour
      Serial.print(" L/hour\n흐른 유량: ");
      Serial.print(total, DEC); // Print litres/hour
      Serial.println(" L");
   }
}
