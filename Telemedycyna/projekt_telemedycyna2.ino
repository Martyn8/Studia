//Martyna Jurusik

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> //wyświetlacz
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "HX711.h" //wzmacniacz belki tensometrycznej

#include "index.h" //szkielet strony internetowej

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Deklaracja wyśwoetlacza SSD1306:
#define OLED_MOSI  D7
#define OLED_CLK   D5
#define OLED_DC    D2
#define OLED_CS    D8
#define OLED_RESET D3
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

//Deklaracja wzmacniacza belki tensometrycznej
#define DOUT 10
#define CLK  9
HX711 scale(DOUT, CLK);

const char* ssid     = "Orange_Swiatlowod_41D0";
const char* password = "uzumymws";

AsyncWebServer server(80);

float weight = 0;

float calibration_factor = -98000.00;


void setup() {
  pinMode(10, INPUT);
  pinMode(9, OUTPUT);
  pinMode(12, OUTPUT);

  Serial.begin(115200);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }


  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);


  /*************************Nawiązywanie połączenia WiFi****************************************/
  WiFi.begin(ssid, password);
  display.println("Connecting to the ");
  display.println("server...");
  display.display();
  //display.clearDisplay();
  Serial.println("Connecting");
  display.println();
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
    display.print(".");
    display.display();
  }
  display.clearDisplay();

  Serial.println("connected");
  display.println("connected");
  display.clearDisplay();
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index_html);
  });

  server.on("/weight", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(weight).c_str());
  });

  server.begin();

  /*************************Kalibracja belki tensometrycznej*****************************/

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  display.println("Calibration...");
  delay(1000); // Pause for 2 seconds
  display.println();

  display.println("Remove all weight ");
  display.println("from scale");
  display.display();
  delay(4000); // Pause for 2 seconds

  scale.set_scale();
  scale.tare();  //Reset the scale to 0

  // Clear the buffer
  display.clearDisplay();
  display.setCursor(0, 0);

  display.println("After readings begin, ");
  display.println("place weight on scale");
  display.display();
  delay(4000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();


  /*************************Konunikacja z serwerem****************************************/
  
void loop() {
  digitalWrite(12, LOW);
  
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  weight = scale.get_units();
  
  Serial.println(weight, 4);
  
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  /*************Ostrzeżenie przed przeciążeniem urządzenia*****************/
  if (weight >= 5) {
    Serial.println("TOO MUCH PRESSURE!!!");
    weight = scale.get_units();
    Serial.println(weight);
    display.clearDisplay();
    display.setCursor(10, 0);
    display.println("!WARNING!");
    
    /*************Uruchomienie buzzera*****************/
    digitalWrite(12, HIGH);
    delay(500);
    digitalWrite(12, LOW);
    delay(500);

    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(15, 20);
    display.println("TOO MUCH");
    display.setCursor(15, 40);
    display.println("PRESSURE!");
    display.display();
  }
  else {
    display.println("Pressure:");
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(0, 30);
    display.print(constrain(weight, 0.0001, 25), 2);
    display.print("kg");
    display.display();
    display.clearDisplay();
  }
}
