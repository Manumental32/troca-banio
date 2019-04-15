#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library for WiFiManager AutoconectAP
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

char* host = "troca-banio.herokuapp.com";
int httpPort = 80;
char* request_delay = "5000";
int int_request_delay = 5000;

void setup() {
    Serial.begin(115200);
    
    WiFiManager wifiManager; //Local intialization. Once its business is done, there is no need to keep it around    
    //wifiManager.resetSettings(); //reset saved settings (for testing)
    
    WiFiManagerParameter custom_host("host", "host default: troca-banio.herokuapp.com", host, 56);
    wifiManager.addParameter(&custom_host);
    WiFiManagerParameter custom_request_delay("request_delay", "request_delay default: 5000", request_delay, 56);
    wifiManager.addParameter(&custom_request_delay);

    //fetches ssid and pass from eeprom and tries to connect if it does not connect it starts an access point with the specified name here  "AutoConnectAP" and goes into a blocking loop awaiting configuration
    if (!wifiManager.autoConnect("Troca-baño AutoConnectAP")) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(5000);
    }
    
    //read updated parameters
    strcpy(host, custom_host.getValue());
    strcpy(request_delay, custom_request_delay.getValue());
    int_request_delay = atoi(request_delay);  
    Serial.println(int_request_delay);
    Serial.println("connected...yeey :)");  //if you get here you have connected to the WiFi
}

void loop() {
    
  WiFiClient client;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    Serial.println(host);
    return;
  }
    
  int analogValue = analogRead(0);
  Serial.println(analogValue);

  String url = "/ldr/";
  url = url + String(analogValue);
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  client.print( "POST " + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.println(line);
  }
  
  Serial.println("closing connection");
  delay(int_request_delay);
}
