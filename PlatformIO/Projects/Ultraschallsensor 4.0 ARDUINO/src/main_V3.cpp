#include <Arduino.h>
#include <stdlib.h>
//#include <pwm_lib.h>

#define maxloops 20000
#define maxloops2 500
#define len 20 

//Function prototypes
void measure();
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

  pinMode(49, INPUT);  //Pulse detection
  pinMode(46, OUTPUT); //Transmitter start
  PWM_Setup(); 
  Serial.begin(9600);

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
      
      measure();
      
      if(Serial.available() > 0)
      {
        char dump = Serial.read();
        STATE_VARIABLE = 6;
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
      //Serial.print("Duration:, ");
      //Serial.println(time_ms);
      
      
      if(time_ms > 0)
      {
        long delta = 0;
        long start = millis();

        while(delta <= time_ms)
        {
          measure();
          delta = millis()-start;
        }
        
        Serial.println("Measurement ended.");
        STATE_VARIABLE = 4;
      }

      break;
    }

    case 3:
    {
      Serial.println("Ending programm.");
      while(1)
      STATE_VARIABLE = 3;
      break;
    } 

    case 4:
    {
      char choice;
      Serial.println("Restart measurement? Y/N");
      
      while((choice != 'y') && (choice != 'n'))
      {
        choice = READ_Char();
      }
      
      if(choice == 'y')
      {
        STATE_VARIABLE = 5;
      }

      if(choice == 'n')
      {
        STATE_VARIABLE = 6;
      }

      break;
    }

    case 5:
    {
      Serial.println("Restarting previous measurement.");

      long delta = 0;
      long start = millis();

      while(delta <= time_ms)
      {
        measure();
        delta = millis()-start;
      }
        
      Serial.println("Measurement ended.");
      STATE_VARIABLE = 4;

      break;
    }

    case 6:
    {
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
        STATE_VARIABLE = 3;
      }

      else
      {
        Serial.println("Invalid input");
        STATE_VARIABLE = 6;
      }
      
      break;
    }

    default:
    {
      STATE_VARIABLE = 0;
      break;
    }


  }
}

 
/* void loop()
{
  measure();
}*/


void home_menu()
{
  Serial.println("Hello!");
  Serial.println("Please choose your next action...");
  Serial.println("1: continuos measurement");
  Serial.println("2: measurement interval");
  Serial.println("3: End programm");
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

void measure()
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
  
  //Start measurement
  PWM_EN();
  start = micros();               //begin of measurement
  delayMicroseconds(150);         //wait to send a pulse chain
  PWM_DIS();
  delayMicroseconds(2500);        //Wait for Transmitter interfernce to decay
  //End of period
  
  while((i == 0) &&  (loops < maxloops))       //Wait for comparator to sense reflected signal
  { 
    i = digitalRead(49);          //Check if comparator Output is high
    loops++;
    //Wait for response
  }

  duration = micros()-start;     //Calculate duration for reflected signal to reach receiver

  start2 = micros();             //time reference for pulse duration (refers to the pulses created by objects)
  
  //find max. amplitude in the reflected signal 
  while((end >= 0.5*adc1) && (loops2 < maxloops2))       //The factor 0.6 was experimentally determined
  {
    adc1 = adc0;                 //Store last conversion result for comparison

    adc0 = analogRead(A0);       //Load ADC value                   
    end = adc0;                  //Load last stored value to check for the pulse end

    if(adc0 < adc1)              //Check if max is reached
    {
      adc0 = adc1;
    }

     loops2++;                    //infinite loop stopper 
  }
  
  length = micros()-start2;       //Calculate pulse duration

  if(loops < maxloops && loops2 < maxloops2)
  {
    //Calculations and output
    distance = (double) (0.5*Air_Speed*duration)/1000000;
    height = (double) adc0*33/10240;
  
    Serial.print(time_ms);
    Serial.print(CSV_SEPARATOR);
    Serial.print(height);
    Serial.print(CSV_SEPARATOR);
    Serial.print(distance);
    Serial.print(CSV_SEPARATOR);
    Serial.println(length);
  }
  
  else
  {
    Serial.println("Out of range! Data discarded.");
  }
  
  //Slow down the process
  delay(100);

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
    //Ports
    PIOC -> PIO_PDR = PIO_PDR_P21; //Enable peripheral control of PC21

    //PWM Setup Use PWM 9 on Arduino pin 9
    PMC -> PMC_WPMR = PMC_WPMR_WPKEY(0x504D43) | ~PMC_WPMR_WPEN; //Enable PMC write access
    PMC -> PMC_PCER1 = PMC_PCER1_PID36; //Enable PWM peripheral clock
    PMC -> PMC_WPMR = PMC_WPMR_WPKEY(0x504D43) | PMC_WPMR_WPEN; //Disable PMC write access
    PIOC -> PIO_ABSR = PIO_ABSR_P21; //Select peripheral type B which is PWM Output

    PWM -> PWM_WPCR = PWM_WPCR_WPKEY(0x50574D) | 0x00; //Enable write access on all groups
    
    PWM -> PWM_CLK = PWM_CLK_PREA(1); //PREA_DIV  = 2
    REG_PWM_CMR4 = PWM_CMPM_CPR(11); //Select clock A for PWM channel 4, left aligned 
    REG_PWM_CPRD4 = PWM_CPRD_CPRD(0x41A);   //Set PWM period divider to 1050. 84Mhz / (2*1050) = 40 kHz
    REG_PWM_CDTY4 = PWM_CDTY_CDTY(0x20D);
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
  //Serial.println(); //if this line isnt present Serial.print will send a weird combination of the actual string
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