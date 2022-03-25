#include <Arduino.h>
#include <UbiConstants.h>
#include <UbidotsEsp32Mqtt.h>
#include <UbiTypes.h>
#include "UbidotsEsp32Mqtt.h"

#include <TFT_eSPI.h>
#include <SPI.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 27
#define DHTTYPE DHT11

TFT_eSPI tft = TFT_eSPI();

DHT dht(DHTPIN, DHTTYPE);

const char *UBIDOTS_TOKEN = "BBFF-co4IS9ILXqSDZogZGTAC3DMUnyAt9s";
const char *WIFI_SSID = "LIBARDO SALDARRIAgA";
const char *WIFI_PASS = "libardo302";
const char *DEVICE_LABEL = "esp32";
const char *VARIABLE_LABEL = "temp";
const char *VARIABLE_LABEL2 = "hum";

const int PUBLISH_FREQUENCY = 5000;

int i = 0;

unsigned long timer;

Ubidots ubidots(UBIDOTS_TOKEN);

void loop();
void setup();
void callback(char *topic, byte *payload, unsigned int length);

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup()
{
  // put your setup code here, to run once:
  tft.init();
  tft.setTextColor(TFT_WHITE);
  tft.fillScreen(TFT_BLACK); // Color de pantalla

  Serial.begin(115200);
  Serial.println(F("DHTxx test!"));

  dht.begin();

  // ubidots.setDebug(true);  // uncomment this to make debug messages available
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();

  timer = millis();
}

void loop()
{
  delay(1000);
  // put your main code here, to run repeatedly:
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t))
  {
    tft.fillScreen(TFT_RED);
    tft.drawString("Failed to read from", 10, 20, 2);
    tft.drawString("DHT sensor!", 10, 40, 2);
    return;
  }

  tft.fillScreen(TFT_BLACK);
  tft.drawString("Humedad (%): ", 10, 10, 2);
  tft.drawString(String(h), 0, 40, 7);
  tft.drawString("Temperatura (°C): ", 10, 100, 2);
  tft.drawString(String(t), 0, 130, 7);

  if (!ubidots.connected())
  {
    ubidots.reconnect();
  }
  if (abs(millis() - timer) > PUBLISH_FREQUENCY) // triggers the routine every 5 seconds
  {

    ubidots.add(VARIABLE_LABEL, t); // Insert your variable Labels and the value to be sent
    ubidots.add(VARIABLE_LABEL2, h);
    ubidots.publish(DEVICE_LABEL);
    timer = millis();
  }
  ubidots.loop();
}
