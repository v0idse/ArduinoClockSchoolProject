//Подключение библиотек
#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <ThreeWire.h>  
#include <RtcDS1302.h>
#include <SPI.h>image.png
#include <OneButton.h>
//Создание и установка переменных
ThreeWire myWire(4, 5, 2);
RtcDS1302<ThreeWire> Rtc(myWire);
#define TFT_CS     10
#define TFT_RST    8 
#define TFT_DC     9
#define PIN_PH     A4
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
char Printout[23];
#define countof(a) (sizeof(a) / sizeof(a[0]))
const String intro = "BETA BUILD - Clock.INO \nMade by Darky";
int gSeconds = 0;
int gMinutes = 0;
int gHours = 0;
int gDay = 0;
int gMonth = 0;
int gYear = 0;
const int chipSelect = 3;
int AlarmState = 1;
OneButton btn = OneButton(
  A0,  // Input pin for the button
  true,        // Button is active LOW
  true         // Enable internal pull-up resistor
);
//Функция для получения времени с RTC 
void printDateTime(const RtcDateTime & dt)
{
    snprintf_P(Printout, 
            countof(Printout),
            PSTR("%02u/%02u/%04u\n   %02u:%02u:%02u"),
            dt.Day(),
            dt.Month(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    gSeconds = dt.Second();
    gMinutes = dt.Minute();
    gHours = dt.Hour();
    gDay = dt.Day();
    gMonth = dt.Month();
    gYear = dt.Year(); 
}
//Первичная настройка
void setup() {
  setTime(gHours, gMinutes, gSeconds, gDay, gMonth, gYear);
  Rtc.Begin();
  Serial.begin(9600);
  Serial.println(__DATE__);
  Serial.println(__TIME__);
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  if (!Rtc.IsDateTimeValid()) 
    {
        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_WHITE);
  tft.setRotation(1);
  tft.setTextWrap(false);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0, 0);
  tft.setTextSize(1);
  tft.setTextColor(ST7735_WHITE);
  tft.println(intro);
  Serial.begin(9600);
  pinMode(A1, OUTPUT);
  btn.attachClick(click);
  btn.setClickTicks(100);
  Alarm.timerRepeat(15, AlarmMain);  //Для демонстрации!
  }  
//Вспомогательные функции
void AlarmMain(){
  if (AlarmState == 1){
    tft.setCursor(40, 50);
    tft.setTextColor(ST7735_BLACK, ST7735_BLACK);
    tft.setTextSize(2);
    tft.println(Printout);
    tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
    tft.setCursor(25, 50);
    tft.setTextSize(3);
    tft.println("Alarm!");
    digitalWrite(4, HIGH);
  }
  if (AlarmState == 0){
    digitalWrite(4, LOW);
  }
  while (AlarmState == 1){
    tone(A5, 1000, 100);
    delay(100);
    btn.tick();
    tone(A5, 500, 100);
    delay(100);
    btn.tick();
    tone(A5, 1000, 100);
    delay(100);
    btn.tick();
    tone(A5, 500, 100);
    delay(500);
    btn.tick();
  }
}
void click(){
  Serial.println("Received click signal!");
  if (AlarmState == 0){
    
    analogWrite(A1, 100);
    Serial.println("LED ON");
    AlarmState = 1;
    tone(A5, 600, 100);
    delay(100);
    tone(A5, 800, 100);
    digitalWrite(7, HIGH);
    tft.setCursor(155, 120);
    tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
    tft.setTextSize(1);
    tft.println("0");
  }
  else if (AlarmState == 1){
    
    analogWrite(A1, 0);
    Serial.println("LED Off");
    AlarmState = 0;
    tone(A5, 800, 100);
    delay(100);
    tone(A5, 400, 100);
    digitalWrite(7, LOW);
    tft.setCursor(155, 120);
    tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
    tft.setTextSize(1);
    tft.println("0");
  }
}
//Основной алгоритм
void loop() {
  Serial.println(analogRead(PIN_PH));
  tft.setCursor(25, 50);
  if (int((analogRead(PIN_PH))) < 900){
    tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
  }
  else if (int((analogRead(PIN_PH))) > 900){
    tft.setTextColor(ST7735_BLUE, ST7735_WHITE);
  }
  tft.setTextSize(2);
  tft.println(Printout);
  RtcDateTime now = Rtc.GetDateTime();
  printDateTime(now);
  
  Alarm.delay(100);
  btn.tick();

}
