/*************************************************************
**************************************************************
** $                 IPACEMAKER                            $** 
** $       by ladvine,dileep,eldho,nidhish                 $**
**************************************************************/

#include <SPI.h>
#include <WiFi.h>

char ssid[] = "Ladvine";      //  your network SSID (name) 
char pass[] = "nightelf123";   // your network password
IPAddress ip(192,168,1, 177);
int status = WL_IDLE_STATUS;
WiFiServer server(80);

const int QRS = 7;   
const int Pin =  A0;               // the Pin where pacing occurs
int PinState = 0;                  // initial pacing state
unsigned long flag=0;
long previousMillis = 0;          // will store last time of pacing
long currentMillis;
int QRSv;

/*******************************************************************************/
int mode=3;                         // 1.NORMAL 2.VENT INHIBITED 3.VENT TRIGGERED
int volt=3;                         //Pacing Voltage
int rfpd=150;                      //Refractory period      
/*******************************************************************************/

void pacing(unsigned long interval1,unsigned long interval2)
{
   if((currentMillis - previousMillis > interval1)&&(PinState == 0)) 
  {
      previousMillis = currentMillis;   
      PinState = volt;
      analogWrite(Pin, PinState);
  }
  
  currentMillis = millis();
 
  if((currentMillis - previousMillis > interval2)&&(PinState == volt)) 
  {
      previousMillis = currentMillis;   
      PinState = 0;
      analogWrite(Pin, PinState);
  }
  
}
/*******************/
void pacing_call()
{
  if(mode==1){pacing(835,5);}  //NORMAL PACING
  else
  {
     QRSv=0;
     flag=millis();
     while (QRSv==0)
     { 
       WiFiClient client = server.available();   // listen for incoming clients
       if (client) {break;}
       QRSv=digitalRead(QRS);
       if((millis()-flag)>900)pacing(835,5);
      }
    if(mode==3)
    {
     delay((120%rfpd));      //VENTRICULAR TRIGGERED
     PinState = volt;
     analogWrite(Pin, PinState);      
     delay(5);
     PinState = 0;
     analogWrite(Pin, PinState);
    }
  }  
}
/******************/

void setup() {
   
   pinMode(QRS,INPUT);
   volt = map(volt,0,5,0,255);
   Serial.begin(9600);               // GSM "Baud Rate"
   Serial.println("AT");             //GSM module working ??
   delay(2000);
   Serial.println("AT+CMGF=1");     // set Text Mode
   delay(2500);
   Serial.print("AT+CMGS=");
   Serial.write(34);
   Serial.print("+919946059175");
   Serial.write(34);
   Serial.write(10);
   delay(1500);
   Serial.println("GSM STATUS: On Air"); //this is the message to be sent
   delay(1500);
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present"); 
    while(true);        // don't continue
  } 

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(1000);
      } 
  server.begin();                           // start the web server on port 80
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);                       // you're connected now, so print out the status
}


void loop() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                    // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {  
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:    
            // the content of the HTTP response follows the header:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connnection: close");
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.print("<body style=");client.write(34);client.print("background-color: #C0C0C0; height: 82px;");client.write(34);client.println(">");
            client.print("<h1 class=");client.write(34);client.print("auto-style1");client.write(34);client.println(">iPACEMAKER</h1>");
            client.print("<form method=");client.write(34);client.print("get");client.write(34);client.print(" class=");client.write(34);client.print("auto-style3");client.write(34);client.println(">");
            client.println("	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
            client.print("<button name=");client.write(34);client.print("link");client.write(34);client.print(" style=");client.write(34);client.print("width: 56px");client.write(34);client.print(" value=");client.write(34);client.print("1");client.write(34);client.println(">HOME</button>");
            client.println("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;");
            client.print("<button name=");client.write(34);client.print("link");client.write(34);client.print(" style=");client.write(34);client.print("width: 75px");client.write(34);client.print(" value=");client.write(34);client.print("2");client.write(34);client.println(">&nbsp; SETTINGS</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
            client.print("<button name=");client.write(34);client.print("link");client.write(34);client.print(" style=");client.write(34);client.print("width: 69px");client.write(34);client.print(" value=");client.write(34);client.print("3");client.write(34);client.println(">&nbsp; LIVE ECG</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
            client.print("<button name=");client.write(34);client.print("link");client.write(34);client.print(" style=");client.write(34);client.print("width: 69px");client.write(34);client.print(" value=");client.write(34);client.print("4");client.write(34);client.println(">&nbsp;DATA LOG</button></form>");
            client.println("&nbsp;"); 
            client.println("</body>");
            client.println("</html>");
           
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;         
          } 
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }     
        else if (c != '\r') 
{    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /?link=1 HTTP/1.1")) {
                         // HOME PAGE
                         
          Serial.println("HOME PAGE");               
        }
        if (currentLine.endsWith("GET /?link=2 HTTP/1.1")) {
                          // SETTINGS PAGE
         Serial.println("SETTINGS PAGE");                  
        }
        if (currentLine.endsWith("GET /?link=3 HTTP/1.1")) {
                          // LIVE ECG
         Serial.println("LIVE ECG");                 
        }
        if (currentLine.endsWith("GET /?link=4 HTTP/1.1")) {
                          // DATA LOG
         Serial.println(" DATA LOG");                  
        }
      }
    }
    // close the connection
    client.stop();
    Serial.println("client disonnected");
  }
  else
  {
   pacing_call();  
  }
  
}


