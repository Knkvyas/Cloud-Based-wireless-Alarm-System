#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <CayenneMQTTESP8266.h>

int flag;

char username[] = "dbe3e680-d758-11e8-810f-075d38a26cc9";
char password[] = "91b6fd64edc8203d1ebd3ca000bc78520ad1c74a";
char clientID[] = "ecea4840-d7a6-11e8-898f-c12a468aadce";


MDNSResponder mdns;

boolean trigger_sms = false;
boolean power_module = false;


char ssid[] = "halamadrid";  //Enter you Wifi SSID here
 
char wifiPassword[] = "vitcc123";

 

ESP8266WebServer server(80);

String mainPage = ""; //The default page
String feedback = ""; //Gives status of the switch
String currentPage = ""; //Combines main and feedback page

int pir = 13; //D7
int power_pin=15;
int pirvalue;

void setup(void){

 Cayenne.begin(username, password, clientID, ssid, wifiPassword);
  
  //HTML code for webpage//
  mainPage += "<h1 align=\"center\">IOT based security System</h1><h2 align=\"center\">-Locker Security System</h2><h1 align=\"center\"><p>Alert Status: <a href=\"switch2On\"><button>ARM</button></a>&nbsp;<a href=\"switch2Off\"><button>DISARM</button>  </a></p>";
  feedback = "<h3 align=\"center\">Click on ARM to enable the security system</h3>";
  //End of HTML code//
  currentPage = mainPage+feedback;
  
  // preparing GPIOs
  pinMode(pir, INPUT); //feeded by PIR sensor
  pinMode(power_pin, OUTPUT); //used to turn on PIR and Voice module
  digitalWrite(power_pin, LOW);
  
  delay(1000);
  Serial.begin(115200);
  WiFi.begin(ssid, wifiPassword);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // config static IP

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); //Serial monitor will give the IP addrss of your ESP module
  
  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  
  server.on("/", [](){
    currentPage = mainPage+feedback;
    server.send(200, "text/html", currentPage);
    currentPage = "";
  });

  
  server.on("/switch2On", [](){
    feedback = "<h3 align=\"center\">Alert System is up and running </h3>"; //HTML code modification
    currentPage=mainPage+feedback;
    server.send(200, "text/html", currentPage);
    currentPage="";
   digitalWrite(power_pin, HIGH); //Turn on PIR and Voice module
    power_module=true;
    delay(1000);
  });
  server.on("/switch2Off", [](){
    feedback = "<h3 align=\"center\">Alert System is down</h3>"; //HTML code modification 
    currentPage=mainPage+feedback;
    server.send(200, "text/html", currentPage);
    currentPage="";
   digitalWrite(power_pin, LOW); //Turn off PIT and Voice Module
   power_module=false;
    delay(1000); 
  });

  server.begin();
  Serial.println("IOT security system is up and running");
}

//Function to send an E-mail//
void send_Email()
{
const char* host = "maker.ifttt.com"; //The host of API URL will be the same for all
const int httpsPort = 443;

   WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

 

  String url = "/trigger/security_alert/with/key/cOPA6pGoejY4FPgPTlCdfNDV2wSMd284Adrtdbxq0YA"; //Must change it to your API URL
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');

  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
}
//Function to send an E-mail

 
void loop(void){
  Cayenne.loop();
  server.handleClient();
  //email logic enabled and buzzer sound enabled here
  if (digitalRead(13)==LOW && power_module==true){  // from 0 to 13 Changed and 13 is low
  trigger_sms=true;
  digitalWrite(15,HIGH);
  }
  else{
  trigger_sms=false;
  digitalWrite(15,LOW);
  }
  //mail and Cayenne cloud triggered here -
  if (trigger_sms==true)
  {
    //attempt to write to cayenne - 1
    flag = 1;
    Cayenne.virtualWrite(0, flag);      
    //If the PIR module is powered and if a person is detected send a E-mail
    send_Email(); 
    trigger_sms=false;
    //delay(200);
  }
  else{
    //attempt to write to cayenne - 0
    flag = 0;
    Cayenne.virtualWrite(0, flag);
  }  
}

/*CAYENNE_IN(0)
{
  int value = getValue.asInt();
  CAYENNE_LOG("Channel %d, pin %d, value %d", 0, 15, value);
  // Write the value received to the digital pin.
  digitalWrite(15, value);
}*/
