// SERVO
#include <Servo.h>
#include <Wire.h>

// OLED
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
char timeline[16];

// OLED
#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Ultrasonic Sensor
#define echoPin 2 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 3 //attach pin D3 Arduino to pin Trig of HC-SR04
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement

Servo MyservoL;
Servo MyservoR;
int buttonPinL = 7;
int buttonPinR = 9;
int studyMinutes = 00;
int breakMinutes = 00;
int studySeconds = 10;
int breakSeconds = 05;
// 0:strart sceen [button] 1:study -> 2:break -> 3:end time [button] 0
int mode = 0;
int pom = 3;
bool glassUpBool = true;
bool present = true;
int buttonPin = 8;
int buttonStateLast;
int buttonState;
int count;

void setup()
{
  Serial.begin(9600);
  
  //consist button
  pinMode(buttonPin, INPUT);
  buttonState = digitalRead(buttonPin);
  buttonStateLast = buttonState;
  
  MyservoL.attach(11);
  MyservoR.attach(12);

  //Setup timer
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("{o,o} Welcome");
  lcd.setCursor(0, 1);
  lcd.print("/)_)  Hoo-sier");

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  Serial.println("Ultrasonic Sensor HC-SR04 Test"); // print some text in Serial Monitor
  Serial.println("with Arduino UNO R3");

  // OLED initialize OLED display with address 0x3C for 128x64
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }
  defaultFace();
  glassUp();
}

void loop()
{
  if (mode == 0) { //unless in break
    checkDistance();
  }

  if (present == false) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("{o,o} Where Are");
      lcd.setCursor(0, 1);
      lcd.print("/)_)  Are You?");
  }
  
buttonState = digitalRead(buttonPin);
  if(buttonState != buttonStateLast){
    buttonStateLast = buttonState;
    delay(50);  // debouncing

    if(buttonState == HIGH){
      if (mode == 0) {
        mode = 1;
        Serial.println(String("Study Mode ") + mode);
        lcd.clear();
      }
      else if (mode == 3) {
        mode = 0;
        Serial.println(String("Start Screen ") + mode);
        lcd.clear();
      }
    }
  }
 
  //Start Screen. Mode 0
  if(mode == 0)
  {
    lcd.setCursor(0, 0);
    lcd.print("{o,o} Welcome");
    lcd.setCursor(0, 1);
    lcd.print("/)_)  Hoo-sier");
    Serial.println("Mode 0");
  }

  // study
  else if (mode == 1) {
    mode = studyUpdate();
    lcd.setCursor(0, 0);
    lcd.print("Study Timer :");
    Serial.println("Mode 1");
  }
  // break
  if (mode == 2) {
    // timer to 0: smile face
    mode = breakUpdate();
    Serial.println("Mode 2");
  }
  // end screen
  if (mode == 3) {
    Serial.println("Mode 3");
  }
}

void glassDown() {
  if (glassUpBool) {
    MyservoL.write(15);
    MyservoR.write(60);
    glassUpBool = false;
  }
}

void glassUp() {
  if (glassUpBool == false) {
    MyservoL.write(60);
    MyservoR.write(15);
    glassUpBool = true;
  }
}

void reset() {
  studyMinutes = 00;
  breakMinutes = 00;
  studySeconds = 05;
  breakSeconds = 05;
}

int studyUpdate() {
  glassDown();
  lcd.setCursor(0, 0);
  lcd.print("Study Timer :");
  lcd.setCursor(0, 1);
  sprintf(timeline, "%0.2d mins %0.2d secs", studyMinutes, studySeconds);
  lcd.print(timeline);
  delay(1000);
  studySeconds --; //decreases seconds by 1
  focusFace();
  if (studySeconds == 00){
    studyMinutes --;
    studySeconds = 60;
  }
   if (studyMinutes < 00) {
    mode = 2;
    Serial.println(mode);
    lcd.clear();
    reset();
    glassUp();
    pom += 1;
    smileFace();
    return mode;
  }
  return mode;
}

int breakUpdate() {
  lcd.setCursor(0, 0);
  lcd.print("Break Timer :");
  lcd.setCursor(0, 1);
  sprintf(timeline, "%0.2d mins %0.2d secs", breakMinutes, breakSeconds);
  lcd.print(timeline);
  delay(1000);  
  breakSeconds --; //decreases seconds by 1
  
  if (breakSeconds == 00){
    breakMinutes --;
    breakSeconds = 60;
  }
    if (breakMinutes < 00) {
    mode = 3;
    Serial.println(mode);
    lcd.clear();
    reset();

    lcd.setCursor(0, 0);
    lcd.print("{o,o}");
    lcd.setCursor(6, 0);
    for(int i = 0; i < pom; i++){
      lcd.print("O");
    }
    lcd.setCursor(0, 1);
    lcd.print("/)_)");
    lcd.setCursor(6, 1);
    lcd.print("Well Done");
    return mode;
  }
  
  return mode;
}

//  Ultrasonic
void checkDistance() {
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  Serial.print(distance);

  if ((mode == 0) or (mode == 3)) {
    if (distance > 800) {
      defaultFace();
      Serial.println("defaultFace");
    } else {
      smileFace();
      Serial.println("smile");
    }
  } else if (mode == 1) { // study mode
    if (distance > 800) {
      defaultFace();
      present == false;
      delay(2000);
    } else {
      present == true;
    }
  }
}

void defaultFace() {
  oled.clearDisplay();
  oled.drawCircle(30, 35, 27, WHITE);
  oled.drawCircle(100, 35, 27, WHITE);
  oled.display();
  delay(300);
}
void focusFace() {
  oled.clearDisplay();
  oled.fillCircle(30, 35, 27, WHITE);
  oled.fillCircle(100, 35, 27, WHITE);
  oled.display();
  delay(300);
}
void smileFace() {
  // timer to 0: smile face
  oled.clearDisplay();
  // start x0 y0 end x1 y1
  oled.drawCircle(30, 35, 27, WHITE);
  oled.drawLine(20, 40, 30, 30, WHITE);
  oled.drawLine(40, 40, 30, 30, WHITE);
  oled.drawLine(90, 40, 100, 30, WHITE);
  oled.drawLine(110, 40, 100, 30, WHITE);
  oled.drawCircle(100, 35, 27, WHITE);
  oled.display();
  delay(300);
}
