
#include <Joystick.h>

// 1. ПЕРЕМЕННЫЕ ДЛЯ ХРАНЕНИЯ НАЧАЛЬНЫХ (НУЛЕВЫХ) ЗНАЧЕНИЙ ПОТЕНЦИОМЕТРОВ


int zero1;  // начальное значение с аналогового входа A0
int zero2;  // начальное значение с аналогового входа A9
int zero3;  // начальное значение с аналогового входа A10
int zero4;  // начальное значение с аналогового входа A2
int zero5;  // начальное значение с аналогового входа A3
int zero6;  // начальное значение с аналогового входа A1


// 2. ПЕРЕМЕННЫЕ ДЛЯ ХРАНЕНИЯ ПРЕДЫДУЩИХ СОСТОЯНИЙ КНОПОК


bool lastBtnS1 = HIGH;   // предыдущее состояние кнопки S1 (D4)
bool lastBtnS2 = HIGH;   // предыдущее состояние кнопки S2 (C6)
bool lastBtnS3 = HIGH;   // предыдущее состояние кнопки S3 (D7)
bool lastBtnS4 = HIGH;   // предыдущее состояние кнопки S4 (E6)
bool lastBtnS5 = HIGH;   // предыдущее состояние кнопки S5 (B4)


// 3. СОЗДАНИЕ ОБЪЕКТА ДЖОЙСТИКА


Joystick_ Joystick;


// 4. ИНИЦИАЛИЗАЦИЯ 

void setup() {
  // Запуск последовательного порта для отладки со скоростью 9600 бод
  // В мониторе порта можно наблюдать значения потенциометров и состояния кнопок
  Serial.begin(9600);
  
  // НАСТРОЙКА ЦИФРОВЫХ ПИНОВ ДЛЯ КНОПОК (ВХОДЫ С ПОДТЯЖКОЙ К ПИТАНИЮ)

  
  pinMode(4, INPUT_PULLUP);   // пин D4 (J1-9)  – кнопка S1 (Эскиз)
  pinMode(6, INPUT_PULLUP);   // пин C6 (J1-10) – кнопка S2 (Выдавливание)
  pinMode(7, INPUT_PULLUP);   // пин D7 (J1-11) – кнопка S3 (Фаска)
  pinMode(17, INPUT_PULLUP);  // пин E6 (J1-12) – кнопка S4 (Измерение)
  pinMode(8, INPUT_PULLUP);   // пин B4 (J1-13) – кнопка S5 (Сброс ориентации)
  

  // ИНИЦИАЛИЗАЦИЯ USB-ДЖОЙСТИКА

  Joystick.begin(false);
  
  // Установка диапазона значений для всех 6 осей (от -512 до +512)
  // Диапазон выбран симметричным относительно нуля для удобства обработки наклонов
  Joystick.setXAxisRange(-512, 512);
  Joystick.setYAxisRange(-512, 512);
  Joystick.setZAxisRange(-512, 512);
  Joystick.setRxAxisRange(-512, 512);
  Joystick.setRyAxisRange(-512, 512);
  Joystick.setRzAxisRange(-512, 512);
  
  // КАЛИБРОВКА ПОТЕНЦИОМЕТРОВ

  
  Serial.println("Калибровка потенциометров... Держите ручку неподвижно!");
  delay(1000);  // пауза перед началом считывания
  
  zero1 = analogRead(A0);   // запоминаем нулевое значение для потенциометра на A0
  zero2 = analogRead(A9);   // запоминаем нулевое значение для потенциометра на A9
  zero3 = analogRead(A10);  // запоминаем нулевое значение для потенциометра на A10
  zero4 = analogRead(A2);   // запоминаем нулевое значение для потенциометра на A2
  zero5 = analogRead(A3);   // запоминаем нулевое значение для потенциометра на A3
  zero6 = analogRead(A1);   // запоминаем нулевое значение для потенциометра на A1
  
  // Вывод нулевых значений в монитор порта для проверки
  Serial.print("Нулевые значения: ");
  Serial.print(zero1); Serial.print(", ");
  Serial.print(zero2); Serial.print(", ");
  Serial.print(zero3); Serial.print(", ");
  Serial.print(zero4); Serial.print(", ");
  Serial.print(zero5); Serial.print(", ");
  Serial.println(zero6);
  
  Serial.println("Калибровка завершена. Устройство готово к работе.");
  Serial.println("==================================================");
}


// 5. ОСНОВНОЙ ЦИКЛ 


