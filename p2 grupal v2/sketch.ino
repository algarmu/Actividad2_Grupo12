#include <DHT22.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <string.h>
#include <Servo.h>

//System tick period in ms
#define TICK_PERIOD 100

//DHT22 sensor connected pin
#define DHT22_PIN 12

//LDR measurement pin
#define LDR_PIN A0

//Positioning led output pin
#define LED_PIN 5

//Servo Pins
#define HEATING_SERVO_PIN 3
#define COOLING_SERVO_PIN 4
#define DRYING_SERVO_PIN 2

//Servo definitions
Servo heatingServo;
Servo coolingServo;
Servo dryingServo;

//Temperature actuator parameters
#define MIN_TEMPERATURE 21
#define MAX_TEMPERATURE 27
#define TEMPERATURE_BIAS 2

//Humidity actuator parameters
#define MAX_HUMIDITY 80
#define HUMIDITY_BIAS 10

//Bridhtness actuator parameters
#define MIN_BRIDHTNESS 50
#define BRIDHTNESS_PER_THOUSAND_LUX 10
#define MIN_LUX 6000 

//Servo turning speed (in degrees per second)
#define SERVO_TURNING_PER_SECOND 20

//DHT22 sensor control object instance
DHT22 dht22(DHT22_PIN);

//Display control object instance
#define DISPLAY_HEIGHT 4
#define DISPLAY_WITH 20
LiquidCrystal_I2C lcd(0x27,DISPLAY_WITH,DISPLAY_HEIGHT);

//Custom display character for representing degree, a sun and a moon
#define DEGREE_CHARACTER_ID 0
const uint8_t DEGREE_CHARACTER[8]  = {0x7,0x5,0x7,0x0,0x0,0x0,0x0};

#define SUN_CHARACTER_ID 1
const uint8_t SUN_CHARACTER[8] = {B00100, B10101, B01110, B11111, B11111, B01110, B10101, B00100};

#define MOON_CHARACTER_ID 2
const uint8_t MOON_CHARACTER[8] = {B00011, B00111, B01110, B11100, B11100, B01110, B00111, B00011};

//LDR sensor parameters
const float GAMMA = 0.7;
const float RL10 = 50;

//####################
//### Program Init ###
//####################

//Program init
void setup() {
    //Initialice the display control object instance
    lcd.init();

    //Insert the custom dregree character to the display control object instance
    lcd.createChar(DEGREE_CHARACTER_ID, DEGREE_CHARACTER);
    lcd.createChar(SUN_CHARACTER_ID, SUN_CHARACTER);
    lcd.createChar(MOON_CHARACTER_ID, MOON_CHARACTER);

    //Power on the display backlight
    lcd.backlight();

    //Define as analog input
    pinMode(LDR_PIN, INPUT);

    //Define positioning LED pin as output
    pinMode(LED_PIN, OUTPUT);
    analogWrite(LED_PIN, 0);
    
    //Servo initialization
    dryingServo.attach(DRYING_SERVO_PIN); 
    heatingServo.attach(HEATING_SERVO_PIN); 
    coolingServo.attach(COOLING_SERVO_PIN); 
}

//#################
//### Main Loop ###
//#################
void loop() {
    //Boolean variable storing if last loop sensor reading returned an error
    static bool lastSensorError = false;

    //Integer temperature and humidity readings
    float temperature = dht22.getTemperature();
    float humidity = dht22.getHumidity();

    //If sensor reading error code is no error
    if (dht22.getLastError() == dht22.OK) {
      //If in last loop a reading error occurred
      if(lastSensorError){
        //Clear the error message in the screen
        lcd.clear();
      }

      //Read Lux
      float lux = readLux();

      //Execute actuator functions
      bool humiditySystemStatus = controlHumidity(humidity);
      bool heatingSystemStatus = controlHeating(temperature);
      bool coolingSystemStatus = controlCooling(temperature);
      unsigned int brightnessPowerPercent = controlBrightness(lux);

      //Print the readings on the display
      lcdPrint(temperature, humidity, lux, coolingSystemStatus, heatingSystemStatus, humiditySystemStatus, brightnessPowerPercent);
    //If a reading error ocurred
    } else {
        //If in last loop a reading error didn't occur
        if(!lastSensorError){
            //Clear the last reading values from screen
            lcd.clear();
        }

        //Show sensor error message in screen
        printTitle("ERROR", '!');
        lcdPrint("SENSOR READ", 0, 2);
    }
    
    //Store if in this loop a reading error occurred
    lastSensorError = dht22.getLastError() != dht22.OK;

    //Main tick time (aprox: delay function is not time precise)
    delay(TICK_PERIOD);
}

