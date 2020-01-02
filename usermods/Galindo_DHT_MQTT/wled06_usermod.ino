#include "DHT.h"
//#define WLED_DEBUG_DHT //uncoment this line to see the DHT debugs 
#define DHTTYPE DHT22
long lastMeasure = 0;
long temptimer = 0;
int delayMs = 10000; //10 sec
const int DHTPIN = D3; //DHT data pin
DHT dht(DHTPIN, DHTTYPE);

//starts Dallas Temp service on boot
void userSetup(){
  pinMode(DHTPIN, INPUT_PULLUP);
  //Start the DHT sensor
  dht.begin();
}
    
//gets called every time WiFi is (re-)connected. Initialize own network interfaces here
void userConnected(){
}

void userLoop(){
  temptimer = millis();
  
  //Timer to publishe new temperature every 10 seconds
  if (temptimer - lastMeasure > delayMs) {
    lastMeasure = temptimer;
    
    //Check if MQTT Connected, otherwise it will crash the 8266
    if (mqtt != nullptr){
      //DHT readings
      float dht_humidity = dht.readHumidity();
      float dht_temperature = dht.readTemperature();

      if (isnan(dht_humidity) || isnan(dht_temperature)) {
        #ifdef WLED_DEBUG_DHT
          DEBUG_PRINTLN("---DEBUG INFO---");
          DEBUG_PRINTLN("Failed to read from DHT sensor!");
        #endif
        return;
      }

      //Create character string populated with user defined device topic from the UI, and the read temperature. Then publish to MQTT server.
      char subufTemp[80];
      strcpy(subufTemp, mqttDeviceTopic);
      strcat(subufTemp, "/temperature");
      mqtt->publish(subufTemp, 0, true, String(dht_humidity).c_str());

      char subufHumidity[80];
      strcpy(subufHumidity, mqttDeviceTopic);
      strcat(subufHumidity, "/humidity");
      mqtt->publish(subufHumidity, 0, true, String(dht_temperature).c_str());
      return;
    }
    return;
  }
  return;
}
