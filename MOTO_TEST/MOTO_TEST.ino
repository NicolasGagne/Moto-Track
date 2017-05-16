/* 
 *  Moto  Data registre TEST
 */

// Libraries

# include <LiquidCrystal.h>

// initialize the librarys and the numbers of the interface pins
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);  


// Variables
int mode = 0; // Deterniner mode d'affichage
int nbTurn = 0; // Compte le nombre de tour en memoire. 
int setRPMLight = 8000;
bool shiftLightON = true;


const int maxSizeTimeArray = 10;  // limite le nombre de Tour en memoire. 

unsigned long timeLap;
unsigned long timeLastLap = 0;
unsigned long timeBestLap = 0; 
unsigned long timeArray[maxSizeTimeArray];

int lastMode = 6;  // Determine le dernier mode 
int shiftLight = 8;  // Shift light
int RPM = 9; // RPM inn for Engine
int Boutton1 = 12; // PIN need to be change
int Boutton2 = 11; // PIN need to be change
int Boutton3 = 13; // PIN Need to be Change or change for IR reader. 


/// Accelorometer 




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
  //pinMode(A4, INPUT); 
  //pinMode(A5, INPUT); 
  
  lcd.begin(16, 2);  //LCD Columns and rows
  lcd.clear();
  lcd.print("WELCOME MOTOTEST");
  delay(2000);
  lcd.clear();

///// ADXL345


  SwitchMode();

}

void loop() 
{
  switch(mode)
  {
    case 1: // LAP TIME
      {
      modeChrometer();
      mode = 0;
      break;
      }
    case 2:  // LEAN ANGLE
      {
        modeLeanAngle();
        mode = 0;  
        break;
      }
    case 3:  // G FORCE
      {
        modeGForce();
        mode = 0;
        break;
      }
      case 4:  // Shift light
      {
        modeShiftLight();
        mode = 0;
        break;
      }
    case 5:  // VIEW DATA
      {
        //modeView();
        mode = 0;
        break;
      }

    case 6:  // ERASE DATA
      {
        modeErase();
        mode = 0;
        break;
      }

    default:
      {
        SwitchMode();
        break;
      }
  }

}

void SwitchMode()
{
  lcd.setCursor(0,0);
  lcd.print("CHOSE YOUR MODE ");
  delay(100);
  
  
  while(digitalRead(Boutton2) == LOW)
  {
    if (digitalRead(Boutton1) == HIGH)
    {
     mode = mode + 1;
     delay(200); 
    }
    if (mode > lastMode){mode = 1;}
    
    switch(mode)
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
}

void modeChrometer()
//fonction chronometer. 
{
  lcd.clear();
  lcd.print("PUSH 2 BUTTON ");
  lcd.setCursor(0,1);
  lcd.print("TO RETURN MENU");
  delay(5000);
  
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
  shiftLightFunction();  // allumer ou non la shift light
}

void modeGForce()
{
  int gMode = 0;
  
  lcd.clear();
  lcd.print("PUSH 2 BUTTON ");
  lcd.setCursor(0,1);
  lcd.print("TO RETURN MENU");
  delay(2000);
  lcd.clear();
  lcd.print("B #2 TO CONFIRM ");
  lcd.setCursor(0,1);
    
  while(digitalRead(Boutton2) == LOW)
  {
    if (digitalRead(Boutton1) == HIGH)
    {
     gMode = gMode + 1;
     delay(200);    
    }
    if (gMode > 6){gMode = 1;}   
    switch(gMode)
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
  
 
  while (digitalRead(Boutton1) == LOW and digitalRead(Boutton2) == LOW)
  {
    shiftLightFunction();  // allumer ou non la shift light
    switch(gMode)
    {
    case 1:
    {
      break;
    }
    case 2:
    {
      break;
    }
    case 3:
    {
      break;
    }
    case 4:
    {
      break;
    }
    case 5:
    {
      break;
    }
    case 6:
    {
      break;
    }
    }
  }
  
}

void modeShiftLight()
{
  lcd.clear();
  lcd.print("PUSH 2 BUTTON ");
  lcd.setCursor(0,1);   
  lcd.print("TO RETURN MENU");
  delay(2000);
  
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
    lcd.clear();
    lcd.print("PUSH 2 BUTTON ");
    lcd.setCursor(0,1);
    lcd.print("TO RETURN MENU");
    delay(2000);
  
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


