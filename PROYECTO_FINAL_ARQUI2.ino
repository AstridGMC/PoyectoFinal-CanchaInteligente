#include <Servo.h>
#include "DHT.h"
#include <Stepper.h> 
Servo servo;

//servomotor y LDR
int eLDRPin = A0;// asignando pin al LDR e
int oLDRPin = A1;// asignando pin al LDR o
int esteLDR = 0; //variable para almacenar los valores del LDR e
int oesteLDR = 0; //variable para almacenar los valores del LDR o
int diferencia = 0; //variable para comparacion de los dos LDR's
int error = 10;  // Variable por si hay una diferencia notoria entre los LDR´s
int servoPin = 8;// pin 8 para el servo
int pulsoMinimo = 700;
int pulsoMaximo = 2300;
int servoSet = 130; //Variable for position of servo - will be different for each device

//int pinesLuces
int ledExterior1 = 9;
int ledExterior2 = 10;
int ledExterior3 = 11;
int ledExterior4 = 12;

//stepper motor
Stepper motor1(2048,2,4,3,5);
int MEDIA_VUELTA=1024;
int totalVueltas = MEDIA_VUELTA*8;// 4 vueltas enteras
int vueltas=0;

//sensor de agua
const int sensorAgua = 6;
int sensorValue = 0;
float jardinHumedo = 0.0;

void setup() {
  Serial.begin(9600); 
  servo.attach(servoPin, pulsoMinimo, pulsoMaximo);// inicializar servomotor
  pinMode(ledExterior1, OUTPUT);     // pin 9 como salida
  pinMode(ledExterior2, OUTPUT);     // pin 10 como salida
  pinMode(ledExterior3, OUTPUT);     // pin 11 como salida
  pinMode(ledExterior4, OUTPUT);     // pin 12 como salida
  motor1.setSpeed(2); //numero rep. RPM (valores de 1,2 o 3 para 28BYJ-48
}

void loop() {
   MoverPanel();
   EncenderLucesExterior();
   TechoInteligente();
}

void MoverPanel(){
   esteLDR = analogRead(eLDRPin); //Leer el valor de los LDR 
   oesteLDR = analogRead(oLDRPin);
   if (esteLDR < 400 && oesteLDR < 400) {  //revisamos no hay luz en ambos LDR's
      while (servoSet <=140 && servoSet >=15) {     //si lo es, enviamos el panel al este donde saldra el sol al dia siguiente
        servoSet ++;
        servo.write(servoSet);
        delay(100);
      }
    }
    diferencia = esteLDR - oesteLDR ; //revisamos la diferencia de luz
    if (diferencia > 10) {          //guiamos el panel hacia la posicion donde esta el LDR Ccon mayor lectura
       if (servoSet <= 140) {
         servoSet ++;
         servo.write(servoSet);
       }
    } else if (diferencia < -10) {
      if (servoSet >= 15) {
        servoSet --;
        servo.write(servoSet);
       }
    }
 Serial.print(esteLDR);      //Serial monitor can be useful for debugging/setting up
 Serial.print("   -   ");    //Use it to see if your LDR's are noticeably different when
 Serial.print(oesteLDR);      //They have equal light shining on them, if so, correct with the error value
 Serial.print("   -   ");
 Serial.print(diferencia);   
 Serial.print("   -   ");
 Serial.print(servoSet);     //Fine tune the servo settings, to maximise swing available
 Serial.print("   -   ");
 Serial.println(".");
 delay(100);
}

void EncenderLucesExterior(){
   if (analogRead(eLDRPin) < 150 && analogRead(oLDRPin) < 150) {
    digitalWrite(ledExterior1, HIGH);
    digitalWrite(ledExterior2, HIGH);
     digitalWrite(ledExterior3, HIGH);
    digitalWrite(ledExterior4, HIGH);
  }
  else {
    digitalWrite(ledExterior1, LOW);
    digitalWrite(ledExterior2, LOW);
    digitalWrite(ledExterior3, LOW);
    digitalWrite(ledExterior4, LOW);
  }
  delay(100);
}

void TechoInteligente(){
  if((analogRead(eLDRPin) > 250) && (analogRead(oLDRPin) > 250)){
    MoverStepperAdelante();
  }else if( Lluvia()){
    MoverStepperAdelante();
  }else{
    MoverStepperAtras();
  }
}


void MoverStepperAdelante(){
  //si la variable vueltas es menor que totalVueltas aun 
  //no ha terminado de colocarse el techo
  if(vueltas<totalVueltas){
    vueltas= vueltas + MEDIA_VUELTA;
    motor1.step(MEDIA_VUELTA);// 1 media de vuelta hacia adelante
  delay(1000);
  }
}

void MoverStepperAtras(){
  //si la variable vueltas es mayor a cero aun no ha terminado 
  //de guardarse el techo
  if(vueltas>0){
    vueltas= vueltas - MEDIA_VUELTA;
    motor1.step(-MEDIA_VUELTA);// 1 media de vuelta hacia adelante
    delay(1000);
  }
}

boolean Lluvia() {
  sensorValue = analogRead(sensorAgua);
  jardinHumedo = sensorValue * 100 / 1024;
  if (sensorValue > 120) {
    return true;
  } else {
    return false;
  }
}
