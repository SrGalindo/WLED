#include "DHT.h"
#define DHT_DEBUG //uncoment this line to see the DHT debugs 
#define DHTTYPE DHT22
long lastMeasure = 0;
long temptimer = 0;
int delayMs = 2000; //2 sec
const int DHTPIN = D3; //DHT data pin
DHT dht(DHTPIN, DHTTYPE);

//starts Dallas Temp service on boot
void userSetup(){
    Serial.print("DHT setup");
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
    Serial.println("codigo DHT");
    lastMeasure = temptimer;
    
    //Check if MQTT Connected, otherwise it will crash the 8266
    if (mqtt != nullptr){
      //DHT readings
      float dht_humidity = dht.readHumidity();
      float dht_temperature = dht.readTemperature();
      Serial.print("hum: ");
      Serial.println(dht_humidity);
      Serial.print("temp: ");
      Serial.println(dht_temperature);

      if (isnan(dht_humidity) || isnan(dht_temperature)) {
        #ifdef DHT_DEBUG
          DEBUG_PRINTLN("---DEBUG INFO---");
          DEBUG_PRINTLN("Failed to read from DHT sensor!");
        #endif
        Serial.println("error al leer temp");
        return;
      }

      //Create character string populated with user defined device topic from the UI, and the read temperature. Then publish to MQTT server.
      char subufTemp[80];
      strcpy(subufTemp, mqttDeviceTopic);
      strcat(subufTemp, "/temperature");
      mqtt->publish(subufTemp, 0, true, String(dht_temperature).c_str());

      char subufHumidity[80];
      strcpy(subufHumidity, mqttDeviceTopic);
      strcat(subufHumidity, "/humidity");
      mqtt->publish(subufHumidity, 0, true, String(dht_humidity).c_str());
      return;
    }
    Serial.println("no conectado a mqtt");
    return;
  }
  return;
}