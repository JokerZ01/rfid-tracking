#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SPI.h>
#include <MFRC522.h>


#define RST_PIN 15 //rst pin gpıo15
#define SS_PIN 2 //sda pin gpıo2
#define role 16

MFRC522 mfrc522(SS_PIN, RST_PIN);  
int statuss = 0;
int out = 0;
 
MDNSResponder mdns;
 
ESP8266WebServer server(80);
String webPage;
const char* ssid     = "-------";      //your wifi name
const char* password = "*******";  //your wifi password
 
void setup() {

  webPage += "<center><h1><i><font color=gold>RFID Tracking</font color=gold></i></h1></center>";


  
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  
  delay(100);
  
 
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  pinMode(role, OUTPUT);
  
  server.begin();
  Serial.println("HTTP server started");
  
}
 
void loop() {

  
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //uid
  Serial.println();
  Serial.print(" UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  Serial.println();


  
  if (content.substring(1) == "01 3A FE E9") //change UID of the card that you want to give access
  {
    Serial.println(" Access Granted ");
    webPage += "<h3><font color=green>&nbsp;Promoted Card  </font color=green></h1><p>";
    webPage += "<h4>&nbsp;&nbsp;Taught UID: </h3>";
    webPage += (content);
    webPage += "<b><font color=green>&nbsp;&nbsp;------Access Granted------&emsp;</font color=green></b>";
    digitalWrite(role, HIGH);
    Serial.println(" ROLE HIGH ");
    delay(1000);
  }

  
  else   {
    Serial.println(" Access Denied ");
    webPage += "<h1><font color=red> x x x &nbsp;&nbsp;  x x x </font color=red></h1>";
    webPage += "<h3><font color=red>&nbsp;Foreign card  </font color=red></h1><p>";
    webPage += "<h4>&nbsp;&nbsp;Taught UID:\n </h3>";
    webPage += (content);
    webPage += ("<b><font color=red>&nbsp;&nbsp;  X-X-X-Access Denied-X-X-X  </font color=red></b>");
    webPage += "<h1><font color=red> x x x &nbsp;&nbsp;  x x x &emsp;</font color=red></h1>";
    digitalWrite(role, LOW);
    Serial.println(" ROLE LOW ");
    delay(1000);
  }

      server.on("/", [](){
    server.send(200, "text/html", webPage);
  });

  server.handleClient();
}
