/* 
 *  Moto  Data registre TEST
 */

// Libraries

# include <LiquidCrystal.h>
#include <SparkFun_ADXL345.h>         // SparkFun ADXL345 Library

// initialize the librarys and the numbers of the interface pins
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);  
ADXL345 adxl = ADXL345();             // USE FOR I2C COMMUNICATION

// Constante
const int maxSizeTimeArray = 10;  // limite le nombre de Tour en memoire. 
const int mGForce = 4;  // Accepted values are 2g, 4g, 8g or 16g


// Variables
int nbTurn = 0; // Compte le nombre de tour en memoire. 
int setRPMLight = 8000;
bool shiftLightON = true;
unsigned long timeLap;
unsigned long timeLastLap = 0;
unsigned long timeBestLap = 0; 
unsigned long timeArray[maxSizeTimeArray];


int shiftLight = 8;  // Shift light
int RPM = 9; // RPMpulse inn form the Engine
int Boutton1 = 12; // PIN need to be change
int Boutton2 = 11; // PIN need to be change
int Boutton3 = 13; // PIN Need to be Change or change for IR reader. 
int maxLean[2] = {14,16};


float maxGForceArray[6] = {0,0,0,0,0,0}; // {minX, maxX, minY, maxY, minZ, maxZ}


void setup() 
{
////// TO BE REMOVE LATER
 Serial.begin(9600);      // open the serial port at 9600 bps: 

//////  
  pinMode(Boutton1, INPUT);
  pinMode(Boutton2, INPUT);
  pinMode(Boutton3, INPUT);
  pinMode(shiftLight, OUTPUT);
  pinMode(RPM,INPUT);
   
  lcd.begin(16, 2);  //LCD Columns and rows
  lcd.clear();
  lcd.print("WELCOME MOTOTEST");
  delay(2000);
  lcd.clear();

///// ADXL345
  adxl.powerOn();                     // Power on the ADXL345
  adxl.setRangeSetting(mGForce);           // Give the range settings
                                      // Accepted values are 2g, 4g, 8g or 16g
  adxl.setActivityXYZ(1, 1, 1);       // Set to activate movement detection in the axes "adxl.setActivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setActivityThreshold(75);      // 62.5mg per increment   // Set activity   // Inactivity thresholds (0-255)                                    

  

}

void loop() 
{
  switch(SwitchMode())
  {
    case 1: // LAP TIME
      {
        modeChrometer();
        //mode = 0;
        break;
      }
    case 2:  // LEAN ANGLE
      {
        modeLeanAngle();
        //mode = 0;  
        break;
      }
    case 3:  // G FORCE
      {
        modeGForce();
        //mode = 0;
        break;
      }
      case 4:  // Shift light
      {
        modeShiftLight();
        //mode = 0;
        break;
      }
    case 5:  // VIEW DATA
      {
        //modeView();
        //mode = 0;
        break;
      }

    case 6:  // ERASE DATA
      {
        modeErase();
        //mode = 0;
        break;
      }

    /*default:
      {
        mode = SwitchMode();
        break;
      }*/
  }

}

int SwitchMode()
{
  int sMode = 0; 
  int lastMode = 6;  // Determine le dernier mode 
  
  lcd.setCursor(0,0);
  lcd.print("CHOSE YOUR MODE ");
  delay(100);
  while(digitalRead(Boutton2) == LOW)
  {
    if (digitalRead(Boutton1) == HIGH){sMode = sMode + 1; delay(200); }
    
    if (sMode > lastMode){sMode = 1;}
    
    switch(sMode)
    { 
    case 1:
      {
      lcd.setCursor(0,1);
      lcd.print("1- LAP TIME     ");
      break;
      }
    case 2:
      {
      lcd.setCursor(0,1);
      lcd.print("2- LEAN ANGLE   ");
      break;
      }
    case 3: 
      {
      lcd.setCursor(0,1);
      lcd.print("3- G Force      ");
      break;
      }
    case 4: 
      {
      lcd.setCursor(0,1);
      lcd.print("4- SET SHIFT LIGHT   ");
      break;
      }
    case 5: 
      {
      lcd.setCursor(0,1);
      lcd.print("5- VIEW LAP     ");
      break;
      }
    case 6: 
      {
      lcd.setCursor(0,1);
      lcd.print("6- ERASE ALL    ");
      break;
      }
    default:
      {
      lcd.setCursor(0,1);
      lcd.print("B #1 TO SWITCH  ");
      break;
      } 
    }
  }
  return sMode;
}