void loop() {

  // 5.1 СЧИТЫВАНИЕ ПОТЕНЦИОМЕТРОВ И ВЫЧИСЛЕНИЕ ОТКЛОНЕНИЙ ОТ НУЛЯ

  
  int s1 = analogRead(A0) - zero1;   // отклонение для потенциометра A0
  int s2 = analogRead(A9) - zero2;   // отклонение для потенциометра A9
  int s3 = analogRead(A10) - zero3;  // отклонение для потенциометра A10
  int s4 = analogRead(A2) - zero4;   // отклонение для потенциометра A2
  int s5 = analogRead(A3) - zero5;   // отклонение для потенциометра A3
  int s6 = analogRead(A1) - zero6;   // отклонение для потенциометра A1
  

  // 5.2 ПРЕОБРАЗОВАНИЕ ПОПАРНЫХ СИГНАЛОВ

  
  int diff1 = s1 - s2;   // разность для пары A0/A9
  int sum1  = s1 + s2;   // сумма для пары A0/A9
  int diff2 = s3 - s4;   // разность для пары A10/A2
  int sum2  = s3 + s4;   // сумма для пары A10/A2
  int diff3 = s5 - s6;   // разность для пары A3/A1
  int sum3  = s5 + s6;   // сумма для пары A3/A1
  

  // 5.3 ПЕРЕСЧЁТ В ШЕСТЬ НЕЗАВИСИМЫХ ОСЕЙ ДВИЖЕНИЯ

  
  int axisY = diff1 + diff2 + diff3;          // ось Y (наклон вперёд/назад)
  int axisZ = sum1 - sum3;                    // ось Z (подъём/опускание)
  int axisX = sum2 - sum1/2 - sum3/2;         // ось X (наклон влево/вправо)
  int axisRy = sum1 + sum2 + sum3;            // вращение вокруг Y (рыскание)
  int axisRx = diff1/2 - diff2 + diff3/2;     // вращение вокруг X (тангаж)
  int axisRz = diff1 - diff3;                 // вращение вокруг Z (крен)
  

  // 5.4 УСТАНОВКА ЗНАЧЕНИЙ ОСЕЙ ДЖОЙСТИКА

  Joystick.setXAxis(axisX);
  Joystick.setYAxis(axisY);
  Joystick.setZAxis(axisZ);
  Joystick.setRxAxis(-axisRx);   // инверсия для согласования направления
  Joystick.setRyAxis(-axisRy);   // инверсия для согласования направления
  Joystick.setRzAxis(axisRz);
  
  
  // 5.5 ОБРАБОТКА КНОПОК 

  
  //  Кнопка S1 на пине D4
  bool curBtnS1 = !digitalRead(4);
  if (curBtnS1 != lastBtnS1) {
    Joystick.setButton(0, curBtnS1);   // кнопка 0  – нажата/отпущена
    lastBtnS1 = curBtnS1;              // запоминаем состояние для следующего цикла
    if (curBtnS1) Serial.println("S1 pressed (Эскиз)");
    else Serial.println("S1 released");
  }
  
  //  Кнопка S2 на пине C6 
  bool curBtnS2 = !digitalRead(6);
  if (curBtnS2 != lastBtnS2) {
    Joystick.setButton(1, curBtnS2);   // кнопка 1
    lastBtnS2 = curBtnS2;
    if (curBtnS2) Serial.println("S2 pressed (Выдавливание)");
    else Serial.println("S2 released");
  }
  
  //  Кнопка S3 на пине D7 
  bool curBtnS3 = !digitalRead(7);
  if (curBtnS3 != lastBtnS3) {
    Joystick.setButton(2, curBtnS3);   // кнопка 2 
    lastBtnS3 = curBtnS3;
    if (curBtnS3) Serial.println("S3 pressed (Фаска)");
    else Serial.println("S3 released");
  }
  
  // Кнопка S4 на пине E6 
  bool curBtnS4 = !digitalRead(17);
  if (curBtnS4 != lastBtnS4) {
    Joystick.setButton(3, curBtnS4);   // кнопка 3 
    lastBtnS4 = curBtnS4;
    if (curBtnS4) Serial.println("S4 pressed (Измерение)");
    else Serial.println("S4 released");
  }
  
  // Кнопка S5 на пине B4 
  bool curBtnS5 = !digitalRead(8);
  if (curBtnS5 != lastBtnS5) {
    Joystick.setButton(4, curBtnS5);   // кнопка 4 
    lastBtnS5 = curBtnS5;
    if (curBtnS5) Serial.println("S5 pressed (Сброс ориентации)");
    else Serial.println("S5 released");
  }
  

  // 5.6 ОТПРАВКА СОСТОЯНИЯ НА КОМПЬЮТЕР

  
  Joystick.sendState();
  
  // 5.7 ЗАДЕРЖКА ДЛЯ СТАБИЛЬНОСТИ РАБОТЫ

  delay(10);
}