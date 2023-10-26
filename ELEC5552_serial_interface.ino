#include <HardwareSerial.h>
#include <ctype.h>
#include <AutoPID.h>
#define OUTPUT_MIN 0
#define OUTPUT_MAX 255
#define KP 0.12
#define KI 0.8
#define KD 0.003
double analogReadValue, setPoint, outputVal;
AutoPID myPID(&analogReadValue, &setPoint, &outputVal, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);
float DC_voltage_output;
float AC_voltage_output;
int AC_frequency;
bool HW_interlock_en[3];
bool Output_enable[3];
byte AM_sw_freq;
int serial_timeout = 5;
volatile int serial_countdown = 0;

void setup(){
  myPID.setBangBang(0.01);
  myPID.setTimeStep(10);
  Serial.begin(9600);
  Serial.println("Waiting for serial receive");
  volatile bool serial_connect = false;
  while (serial_connect == false){
  serial_connect = check_serial_connection(serial_timeout);
}
    Serial.print("\33[2J");
    Serial.print("\33[f");
    print_main_interface();
}

void loop(){
  if(Serial.available()>0){
    
  }
}

bool check_serial_connection(int serial_timeout){
    if(Serial.available()> 0){
      serial_received();
return true;
    }
    if (serial_timeout-serial_countdown>1){
    Serial.print("Checking for serial device. Timeout in ");
    Serial.print(serial_timeout-serial_countdown);
    Serial.print(" seconds.\r");
    serial_countdown++;
    delay(1000);
    Serial.print("\33[2K");
    return false;}
    if (serial_timeout-serial_countdown == 1){
    Serial.print("Checking for serial device. Timeout in 1 second.\r");
    serial_countdown++;
    delay(1000);
    Serial.print("\33[2K");
    return false;}
    
    Serial.println("Checking for serial device. Timeout in 0 seconds.");
    delay(1000);
    Serial.println("No serial device found. Trying again...");
    delay(1000);
    serial_countdown = 0;
    Serial.print("\33[2J");
    Serial.print("\33[f");
    return false;}

void print_main_interface(){
  Serial.println("===================================================================");
  Serial.println("ANFF MEMS Testing Power Supply Unit Serial Interface");
  Serial.print("\33[3;20H");
  Serial.print("| OE | HW_EN |");
  Serial.print("| Set value |");
  Serial.print("| Measured value |\r\n");
  Serial.println("DC output voltage  | \33[44m \33[49m      \33[44m \33[49m       \33[44m       \33[49mV");
  Serial.println("AC output voltage  | \33[44m \33[49m      \33[44m \33[49m       \33[44m       \33[49mV");
  Serial.println("AM output voltage  | \33[44m \33[49m      \33[44m \33[49m");
  Serial.print("\33[4;33H");
  Serial.print("||\33[2D\n");
  Serial.print("||\33[2D\n");
  Serial.print("||###############\n");
  Serial.print("\33[4;46H");
  Serial.print("||\33[2D\n");
  Serial.print("||\33[2D\n");
  Serial.print("||################\n");
  Serial.print("\33[4;64H");
  Serial.print("|\33[D\n");
  Serial.print("|\33[D\n");
  Serial.print("|");
  Serial.print("\33[4;25H");
  Serial.print("|\33[D\n");
  Serial.print("|\33[D\n");
  Serial.print("|");
  Serial.print("\r\n");
  Serial.println("===================================================================");
  Serial.println("Modulated switching frequency (Hz): \33[44m   \33[49m");
  Serial.println("Modulated switching input (DC/AC): \33[44m  \33[49m");
  Serial.println("Current protection threshold (mA): \33[44m    \33[49m");
  Serial.println("AC frequency (kHz): \33[44m   \33[49m");
  Serial.print("===================================================================");
}

void serial_received(){
  if (Serial.available()>0){
          Serial.print("\33[2K");
  Serial.print("Serial connection found! Continuing...");
      delay(1000);

  }
}

void Draw_Results(){
  setPoint = analogRead(A3);         
  analogReadValue = analogRead(A1);   
  myPID.run();                        
  analogWrite(6, outputVal);
  Serial.print("\33[\33[4;51H");
  Serial.print(outputVal,3);
  Serial.print("V");
  //DC PID read
  setPoint = analogRead(A4);         
  analogReadValue = analogRead(A2);   
  myPID.run();                        
  analogWrite(6, outputVal);
  Serial.print("\33[\33[5;51H");
  Serial.print(outputVal,3);
  Serial.print("V");

}