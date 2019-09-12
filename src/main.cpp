#include <Arduino.h>

int dataPin  = 9;   //Пин подключен к DS входу 74HC595
int latchPin = 10;  //Пин подключен к ST_CP входу 74HC595
int clockPin = 11;  //Пин подключен к SH_CP входу 74HC595
int pwmPin   = 6;   //Пин подключен к OE входу 74HC595 для управления ШИМ
int pos, posOld = 0;
volatile int tic, Dimmer;

void showLed(int bitPos, int pwm);
void animate_run(int _bitPos);
void animate_setup();

unsigned long timing;
 int pwm = 50;

void setup() {
  Serial.begin(57600);
  //устанавливаем режим OUTPUT
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(pwmPin, OUTPUT);

  animate_setup();

}

void loop() {
  if (millis() - timing > 200){
     timing = millis();
     Dimmer = map(analogRead(0), 0, 1024, 0, 240);
     pos = map(Dimmer, 0,240, 0, 8);
     if (pos != posOld){
       Serial.println( Dimmer);
       animate_run(pos);
       showLed(pos, 240);
       posOld = pos;
    }
  }
}

void showLed(int bitPos, int pwm) {   // прерывания таймера срабатывают каждые 40 мкс
    byte byteToSend = 0; //Создаем пустой байт B00000000
    digitalWrite(pwmPin, LOW);
    bitWrite(byteToSend, bitPos, HIGH); // При bitPos=0 получим B00000001, при bitPos=1 - B00000010, при bitPos=2 - B00000100 и т.д.
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, byteToSend); // Инвертируем сигнал при помощи MSBFIRST, грузим с первого бита
    digitalWrite(latchPin, HIGH);
    analogWrite(pwmPin,pwm /*random(0, 254)*/); // Назначаем выходу ШИМ разные значения
}

void animate_setup() {
byte path[4]= {
  B10000001,
  B01000010,
  B00100100,
  B00011000
  };
  for (int y=0; y<8; y++){
   for (int i=0; i<4; i++) {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, path[i]);
    digitalWrite(latchPin, HIGH);
    delay(90);
    }
  for (int i=0; i<4; i++) {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, path[3-i]);
    digitalWrite(latchPin, HIGH);
    delay(90);
    }
  }
}

 void animate_run(int _bitPos) {
  byte byteToSend = 0; //Создаем пустой байт B00000000
  for (int bitPos = 0; bitPos < _bitPos+1; bitPos++) { // В переменной хранится позиция изменяемого бита
    byteToSend = 0; // Обнуляем байт при каждом проходе
    bitWrite(byteToSend, bitPos, HIGH); // При bitPos=0 получим B00000001, при bitPos=1 - B00000010, при bitPos=2 - B00000100 и т.д.

    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, byteToSend); // Инвертируем сигнал при помощи MSBFIRST, грузим с первого бита
    digitalWrite(latchPin, HIGH);
    delay(90);
  }
}
