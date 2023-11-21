#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <DFRobot_DHT11.h>
DFRobot_DHT11 DHT;
#define DHT11_PIN D5
#include <Servo.h>  // servo library
Servo s1;

WiFiClient client;
long myChannelNumber = 2347282;
const char myWriteAPIKey[] = "L1W93DFLWYMBH0HZ";


#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for SSD1306 display connected using I2C
#define OLED_RESET -1  // Reset pin
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool rain = 1;
bool dry = 1;
int pos=0;
void setup() {
delay(2000);
    for (int pos = 0; pos <= 180; pos += 1) {  // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      s1.write(pos);  // tell servo to go to position in variable 'pos'
      delay(15);      // waits 15ms for the servo to reach the position
    }

  Serial.begin(9600);

  // initialize the OLED object
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
    // pinMode(2,OUTPUT);//hardware 4
  }


  WiFi.begin("BRUR-Teachers", "BRUR@2018");
  // while(WiFi.status() != WL_CONNECTED)
  // {
  //   delay(200);
  //   Serial.print("..");
  // }
  Serial.println();
  Serial.println("NodeMCU is connected!");
  Serial.println(WiFi.localIP());

  ThingSpeak.begin(client);
  s1.attach(15);  // servo attach D3 pin of arduino
}

void loop() {
  // int t = 30;
  // int h = 60;
  // int r = 1;
  // int l = 100;


  DHT.read(DHT11_PIN);
  float t = DHT.temperature;
  float h = DHT.humidity;

  int l = analogRead(A0);
  l = map(l, 0, 1024, 100, 0);

  int r = !digitalRead(2);




  Serial.println("Temperature: " + (String)t);
  Serial.println("Humidity: " + (String)h);
  Serial.println("Rain: " + (String)r);
  Serial.println("Light: " + (String)l);
  ThingSpeak.writeField(myChannelNumber, 1, t, myWriteAPIKey);
  ThingSpeak.writeField(myChannelNumber, 2, h, myWriteAPIKey);
  ThingSpeak.writeField(myChannelNumber, 3, r, myWriteAPIKey);
  ThingSpeak.writeField(myChannelNumber, 4, l, myWriteAPIKey);
  delay(0);


  if (r == 1 && rain == 1) {
    for (int pos = 0; pos <= 180; pos += 1) {  // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      s1.write(pos);  // tell servo to go to position in variable 'pos'
      delay(15);      // waits 15ms for the servo to reach the position
    }
    rain = 0;
    dry = 1;
  }
  if (r == 0 && dry == 1) {
    for (int pos = 180; pos >= 0; pos -= 1) {  // goes from 180 degrees to 0 degrees
      s1.write(pos);                           // tell servo to go to position in variable 'pos'
      delay(15);                               // waits 15ms for the servo to reach the position
    }
    rain = 1;
    dry = 0;
  }










  display.clearDisplay();
  // Display Text
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Weather station");
  display.display();
  display.startscrollright(0x00, 0x00);
  display.setCursor(0, 10);
  display.print("\nTemprature:");
  display.print(t);
  display.println("'C");
  display.setCursor(0, 20);
  display.print("\nHumidity:");
  display.print(h);
  display.println("%");
  display.setCursor(0, 30);
  display.print("\nForecast :");
  if (r == 1 && l >= 80)
    display.print("Fox marriage");
  else if (r == 1)
    display.print("Rain");

  else if (l >= 95)
    display.print("Clean sky");
  else if (l <= 100 && l >= 80)
    display.print("Sunny");
  else if (l <= 80 && l >= 10)
    display.print("Cloudy");
  else
    display.print("Night");



  display.setCursor(0, 40);
  display.print("\nLight  :");
  display.print(l);
  display.println("%");
  display.display();
  delay(2000);
}