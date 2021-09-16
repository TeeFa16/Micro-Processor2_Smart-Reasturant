
#include<LiquidCrystal.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>

////////////////////////ahmed hussin/////////////////////////////
bool mode_choice = true ;   
bool mode = false ; // false ---> cleaning    true ----> active restaurant  
#define LDR_PIN A1
#define S0 32
#define S1 34
#define S2 36
#define S3 38
#define sensorOut 30
/* ------------------------------------------------
  |                  Variables                     |
  |                                                |
   ------------------------------------------------*/
//Motor Objects
Servo UpperServo;
Servo BottomServo;

//Sensor Readings
int LDR_Reading;
int Mapped_LDR_Reading;
int color;
int frequency = 0;

//Digital Readings
int Reset_Status;

//LCD Object
//LiquidCrystal lcd(12, 8, 5, 4, 3, 2);

//Money Counters
int Red_Counter=0;
int Blue_Counter=0;
int Green_Counter=0;
int Yellow_Counter=0;


  unsigned long previousMillisAH = 0; 

  void timeDelay(unsigned long timetoDelay)
  {
    while (1)
    {
      SolarTrackingUnit();
      if((unsigned long)( millis()- previousMillisAH) >= timetoDelay)
      {
        SolarTrackingUnit();
        previousMillisAH = millis();
        return;
      }
    }
  }


int readColor() {
  // Setting red filtered photodiodes to be read
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  // Reading the output frequency
  frequency = pulseIn(sensorOut, LOW);
  int R = frequency;
  // Printing the value on the serial monitor
  //Serial.print("R= ");//printing name
  Serial.print(frequency);//printing RED color frequency
  Serial.print("  ");
  timeDelay(1000);
  // Setting Green filtered photodiodes to be read
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  // Reading the output frequency
  frequency = pulseIn(sensorOut, LOW);
  int G = frequency;
  // Printing the value on the serial monitor
  //Serial.print("G= ");//printing name
  Serial.print(frequency);//printing RED color frequency
  Serial.print("  ");
  timeDelay(1000);
  // Setting Blue filtered photodiodes to be read
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  // Reading the output frequency
  frequency = pulseIn(sensorOut, LOW);
  int B = frequency;
  // Printing the value on the serial monitor
  //Serial.print("B= ");//printing name
  Serial.print(frequency);//printing RED color frequency
  Serial.println("  ");
  timeDelay(1000);
  if(B<1500)
  {
    color=2; //white
    }else if(G>2500)
    {
      color=1; //red
      }else if(G<2500)
      {
        color=4; //yellow
        }
  
  return color;  

}





///////////////////////////////////////////////////////////////
int tables[6] = {1, 1, 1, 1, 1, 1}; //1 table available , 0 table occupied
int orders[6] = {0, 0, 0, 0, 0, 0};
int price[6] = {0, 0, 0, 0, 0, 0}; // price of each order
int currtable = 0;
bool OrderState = false; //true if choosing from menu, false if choosing table
bool Pay_Ord = false;    // false print payment and order menu ; true go to welcome screen
int state = 0;
int choice = 0;
LiquidCrystal_I2C lcd(0x27, 16, 2);
//////////////////Lucio
#define fire 10000000000
#define sensorPIR 2
#define hour9PM 90000
#define hour9AM 180000
#define sensorSmoke A0

short dataFromSlave = 0;

unsigned long long previousMilliePIR=0;
bool PIRstate=LOW;

///////////////////

////////////////hazem/////////////////
#define order_trigger  12
bool order_trigger_state = 0;
//////////////////////////////////

//////////////////////////STU////////////////////////////////
//Photoresist pin
#define STU_ldrtopl A10 //top left
#define STU_ldrtopr A9  //top right
#define STU_ldrbotl A11 // bottom left
#define STU_ldrbotr A8  // bottom right

// Time
// REVIEWME:

