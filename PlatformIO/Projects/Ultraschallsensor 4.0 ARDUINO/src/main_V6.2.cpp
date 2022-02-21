#include <Arduino.h>
#include <stdlib.h>

#define maxloops 20000
#define maxloops2 500
#define len 20 
#define max_duration 45000 //unit is  us 

//Function prototypes
void measure(int PulseDetect, int ReadChannel, int waiting);
void home_menu();
void RESET_STRING(char*);
void READ_String(char*);
char READ_Char();
int READ_int(char*);
double READ_double (char*);
int SELECT_Time();
char SERIAL_FLUSH();
int READ_Digit();
void PWM_Setup();
void PWM_EN();
void PWM_DIS();
void CALC_Speed();
void LED_RangeMeter(double distance, double height);
void ResetMeter();
int MotorControl(int PulseDetect, double distance, int SEC_STATE);
void MotorOff();

//Global variables
const char CSV_SEPARATOR = ';';
int STATE_VARIABLE = 0;
int STRING_POSITION = 0;
char str[len];
int RX_STRING_READY = 0;
long time_ms = 0;
int run = 0;
double Air_Speed = 0;



void setup() 
{
  pinMode(49, INPUT);  //Pulse detection left
  pinMode(47, INPUT);  //Pulse detection right

  pinMode(53, OUTPUT); //Trigger Pin for Arduino Nano (Transmitter PWM)

  PWM_Setup(); 
  Serial.begin(115200); //9600 not sufficient here. Buffer overflow will occur and thus the measurement resolution will be lowered (less samples per measurement will be sent via serial)
}
  
