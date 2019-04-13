#include <ESP8266WiFi.h>

const char* ssid     = "";
const char* password = "";
const char* host = "troca-banio.herokuapp.com";

int lightLimit = 500; //mas grande mas oscuro
boolean aux;

void WiFiEvent(WiFiEvent_t event) {
    Serial.printf("[WiFi-event] event: %d\n", event);

    switch(event) {
        case WIFI_EVENT_STAMODE_GOT_IP:
            Serial.println("WiFi connected");
            Serial.println("IP address: ");
            Serial.println(WiFi.localIP());
            break;
        case WIFI_EVENT_STAMODE_DISCONNECTED:
            Serial.println("WiFi lost connection");
            break;
    }
}

void setup() {
    Serial.begin(9600); //Serial.begin(115200);
    WiFi.disconnect(true);// delete old config
    delay(1000);

    WiFi.onEvent(WiFiEvent);
    WiFi.begin(ssid, password);
    Serial.println();
    Serial.println("Wait for WiFi... ");
    
    Serial.print("connecting to ");
    Serial.println(host);
  
}

void loop() {
  delay(5000);
    
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
    
  int analogValue = analogRead(0);
  Serial.println(analogValue);
  //aux = isLightOn(analogValue);
  //Serial.println(aux);
  delay(1000);

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
  
}


boolean isLightOn(int value) {
  return (value < lightLimit);
}
