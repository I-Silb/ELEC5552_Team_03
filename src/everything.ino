const byte OC1A_PIN = 9;
const byte OC1B_PIN = 10;
const word PWM_FREQ_HZ = 2000; //Adjust this value to adjust the frequency
const word TCNT1_TOP = 16000000/(2*PWM_FREQ_HZ);
word voltage;
char modulation;
word dutycycle_buck;
word dutycycle_boost;
const int pwmPin = 3;
char command;


void setup() {  
  Serial.begin(9600);
  const int analogPin = A0; //analogue input for AC signal interlock
  pinMode(OC1A_PIN, OUTPUT);//PWM outout
  pinMode(10, OUTPUT);//first switch buck
  pinMode(11, OUTPUT);//second switch buck
  pinMode(8, OUTPUT);//first switch boost
  pinMode(7, OUTPUT);//second switch boost
  pinMode(6, OUTPUT);//first switch AC function
  pinMode(5, OUTPUT);//second switch AC function
  pinMode(4, OUTPUT);//DC interlock
  pinMode(2, OUTPUT);//AC interlock
  
//  pinMode(OC1B_PIN, OUTPUT);
  // Clear Timer1 control and count registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  // Set Timer1 to fast PWM mode, non-inverting
  // Set prescaler to 1024
  TCCR1A |= (1 << COM1A1) | (1 << WGM11);
  TCCR1B |= (1 << WGM13) | (1 << CS10);
  ICR1 = TCNT1_TOP;
  // Set Timer2 to fast PWM mode, non-inverting
  // Set prescaler to 1024
  TCCR2A = (1 << COM2A1) | (1 << WGM21) | (1 << WGM20);
  TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);
  // Set a 50% duty cycle as an example
  analogWrite(pwmPin, 128); // 50% duty cycle
}

void loop() {
if (Serial.available() > 0) { // any bytes in coming? If so:
 command = Serial.read(); // Read the user input

   switch (command) {
     case 'A':
     digitalWrite(8,LOW);
     digitalWrite(7,LOW);
     delay(3000);
      digitalWrite(10,HIGH);
      Serial.println("You chose option Buck.What is the voltage you want");
      delay(1000);//charge the capacitor
      voltage = Serial.read();
      dutycycle_buck=30/voltage;//linkage between voltage and duty cycle
      setPwmDuty(dutycycle_buck);
      delay(1000);
      digitalWrite(11,HIGH);
      break;

      
     case 'B':
     digitalWrite(10,LOW);
     digitalWrite(11,LOW);
     Serial.println("You chose option Boost.What is the voltage you want");
     voltage = Serial.read();
     if (voltage>100){
      Serial.println("do you want DC interlock, Y for yes, N for no");
      char decision=Serial.read();
      if(decision == 'Y'){
        digitalWrite(4,HIGH);//enable DC interlock
      }else{
        digitalWrite(4,LOW);//disable DC interlock
      }
     }
     delay(3000);
     digitalWrite(8,HIGH);
     delay(1000);//charge the capacitor
     dutycycle_boost=1-30/voltage;//linkage between voltage and duty cycle
     
     setPwmDuty(dutycycle_boost);
     delay(1000);
     digitalWrite(7,HIGH);
       break;

     case 'C':
     digitalWrite(7,LOW);
     digitalWrite(8,LOW);
     digitalWrite(10,LOW);
     digitalWrite(11,LOW);
      Serial.println("You chose option AC mode.What is the voltage you want");
      voltage= analogRead(A0);//read the AC voltage
      if(voltage>50){
        Serial.println("do you want AC interlock, Y for yes, N for no");
       char decision=Serial.read();
       if(decision=='Y'){
        digitalWrite(2,HIGH);//enable AC interlock
      }else{
        digitalWrite(2,LOW);//disable AC interlock
      }
      }
      Serial.println("do you want AC modulation, Y for yes, N for no");
      modulation=Serial.read();
       if(modulation=='Y'){
        Serial.println("enter the modulating frequency:");
        int frequency=Serial.read();
        setPWMFrequency(frequency);//enable AC modulation
      }else{
        setPWMFrequency(0);//disable AC modulation
      }
       break;

     default:

      Serial.println("Invalid command. Please enter A, B, C.");

   }
    

    while (Serial.available()) Serial.read(); // clear out any remaining bytes in the incoming buffer.
  }

}

void setPwmDuty(byte duty) {
  OCR1A = (word) (duty*TCNT1_TOP)/100;
}

//set the compare resister linkage with the frequency modulation
void setPWMFrequency(int freq) {
  if (freq <= 0) freq = 1;
  if (freq > 100) freq = 100;

  // Calculate top value based on desired frequency
  int topValue = (16000000L / (1024L * freq)) - 1;
  
  // Set the top value. Since Timer2 is 8-bit, we need to ensure the value is between 0 and 255.
  if (topValue > 255) topValue = 255;
  if (topValue < 0) topValue = 0;
  
  OCR2A = topValue;
}