void loop() 
{
  switch(STATE_VARIABLE)
  {
    //------------------------------------
    
    case 0:
    {
      CALC_Speed();
      home_menu();
      RESET_STRING(str);
      break;
    }
    //------------------------------------  
    
    case 1:
    {
      if(run == 0)
      {
        Serial.println("Press any key to stop the measurement");
        Serial.println("Starting measurement.");
        run = 1;
      }
      
      measure(49, A0, 0);
      measure(47, A1, 0);
      
      if(Serial.available() > 0)
      {
        char dump = Serial.read();
        STATE_VARIABLE = 7;
        run = 0;
      }
      
      else
      {
        STATE_VARIABLE = 1;
        run = 1;
      }

      break;
    }
    
    //------------------------------------
    
    case 2:
    { 
      time_ms = (long) SELECT_Time();
      
      Serial.println("Starting measurement.");
      
      
      if(time_ms > 0)
      {
        long delta = 0;
        long start = millis();

        while(delta <= time_ms)
        {
          measure(49, A0, 25);
          measure(47, A1, 100);
          delta = millis()-start;
        }
        
        Serial.println("Measurement ended.");
        STATE_VARIABLE = 5;
      }

      break;
    }

    case 3:
    {
      Serial.println(); //extra new line for better readability
      //Motor Testing procedure
      char NextEngine = '0';

      Serial.println("Testing rumble engine @ index finger");
      Serial.println("Press space to test next rumble engine");
      //Motor 1 (index finger)
      while(NextEngine != ' ')
      {
        PWM -> PWM_ENA = PWM_ENA_CHID3 | PWM_ENA_CHID4;
        REG_PWM_CDTYUPD3 = PWM_CDTYUPD_CDTYUPD(0);
        REG_PWM_CDTYUPD4 = PWM_CDTYUPD_CDTYUPD(0);
        NextEngine = READ_Char();
  
      }
      PWM -> PWM_DIS = PWM_DIS_CHID3 | PWM_DIS_CHID4;
      NextEngine = '0';
      

      Serial.println("Testing rumble engine @ middle finger");
      Serial.println("Press space to test next rumble engine");
      while(NextEngine != ' ')
      {
        PWM -> PWM_ENA = PWM_ENA_CHID2 | PWM_ENA_CHID5;
        REG_PWM_CDTYUPD2 = PWM_CDTYUPD_CDTYUPD(0);
        REG_PWM_CDTYUPD5 = PWM_CDTYUPD_CDTYUPD(0);
        NextEngine = READ_Char();
      }
      PWM -> PWM_DIS = PWM_DIS_CHID2 | PWM_DIS_CHID5;
      NextEngine = '0';


      Serial.println("Testing rumble engine @ ring finger");
      Serial.println("Press space to test next rumble engine");
      while(NextEngine != ' ')
      {
        PWM -> PWM_ENA = PWM_ENA_CHID1 | PWM_ENA_CHID6;
        REG_PWM_CDTYUPD1 = PWM_CDTYUPD_CDTYUPD(0);
        REG_PWM_CDTYUPD6 = PWM_CDTYUPD_CDTYUPD(0);
        NextEngine = READ_Char();
      }
      PWM -> PWM_DIS = PWM_DIS_CHID1 | PWM_DIS_CHID6;
      NextEngine = '0';

      Serial.println("Testing rumble engine @ pinky");
      Serial.println("Press space to test next rumble engine");
      //Motor 4 (pinky)
      while(NextEngine != ' ')
      {
        PWM -> PWM_ENA = PWM_ENA_CHID0 | PWM_ENA_CHID7;
        REG_PWM_CDTYUPD0 = PWM_CDTYUPD_CDTYUPD(0);
        REG_PWM_CDTYUPD7 = PWM_CDTYUPD_CDTYUPD(0);
        NextEngine = READ_Char();
      }
      PWM -> PWM_DIS = PWM_DIS_CHID0 | PWM_DIS_CHID7;
      NextEngine = '0';

      Serial.println("All rumble engines have been tested.");

      STATE_VARIABLE = 7;
      break;
    }

    case 4:
    {
      Serial.println("Ending programm.");
      MotorOff();
      
      while(1)

      STATE_VARIABLE = 4;
      break;
    } 

    case 5:
    {
      MotorOff();
      char choice;
      Serial.println("Restart measurement? Y/N");
      
      while((choice != 'y') && (choice != 'n'))
      {
        choice = READ_Char();
      }
      
      if(choice == 'y')
      {
        STATE_VARIABLE = 6;
      }

      if(choice == 'n')
      {
        STATE_VARIABLE = 7;
      }

      break;
    }

    case 6:
    {
      Serial.println("Restarting previous measurement.");

      long delta = 0;
      long start = millis();

      while(delta <= time_ms)
      {
        measure(49, A0, 25);
        measure(47, A1, 100);
        delta = millis()-start;
      }
        
      Serial.println("Measurement ended.");
      STATE_VARIABLE = 5;
      MotorOff();

      break;
    }

    case 7:
    {
      MotorOff();
      int TEMP_STATE = 0;
      Serial.println("What now?");
      Serial.println("1: Go to main menu    2: Exit");
      
      while(TEMP_STATE == 0)
      {
        TEMP_STATE = READ_Digit();
      }

      if(TEMP_STATE == 1)
      {
        STATE_VARIABLE = 0;
      }

      else if(TEMP_STATE == 2)
      {
        STATE_VARIABLE = 4;
      }

      else
      {
        Serial.println("Invalid input");
        STATE_VARIABLE = 7;
      }
      
      break;
    }


    default:
    {
      MotorOff();
      STATE_VARIABLE = 0;
      break;
    }


  }
}

void home_menu()
{
  Serial.println("Hello!");
  Serial.println("Please choose your next action...");
  Serial.println("1: continuos measurement");
  Serial.println("2: measurement interval");
  Serial.println("3: Test rumble motors");
  Serial.println("4: End program");
  Serial.println();
  Serial.println("Enter mode of operation: ");

  while(STATE_VARIABLE <= 0)
  {
    STATE_VARIABLE = READ_Digit();
  }
  SERIAL_FLUSH(); 
}