//we define the horizontal servomotors
Servo STU_servohori;
const byte STU_servohoriPIN = 10;
int STU_servoh = 0;
int STU_servohLimitHigh = 160;
int STU_servohLimitLow = 60;
//we define the vertical servomotors
Servo STU_servoverti;
const byte STU_servovertiPIN = 11;
int STU_servov = 0;
int STU_servovLimitHigh = 160;
int STU_servovLimitLow = 60;
// Measure loop Control
unsigned long STU_Measure_interval = 50;
unsigned long STU_Measure_previousMills = 0;
// Effect delay
unsigned long STU_Effect_interval = 10;
unsigned long STU_Effect_previousMills = 0;

//////////////////////////////////////////////////////////////

const byte ROWS = 4;
const byte COLS = 3;

char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3};

void Get_Money(int price)
{
   
  while(price>0)
  {
  UpperServo.write(0);
  BottomServo.write(30);
  timeDelay(1000);
  LDR_Reading=analogRead(LDR_PIN);
  //Mapped_LDR_Reading=map(LDR_Reading,344,1017,0,500);
  Serial.println(LDR_Reading);
  lcd.clear();
  lcd.print("Rem: " + String(price));

   if(LDR_Reading<=30)
  {
    //red 5
    // White 10
    //yellow 15
    SolarTrackingUnit();
     timeDelay(100);
     UpperServo.write(45);
     color= readColor();
     timeDelay(1000);
    switch(color)
    {
      case 1:
      {
        Red_Counter++;
        //BottomServo.write(50);
        price-=5;
        Serial.println("Red Detected");
        timeDelay(250);
        break;
      }
      case 2:
      {
        Blue_Counter++;
        //BottomServo.write(70);
        price-=10;
        Serial.println("White Detected");
        break;
      }
      case 4:
      {
        Yellow_Counter++;
        //BottomServo.write(110);
        price-=15;
        Serial.println("Yellow Detected");
        break;
      }
    }
     UpperServo.write(90);
     timeDelay(500);
   }
  }
}

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup()
{
  //////hussin///////////////
  UpperServo.attach(42);
  BottomServo.attach(44);
 // lcd.begin(16, 2);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW); 
    // put your setup code here, to run once:
    Serial.begin(38400);
    lcd.begin();
    lcd.backlight();
    lcd.clear();
    pinMode(sensorPIR, INPUT_PULLUP);
  pinMode(sensorSmoke,INPUT);
  attachInterrupt(digitalPinToInterrupt(sensorPIR),detector,CHANGE);//

  //////////////////////hazem///////////////////
  pinMode(order_trigger, INPUT);
  /////////////////////////////////////////////

  //////////////////STU//////////////////////
  STU_servohori.attach(STU_servohoriPIN);
  STU_servohori.write(60);
  STU_servoverti.attach(STU_servovertiPIN);
  STU_servoverti.write(60);
  ////////////////////////////////////
}

void print_WELCOM()
{
    lcd.setCursor(0, 0);
    lcd.print("Tables Available");
    lcd.setCursor(0, 1); // go to start of 2nd line
    int j = 0;
    for (int i = 0; i < 6; ++i)
    {
        lcd.setCursor(j, 1);
        if (tables[i] == 1)
        {
            lcd.print(i + 1, DEC);
            j = j + 2;
        }
    }
    return;
}
void print_PayOrOrd()
{
    lcd.setCursor(0, 0);
    lcd.print("Press 1(Order)");
    lcd.setCursor(0, 1);
    lcd.print("Press 2(Payment)");
}

int keypad_input(int case_check)
{ // case_check = 0 --> check for reserving table    || case_check = 1 --> check for paying price
    char customKey = customKeypad.getKey();
    if (customKey != NO_KEY)
    {
        int value = customKey - '0';
        if (value == 9) // for return to the main screen
            return 9;
        if (value > 6 || value < 1)
        {
            return 0;
        }
        if (case_check == 0)
        {
            if (tables[value - 1] == 0)
            {
                return 0;
            }
            tables[value - 1] = 0;
            currtable = value;
            return 1;
        }
        else
        {
            if (tables[value - 1] == 0)
            {
                tables[value - 1] = 1;
                orders[value - 1] = 0;
                return price[value - 1];
            }
            else
                return -1;
        }
    }
    else
    {
        return -1; //no key pressed
    }
}

