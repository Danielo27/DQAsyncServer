#include <DQAsyncServer.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

void setup(){
      //Initialize
      Serial.begin(115200);
      
      for(int i = 0; i < 7; i++){
        Serial.print(".");
        delay(300);
        Serial.flush();
      }
      
      Serial.print("\n");
      Serial.println("DQAsyncServer Cargado Correctamente");
      
      //Create Parameters
      const char* ssid     = "ESP8266-Access-Point";
      const char* password = "123456789";
      int port = 80;
      
      server.on("/say", HTTP_GET, [](AsyncWebServerRequest *request) {
         request->send(200, "text/plain", "Hola! Bienvenido al Proyecto DQAsyncServer.");
      });
      
      DQAsyncServer.launch(&server);
      server.begin();

      WiFi.mode(WIFI_AP);
      WiFi.softAP(ssid, password);

      SPIFFS.begin();
      Serial.printf(" Servidor HTTP iniciado en el puerto: ", port);
}

void loop(){

}