int SELECT_Time()
{
  SERIAL_FLUSH();
  long time_ms = 0;
  char TEMP_STATE = 0;
  int TIME_READY = 0;
  Serial.println("Enter measurement duration: ");

  while(RX_STRING_READY == 0)
  {
    time_ms = (long) READ_int(str);
  }

  if(time_ms <= 0)
  {
    while(TIME_READY == 0)
    {
      SERIAL_FLUSH();
      Serial.println("Invalid input. Choose next step.");
      Serial.println("1: Try again    2: Go back to menu");
     
      while(TEMP_STATE == 0)
      {
        TEMP_STATE = READ_Char();
      }

      if(TEMP_STATE == 1)
      {
        STATE_VARIABLE = 2;
        TIME_READY = 1;
        time_ms = -1;
      }

      else if(TEMP_STATE == 2)
      {
        STATE_VARIABLE = 0;
        TIME_READY = 1;
        time_ms = -1; 
      }
      
      else
      {
        Serial.println("Invalid input");
        TIME_READY = 0;
        TEMP_STATE = 0;
      }
    }
  }

  else
  {
    Serial.print("You entered: ");
    Serial.println(time_ms);
    Serial.println();
  }

  TEMP_STATE = 0;
  RESET_STRING(str);  
  SERIAL_FLUSH();
  return time_ms;

}

void measure(int PulseDetect, int ReadChannel, int waiting)
{
  //Variables
  unsigned long start = 0;
  unsigned long duration = 0;
  double distance = 0;
  unsigned long start2 = 0;
  unsigned long length = 0;
  int adc0 = 0;
  int adc1 = 0;
  double height = 0;
  int end = 9999;
  int loops = 0;
  int loops2 = 0;
  int i = 0;
  int SEC_STATE = 0;
  unsigned long measure_duration = 0;
  int first_run = 0;



  //Start measurement
  digitalWrite(53, HIGH);         //Start PWM @ Nano
  start = micros();               //begin of measurement
  delayMicroseconds(150);         //wait to send a pulse chain
  digitalWrite(53, LOW);          //Stop PWM @ Nano
  delayMicroseconds(2500);        //Wait for Transmitter interfernce to decay
  //End of period
  
  if(PulseDetect == 49)
  {
    Serial.println("Left----------------------------------------");
  }

  else if(PulseDetect == 47)
  {
    Serial.println("Right---------------------------------------");
  }

  SEC_STATE = 0;
  
  while(measure_duration < max_duration)
  {
    adc0 = 0;
    adc1 = 0;
    length = 0;
    i = 0;

    while(i != 1 && measure_duration < max_duration)       //Wait for comparator to sense reflected signal
    { 
      i = digitalRead(PulseDetect);          //Check if comparator Output is high
      loops++;
      //Wait for response
      measure_duration = micros()-start; //before it was -start2. This might havve caused the error with the measured obstacle which isnt actually present!
    }

    duration = micros()-start;     //Calculate duration for reflected signal to reach receiver

    start2 = micros();             //time reference for pulse duration (refers to the pulses created by objects)
    
    //find max. amplitude in the reflected signal 
    while(length < 500)
    {
      adc1 = adc0;                 //Store last conversion result for comparison
      adc0 = analogRead(ReadChannel);       //Load ADC value                   

      if(adc0 < adc1)              //Check if max is reached
      {
        adc0 = adc1;
      }

      length = micros()-start2;
    }
    
    //length = micros()-start2;       //Calculate pulse duration, information about pulse duration discarded in this version since the pulse duration is a result of the excitation which resluts in pulse heigth!

    if(loops < maxloops )
    {
      //Calculations and output
      distance = (double) (0.5*Air_Speed*duration)/1000000;
      height = (double) adc0*33/10240;
    
      //double temp_speed = (double) (-height*152 + 182);
      //int speed = (int) temp_speed;


      SEC_STATE = MotorControl(PulseDetect, distance, SEC_STATE);
      
         
      Serial.print(time_ms);
      Serial.print(CSV_SEPARATOR);
      Serial.print(height);
      Serial.print(CSV_SEPARATOR);
      Serial.print(distance);
      Serial.print(CSV_SEPARATOR);
      Serial.print(" ");
      Serial.print(SEC_STATE);
      Serial.print(" ");
      Serial.print(length);
      Serial.print("  ");

    }
    
    else
    {
      Serial.println("Out of range! Data discarded.");
    }
  
  measure_duration = micros() - start;

  
  }

  //Slow down the process
  delay(waiting);
  Serial.println();
 }

