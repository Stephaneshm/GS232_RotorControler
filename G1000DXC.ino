/* ------------------------------------------------------------------------- *
 * Name   : YAESU G1000DXC Controler via PSTROTATOR
 * Author : Stéphane HELAIEM - F4IRX
 * Date   : december 12, 2024
 * Purpose: PSTRotator controler YAESU G1000DXC via est port
 * Versions:
 *    1.0  : Test  
 *    1.1a : update Comment
 *     
 *     
 *     
 *     
 *     
 *     
 *     
 *   
 * ------------------------------------------------------------------------- */



/* ------------------------------------------------------------------------- *
 *       Include libraries 
 * ------------------------------------------------------------------------- */
#include <LCD-I2C.h>
#include <Wire.h>
/* ------------------------------------------------------------------------- *
 *       Other definitions
 * ------------------------------------------------------------------------- */
int8_t cw=4;   //relai cw
int8_t ccw=5;  //relai ccw
int analogPin = A0;
unsigned long currentTime=0;
unsigned long previousTime=0;
unsigned long lgMoyennePos;
int valPosAnt = 0;
int bCommand=0;
int valPosCmd;
bool FlagCW=LOW;
bool FlagCCW=LOW;
bool FlagRS232=LOW;
/* ------------------------------------------------------------------------- *
 *       Object
 * ------------------------------------------------------------------------- */
LCD_I2C lcd(0x27, 16, 2); 

/* ------------------------------------------------------------------------- *
 *       SETUP
 * ------------------------------------------------------------------------- */
void setup() {
    Wire.begin();
    lcd.begin(&Wire);
    lcd.display();
    lcd.backlight();
    Serial.begin(9600);
    pinMode(cw, OUTPUT);    //relai cw 
    pinMode(ccw, OUTPUT);   //relai ccw 
    digitalWrite(cw,LOW);delay(10);
    digitalWrite(ccw,LOW);delay(10);
    lcd.print("F4IRX - G1000DXC"); 
    lcd.setCursor(5, 1); 
    lcd.print("Version ");
    delay(1000);
    lcd.clear();
    printAZLCD();
}
/* ------------------------------------------------------------------------- *
 *       Print Rotor position en LCD
 * ------------------------------------------------------------------------- */
void printAZLCD() {
    lcd.setCursor(0, 1);
    lcd.print("AZ=");lcd.print( valPosAnt);lcd.print("     ");
}

/* ------------------------------------------------------------------------- *
 *       MOVE CW
 * ------------------------------------------------------------------------- */
void MoveCW() {
   if( valPosAnt<valPosCmd) {
       digitalWrite(cw,HIGH);
       lcd.setCursor(13, 1);
       lcd.print("CW ");
       } else { 
       digitalWrite(cw,LOW);
       lcd.setCursor(13, 1);
       lcd.print("   ");
       FlagCW=LOW;
       FlagRS232=LOW;
       ClearLCDCommand();
       }
}

/* ------------------------------------------------------------------------- *
 *       MOVE CCW
 * ------------------------------------------------------------------------- */
void MoveCCW() {
   if( valPosAnt>valPosCmd) {
       digitalWrite(ccw,HIGH);
       lcd.setCursor(13, 1);
       lcd.print("CCW");
       } else { 
       digitalWrite(ccw,LOW);
       lcd.setCursor(13, 1);
       lcd.print("   ");
       FlagCCW=LOW;
       FlagRS232=LOW;
       ClearLCDCommand();
       }
}

/* ------------------------------------------------------------------------- *
 *       Clear PSTRotator Command
 * ------------------------------------------------------------------------- */
void ClearLCDCommand(){
    lcd.setCursor(0, 0);
    lcd.print("                ");
}

/* ------------------------------------------------------------------------- *
 *       LOOP
 * ------------------------------------------------------------------------- */
void loop()
{
  currentTime=millis();
  if((currentTime-previousTime)>100){  /// 100ms
		previousTime=currentTime;
     valPosAnt=analogRead(analogPin);
     valPosAnt = map(valPosAnt, 75, 1000, 0, 450); // on determine l'angle 
    printAZLCD();
    Serial.print("AZ=");Serial.println(valPosAnt); 
  }
  if (Serial.available()>0 && FlagRS232==LOW ){  
    String a = (Serial.readString());  
    if(a.indexOf('M') >=0) { 
      String  az= a.substring((a.indexOf('M')+1), (a.indexOf('M')+4));
      valPosCmd = az.toInt();
      lcd.setCursor(0, 0);
      lcd.print("Pstrotator >");
      lcd.setCursor(13, 0);
      lcd.print(valPosCmd);lcd.print("   ");
      FlagRS232=HIGH;
      if( valPosCmd>valPosAnt) { FlagCW=HIGH;FlagCCW=LOW; }
      if( valPosCmd<valPosAnt) { FlagCCW=HIGH; FlagCW=LOW;}
     }
    if(a.indexOf('C') >=0 ) {
       Serial.print("AZ=");Serial.println(valPosAnt); 
     }
   }
  if(FlagCW==HIGH && FlagCCW==LOW ) {MoveCW();}
  if(FlagCCW==HIGH && FlagCW==LOW ) {MoveCCW();}

} // fin LOPP


