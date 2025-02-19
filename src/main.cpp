#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Keypad.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define TFT_CS 2
#define TFT_RST 5
#define TFT_DC 4
#define TFT_SCLK 18
#define TFT_MOSI 23

#define ROWS 4
#define COLS 4

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
WiFiClient espClient;
PubSubClient client(espClient);

bool readKp4x4();
void send();
void clearDisplay();
void clearLast();

const char* ssid = "NoProblem";
const char* password = "60214809";
const char* mqtt_server = "192.168.165.191"; 

const char kp4x4Keys[COLS][ROWS] = {{'1', '4', '7', '*'}, {'2', '5', '8', '0'}, {'3', '6', '9', '#'}, {'A', 'B', 'C', 'D'}};
byte rowKp4x4Pin[4] = {12, 13, 14, 27};
byte colKp4x4Pin[4] = {26, 25, 33, 32};
String message = "";
char customKey;

Keypad kp4x4 = Keypad(makeKeymap(kp4x4Keys), rowKp4x4Pin, colKp4x4Pin, ROWS, COLS);

void setup()
{
  Serial.begin(115200);

  tft.initR(INITR_GREENTAB);
  clearDisplay();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    tft.print("Successfull connection");
    delay(1000);
  }
  client.setServer(mqtt_server, 1883);

  clearDisplay();

}

void loop()
{
  while (kp4x4.getKeys())
  {
    for (int i = 0; i < LIST_MAX; i++)
    {
      if (kp4x4.key[i].kstate == PRESSED)
      {
        customKey = kp4x4.key[i].kchar;
        if (customKey == 'D') send();
        else if(customKey == 'C') clearLast();
        else
        {
          message += customKey;
          clearDisplay();
        }
      }
    }
  }
}

void send()
{
  if (!client.connected()) {
    tft.println("MQTT: Connection...");
    if (client.connect("ESP32_Client")) {
      tft.println("MQTT: Connected!");
    } else {
      tft.print("Error dirring connection to MQTT, code: ");
      tft.println(client.state());
      return; 
    }
  }

  tft.setTextColor(0x11FF11);
  tft.println("sending ...");
  
  bool result = client.publish("test", message.c_str());
  
  if (result) {
    tft.println("MQTT: Message successfully send");
    tft.println("success");
  } else {
    tft.println("MQTT: Error durring sending");
    tft.println("error");
  }

  delay(3000);
  message = "";
  clearDisplay();
}

void clearDisplay()
{
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(10, 10);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.println("Print the message:");
  tft.println(message);
}

void clearLast(){
  if(message.length()!=0) message.remove(message.length()-1);
  clearDisplay();
}