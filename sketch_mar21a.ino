/*------------------------------------------------------------------------*/
// ИТА ЮФУ, ИКТИБ, Кафедра МОП ЭВМ, Группа КТбо3-7
// Разработчики: Иванов Д.А , Тесленко Г.Ю.
// Дата последней модификации: 05.04.2019
// Назначение модуля: Программная часть программно-аппаратной системы,
// реализующей функционал будильника с возможностью установки и
// отображения времени, а также включающей в себя систему автоматического освещения.
//
// Copyright (c) 2019-2019 
//
// Дается разрешение на бесплатное использование, копирование, распостранение, модификацию и другие действия с
// данным программным обеспечением и любыми его компонентами, а также связанными с ними документами любому лицу. 
// Программное обеспечение предоставляется без какого-либо вида гарантии.
// Авторы программного обеспечения не являются ответсвенными за несанкционированные действия и нанесенный урон, 
// являющиеся поледствием использования данного программного обеспечения и любых его компонентов.
/*------------------------------------------------------------------------*/

/*----Подключение библиотек----*/
#include <iarduino_RTC.h>
#include <LiquidCrystal.h>
/*----Глобальные переменные----*/
int mins =0;
int hours = 0;
boolean hour_flag =0;
boolean mins_flag =0;
boolean alarm_set_flag =0;
boolean setter_flag =0;
/*----Инициализация Часов РВ----*/
iarduino_RTC time(RTC_DS3231);
/*----Инициализация ЖК дисплея----*/
LiquidCrystal lcd(13,12,11,10,9,8);

/*----Setup----*/
/*--Автор: Иванов Д.А.--*/
void setup() {

/*----Установка режима работы пина----*/
  pinMode(7,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(A1,INPUT);
  pinMode(A3,INPUT_PULLUP);
  pinMode(A2,INPUT_PULLUP);
  pinMode(A0,INPUT_PULLUP);
/*----Инициализация последовательного соединения----*/
/*----9600 - Скорость передачи----*/
  Serial.begin(9600);
/*----Включение Дисплея----*/
  lcd.begin(16,2);
  lcd.print("Hello User,");
  lcd.setCursor(0,1); 
  lcd.print("Have a nice day!"); 
  delay(2000);
  lcd.clear();
  lcd.print("Set time:");
  printTime();
/*----Установка начального времени----*/  
  while(digitalRead(A0)){
    timeSetter(false);
  }
  delay(200);
/*----Запуск Часов РВ----*/ 
  time.begin();
/*----Установка выбранного времени----*/
  time.settime(0,mins,hours);
  hours =12;
  mins =0;
}

/*----Основной цикл----*/
/*--Автор: Тесленко Г.Ю.--*/
void loop() {
/*----Вывод времени на дисплей каждую секунду----*/
  if(millis()%1000==0){  
      if(alarm_set_flag == 0){
        lcd.clear();
        lcd.print("Current time");
        lcd.setCursor(0,1);
        lcd.print(time.gettime("H:i:s"));      
      }
      delay(1);                                             
   }   
  
/*----Проверка будильника----*/
  checkTime();
  
/*----Опрос фоторезистора----*/
  int raw = analogRead(A1);
/*----Включение/выключение освещения----*/
  if(raw<600){
     digitalWrite(7,HIGH);
  }
  else{
     digitalWrite(7,LOW);
  }
  
/*----Опрос кнопок----*/

  boolean buttonSetTime = !digitalRead(A0);
/*----Кнопка переключения режима----*/  
  if(buttonSetTime ==1 && setter_flag  ==0){
    setter_flag =1;
    analogWrite(3,50);
    delay(10);
  }
  if(buttonSetTime ==0 && setter_flag ==1){
    setter_flag =0;
    if(alarm_set_flag == 0){
      alarm_set_flag =1;
      Serial.println("Entering alarm set mode");
      if(alarm_set_flag ==1){
      lcd.clear();
      lcd.print("Set timer:");
      printTime();
      }
    }
    else{
      alarm_set_flag =0;
      Serial.println("Leaving alarm set mode");
    }
    analogWrite(3,0);
    delay(10);
  }
  
/*----Кнопки часов и минут работают только в режиме установки будильника----*/ 
  if(alarm_set_flag == 1){     
/*----Установка часов----*/ 
    timeSetter(true);
  }
}

/*-------Блок функций-------*/

/*----Проверка нажатия на кнопки---*/
/*--Автор: Иванов Д.А.--*/
void timeSetter(bool isTimer){
    boolean buttonHour = !digitalRead(A2);
    boolean buttonMin = !digitalRead(A3);
/*----Установка часов----*/
    if(buttonHour ==1 && hour_flag  ==0){
      hour_flag =1;
      delay(10);
    }
    if(buttonHour ==0 && hour_flag ==1){
      hour_flag =0;
      updateHours();
      lcd.clear();
      if(isTimer == true){
        lcd.print("Set timer:");
      }
      else{
        lcd.print("Set time:");
      }
      printTime();
      delay(10);
    }
/*----Установка минут----*/
    if(buttonMin ==1 && mins_flag  ==0){
      mins_flag =1;
      delay(10);
    }
    if(buttonMin ==0 && mins_flag  ==1){
      mins_flag =0;
      updateMins();
      lcd.clear();
      if(isTimer == true){
        lcd.print("Set timer:");
      }
      else{
        lcd.print("Set time:");
      }
      printTime();
      delay(10);
    }
}

/*-------Вывод времени в режиме настройки-------*/
/*--Автор: Иванов Д.А.--*/
void printTime(){
  lcd.setCursor(0,1);
  if(hours<10){
    lcd.print("0");
  }
  lcd.print(hours);
  lcd.print(":"); 
  if(mins<10)
  {
    lcd.print("0");
  }
  lcd.print(mins);
}

/*----Инкремент часов----*/ 
/*--Автор: Тесленко Г.Ю.--*/
void updateHours(){
  hours++;
  if(hours == 24){
    hours =0;
  }
}

/*----Инкремент минут----*/ 
/*--Автор: Тесленко Г.Ю.--*/
void updateMins(){
  mins++;
  if(mins == 60){
    mins =0;
    updateHours();
  }
}

/*----Проверка времени будильником----*/
/*--Автор: Тесленко Г.Ю.--*/ 
void checkTime(){
  /*----Включение звука на 10 секунд----*/ 
  if(time.Hours == hours && time.minutes == mins && time.seconds <=10){
    analogWrite(3,50); 
  }
  else{
    analogWrite(3,0);
  }
}
