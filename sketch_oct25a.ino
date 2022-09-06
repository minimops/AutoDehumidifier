#include "virtuabotixRTC.h"
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

virtuabotixRTC myRTC(6, 7, 8);

const int ledPin = 3;
const int transPin = 4;
const int onPin = A2;

static int maxRunTime = 3 * 60;
static int minRunTime = 3;

static int humUpperBound = 55;
static int humLowerBound = 50;

boolean onState = false;
int currentHumidity;

// time
long startTime;

void setup() {
  
 Serial.begin(9600);

 dht.begin();

 // myRTC.setDS1302Time(0, 50, 10, 1, 11, 4, 2022);

 pinMode(ledPin, OUTPUT);
 pinMode(transPin, OUTPUT);

 light_test();
 beep();

 // fix for automatic turn on
 delay(3000);
    if(checkOn()){
      onState = true;
      change_state(0);
    }
 delay(5000);
}

void loop() {

   myRTC.updateTime();
 
// Start printing elements as individuals
 Serial.print("Current Date / Time: ");
 Serial.print(myRTC.dayofmonth); //You can switch between day and month if you're using American system
 Serial.print("/");
 Serial.print(myRTC.month);
 Serial.print("/");
 Serial.print(myRTC.year);
 Serial.print(" ");
 Serial.print(myRTC.hours);
 Serial.print(":");
 Serial.print(myRTC.minutes);
 Serial.print(":");
 Serial.println(myRTC.seconds);

  Serial.println(onState);
  
  boolean isOn = checkOn();
  Serial.println(isOn);
  if (onState != isOn) {
    if (isOn) {
      onState = 1;
      change_state(0);
    } else {
      error_loop('u');
    }
  }

  check_full();
  currentHumidity = measure_humidity();

  Serial.print(F(" Humidity: "));
  Serial.println(currentHumidity);

  if (myRTC.hours < 8) {
    Serial.println("in for loop");
    if (onState) change_state(0);
  }
  else {
    Serial.println("in else loop");
    if (onState) {
      int currentRT = calc_runtime(myRTC.hours, myRTC.minutes);
      Serial.println(startTime);
      Serial.println(currentRT);
     
      if (currentHumidity < humLowerBound && currentRT > minRunTime) {
        change_state(0);  
      }
      if (currentRT > maxRunTime) {
        change_state(0);
        error_loop('r');
      }
    }
    else {
      if (currentHumidity > humUpperBound) change_state(1);
    }
  }
  delay(5000);
}

void light_test() {
  // blink led
}

void beep() {
  // make noise
}

void check_full() {
  // if(fullPin == HIGH) error_loop('f');
}

// returns a smoothed humidity reading
double measure_humidity() {
  int measureNum = 10;
  int humiditySum = 0;

  Serial.println(dht.readHumidity());

  for (byte i = 0; i < measureNum; i++) {
    float h = dht.readHumidity();
    if (isnan(h)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    humiditySum += h; // get humidity
    delay(80);
  }
  return(humiditySum / measureNum);
}

// turn on or off
void change_state(int state) {

  if (state != onState) {
    // set pin high
    Serial.println("High");
    digitalWrite(transPin, HIGH);
    delay(500);
    // set pin low
    Serial.println("Low");
    digitalWrite(transPin, LOW);

    onState = state;
    if (state) {
      digitalWrite(ledPin, HIGH);
      startTime = myRTC.hours * 60 + myRTC.minutes;
    }
    else {
      digitalWrite(ledPin, LOW);
    }
  }
  delay(1000);
}

// calculates runtime of current event
long calc_runtime(int cHour, int cMinute) {
  return((cHour * 60 + cMinute) - startTime);
}

// infinite error loop with coded led lighting
void error_loop(char code) {

  // change_state(0);
  int intervalBlinks[3] = {0,0,0};

   switch(code) {
    case 'r':
      intervalBlinks[0] = 1;
      intervalBlinks[1] = 1;
      break;
    case 'u':
      intervalBlinks[0] = 1;
      break;
    default:
      break;
   }

   while(true) {
    for (byte bl = 0; bl < sizeof(intervalBlinks); bl ++){
      // blink if intervalblinks[index] is 1
      if (intervalBlinks[bl] = 1) {
        digitalWrite(ledPin, HIGH);
        delay(500);
        digitalWrite(ledPin, LOW);
      }
      // beep
      delay(500);
    }
   }
}

boolean checkOn() {
    long collect = 0;
  for(byte i =0; i < 30; i++) {
    collect += analogRead(onPin);
    delay(10);
  }
  Serial.println((collect / 30));
  return((collect / 30) > 100);
}
