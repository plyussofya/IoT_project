#include <SoftwareSerial.h>

SoftwareSerial sdsSerial(D4, D3);  // Подключение датчика SDS011 к пинам D4 (RX) и D3 (TX) на плате Wemos D1 Lite mini

const int ledPin = D2;       // Пин подключения светодиода
const int buttonPin = D1;    // Пин подключения кнопки TTP223
const int relayPin = D5;     // Пин подключения реле

bool buttonState = false;
bool previousButtonState = false;
bool relayState = false;
int sensorValue = 0;

void setup() {
  pinMode(ledPin, OUTPUT);              // Установка пина светодиода в режим вывода
  pinMode(buttonPin, INPUT_PULLUP);     // Установка пина кнопки в режим входа с подтяжкой вверх
  pinMode(relayPin, OUTPUT);            // Установка пина реле в режим вывода
  
  digitalWrite(relayPin, LOW);          // Изначально реле выключено
  
  Serial.begin(9600);                    // Инициализация последовательного порта
  sdsSerial.begin(9600);                  // Инициализация SoftwareSerial для работы с датчиком SDS011
}

void loop() {
  nova_sensor();                          // Считывание данных с датчика
  
  // Управление светодиодом
  if (sensorValue > 35000) {
    digitalWrite(ledPin, HIGH);           // Включение светодиода, если значение с датчика выше 35000
  } else {
    digitalWrite(ledPin, LOW);            // Выключение светодиода в остальных случаях
  }

  buttonState = digitalRead(buttonPin);  // Считывание состояния кнопки

  if (buttonState != previousButtonState) {  // Если состояние кнопки изменилось
    if (buttonState == LOW) {  // Если кнопка нажата (логический уровень LOW)
      Serial.println("Button pressed!");
      relayState = !relayState;  // Инвертирование состояния реле
      
      // Управление состоянием реле
      if (relayState) {
        digitalWrite(relayPin, HIGH);  // Включение реле
        Serial.println("Relay turned ON!");
      } else {
        digitalWrite(relayPin, LOW);   // Выключение реле
        Serial.println("Relay turned OFF!");
      }
    }
  }

  previousButtonState = buttonState;  // Обновление предыдущего состояния кнопки
  delay(1000);  // Задержка для стабилизации считывания состояния кнопки
}

void nova_sensor() {
  if (sdsSerial.available()) {
    if (sdsSerial.find(0xAA)) {   // Поиск начала пакета данных
      byte data[10];
      if (sdsSerial.readBytes(data, 10) == 10) {
        // Извлечение данных из пакета
        int pm25 = (data[3] * 256 + data[2]);   // Значение PM2.5
        int pm10 = (data[5] * 256 + data[4]);   // Значение PM10
        
        // Вывод данных в монитор порта
        Serial.print("PM2.5: ");
        Serial.print(pm25);
        Serial.print(" ug/m3, PM10: ");
        Serial.print(pm10);
        Serial.println(" ug/m3");
        
        // Проверка значения PM2.5
        if (pm25 > 35000) {
          sensorValue = pm25;
        } else {
          sensorValue = 0;
        }
      }
    }
  }
  
  delay(100);  // Пауза 1 секунда между измерениями
}
