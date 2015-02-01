#include <AccelStepper.h>
#include "OneButton.h"
#include <LiquidCrystal.h>
#include <TimerOne.h>


 #include "pitches.h"

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4,4,4,4,4 };

#define PIN_SENZOR 0
#define ZGOMOT 5

#define FULLSTEP 4
#define HALFSTEP 8

#define motorPin5  8     // Blue   - 28BYJ48 pin 1
#define motorPin6  10     // Pink   - 28BYJ48 pin 2
#define motorPin7  9    // Yellow - 28BYJ48 pin 3
#define motorPin8  11    // Orange - 28BYJ48 pin 4
                        // Red    - 28BYJ48 pin 5 (VCC)
                        
OneButton viteza(A5,false); //setare viteza
OneButton distanta(2,false); //setare distanta
OneButton startstop(3,false); //start stop


AccelStepper stepper2(HALFSTEP, motorPin5, motorPin7, motorPin6, motorPin8);

LiquidCrystal lcd(12, 13, 4,5,6,7);//definire pini LCD

int senzor;
int apas_b1=0;
int D=25;//se stabileste distanta de rulare
int distanta_limita=28;

int apas_b2=0;
int V=5;//se stabileste distanta de rulare
int viteza_limita=5;

byte stare;

void initializare()
{
lcd.clear();
lcd.print("Initializare!");  
//codul de revenire a ledurilor in punctul de plecare
stepper2.setMaxSpeed(1150);
stepper2.setAcceleration(500.0);
stepper2.moveTo(-200000);
senzor = analogRead(A3);
while (senzor>0){
senzor = analogRead(A3);
stepper2.run();
}
stepper2.setCurrentPosition(0);
stepper2.stop();
beep();
digitalWrite(A1, LOW);
digitalWrite(A0, HIGH);




}

//aceasta functie e necesara pentru a tipari corect pe LCD 16x1
void print_lcd(){
lcd.clear();
lcd.setCursor(0,0);
String mesaj="V=";
String mesaj1=mesaj+V;
String mesaj2=mesaj1+"cm/min";
lcd.print(mesaj2);
lcd.setCursor(0,1);
mesaj="D=";
mesaj1=mesaj+D;
mesaj2=mesaj1+"cm";
lcd.print(mesaj2);
}

void setVit()
{
if (stare!=1){    
apas_b2++;
V=apas_b2;
print_lcd();
if (V==viteza_limita){
apas_b2=0;
}
}
}

void setDist()
{
if (stare!=1){  
apas_b1++;
if (apas_b1==6)
{
  D=28;
}
else
{
D=apas_b1*5;}
print_lcd();
if (D==distanta_limita){
apas_b1=0;
}
}
}

void start()
{
stare++;
}


void setup() {
lcd.begin(8, 2);

pinMode(A0, OUTPUT);
digitalWrite(A0, LOW);

pinMode(A1, OUTPUT);
digitalWrite(A1, LOW);

pinMode(A2, OUTPUT);

viteza.attachClick(setVit);
distanta.attachClick(setDist);
startstop.attachClick(start);

Timer1.initialize(200);
Timer1.attachInterrupt(butoane); // verifica butoanele la 0.15 seconds
 	
stepper2.setMaxSpeed(1150);
initializare();
print_lcd();
delay (2000);
}

void butoane()
{
 viteza.tick();
 distanta.tick();
 startstop.tick();
}


void loop() {
  if (stare==1){
  lcd.clear();
  lcd.print("Am parcurs:");
  lcd.setCursor(2,1);
  String mesaj=" din ";
  String mesaj1=mesaj+D;
  String mesaj2=mesaj1+" cm";
  lcd.print(mesaj2);
  vit_const();
  stare=0;
}
else if (stare==2)
{
 stare=0;
}

}

void vit_const()
{
   int tmp;
   digitalWrite(A0, LOW);
   digitalWrite(A1, HIGH);   
   stepper2.moveTo(D*475);
   stepper2.setSpeed(V*7.88);
   while ((stepper2.distanceToGo() != 0)&&(stare!=2)){
   tmp=D-(stepper2.distanceToGo()/475)-1;
   lcd.setCursor(0,1);
   if (tmp>=0)
   lcd.print(tmp);
   stepper2.runSpeedToPosition();  
   }
   if (stare!=2){
   lcd.clear();
   lcd.print("Termin expunerea");
   lcd.setCursor(0,1);
   lcd.print("in: ");
   stepper2.move(1425);//1425 reprezinta cei 3 cm(dimensiunea ledurilor)
   stepper2.setSpeed(V*7.88);
   while ((stepper2.distanceToGo() != 0)&&(stare!=2)){
   lcd.setCursor(3,1);
   lcd.print(stepper2.distanceToGo()/475+1);
   stepper2.runSpeedToPosition();  
   }   
   }
   
   stepper2.stop();
   delay (2000);
   initializare();
   delay (2000);
   print_lcd();
}

void beep(){
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
    tone(A2, melody[thisNote],noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(A2);
  }
}  