void printMenu()
{
    lcd.setCursor(0, 0);
    lcd.print("Menu: 1-x 2-y");
    lcd.setCursor(0, 1);
    lcd.print("3-z 4-m 5-c");
    return;
}

//Code for sending from resturant arduino to robot arduino
//Always send 3 digit code
//    operation
// |------------------------------------------ |
// |      1     |table Number | OrderNumber    | ---> sending order and table data
// |            |             |                |
// |      2     |      x      |       x        | ---> Fire Signal  (0 open)
// |            |             |                |
// |      3     |      x      |       x        | ---> Cleaning mode signal
// |            |             |                |
// |      4     |      x      |       x        | ---> Security mode (0 open)
// |-------------------------------------------|

void sendToRobot(int code)
{
    if (Serial.available() > 0)
    {                          // Checks whether data is comming from the serial port
        state = Serial.read(); // Reads the data from the serial port
    }
    Serial.write(code);
    delay(10);
    return;
}

void getOrder()
{
    char customKey = customKeypad.getKey();
    if (customKey != NO_KEY)
    {
        int value = customKey - '0';
        if (value == 9) // for return to the main screen
        {
            OrderState = false;
            choice = 1;
            lcd.clear();
            tables[currtable - 1] = 1;
            return;
        }
        if (value >= 1 && value <= 5)
        {
            orders[currtable - 1] = value;
            switch (value)
            {
            case 1:
                price[currtable - 1] = 5;
                break;
            case 2:
                price[currtable - 1] = 10;
                break;
            case 3:
                price[currtable - 1] = 5;
                break;
            case 4:
                price[currtable - 1] = 10;
                break;
            case 5:
                price[currtable - 1] = 15;
                break;
            }
            OrderState = false;
            lcd.clear();
            int code = 200 + (currtable * 10) + value;
            sendToRobot(code);
            currtable = 0;
            Pay_Ord = false;
            choice = 0;
            lcd.setCursor(3, 0);
            lcd.clear();
        }
        else
        {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.clear();
        }
        return;
    }
}

void detector()
{
  
  if ( digitalRead(sensorPIR))
   PIRstate=HIGH;
   else
    PIRstate=LOW; 
  
}

/////////////////////////////////////////////////STU///////////////////////////////

void SolarTrackingUnit() {
  // [1]: Get current angel
  STU_servoh = STU_servohori.read();
  STU_servov = STU_servoverti.read();

  // [2]: Get Analog value of the photoresistor
  int topl = analogRead(STU_ldrtopl);
  int topr = analogRead(STU_ldrtopr);
  int botl = analogRead(STU_ldrbotl);
  int botr = analogRead(STU_ldrbotr);

  // [3]: Get average
  int avgtop   = (topl + topr); //average of top
  int avgbot   = (botl + botr); //average of bottom
  int avgleft  = (topl + botl); //average of left
  int avgright = (topr + botr); //average of right

  // [4]: Get current time
  // REVIEWME:
  if ((unsigned long)(millis() - STU_Measure_previousMills) >= STU_Measure_interval) {
    // [5]: Operate Vertically
    STU_OperateVertical(avgtop, avgbot);
    // [6]: Operate Horizentally
    STU_OperateHorizental(avgleft, avgright);
    STU_Measure_previousMills = millis();
  }
}

void STU_OperateVertical(int avgtop, int avgbot) {
  if (avgtop < avgbot)
  {
    STU_servoverti.write(STU_servov + 1);
    if (STU_servov > STU_servovLimitHigh)
    {
      STU_servov = STU_servovLimitHigh;
    }
    // delay(10);
  }
  else if (avgbot < avgtop)
  {
    STU_servoverti.write(STU_servov - 1);
    if (STU_servov < STU_servovLimitLow)
    {
      STU_servov = STU_servovLimitLow;
    }
    // delay(10);
  }
  else
  {
    STU_servoverti.write(STU_servov);
  }
}

