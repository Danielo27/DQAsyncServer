#ifndef DQAsyncServer_h
#define DQAsyncServer_h

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <Updater.h>

/*PAGES GZIP*/
#include "bootstrap.min.css.h"
#include "bootstrap.min.js.h"
#include "index.h"
#include "fimware.h"
#include "error.h"

class DQAsyncServerClass{
  
    public: 

      void launch(AsyncWebServer *server) {
        _server = server; 
		 
        _server->on("/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest * request) {      //ASSETS
            AsyncWebServerResponse *response = request->beginResponse_P(200, "text/css",bootstrap_min_css_gz, bootstrap_min_css_gz_len);
            response->addHeader("Content-Encoding", "gzip");
            request->send(response);
         });

        _server->on("/bootstrap.min.js", HTTP_GET, [](AsyncWebServerRequest * request) {     //ASSETS
           AsyncWebServerResponse *response = request->beginResponse_P(200, "text/javascript",bootstrap_min_js_gz, bootstrap_min_js_gz_len);
           response->addHeader("Content-Encoding", "gzip");
           request->send(response);
        });

        _server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){        //INDEX GET
           AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html",index_html_gz, index_html_gz_len);
           response->addHeader("Content-Encoding", "gzip");
           request->send(response);
        });

        _server->on("/", HTTP_POST, [](AsyncWebServerRequest *request){        //INDEX POST
          if(request->hasParam("NameWirseless", true) && request->hasParam("PasswordWirseless",true)){
          String NetworkName = request->getParam("NameWirseless", true)->value();
          String NetworkPassword = request->getParam("PasswordWirseless",true)->value();
      
          WiFi.softAPdisconnect(true);
          WiFi.mode(WIFI_STA);
          WiFi.begin(NetworkName, NetworkPassword);
          }
        }); 

        _server->onNotFound([](AsyncWebServerRequest *request){                 //404 ERROR
          AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html",error_html_gz, error_html_gz_len);
          response->addHeader("Content-Encoding", "gzip");
          request->send(response);
        });

        _server->on("/update", HTTP_GET, [](AsyncWebServerRequest * request) {  //UPDATE GET 
          AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html",firmware_html_gz, firmware_html_gz_len);
          response->addHeader("Content-Encoding", "gzip");
          request->send(response);
        });

        _server->on("/update", HTTP_POST, [](AsyncWebServerRequest *request) {}, //UPDATE POST
        [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data,size_t len, bool final) {
			
		  #define U_PART U_FS
          if("update", true, true){
                      size_t content_len;
          if (!index){
            Serial.println("Update");
            content_len = request->contentLength();
            int cmd = (filename.indexOf("spiffs") > -1) ? U_PART : U_FLASH;
            Update.runAsync(true);
            if (!Update.begin(content_len, cmd)) {
              Update.printError(Serial);
            }
          }

          if (Update.write(data, len) != len) {
            Update.printError(Serial);
          } else {
            Serial.printf("Progreso: %d%%\n", (Update.progress()*100)/Update.size());
          }

          if (final) {
            AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "Porfavor espere mientras se reinicia el dispositivo");
            response->addHeader("Refresh", "20");  
            response->addHeader("Location", "/");
            request->send(response);
            if (!Update.end(true)){
              Update.printError(Serial);
            } else {
              Serial.println("Actualizacion Completa");
              Serial.flush();
              ESP.restart();
            }
          } 
          }   
        });

        }

        private: 
        AsyncWebServer *_server;
		
};

DQAsyncServerClass DQAsyncServer;
#endif
