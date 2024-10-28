#include <Sipeed_ST7789.h>
#include<BH1750.h>
#include<Wire.h>
#include "INA219.h"
#include <DHT11.h>
DHT11 dht11(3);

INA219 INA(0x41);
BH1750 lightMeter;
unsigned long start_time = 0;
unsigned long end_time = 0;
int steps=0;
float steps_old=0;
float temp=0;
float RPS=0;
float RPM = 0;
int counter = 2; //opto interruptor pin 
unsigned long millisBefore;
volatile int slotcount;

int temperature = 0;
int humidity = 0;

SPIClass spi_(SPI0); // MUST be SPI0 for Maix series on board LCD
Sipeed_ST7789 lcd(320, 240, spi_);
//L298N pin connections
int PWM_EN = 6; //Connected to enable pin for L298N module
int M1 = 8; // Connected to IN1 of L298N module
int M2 = 9; // Connected to IN2 of L298N module
int SW = 7; //Switch to turn on motor

//funtion protypes
void title();
void motorinit();
void motoroff();
void motoronforward();
void motoronbackward();
void RPM_speed(int a);
void readlight();
void readrpm();
void readINA();
void temphumi();


void setup()
{
    Wire.begin();
    Serial.begin(115200);
    motorinit();
    motoroff(); 
    pinMode(counter, INPUT_PULLUP);
    //pinMode(sensor,INPUT_PULLUP);
    //pinMode(SW, INPUT);
  //  attachInterrupt(digitalPinToInterrupt(counter), count, RISING);
    lightMeter.begin();
    if (!INA.begin() )
    {
        Serial.println("Could not connect to INA219. Fix and Reboot");
    }
    INA.setMaxCurrentShunt(5, 0.002);
    delay(1000);
    lcd.begin(15000000, COLOR_BLUE);    
    lcd.setRotation(0);
 
}

void loop() {
  title();
  readlight();  
  temphumi();
  
  Serial.println("Duty Cycle 100%");
  RPM_speed(255);
  motoronforward();
  readINA();
  readrpm();
  motoroff();
  delay(1000);

  Serial.println("Duty Cycle 50%");
  RPM_speed(127);
  motoronforward();
  readINA();
  readrpm();
  motoroff();
  delay(1000);

  Serial.println("Duty Cycle 25%");
  RPM_speed(63);
  motoronforward();
  readINA();
  readrpm();
  motoroff();
  delay(1000);
}



void motorinit()
{
  // Set all the motor control pins to outputs
	pinMode(PWM_EN, OUTPUT);
	pinMode(M1, OUTPUT);
	pinMode(M2, OUTPUT);
}
void motoroff()
{	
	// Turn off motors
	digitalWrite(M1, LOW);
	digitalWrite(M2, LOW);
}
void motoronforward()
{
  // Turn ON motors Forward rotation
	digitalWrite(M1, HIGH);
	digitalWrite(M2, LOW);
}
void motoronbackward()
{
  // Turn ON motors backward rotation
	digitalWrite(M1, LOW);
	digitalWrite(M2, HIGH);
}
void RPM_speed(int a)
{
  analogWrite(PWM_EN, a);
}
void readrpm()
{
  //lcd.print("RPM m/s: ");
  start_time=millis();
  end_time=start_time+1000;
  while(millis()<end_time)
  {
    if(digitalRead(counter))
    {
      steps=steps+1; 
      while(digitalRead(counter));
   }
  }
  temp=steps-steps_old;
  steps_old=steps;
  RPS=(temp/30); //change yoursRevolution encoder slot here Example: 30
  RPM=(RPS*60);
  Serial.print("RPM:");
  Serial.println(RPM);
  lcd.setCursor(1,190);
  lcd.print("Motor RPM:");
  lcd.setCursor(180,190);
  lcd.println(RPM);
}
void readlight()
{
  float lux = lightMeter.readLightLevel();
  lcd.setCursor(1,70);
  lcd.print("Light lx: ");
  lcd.setCursor(180,70);
  lcd.println(lux);
  //lcd.println(" lx");

}
void title()
{
    lcd.fillScreen(COLOR_BLUE);
    lcd.setTextSize(2);
    lcd.setTextColor(COLOR_WHITE);
    lcd.setCursor(60,10);
    lcd.println("Sipeed MAIXDUINO");
    lcd.setCursor(60,40);
    lcd.println("IIoT test bed");
}
void readINA()
{
  lcd.setCursor(1,130);
  lcd.print("Shunt mV: ");
  lcd.setCursor(180,130);
  lcd.print(INA.getShuntVoltage_mV(), 2);
  lcd.setCursor(1,150);
  lcd.print("Current mA");
  lcd.setCursor(180,150);
  lcd.print(INA.getCurrent_mA(), 2);
  lcd.setCursor(1,170);
  lcd.print("Power mW:");
  lcd.setCursor(180,170);
  lcd.print(INA.getPower_mW(), 2);
  //delay(500);
  Serial.print(INA.getShuntVoltage_mV(), 2);
  Serial.print("\t\t");
  Serial.print(INA.getCurrent_mA(), 2);
  Serial.print("\t\t");
  Serial.print(INA.getPower_mW(), 2);
  Serial.println("\t\t");
  //delay(500);
}
void temphumi()
{
      // Attempt to read the temperature and humidity values from the DHT11 sensor.
    int result = dht11.readTemperatureHumidity(temperature, humidity);

    // Check the results of the readings.
    // If the reading is successful, print the temperature and humidity values.
    // If there are errors, print the appropriate error messages.
    if (result == 0) {
        Serial.print("Temperature °C: ");
        Serial.println(temperature);
        
        Serial.print("Humidity %:");
        Serial.println(humidity);
        lcd.setCursor(1,90);
        lcd.print("Temperature C: ");
        lcd.setCursor(180,90);
        lcd.println(temperature);
        lcd.setCursor(1,110);
        lcd.print("Humidity %: ");
        lcd.setCursor(180,110);
        lcd.println(humidity);

    } else {
        // Print error message based on the error code.
        Serial.println(DHT11::getErrorString(result));
    }
}