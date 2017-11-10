/*********************************************************************
 * Часы с выключателем телевизора в установленное время
 * Платформа: Arduino NANO
 * Подключенные модули: Инфракрасный диод - пин: D3 
 *                      Часы реального времени DS3231 - пин: A4, A5
 *                      Дисплей ТМ1637 - пин: CLK - D8, DIO - D7
 * Описание: Выводит на дисплей время. 
 *           Выключает телевизор (Samsung) в установленное время
 *********************************************************************/
#include <iarduino_4LED.h>  // Библиотека дисплея.
#include <IRremote.h> // Библиотека управления инфракрасным диодом.
#include <DS3231.h> // Библиотека часов реального времени.

#define CLK 8      
#define DIO 7
#define SAMSUNG_POWER 0xE0E040BF
#define SAMSUNG_BITS  32 

IRsend irsend;
DS3231  rtc(SDA, SCL);
Time rtc_time;
iarduino_4LED  dispLED(CLK, DIO);

int hours, minutes;

void setup()
{
  
  // Для теста. 
  // Вывод в Serial порт.
  // Serial.begin(9600); 
  // Послать сигнал.
  // irsend.sendSAMSUNG(SAMSUNG_POWER, SAMSUNG_BITS); 

  // Инициализация часов.
  rtc.begin(); 

  // Инициализация дисплея.
  dispLED.begin(); 
  // Яркость. От 0 до 5.
  dispLED.light(1); 
}

void loop() 
{   
  rtc_time = rtc.getTime();

  // Вывод времени. 
  dispLED.print(rtc_time.hour, rtc_time.min, TIME);
  // Двоеточие между часами и минутами.
  dispLED.point(0, false);  
  delay(500);
  
  dispLED.print(rtc_time.hour, rtc_time.min, TIME);
  dispLED.point(0, true); 
  delay(500);

  // Выключает телевизор в 23 : 30
  if (rtc_time.hour == 23 && rtc_time.min == 30 && rtc_time.sec == 10)
  {
    irsend.sendSAMSUNG(SAMSUNG_POWER, SAMSUNG_BITS); 
  }
  
  // Для теста. Вывод в Serial порт.
   /*Serial.print("Hours: ");
  Serial.println(rtc_time.hour);
  Serial.print("Minutes: ");
  Serial.println(rtc_time.min);
  Serial.print("Seconds: ");
  Serial.println(rtc_time.sec);
  delay(1000);*/
}