char READ_Char()
{
  char temp = 0;
  if(Serial.available() > 0)
  {
    temp = Serial.read();  
  }

  return temp;
}

int READ_Digit()
{
  char temp = 0;
  int num = 0;
  
  if(Serial.available() > 0)
  {
    temp = Serial.read();
    num = temp - '0';  
  }
  
  return num;
}

void READ_String(char* str)
{
  if(Serial.available() > 0)
  {
    
    str[STRING_POSITION] = Serial.read();
     
    if(str[STRING_POSITION] == '\n')
    {
      RX_STRING_READY = 1;
      str[STRING_POSITION] = '\0';                //The string position to be exchanged with '\0' depends on the terminal. If EOL is CRLF then -1 else no substraction needed.
    } 

    else
    {
      if(STRING_POSITION < len)
      {
        STRING_POSITION++;
      }

      else
      {
        STRING_POSITION = 0;
      }
    }
  }
}

int READ_int(char* str)
{
  READ_String(str);
  return atoi(str);
}

double READ_double (char* str)
{
  READ_String(str);
  return strtod(str, NULL);
}

void RESET_STRING(char* str)
{
  
  while(STRING_POSITION >= 0)
  {
    str[STRING_POSITION] = '\0';
    STRING_POSITION--;
  }

  RX_STRING_READY = 0;
  STRING_POSITION = 0;
}

char SERIAL_FLUSH()
{
  char t;
  while(Serial.available() > 0)
  {
    t = Serial.read();
    Serial.println("flushing");
    
  }
  return t;
}

void PWM_Setup()
{
    int default_speed = 250;
    
    //Ports
    PIOC -> PIO_PDR = PIO_PDR_P21 | PIO_PDR_P2 | PIO_PDR_P4 | PIO_PDR_P6 | PIO_PDR_P8 | PIO_PDR_P22 | PIO_PDR_P23 | PIO_PDR_P24; //Enable peripheral control of PC21

    //PWM Setup Use PWM 9 on Arduino pin 9
    PMC -> PMC_WPMR = PMC_WPMR_WPKEY(0x504D43) | ~PMC_WPMR_WPEN; //Enable PMC write access
    PMC -> PMC_PCER1 = PMC_PCER1_PID36; //Enable PWM peripheral clock
    PMC -> PMC_WPMR = PMC_WPMR_WPKEY(0x504D43) | PMC_WPMR_WPEN; //Disable PMC write access
    
    PIOC -> PIO_ABSR = PIO_ABSR_P21 | PIO_ABSR_P2 | PIO_ABSR_P4 | PIO_ABSR_P6 | PIO_ABSR_P8 | PIO_PDR_P22 | PIO_PDR_P23 | PIO_PDR_P24; //Select peripheral type B which is PWM Output

    PWM -> PWM_WPCR = PWM_WPCR_WPKEY(0x50574D) | 0x00; //Enable write access on all groups
    PWM -> PWM_CLK = PWM_CLK_PREA(1); //PREA_DIV  = 2
    REG_PWM_CMR4 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 

    //PWM Channel 0 - Pin 34 = PC2 (PWML0)
    REG_PWM_CMR0 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
    REG_PWM_CPRD0 = PWM_CPRD_CPRD(250);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY0 = PWM_CDTY_CDTY(default_speed);   //default value

    //PWM Channel 1 - Pin 36 = PC4 (PWML1)
    REG_PWM_CMR1 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
    REG_PWM_CPRD1 = PWM_CPRD_CPRD(250);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY1 = PWM_CDTY_CDTY(default_speed); //default value
  
    //PWM Channel 2 - Pin 38 = PC6 (PWML2)
    REG_PWM_CMR2 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
    REG_PWM_CPRD2 = PWM_CPRD_CPRD(250);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY2 = PWM_CDTY_CDTY(default_speed); //default value

    // PWM Channel 3 - Pin 40 = PC8 (PWML3)
    REG_PWM_CMR3 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
    REG_PWM_CPRD3 = PWM_CPRD_CPRD(250);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY3 = PWM_CDTY_CDTY(default_speed); //default value

    //PWM Channel 4 - Pin 9 = PC21 (PWM4L)
    REG_PWM_CPRD4 = PWM_CPRD_CPRD(250);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY4 = PWM_CDTY_CDTY(default_speed);

    //PWM Channel 5 - Pin 8 = PC22 (PWML5)
    REG_PWM_CMR5 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
    REG_PWM_CPRD5 = PWM_CPRD_CPRD(250);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY5 = PWM_CDTY_CDTY(default_speed); //default value

    //PWM Channel 6 - Pin 7 = PC23 (PWML6)
    REG_PWM_CMR6 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
    REG_PWM_CPRD6 = PWM_CPRD_CPRD(250);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY6 = PWM_CDTY_CDTY(default_speed); //default value

    //PWM Channel 7 - Pin 6 = PC24 (PWML7)
    REG_PWM_CMR7 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
    REG_PWM_CPRD7 = PWM_CPRD_CPRD(250);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY7 = PWM_CDTY_CDTY(default_speed); //default value

    PWM -> PWM_ENA = PWM_ENA_CHID0 | PWM_ENA_CHID1 | PWM_ENA_CHID2 | PWM_ENA_CHID3 | PWM_ENA_CHID4 | PWM_ENA_CHID5 | PWM_ENA_CHID6 | PWM_ENA_CHID7;

}