void modeChrometer()
//fonction chronometer. 
{
  modeStart();
  
  lcd.clear();
  lcd.print ("START IN 5 SEC");
  delay(1000);

  lcd.clear();
  lcd.print ("START IN 4 SEC");
  delay(1000);

  lcd.clear();
  lcd.print ("START IN 3 SEC");
  delay(1000);

  lcd.clear();
  lcd.print ("START IN 2 SEC");
  delay(1000);

  lcd.clear();
  lcd.print ("START IN 1 SEC");
  delay(1000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("LAP TIME " );
  

  timeArray[nbTurn] = millis();

  while (digitalRead(Boutton1) == LOW or digitalRead(Boutton2) == LOW)
  {
    Serial.print(nbTurn);
    Serial.print(digitalRead(Boutton1));
    Serial.print(digitalRead(Boutton2));
    Serial.println(digitalRead(Boutton3));
    shiftLightFunction();  // allumer ou non la shift light
    timeLap = millis() - timeArray[nbTurn];  

    if (digitalRead(Boutton3) == HIGH)
    {      
      nbTurn = nbTurn + 1;
      timeArray[nbTurn] = millis();
      timeLastLap = timeArray[nbTurn ] - timeArray[nbTurn -1];
      if (nbTurn == 1){timeBestLap = timeLastLap;}
      else if (timeBestLap > timeLastLap){timeBestLap = timeLastLap;}
      delay(500);
    }
    
    if (nbTurn == 0){timeLastLap = 0; timeBestLap = 0;}
    
    if (nbTurn >= maxSizeTimeArray - 1)
    {
      Serial.print("Max Size Array");
      nbTurn = 0;
      lcd.clear();
      lcd.print("ERROR   ERROR  "); 
      lcd.setCursor(0,1);
      lcd.print("PUSH BOTH BUTTONS"); 
      while (digitalRead(Boutton1) == LOW and digitalRead(Boutton2) == LOW){}
    }
    
    if (timeLap < 10000 and nbTurn > 0)
    {
      lcd.setCursor(0,0);
      lcd.print("LAST   " );
      timePrint(timeLastLap,1);
    }
    else
    {
      lcd.setCursor(0,0);
      lcd.print("LAP ");
      if (nbTurn < 9){lcd.print(" ");}
      lcd.print(nbTurn + 1 );
      lcd.print(" ");
      timePrint(timeLap,0);
    }
    
    
    lcd.setCursor(0,1);
    lcd.print("BEST   ");
    timePrint(timeBestLap,1);   
  }
}

void modeLeanAngle()
{
  int modeLA = 0;
  modeStart();
  
  lcd.clear();
  lcd.print("MODE Lean Angle");
  
  while(digitalRead(Boutton2) == LOW)
  {   
    if (digitalRead(Boutton1) == HIGH){ modeLA = modeLA + 1; delay(200);  }
    if (modeLA > 4){modeLA = 1;}
       
    switch(modeLA)
    { 
      case 1:
      {
        lcd.setCursor(0,1);
        lcd.print("1- Lean Angle    ");
        break;
      }
      case 2:
      {
        lcd.setCursor(0,1);
        lcd.print("2- Max Lean   ");
        break;
      }
      case 3:
      {
        lcd.setCursor(0,1);
        lcd.print("3- View Max  ");
        break;
      }  
      case 4:
      {
        lcd.setCursor(0,1);
        lcd.print("4- Erase LA Record   ");
        break;
      }
      default:
      {
        lcd.setCursor(0,1);
        lcd.print("B #1 TO SWITCH  ");
        break;
      }
    }
  }  
  
  switch (modeLA)
  {
   case 1:
   {
     int lean;
         
     lcd.clear();
     lcd.print("Actual Lean ");
     lcd.setCursor(0,1);
     lcd.print("LEFT ");
     lcd.setCursor(8,1);
     lcd.print("RIGHT ");
     while (digitalRead(Boutton1) == LOW or digitalRead(Boutton2) == LOW)
     {
      shiftLightFunction();  // allumer ou non la shift light
      lean = accRead(); // lean = accelerometer Read data Function Return array with actual Lean and update maxLean. 
      if (lean == 0)
      { 
        lcd.setCursor(0,1);
        lcd.print("  CENTER        ");
      }  
      else if (lean < 0)
      {
        lcd.setCursor(0,1);
        lcd.print("  LEFT         ");
        lcd.setCursor (9,1);
        lcd.print(abs(lean));
      }
      else if (lean > 0)
      {
        lcd.setCursor(0,1);
        lcd.print("  RIGHT           ");
        lcd.setCursor (9,1);
        lcd.print(abs(lean));
      }
     }
   }
   case 2: // Max lean
   {
    int lean = 0;
    
    lcd.clear();
    lcd.print("MAX LEAN ANGLE");
    lcd.setCursor(0,1);
    lcd.print("LEFT ");
    lcd.setCursor(8,1);
    lcd.print("RIGHT ");
    while (digitalRead(Boutton1) == LOW or digitalRead(Boutton2) == LOW)
    {
      shiftLightFunction(); // allumer ou non la shift light
      lean = accRead(); // accelerometer Read data Function Return array with actual Lean and update maxLean. 
      lcd.setCursor(5,1);
      lcd.print(abs(maxLean[0]));
      lcd.setCursor(14,1);
      lcd.print(maxLean[1]);
    }

    break;
   }
   case 3: // View Max
   {
    lcd.clear();
    lcd.print("VIEW MAX LEAN ");
    lcd.setCursor(0,1);
    lcd.print("LEFT ");
    lcd.print(abs(maxLean[0]));
    lcd.print("RIGHT ");
    lcd.print(maxLean[1]);
    while (digitalRead(Boutton1) == LOW or digitalRead(Boutton2)== LOW)
     {
      delay(200);
     }
     
    break;
   }
   case 4:
   {
    lcd.clear();
    lcd.print("CAUTION ERASE LA");
    lcd.setCursor(0,1);
    lcd.print("Both Button to CFM");
    delay(3000);
    if (digitalRead(Boutton1) == HIGH and digitalRead(Boutton2) == HIGH)
    {
      lcd.clear();
      lcd.print("WARNING");
      lcd.setCursor(0,1);
      lcd.print("WILL ERASE LA");
      delay(2000);    
      if (digitalRead(Boutton1)== HIGH and digitalRead(Boutton2) == HIGH) {maxLean[0]= 0; maxLean[1] = 0;}
      lcd.clear();
      lcd.print("LA Erase complete");
      delay(2000);      
    }
    
   }
 }
}

// accelerometer Read data Function Return array with actual Lean and update maxLean. 
int accRead()
{
  int x,y,z;  
  int roll = 0;
  
  adxl.readAccel(&x, &y, &z);         // Read the accelerometer values and store them in variables declared above x,y,z

  roll = -(int(atan2(float(x), float(z)) * 57.3));
  Serial.print("ROLL ");
  Serial.println(roll);

  //Update the max lean
  if (roll > 0)
  {
    if (maxLean[1] < roll) { maxLean[1] = roll;}
  }
  else 
  {
    if (maxLean[0] > roll) {maxLean[0] = roll;}
  }

  Serial.print("maxLean ");
  Serial.print(maxLean[0]);
  Serial.print("   ");
  Serial.println(maxLean[1]);

  return roll;
}

void modeGForce()
{
  modeStart();
  int gMode = gForceModeSwitch();
  
  delay(200);
  while (digitalRead(Boutton1) == LOW or digitalRead(Boutton2) == LOW)
  {
    int x,y,z;  
    float gX,gY,gZ; 
    
    if (digitalRead(Boutton1) == HIGH or digitalRead(Boutton2 == HIGH)) {gMode = gForceModeSwitch();}  // retour au menu de G Force. 
    
    if (gMode <= 4)  // Arret de nouvelle lecture en visionement et effacement. 
    {
      shiftLightFunction();  // allumer ou non la shift light
      // Accelerometer Readings 
      adxl.readAccel(&x, &y, &z);         // Read the accelerometer values and store them in variables declared above x,y,z
      // Conversion du reading en G force. 
      gX = mapfloat(x,-255,255,-mGForce/2,mGForce/2);
      gY = mapfloat(y,-255,255,-mGForce/2,mGForce/2);
      gZ = mapfloat(z,-255,255,-mGForce/2,mGForce/2);

      //Check againgst the Minium and Maximum
      if (gX < maxGForceArray[0] ) {maxGForceArray[0]  = gX;}
      if (gX > maxGForceArray[1] ) {maxGForceArray[1]  = gX;}
      if (gY < maxGForceArray[2] ) {maxGForceArray[2]  = gY;}
      if (gY > maxGForceArray[3] ) {maxGForceArray[3]  = gX;}
      if (gZ < maxGForceArray[4] ) {maxGForceArray[4]  = gZ;}
      if (gZ > maxGForceArray[5] ) {maxGForceArray[5]  = gZ;}
    }
  
    switch(gMode)
    {
    case 1: // Actual G force
    {
      lcd.clear();
      lcd.print("G Force X ");
      lcd.print(gX);
      lcd.setCursor(0,1);
      lcd.print("Y ");
      lcd.print(gY);
      lcd.setCursor(8,1);
      lcd.print("Z ");
      lcd.print(gZ);
      break;
    }
    case 2: // X G force
    {
      lcd.clear();
      lcd.print("Force X   ");
      lcd.print(gX);
      lcd.setCursor(0,1);
      lcd.print("MIN");
      lcd.print(maxGForceArray[0] );
      lcd.setCursor(8,1);
      lcd.print("MAX ");
      lcd.print(maxGForceArray[1] );
      break;
    }
    case 3: // Y G Force
    {
      lcd.clear();
      lcd.print("Force Y   ");
      lcd.print(gY);
      lcd.setCursor(0,1);
      lcd.print("MIN");
      lcd.print(maxGForceArray[2] );
      lcd.setCursor(8,1);
      lcd.print("MAX ");
      lcd.print(maxGForceArray[3] );
      break; 
    }
    case 4:   // Z G Force
    {
      lcd.clear();
      lcd.print("Force Z   ");
      lcd.print(gZ);
      lcd.setCursor(0,1);
      lcd.print("MIN");
      lcd.print(maxGForceArray[4] );
      lcd.setCursor(8,1);
      lcd.print("MAX ");
      lcd.print(maxGForceArray[5] );
      break;
    }
    case 5:  // All Best
    {
      bool m = true;
      lcd.clear();
      while (digitalRead(Boutton2) == LOW)
      {
         if (digitalRead(Boutton1) == HIGH) {m = !m; lcd.clear();}
         if (m == true)
         {
          lcd.setCursor(0,0);
          lcd.print("MAX G   X ");
          lcd.print(maxGForceArray[1] );
          lcd.setCursor(0,1);
          lcd.print("Y ");
          lcd.print(maxGForceArray[3] );
          lcd.setCursor(8,1);
          lcd.print("Z ");
          lcd.print(maxGForceArray[5] ); 
          delay(200);    
         }
         else
         {
          lcd.setCursor(0,0);     
          lcd.print("MIN G   X ");
          lcd.print(maxGForceArray[0] );
          lcd.setCursor(0,1);
          lcd.print("Y ");
          lcd.print(maxGForceArray[2] );
          lcd.setCursor(8,1);
          lcd.print("Z ");
          lcd.print(maxGForceArray[4] ); 
          delay(200);       
         }        
      }
      break;
    }
    case 6:  // Erase Data
    {
      lcd.clear();
      lcd.print(" G Force Mode");
      lcd.setCursor(0,1);
      lcd.print("WILL ERASE ALL G Rec");
      delay(2000);
      while(digitalRead(Boutton1)== LOW and digitalRead(Boutton2) == LOW)
      {
        lcd.clear();
        lcd.print("PRESS TOW BUTTON");
        lcd.setCursor(0,1);
        lcd.print("FOR 2 SEC TO CFM");
        delay(1500);
        lcd.clear();
        lcd.print("PRESS 1 BUTTON");
        lcd.setCursor(0,1);
        lcd.print("TO RETURN MENU");
        delay(1500); 
      }
  
      delay(200);
      if (digitalRead(Boutton1)== HIGH and digitalRead(Boutton2) == HIGH)
      {
        lcd.clear();
        lcd.print("WARNING");
        lcd.setCursor(0,1);
        lcd.print("ERASE G Force");     
        delay(2000);
        if (digitalRead(Boutton1)== HIGH and digitalRead(Boutton2) == HIGH)
        {
          for (int i = 0; i <= 5 ; i++){maxGForceArray[i] = 0;}
          lcd.clear();
          lcd.print("ERASE COMPLETE");
          delay(2000);
        }
       } 
       break;
     }
    }
  }
}

int gForceModeSwitch()
{
  int gModeSwitch = 0;
  lcd.clear();
  lcd.print("B #2 TO CONFIRM ");
  lcd.setCursor(0,1);
    
  while(digitalRead(Boutton2) == LOW)
  {
    if (digitalRead(Boutton1) == HIGH)
    {
     gModeSwitch = gModeSwitch + 1;
     delay(200);    
    }
    if (gModeSwitch > 6){gModeSwitch = 1;}   
    switch(gModeSwitch)
    { 
    case 1:
      {
      lcd.setCursor(0,1);
      lcd.print("1- ACTUAL G Force");
      break;
      }
    case 2:
      {
      lcd.setCursor(0,1);
      lcd.print("2- X Force      ");
      break;
      }
    case 3: 
      {
      lcd.setCursor(0,1);
      lcd.print("3- Y Force      ");
      break;
      }
    case 4: 
      {
      lcd.setCursor(0,1);
      lcd.print("4- Z Force      ");
      break;
      }
    case 5: 
      {
      lcd.setCursor(0,1);
      lcd.print("4- ALL Best     ");
      break;
      }  
    case 6: 
      {
      lcd.setCursor(0,1);
      lcd.print("5- ERASE GForce");
      break;
      }
    default:
      {
      lcd.setCursor(0,1);
      lcd.print("B #1 TO SWITCH  ");
      break;
      } 
    }
  }
  return gModeSwitch;
}

void modeShiftLight()
{
  modeStart();
  
  lcd.clear();
  lcd.print("PRESS 1 SET RPM");
  lcd.setCursor(0,1);
  lcd.print("PRESS 2 ON/OFF");
  delay(2000);

  lcd.clear();
  lcd.print("SETTING LIGHT");
  lcd.setCursor(0,1);
  lcd.print("RPM LIGHT ");
  lcd.setCursor(10,1);
  lcd.print(setRPMLight);
  lcd.print("       ");
  
  while(digitalRead(Boutton1)== LOW or digitalRead(Boutton2) == LOW)
  {
    if (digitalRead(Boutton1) == HIGH)
    {
      setRPMLight = setRPMLight + 250;
      if (setRPMLight > 18000){setRPMLight = 4000;}
      lcd.setCursor(10,1);
      lcd.print(setRPMLight);
      lcd.print("       ");
      delay(200);     
    }  
    if (digitalRead(Boutton2) == HIGH)
    {
      lcd.setCursor(10,1);
    
      if (shiftLightON == false)
      {
        shiftLightON = true;
        lcd.print("  ON         ");
        delay(200);
      }
      else 
      {
        shiftLightON = false;
        lcd.print("  OFF         ");
        delay(200);
      }      
      lcd.print("       ");    
    }
    shiftLightFunction();  // allumer ou non la shift light
  }  
}

void modeView()
{
  int nbLap = 1;
  delay(200);
  while(digitalRead(Boutton1)== LOW and digitalRead(Boutton2) == LOW)
  {
    modeStart();
  
    lcd.clear();
    lcd.print("PRESS 1 GO FOWARD");
    lcd.setCursor(0,1);
    lcd.print("PRESS 2 GO BACK");
    delay(2000);
  }
  
  while(digitalRead(Boutton1)== LOW or digitalRead(Boutton2) == LOW)
  {
    if (digitalRead(Boutton1) == HIGH)
    {
      nbLap = nbLap + 1;
      if (nbLap > nbTurn){nbLap = 1;}
    }
    if (digitalRead(Boutton2) == HIGH)
    {
      nbLap = nbLap - 1;
      if (nbLap < 1) 
      {
        if (nbTurn > 0){nbLap = nbTurn;}
        else {nbLap = 1;}
      }
    }
    lcd.clear();
    lcd.print("LAP no ");
    lcd.print(nbLap);
    lcd.setCursor(3,1);
    timePrint(timeArray[nbLap] - timeArray[nbLap-1], 1);
    delay(200);   
  }  
}

void modeErase()
{
  lcd.clear();
  lcd.print("ARE YOU SURE IT");
  lcd.setCursor(0,1);
  lcd.print("WILL ERASE ALL DATA");
  delay(2000);
  while(digitalRead(Boutton1)== LOW and digitalRead(Boutton2) == LOW)
  {
    lcd.clear();
    lcd.print("PRESS TOW BUTTON");
    lcd.setCursor(0,1);
    lcd.print("FOR 2 SEC TO CFM");
    delay(1500);
    lcd.clear();
    lcd.print("PRESS 1 BUTTON");
    lcd.setCursor(0,1);
    lcd.print("TO RETURN MENU");
    delay(1500); 
  }
  
  delay(200);
  if (digitalRead(Boutton1)== HIGH and digitalRead(Boutton2) == HIGH)
  {
    lcd.clear();
    lcd.print("WARNING");
    lcd.setCursor(0,1);
    lcd.print("WILL ERASE DATA");
    delay(1000);
    delay(2000);
    if (digitalRead(Boutton1)== HIGH and digitalRead(Boutton2) == HIGH)
    {
      for (int i = 0; i < maxSizeTimeArray; i++){timeArray[i] = {};}
      nbTurn = 0;
      for (int i = 0; i <= 5 ; i++){maxGForceArray[i] = 0;}
      for (int i = 0; i <= 1; i ++) {maxLean[i] = 0; }
      lcd.clear();
      lcd.print("ERASE COMPLETE");
      delay(1000);
    }
  }
}
    

void timePrint(long int timemillis, bool fix)
// impression formaté des temps
{
  float heure = 0 ;
  float minute = 0 ;
  float seconde = 0 ;
  float milliseconde = 0; 
  unsigned long over;

  heure = int(timemillis/3600000);
  over = timemillis%3600000;
  minute = int(over/60000);
  over = over%60000;
  seconde = int(over/1000);
  milliseconde = over%1000;
  
  if (heure > 0 )
  {
    lcd.print(int(heure));
    lcd.print(":");
  }
  if (minute < 10){lcd.print("0");}
  lcd.print(int(minute));
  lcd.print(":");
  if (seconde < 10){lcd.print("0");}
  lcd.print(int(seconde));
  lcd.print(":");
  if (fix == true)
  {
    if (milliseconde < 10){lcd.print("0");}
    if (milliseconde < 100){lcd.print("0");}
  }
  lcd.print(int(milliseconde));  
}

void shiftLightFunction()
// function permettant a la shift light de fonctionner
{
  int RPMPulse, RPM;
  
  if (shiftLightON == true) // vérifie que l'utilisateur veux le shift light. 
  {
    RPMPulse = pulseIn(RPM,HIGH,75000);  // pulseIn fonctionne en MICRO seconde  1millis = 1000micro
    if (RPMPulse < 1) {RPMPulse = 1;}
    RPM = int(60000000 / RPMPulse);
    Serial.print(setRPMLight);
    Serial.print(RPM);
    Serial.print(shiftLightON);
    Serial.println(RPMPulse);
    
    if (RPM >= setRPMLight) // check RPM vs setting. 
      {digitalWrite(shiftLight, HIGH); } // allume la shift light
    else  // Ferme la shift light pour éviter qu'elle rest allumée. 
      {digitalWrite(shiftLight,LOW);}
  }
  else {digitalWrite(shiftLight,LOW); }  // s'assure que la shift light reste a OFF
}

void modeStart()
{
  lcd.clear();
  lcd.print("PUSH 2 BUTTON ");
  lcd.setCursor(0,1);
  lcd.print("TO RETURN MENU");
  delay(2000);
} 

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
// Same as Map but work with Float.
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


