
/*
 * Скетч для управления реле по времени и показателям фоторезистора.
 * Для определения времени используется модуль RTC на базе чипа DS1302.
 * Для работы с модулем взята библиотка iarduino_RTC.h (https://github.com/iarduino/iarduino_RTC).
 * В секцию НАСТРОЙКИ вынесены параметры влияющие на работу скетча.
 * Можно настроить диапозон времени работы, пороговое значение на фоторезисторе и интервал опроса датчика в секундах.
 */


#include <iarduino_RTC.h>//библиотека часов
iarduino_RTC time(RTC_DS1302, 10, 8, 9); //пины часов RST, CLK, DAT
#define PIN_PHOTO_SENSOR A0//пин фоторезистора А0
//#define DEBUG // Если закоментировать всю эту строку, то отладочные сообщения будут отключены

//----НАСТРОЙКИ----
byte startTimeH = 6;//начальное время, часы
byte endTimeH = 22;//конечное время, часы
word minLight = 300;//пороговое значение на фоторезисторе,1024 позиции
word checkInterval = 60; //интервал опроса датчика в секундах. от 0 до 65535
//-----------------

byte PIN_RELAY = 5;//пин реле
unsigned long prevMillis=0;//храним время срабатывания


void setup() {

#ifdef DEBUG
  Serial.begin(9600);
#endif

  time.begin();
  pinMode(PIN_RELAY, OUTPUT);
  time.gettime();
  OnOff();

}

void loop() {
  if (millis() - prevMillis > (checkInterval * 1000)) {
    prevMillis = millis(); //запоминаем время срабатываия
    OnOff();
  }
}

void OnOff() {
  int light = analogRead(PIN_PHOTO_SENSOR);//считываем значение фотодатчика

  /*
     в зависимости от способа подключения высокое значение данной переменной
     может обозначать как яркий свет, так и темноту.
     данный скетч написан для подключения с резистором в плече GND,
     т.е. высокое значение==высокое освещенность
     
     схема подключения:
     +-0-+-R-+
     |   |   |
     5v  A0  GND
     
     0-фоторезистор
     R-ограничивающий резистор(1-10КОм)
  */

  time.gettime();
  if ((time.Hours >= startTimeH && time.Hours < endTimeH) && light < minLight) //проверяем диапазоны времени и освещенности
  {
    digitalWrite(PIN_RELAY, HIGH);

  } else {
    digitalWrite(PIN_RELAY, LOW);
  }
  //-----Отладка-----
#ifdef DEBUG
  Serial.print("Датчик света:");
  Serial.println(light);
  Serial.print("Часы:");
  Serial.println(time.Hours);
  Serial.print("Минуты:");
  Serial.println(time.minutes);
  Serial.print("Секунды:");
  Serial.println(time.seconds);
#endif
  //----Конец отладки----
}