//###########################
//### Actuator Funnctions ###
//###########################

//Controls the bridhtness actuator
//Returns the percent of power the actuator is emiting
unsigned int controlBrightness(float lux){
  unsigned int powerPercent = 0;

  //If the minimux level has been reached
  if(lux < MIN_LUX){
    //Calculate the power percent
    powerPercent = MIN_BRIDHTNESS + (BRIDHTNESS_PER_THOUSAND_LUX * (-1 * (lux - MIN_LUX) / 1000));

    if(powerPercent > 100){
      powerPercent = 100;
    }

    setPWM_Percent(LED_PIN, powerPercent);
  }
  else{
    powerPercent = 0;
  }

  setPWM_Percent(LED_PIN, powerPercent);

  return powerPercent;
}

//Control drying system (servo)
//Returns if the drying system is on
bool controlHumidity(float humidity){
  static int position = 0;
  static bool positiveTurning = true;
  static bool isOn = false;

  //If the maximun temperature has been reached
  //or the system is in ON state and the (MAX_TEMPERATURE - TEMPERATURE_BIAS) has not been reached yet
  if((humidity > MAX_HUMIDITY) || (isOn && humidity >= (MAX_HUMIDITY - HUMIDITY_BIAS))){
    moveServo(dryingServo, position, positiveTurning);
    isOn = true;
  }
  else{
    isOn = false;
  }

  return isOn;
}

//Control cooling system (servo)
//Returns if the heating system is on
bool controlCooling(int temperature){
  static int position = 0;
  static bool positiveTurning = true;
  static bool isOn = false;

  //If the maximun temperature has been reached
  //or the system is in ON state and the (MAX_TEMPERATURE - TEMPERATURE_BIAS) has not been reached yet
  if((temperature > MAX_TEMPERATURE) || (isOn && temperature >= ((MAX_TEMPERATURE - TEMPERATURE_BIAS)))){
    moveServo(coolingServo, position, positiveTurning);
    isOn = true;
  }
  else{
    isOn = false;
  }

  return isOn;
}

//Control heating system (servo)
//Returns if the heating system is on
bool controlHeating(int temperature){
  static int position = 0;
  static bool positiveTurning = true;
  static bool isOn = false;

  //If the minimun temperature has been reached
  //or the system is in ON state and the (MAX_TEMPERATURE - TEMPERATURE_BIAS) has not been reached yet
  if((temperature < MIN_TEMPERATURE) || (isOn && temperature <= ((MIN_TEMPERATURE + TEMPERATURE_BIAS)))){
    moveServo(heatingServo, position, positiveTurning);
    isOn = true;
  }
  else{
    isOn = false;
  }

  return isOn;
}

//This function moves the servo at a speed controlled by SERVO_TURNING_PER_SECOND
//Returns the servo's next position and if the turning is at positive or negative state
void moveServo(const Servo& servo, int& position, bool& positiveTurning){
  //First we move the servo
  servo.write(position);

  //If we are turning in clockwise direction
  if(positiveTurning){
    //Calculate the new position
    position += SERVO_TURNING_PER_SECOND / (1000 / TICK_PERIOD);

    //If the new position is more than the maximun one
    if(position > 180){
      positiveTurning = false;

      //Calculate the new position
      position -= (2 * (SERVO_TURNING_PER_SECOND / (1000 / TICK_PERIOD)));
    }
  }
  else{
    //Calculate the new position
    position -= SERVO_TURNING_PER_SECOND / (1000 / TICK_PERIOD);

    //If the new position is less than the maximun one
    if(position < 0){
      positiveTurning = true;

      //Calculate the new position
      position += (2 * (SERVO_TURNING_PER_SECOND / (1000 / TICK_PERIOD)));
    }
  }
}

