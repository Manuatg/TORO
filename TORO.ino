//-----Config inicial para el sensor VL53L0x-----//
#include <Wire.h>
#include <VL53L0X.h>
VL53L0X sensor;
#define LONG_RANGE //Definimos que el sensor trabajará al máximo de su rango de detección
#define HIGH_ACCURACY //Se define que queremos una medición precisa

//-----Config inicial para el stepper-----//
#include <Stepper.h> //Importamos la librería para controlar motores paso a paso
#define STEPS 200 //Ponemos el número de pasos que necesita para dar una vuelta. 200 en nuestro caso
Stepper stepper(STEPS, 6, 7, 8, 9); //Stepper nombre motor (número de pasos por vuelta, pins de control)

//-----VARIABLES FIJAS-----//
//Definición de las patas//
const int encoder = 0; // Señal ENCODER
const int boton = 1; // Entrada de botón
const int ojos =  2;// Señal de los ojos
const int mu =  3; //Señal de la placa de audio
const int mot1 = 10; // Variable para motor 1
const int mot2 = 11; // Variable para motor 1
const int mot3 = 12; // Variable para motor 2
const int mot4 = 13; // Variable para motor 2
const int pfi = 14; // Señal de pata frontal izquierda
const int pfd = 15; // Señal de pata frontal derecha
const int pti = 16; // Señal de pata trasera izquierda
const int ptd = 17; // Señal de pata trasera derecha

//-----VARIABLES DE DATOS-----//
int avi = 0; //Avanzar a la izquierda
int avd = 0; //Avanzar a la derecha
int re1 = 0; //Retroceder 1
int re2 = 0; //Retroceder 2
int origen = 0; //Indicación de origen del stepper
int torero = 0; //Indicador de detección de torero
int modo=LOW; //Indicador del botón
int duty = 64; //Amplitud de PWM
int inicio=HIGH

void setup()
{
  //-----SETUP VL53L0X-----//
  Wire.begin();
  sensor.init();
  sensor.setTimeout(500);
  sensor.setSignalRateLimit(0.1);
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
  sensor.setMeasurementTimingBudget(20000);

  //-----SETUP STEPPER-----//
  stepper.setSpeed(20); //Velocidad en RPM

  //-----SETUP de pines----//
  //ENTRADAS
  pinMode(encoder, INPUT); 
  pinMode(boton, INPUT);
  pinMode(pfi,INPUT);
  pinMode(pfd,INPUT); 
  pinMode(pti,INPUT); 
  pinMode(ptd,INPUT); 

  //SALIDAS----//
  pinMode(ojos, OUTPUT);
  pinMode(mu, OUTPUT);
  //pinMode(mot1, OUTPUT);
  pinMode(mot2, OUTPUT);
  pinMode(mot3, OUTPUT);
  pinMode(mot4, OUTPUT);
  digitalWrite(mu,HIGH);
}

void acero()//Calibración del motor a pasos
{
  origen = digitalRead(encoder);
  if(origen==LOW)
  {
    while(origen==LOW)
    {
      stepper.step(5);
      delayMicroseconds(2);
      origen = digitalRead(encoder);
    }
  }
}

void deteccion()//Lectura del sensor de distancia
{
  if(sensor.readRangeSingleMillimeters()>=2500) //Cuando el sensor no obtiene medición arroja el valor 8190
  {
    torero=sensor.readRangeSingleMillimeters(); //Se devuelve la distancia medida
  }
  else
  {
    torero=0;    // En caso de no detectar el led permanece apagado
  }
}

void ataque() //Comandos de movimientos antes de atacar (solo por show)
{
  acero();
  digitalWrite(mu,LOW);
  digitalWrite(ojos,HIGH);
  delay(200);
  digitalWrite(mu,HIGH);
  digitalWrite(ojos,LOW);
  delay(200);
  digitalWrite(ojos,HIGH);
  delay(200);
  digitalWrite(ojos,LOW);
  delay(200);
  digitalWrite(ojos,HIGH);
  delay(200);
  digitalWrite(ojos,LOW);
}

void patas()//Regreso al ruedo cuando detecta margen alguna de las patas
{
     avi = digitalRead(ptd); // Durante todo el proceso interrogaremos
     avd = digitalRead(pti);
     re1 = digitalRead(pfi);
     re2 = digitalRead(pfd);
     
        if (avi == HIGH){ //Avanza hacia la izquierda cuando la pata trasera derecha se sale del ruedo.
           stepper.step(25);
           digitalWrite(mot1,HIGH); 
           digitalWrite(mot2,LOW);
           delay(3000);
           stepper.step(-25);
         }
        else if (avd == HIGH){ //Avanza hacia la derecha cuando la pata trasera derecha se sale del ruedo.
           stepper.step(-25);
           digitalWrite(mot1,HIGH); 
           digitalWrite(mot2,LOW);
           delay(3000);
           stepper.step(25);
         }
         else if (re1 == HIGH||re2 == HIGH){ //retrocede cuando la pata trasera izquierda se sale del ruedo.
           digitalWrite(mot2,HIGH); 
           digitalWrite(mot1,LOW);
           delay(3000);
         }
         else {
           digitalWrite(mot1,LOW);
           digitalWrite(mot2,LOW);
           delay(3000);
         }
}

void pwm1()//Aumento de la velocidad del motor tracción
{
  if(modo==HIGH)
    duty=255;
  else{
    delayMicroseconds(1250);
    duty++;
    if(duty>=220) 
    duty=220;
}
  
  
void loop()
{
  if(inicio==HIGH) 
  {
    acero();
    inicio=LOW;
  }
  if(digitalRead(boton)==HIGH)
  {
    while(digitalRead(boton)==HIGH)
    modo=!modo;
    duty=64;
    inicio=HIGH;
  }
  if((digitalRead(ptd)||digitalRead(pti)||digitalRead(pfd)||digitalRead(pfi))==HIGH)//Hace lectura de las patas
  {
    patas();
  }
  else
  {
    analogWrite(mot1,duty); 
    pwm1();
  }
}