void STU_OperateHorizental(int avgleft, int avgright) {
  if (avgleft > avgright)
  {
    STU_servohori.write(STU_servoh + 1);
    if (STU_servoh > STU_servohLimitHigh)
    {
      STU_servoh = STU_servohLimitHigh;
    }
    // delay(10);
  }
  else if (avgright > avgleft)
  {
    STU_servohori.write(STU_servoh - 1);
    if (STU_servoh < STU_servohLimitLow)
    {
      STU_servoh = STU_servohLimitLow;
    }
  }
  else
  {
    STU_servohori.write(STU_servoh);
  }
}
//////////////////////////////////////////////////////////////////////////

void loop()
{
    /////////////STU////////////////////
    
    SolarTrackingUnit();
    if ( mode_choice == true )
    {
         lcd.setCursor(0, 0);
         lcd.print("Night 1");
         lcd.setCursor(0, 1);
         lcd.print("Day 2");
        char customKey = customKeypad.getKey();
       if (customKey != NO_KEY)
        {
           int value = customKey - '0';
           if ( value == 1)
           {
              mode = false;
              mode_choice = false;
           }
           if ( value == 2) {mode = true; mode_choice = false;}
        }
    }
    ////////////////////////////////////////////////
    // put your main code here, to run repeatedly:
    if ( mode_choice == false && mode == true)
    {
    if (Pay_Ord == false)
    {
        print_PayOrOrd();
        char customKey = customKeypad.getKey();
        if (customKey != NO_KEY)
        {
            int value = customKey - '0';
            if (value == 2 || value == 1)
            {
                choice = value;
                Pay_Ord = true;
                lcd.clear();
            }
            else if ( value == 9)
            {
              mode_choice = true; 
              lcd.clear();
        }
              
            }
        }
    else if (OrderState == false && choice == 1)
    {
        print_WELCOM();
        int KPI_value = -1;
        KPI_value = keypad_input(0);
        if (KPI_value == 9)
        {
            Pay_Ord = false;
            choice = 0;
            lcd.clear();
        }
        if (KPI_value == 1)
        {
            //take table order
            OrderState = true;
            lcd.clear();
        }
        else if (KPI_value == 0)
        {
            //print lcd unavilable
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Table Unavilable");
            delay(1000);
            lcd.clear();
        }
    }
    else if (OrderState == false && choice == 2)
    {
        lcd.setCursor(0, 0);
        lcd.print("Your Table");
        int KPI_value = -1;
        KPI_value = keypad_input(1);
        if (KPI_value == 9)
        {
            Pay_Ord = false;
            choice = 0;
            lcd.clear();
        }
        else if (KPI_value != -1 && KPI_value != 0)
        {
            Pay_Ord = false;
            choice = 0;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Your Price: " + String(KPI_value));
            timeDelay(1000);
            Get_Money(KPI_value);
            lcd.setCursor(3, 1);
            lcd.clear();
        }
    }
    if (OrderState == true)
    {
        printMenu();
        getOrder();
    }
           if (digitalRead(order_trigger)) {
         sendToRobot(1);
        unsigned long waittime = millis();
        while(millis()-waittime<70){}
       }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////
    /////////// Lucio //////////////////////
    ////////////////////////////////////////////////
  if (analogRead(sensorSmoke) > fire) //senseor smoke reading
     {
      if (Serial.availableForWrite() > 0) {
          sendToRobot(0);
      }
    }
 
 // Reading the masterSwitchValue
 //ACTIVATION MODE SECURITY NIGHT SHIFT 
  if ( mode_choice == false && mode == false)
  {
        lcd.setCursor(0, 0);
        lcd.print("Back Modes");
        lcd.setCursor(0, 1);
        lcd.print("Press 9");
        char customKey = customKeypad.getKey();
       if (customKey != NO_KEY)
        {
           int value = customKey - '0';
           if ( value == 9)
           {
              mode_choice = true; 
              lcd.clear();
           }
        }
       if ( PIRstate== HIGH) {
     sendToRobot(0);// Sends 0 to the master to turn on BUZZER
     }
   }
 }
