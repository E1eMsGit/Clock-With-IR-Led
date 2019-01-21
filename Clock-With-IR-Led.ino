/*********************************************************************
 * Часы с выключателем телевизора в установленное время
 * Платформа: Arduino UNO
 * Подключенные модули: 
 *                      Кнопка переключения режима - пин: 2
 *                      Инфракрасный диод - пин: 3 
 *                      Датчик температуры/влажности DHT11 - пин: 4
 *                      Кнопка увеличения счетчика часов - пин: 5
 *                      Кнопка увеличения счетчика vbyen - пин: 6
 *                      Дисплей ТМ1637 - пин: CLK - 8, DIO - 7
 *                      Часы реального времени DS3231 - пин: A4, A5
 *                     
 * Описание: Режим 0 - Выводит на дисплей время
 *                     Режим 1 - Выводит на дисплей температуру 
 *                     Режим 2 - Выводит на дисплей относительную влажность. 
 *                     Режим 3 - Выводит на дисплей таймер для выключения тв. 
 *                     
 *                     Выключает телевизор (Samsung) в установленное время.
 *********************************************************************/
#include <iarduino_4LED.h>
#include <TroykaDHT.h>
#include <IRremote.h> 
#include <DS3231.h>

#define CLK 8      
#define DIO 7
#define MODE_BUTTON  2
#define HOUR_BUTTON  5
#define MINUTE_BUTTON  6
#define SAMSUNG_POWER 0xE0E040BF
#define SAMSUNG_BITS  32 

IRsend irsend;
DHT dht(4, DHT11);
iarduino_4LED  dispLED(CLK, DIO);
DS3231  rtc(SDA, SCL);
Time rtc_time;

volatile int mode = 0;
int hours, minutes = 0;
unsigned long current_time;

void setup() { 
    pinMode(HOUR_BUTTON, INPUT);  
    pinMode(MINUTE_BUTTON, INPUT);  

    attachInterrupt(digitalPinToInterrupt(MODE_BUTTON), changeMode, RISING); 
        
    dht.begin();
    rtc.begin();
    dispLED.begin(); 
    dispLED.light(1); 
}

void loop() {
    // Выключает телевизор в установленное время.
    if (rtc_time.hour == hours && rtc_time.min == minutes && rtc_time.sec == 10) {
        irsend.sendSAMSUNG(SAMSUNG_POWER, SAMSUNG_BITS); 
    }
       
    switch (mode) {
        // Вывод времени (НН:ММ). 
        case 0:
            rtc_time = rtc.getTime();
            
            dispLED.print(rtc_time.hour, rtc_time.min, TIME);
            dispLED.point(0, false);  
            current_time = millis();
            while (millis() < current_time + 500){}

            dispLED.point(0, true);
            current_time = millis();
            while (millis() < current_time + 500){}
            
            break;
        // Вывод температуры (С). 
        case 1:
            showDhtValue('t');
            break;
        // Вывод относительной влажности (%). 
        case 2:  
            showDhtValue( 'h');
            break;
        // Вывод таймера выключения тв.
        case 3:
            dispLED.print(hours, minutes, TIME);
            dispLED.point(0, true); 
            
            if (digitalRead(HOUR_BUTTON) == 1) {
                if (hours < 23) hours++;
                else hours = 0;
            }
            if (digitalRead(MINUTE_BUTTON) == 1) {
                if (minutes < 59) minutes++;
                else minutes = 0;
            }
            current_time = millis();
            while (millis() < current_time + 200){}
            break;
    }
}

// Функция прерывания (INT0).
void changeMode() {
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();

    // Программное устранение дребезга кнопки.
    if (interrupt_time - last_interrupt_time > 200) {
        if (mode < 3) mode++;
        else mode = 0;
    }
    last_interrupt_time = interrupt_time;
}

void showDhtValue(char measurement) {  
    dht.read();
    
    switch(dht.getState()) {
        case DHT_OK: 
            if (measurement == 't') {
                dispLED.print(dht.getTemperatureC(), TEMP);
            } else if (measurement == 'h') {
                dispLED.print(dht.getHumidity(), POS2 , RIGHT); 
            }
            current_time = millis();
            while (millis() < current_time + 1000){}
            break;
        // Ошибка контрольной суммы.
        case DHT_ERROR_CHECKSUM: Serial.println("Checksum error"); break;
        // Превышение времени ожидания.
        case DHT_ERROR_TIMEOUT: Serial.println("Time out error"); break;
        // Данных нет, датчик не реагирует или отсутствует.
        case DHT_ERROR_NO_REPLY: Serial.println("Sensor not connected"); break;
    }
}