//############################
//### Auxiliary Funnctions ###
//############################

//Read LDR sensor and convert to lux
float readLux(){
  int analogValue = analogRead(LDR_PIN);
  float voltage = analogValue / 1024. * 5;
  float resistance = 2000 * voltage / (1 - voltage / 5);
  return pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));
}

//Sets the PWM pin value to the desired percent
void setPWM_Percent(int pin, unsigned int percent){
  analogWrite(pin, (percent * 255) / 100);
}

//###############################
//### LCD Main Control Method ###
//###############################

//Print sensor readings in screen
void lcdPrint(float temperature, float humidity, float lux, bool coolingOn, bool heatingOn, bool dryingOn, unsigned int lightPercent){
  //Store if we are showing the actuators or the sensors
  static bool showingSensors = true;

  //Store how many times this method has been executed
  static unsigned char counter = 0;

  //Every 5 seconds
  if(counter == (5000 / TICK_PERIOD)){
    counter = 0;
    showingSensors = !showingSensors;
    lcd.clear();
  }

  //If we need to show sensor readings
  if(showingSensors){
    printSensorReadings(temperature, humidity, lux, counter == 0);
  }
  //If we need to show the actuator status
  else{
    printActuatorStatus(coolingOn, heatingOn, dryingOn, (float) lightPercent, counter == 0);
  }

  counter++;
}

//Prints the sensor readigns on the screen
void printSensorReadings(float temperature, float humidity, float lux, bool forcePrint){
  //These are the default (and imposible) values for the sensor readings
  static float lastTemperature = -273.67;
  static float lastHumidity = -100.0;
  static float lastLux = -100.0;

  if(forcePrint || temperature != lastTemperature || humidity != lastHumidity || lux != lastLux){
    //Aux variables for printing in the screen
    int row = 0;
    int column = 0;

    //Clear the screen
    lcd.clear();

    //Print ###SENSORS###
    printTitle("SENSORS", '#');

    //Print temperature as "Temp: XXºC"
    row = 1;
    column = 0;

    column += lcdPrint("Temp: ", column, row);
    column += lcdPrint(temperature, column, row);
    column += lcdPrintCustom(DEGREE_CHARACTER_ID, column, row);
    column += lcdPrint("C", column, row);

    //Update the stored value with the new one
    lastTemperature = temperature;

    //Print humidity as "Humd: XX%"
    row = 2;
    column = 0;

    column += lcdPrint("Humd: ", column, row);
    column += lcdPrint(humidity, column, row);
    column += lcdPrint("%", column, row);

    //Update the stored value with the new one
    lastHumidity = humidity;

    //Print lux as "lux: XXXXXXX"
    row = 3;
    column = 0;

    column += lcdPrint("Lux: ", column, row);
    column += lcdPrint(lux, column, row);

    //Update the stored value with the new one
    lastLux = lux;

    //Print a character indicating if the LDR sensor is reading day or night
    if(lux > 1000.0){
        lcdPrintCustom(SUN_CHARACTER_ID, DISPLAY_WITH -1,1);
    }
    else{
        lcdPrintCustom(MOON_CHARACTER_ID, DISPLAY_WITH -1,1);
    }
  }
}