void PWM_EN()
{
  PWM -> PWM_ENA |= PWM_ENA_CHID4;
  
}

void PWM_DIS()
{
  PWM -> PWM_DIS |= PWM_DIS_CHID4;
}

void CALC_Speed()
{
  Serial.println("Fetching temperature");
  double temp = 0;
  while(RX_STRING_READY == 0)
  {
    temp = READ_double(str);
  }
  Serial.print("\nTemperature: ");
  Serial.print(temp);
  Serial.println(" °C");
  Air_Speed = 331.5 +0.6*temp;
  Air_Speed *= 100; //m/s to cm/s conversion
  Serial.print("AirSpeed: ");
  Serial.print(Air_Speed);
  Serial.println(" cm/s");
  Serial.println();
}

int MotorControl(int PulseDetect, double distance, int SEC_STATE)
{
  double temp_speed = 250;
  int speed = 250;
  int sec0_max = 150;
  int sec1_max = 250;
  int sec2_max = 350;
  
  
  switch(SEC_STATE)
  {
    case 0:
    {
      if(distance < sec0_max)
      {
        
        if(distance < 50)
        {
          speed = 0;
        }
        else
        {
          temp_speed = (double) 2.5*distance - 125;
          speed = (int) temp_speed;
        }
        
        if(PulseDetect == 49)
        {
          PWM -> PWM_ENA = PWM_ENA_CHID3;
          REG_PWM_CDTYUPD3 = PWM_CDTYUPD_CDTYUPD(speed);
        }
        else if( PulseDetect == 47)
        {
          PWM -> PWM_ENA = PWM_ENA_CHID4;
          REG_PWM_CDTYUPD4 = PWM_CDTYUPD_CDTYUPD(speed);
        }
        SEC_STATE = 1;
      }
      
      else if(distance < sec1_max)
      {
        temp_speed = (double) 2.5*distance - 375;
        speed = (int) temp_speed;

        if(PulseDetect == 49)
        {
          REG_PWM_CDTYUPD3 = PWM_CDTYUPD_CDTYUPD(250);
          PWM -> PWM_ENA = PWM_ENA_CHID2;
          REG_PWM_CDTYUPD2 = PWM_CDTYUPD_CDTYUPD(speed);
        }
        else if( PulseDetect == 47)
        {
          REG_PWM_CDTYUPD4 = PWM_CDTYUPD_CDTYUPD(250);
          PWM -> PWM_ENA = PWM_ENA_CHID5;
          REG_PWM_CDTYUPD5 = PWM_CDTYUPD_CDTYUPD(speed);
        }
        SEC_STATE = 2;
      }
      else if(distance <= sec2_max)
      {
        
        temp_speed = (double) 2.5*distance - 625;
        speed = (int) temp_speed;
        
        if(PulseDetect == 49)
        {
          REG_PWM_CDTYUPD3 = PWM_CDTYUPD_CDTYUPD(250);
          REG_PWM_CDTYUPD2 = PWM_CDTYUPD_CDTYUPD(250);
          PWM -> PWM_ENA = PWM_ENA_CHID1;
          REG_PWM_CDTYUPD1 = PWM_CDTYUPD_CDTYUPD(speed);            
        }
        else if( PulseDetect == 47)
        {
          REG_PWM_CDTYUPD4 = PWM_CDTYUPD_CDTYUPD(250);
          REG_PWM_CDTYUPD5 = PWM_CDTYUPD_CDTYUPD(250);
          PWM -> PWM_ENA = PWM_ENA_CHID6;
          REG_PWM_CDTYUPD6 = PWM_CDTYUPD_CDTYUPD(speed);
        }
        SEC_STATE = 3;
      }
      else if(distance > sec2_max)
      { 
        temp_speed = (double) 2.5*distance - 875;
        speed = (int) temp_speed;

        if(PulseDetect == 49)
        {
          REG_PWM_CDTYUPD3 = PWM_CDTYUPD_CDTYUPD(250);
          REG_PWM_CDTYUPD2 = PWM_CDTYUPD_CDTYUPD(250);
          REG_PWM_CDTYUPD1 = PWM_CDTYUPD_CDTYUPD(250);
          PWM -> PWM_ENA = PWM_ENA_CHID0;
          REG_PWM_CDTYUPD0 = PWM_CDTYUPD_CDTYUPD(speed);              
        }
        else if( PulseDetect == 47)
        {
          REG_PWM_CDTYUPD4 = PWM_CDTYUPD_CDTYUPD(250);
          REG_PWM_CDTYUPD5 = PWM_CDTYUPD_CDTYUPD(250);
          REG_PWM_CDTYUPD6 = PWM_CDTYUPD_CDTYUPD(250);
          PWM -> PWM_ENA = PWM_ENA_CHID7;
          REG_PWM_CDTYUPD7 = PWM_CDTYUPD_CDTYUPD(speed);
        }
        SEC_STATE = 4;
      }
      else
      {
        SEC_STATE = 4;
      }
      break;
    }
    
    case 1:
    { 
      if(distance < sec0_max)
      {
        //no action section already used
        SEC_STATE = 1;
      }
      
      else if(distance < sec1_max)
      {
        temp_speed = (double) 2.5*distance - 375;
        speed = (int) temp_speed;
        
        if(PulseDetect == 49)
        {
          PWM -> PWM_ENA = PWM_ENA_CHID2;
          REG_PWM_CDTYUPD2 = PWM_CDTYUPD_CDTYUPD(speed);              
        }
        else if( PulseDetect == 47)
        {
          PWM -> PWM_ENA = PWM_ENA_CHID5;
          REG_PWM_CDTYUPD5 = PWM_CDTYUPD_CDTYUPD(speed);              
        }
        SEC_STATE = 2;
      }
      else if(distance < sec2_max)
      {
        temp_speed = (double) 2.5*distance - 625;
        speed = (int) temp_speed;

        if(PulseDetect == 49)
        {
          REG_PWM_CDTYUPD2 = PWM_CDTYUPD_CDTYUPD(250);
          PWM -> PWM_ENA = PWM_ENA_CHID1;
          REG_PWM_CDTYUPD1 = PWM_CDTYUPD_CDTYUPD(speed);              
        }
        else if( PulseDetect == 47)
        {
          REG_PWM_CDTYUPD5 = PWM_CDTYUPD_CDTYUPD(250);
          PWM -> PWM_ENA = PWM_ENA_CHID6;
          REG_PWM_CDTYUPD6 = PWM_CDTYUPD_CDTYUPD(speed);              
        }
        SEC_STATE = 3;
      }
      else if(distance > sec2_max)
      { 
        temp_speed = (double) 2.5*distance - 875;
        speed = (int) temp_speed;

        if(PulseDetect == 49)
        {
          REG_PWM_CDTYUPD2 = PWM_CDTYUPD_CDTYUPD(250);
          REG_PWM_CDTYUPD1 = PWM_CDTYUPD_CDTYUPD(250);
          PWM -> PWM_ENA = PWM_ENA_CHID0;
          REG_PWM_CDTYUPD0 = PWM_CDTYUPD_CDTYUPD(speed);              
        }
        else if(PulseDetect == 47)
        {
          REG_PWM_CDTYUPD5 = PWM_CDTYUPD_CDTYUPD(250);
          REG_PWM_CDTYUPD6 = PWM_CDTYUPD_CDTYUPD(250);
          PWM -> PWM_ENA = PWM_ENA_CHID7;
          REG_PWM_CDTYUPD7 = PWM_CDTYUPD_CDTYUPD(speed);            
        }
        SEC_STATE = 4;
      }

      else
      {
        SEC_STATE = 4;
      }

      break;
    }

    case 2:
    {
      if(distance < sec1_max)
      {
        //no action section already used
        SEC_STATE = 2;
      }

      if(distance < sec2_max)
      {
        temp_speed = (double) 2.5*distance - 625;
        speed = (int) temp_speed;
        if(PulseDetect == 49)
        {
          PWM -> PWM_ENA = PWM_ENA_CHID1;
          REG_PWM_CDTYUPD1 = PWM_CDTYUPD_CDTYUPD(speed);              
        }
        else if( PulseDetect == 47)
        {
          PWM -> PWM_ENA = PWM_ENA_CHID6;
          REG_PWM_CDTYUPD6 = PWM_CDTYUPD_CDTYUPD(speed);              
        }
        SEC_STATE = 3;
      }
      else if(distance > sec2_max)
      { 
        temp_speed = (double) 2.5*distance - 875;
        speed = (int) temp_speed;
        if(PulseDetect == 49)
        {
          REG_PWM_CDTYUPD1 = PWM_CDTYUPD_CDTYUPD(250);
          PWM -> PWM_ENA = PWM_ENA_CHID0;
          REG_PWM_CDTYUPD0 = PWM_CDTYUPD_CDTYUPD(speed);              
        }
        else if( PulseDetect == 47)
        {
          REG_PWM_CDTYUPD6 = PWM_CDTYUPD_CDTYUPD(250);
          PWM -> PWM_ENA = PWM_ENA_CHID7;
          REG_PWM_CDTYUPD7 = PWM_CDTYUPD_CDTYUPD(speed);              
        }
        SEC_STATE = 4;
      }

      else
      {
        SEC_STATE = 4;
      }
      break;
    }

    case 3:
    {
      if(distance < sec2_max)
      {
        //no action section already used
        SEC_STATE = 3;
      }
      
      if(distance > sec2_max)
      { 
        temp_speed = (double) 2.5*distance - 875;
        speed = (int) temp_speed;
        if(PulseDetect == 49)
        {
          PWM -> PWM_ENA = PWM_ENA_CHID0;
          REG_PWM_CDTYUPD0 = PWM_CDTYUPD_CDTYUPD(speed);              
        }
        else if( PulseDetect == 47)
        {
          PWM -> PWM_ENA = PWM_ENA_CHID7;
          REG_PWM_CDTYUPD7 = PWM_CDTYUPD_CDTYUPD(speed);
        }
        SEC_STATE = 4;
      }
      else
      {
        if(PulseDetect == 49)
        {
          REG_PWM_CDTYUPD0 = PWM_CDTYUPD_CDTYUPD(250);
        }
        else if( PulseDetect == 47)
        {
          REG_PWM_CDTYUPD7 = PWM_CDTYUPD_CDTYUPD(250);
        }
        SEC_STATE = 4;
      }
      break;
    }

    default:
    {
      //nothing
      break;
    }

    }
    //--------------------------------------------------------------------------------------------
      
  
  return SEC_STATE;
}

void MotorOff()
{
  PWM -> PWM_DIS = PWM_DIS_CHID0 | PWM_DIS_CHID1 | PWM_DIS_CHID2 | PWM_DIS_CHID3 | PWM_DIS_CHID4 | PWM_DIS_CHID5 | PWM_DIS_CHID6 | PWM_DIS_CHID7;
}
