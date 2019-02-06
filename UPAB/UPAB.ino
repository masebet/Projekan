#include <SPI.h>
#include <Ethernet.h>
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[]={192,168,0,101};
#define WEBSITE "192.168.0.100"
char server[] = "192.168.0.100";
IPAddress gateway(192,168,0,254);
IPAddress dnsserver(192,168,0,254);
EthernetClient client;

volatile int flow_frequency; // Measures flow sensor pulses
unsigned int l_hour; // Calculated litres/hour
unsigned char flowsensor = 2; // Sensor Input
unsigned long currentTime;
unsigned long cloopTime;
void flow () // Interrupt function
{
   flow_frequency++;
}

String id_pengguna = "1001";
int i;
String readPage();
String data="";
String rfid="";

char inString[32]; 
int stringPos = 0; 
boolean startRead = false; 
char readdat[15];
char updatedat[150];
char * p;
int kode;

void setup() {
  Serial.begin(9600);                   
  Ethernet.begin(mac, ip,dnsserver,gateway);
   pinMode(30,OUTPUT);
   pinMode(7,OUTPUT);
   pinMode(flowsensor, INPUT);
   digitalWrite(flowsensor, HIGH); // Optional Internal Pull-Up
   attachInterrupt(0, flow, RISING); // Setup Interrupt
   sei(); // Enable interrupts
   currentTime = millis();
   cloopTime = currentTime;
  }

void loop() {

  while(Serial.available()){
  rfid=Serial.readString();
  }
   currentTime = millis();
   if(currentTime >= (cloopTime + 1000))
   {
      cloopTime = currentTime; // Updates cloopTime
      // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
      l_hour = (flow_frequency * 60 / 7.5); // (Pulse frequency x 60 min) / 7.5 Q = flowrate in L/hour
      flow_frequency = 0; // Reset Counter
      unsigned int lpd=l_hour*10/36; //ubah ke liter per detik
      Serial.print(lpd); 
      Serial.println(" L/hour");
      data="/upab/get.php?data=";data+=rfid; data+="&dataID="; data+=id_pengguna; data+="&saldo="; data+=moco_epp(25); data+=" HTTP/1.1";
      Serial.println(data);
      String buffget="a";//connectAndRead();
      rfid="";
        if(buffget=="habis"){
        }
      else if(buffget=="hidup"){
        digitalWrite(30,1);
        }
      else if(buffget=="mati"){
        digitalWrite(30,0);
        }
      else if(buffget.toInt()>0){
        nulis_epp(25,buffget);
        Serial.print(buffget.toInt());
        }else{}
   }
}
String connectAndRead(){
char Nilai1[40];
  if (client.connect(server, 80)) {
    client.print("GET ");
    client.println(data);
    client.print( "Host: ");
    client.println(WEBSITE);
    client.println();
 String pageValue = readPage(); 
 int str_len = pageValue.length() + 1; 
 char buffconf[str_len];
 pageValue.toCharArray(buffconf, str_len);
 p = strtok (buffconf,"#");
 if (p != NULL){
      strcpy(Nilai1,p);
      trim(Nilai1,strlen(Nilai1));
      p = strtok (NULL,"#");
      if (p != NULL){} 
    } 
pageValue=Nilai1;
return pageValue;
  }else{
    Serial.println("FAILED"); 
  }
}
String readPage(){
  stringPos = 0;
  memset( &inString, 0, 32 ); 
  while(true){
    if (client.available()) {
      char c = client.read();
      if (c == '<' ) {
        startRead = true;
      }else if(startRead){
        if(c != '>'){
          inString[stringPos] = c;
          stringPos ++;
        }else{
          startRead = false;
          client.stop();
          client.flush();
          return inString;
        }
      }
    }
  }
}
void trim(char *s, const int len){
    int end = len - 1;
    int start = 0;
    int y = 0;
    while ((start < len) && (s[start] <= ' '))
    {
        start++;
    }
    while ((start < end) && (s[end] <= ' '))
    {
        end--;
    }

    if (start > end)
    {
        memset(s, '\0', len);
        return;
    }
    for (y = 0; (y + start) <= end; y++)
    {
        s[y] = s[start + y];
    }
    memset((s + y), '\0', len - y);
}
String moco_epp(int ngone){
  String hasile="";
  unsigned int adde; 
  char dat[10]="";
  byte e;
  for(int i=0;i<=10;i++){
    adde= ngone + i;
    e= eeprom_read_byte((unsigned char *) adde);
    dat[i]= (char )e;
  }
  hasile = (String) dat;
  return hasile;
}
void nulis_epp(int ngone,String isine){
  String hasile="";
  unsigned int adde; 
  char dat[10]="";
 for(int i=0;i<=10;i++) {
        adde= ngone +i;
        eeprom_write_byte((unsigned char *) adde,isine[i]);
      }
}