//Prints the actuator status on the screen
void printActuatorStatus(bool coolingSystemStatus, bool heatingSystemStatus, bool humiditySystemStatus, float lightSystemStatus, bool forcePrint){
  //These are the default (and imposible) values for the actutor status
  static bool lastCoolingSystemStatus = false;
  static bool lastHeatingSystemStatus = false;
  static bool lastHumiditySystemStatus = false;
  static float lastLightSystemStatus = -100.0;

  //If the values had changed
  if(forcePrint || coolingSystemStatus != lastCoolingSystemStatus || heatingSystemStatus != lastHeatingSystemStatus || humiditySystemStatus != lastHumiditySystemStatus || lightSystemStatus != lastLightSystemStatus){
    //Aux variables for printing in the screen
    int row = 0;
    int column = 0;

    //Clear the screen
    lcd.clear();

    //Print title as ###SENSORS###
    printTitle("ACTAUTORS", '#');

    //Print cooling actuator status as: "Cool: ON/OFF"
    row = 1;
    column = 0;

    column += lcdPrint("Cool: ", column, row);
    column += lcdPrint(coolingSystemStatus ? "ON" : "OFF", column, row);

    //Update the stored value with the new one
    lastCoolingSystemStatus = coolingSystemStatus;

    //Print heating actuator status as: "Heat: ON/OFF"
    row = 1;
    column = DISPLAY_WITH /2;

    column += lcdPrint("Heat: ", column, row);
    column += lcdPrint(heatingSystemStatus ? "ON" : "OFF", column, row);

    lastHeatingSystemStatus = heatingSystemStatus;

    //Print drying actuator status as: "Drying: ON/OFF"
    row = 2;
    column = 0;

    column += lcdPrint("Drying: ", column, row);
    column += lcdPrint(humiditySystemStatus ? "ON" : "OFF", column, row);

    //Update the stored value with the new one
    lastHumiditySystemStatus = humiditySystemStatus;

    //Print light actuator status as: "Light: XX%"
    row = 3;
    column = 0;

    column += lcdPrint("Light: ", column, row);
    column += lcdPrint(lightSystemStatus, column, row);
    column += lcdPrint("%", column, row);

    //Update the stored value with the new one
    lastLightSystemStatus = lightSystemStatus;
  }
}


//######################################
//### LCD Screen Auxiliary Functions ###
//######################################

//Prints text in the selected column and row
//Returns number of characters writen
int lcdPrint(const char* text, const int column, const int row){
  lcd.setCursor(column, row);
  lcd.print(text);
  return strlen(text);
}

//Prints a float number (as integer) in the selected column and row
//Returns number of characters writen
int lcdPrint(const float number, const int column, const int row){
  lcd.setCursor(column, row);
  lcd.print(long(number));

  //Calculate float number lenth using an auxiliry string
  char numberStringRepresentation[DISPLAY_WITH +1];
  sprintf(numberStringRepresentation, "%d", long(number));
  return strlen(numberStringRepresentation);
}

//Prints a custom character (defined before) in the selected column and row
//Returns 1 for code cosistency
int lcdPrintCustom(const int characterID, const int column, const int row){
  lcd.setCursor(column, row);
  lcd.write(characterID);
  return 1;
}

//Prints a title whith character at the sides
void printTitle(const char* title, const char character){
  //Calculate the number of tharacters the title needs at the sides
  const unsigned int characterLeght = (DISPLAY_WITH - strlen(title)) / 2;

  //Set the column for printing
  unsigned int column = 0;

  //Initialice a string for creating the characters on the sides for the title
  char titleCharacters[DISPLAY_WITH +1];
  
  //Set the first "characterLeght" to "character" to create a string of them
  for(unsigned int i = 0; i < characterLeght; i++){
    titleCharacters[i] = character;
  }
  
  //Sets the last character as a string termination character
  titleCharacters[characterLeght] = '\0';

  //Print the side characters and the title
  column += lcdPrint(titleCharacters, column, 0);
  column += lcdPrint(title, column, 0);
  column += lcdPrint(titleCharacters, column, 0);

  //If the display with less the title lenght is not even: last character is not printed yet
  if(((DISPLAY_WITH - strlen(title)) % 2) != 0){
    //Create an auxiliary string of "character"
    char auxString[] = {character, '\0'};

    //Print the last character
    lcdPrint(auxString, column, 0);
  }
}