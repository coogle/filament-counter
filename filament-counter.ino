#include <LiquidCrystal.h>
#include <Wire.h>
#include <ps2.h>

#define MOUSE_DATA_PIN 7
#define MOUSE_CLOCK_PIN 8
#define RESET_BTN_PIN 6
#define CALIBRATION_FACTOR 2.5

LiquidCrystal lcd(0, 1, 2, 3, 4, 5);

PS2 sensor(MOUSE_DATA_PIN, MOUSE_CLOCK_PIN);

int tValue = 0;

long newmx = 0;
long newmy = 0;
long total = 0;
boolean direct = true;

int buttonState;
unsigned long timePress = 0;
unsigned long timePressLimit = 0;
int clicks = 0;

void initializeSensor()
{
  lcd.setCursor(0, 1);
  lcd.print("Init Sensor...");
  
  sensor.write(0xff);
  
  sensor.read();
  sensor.read();
  sensor.read();

  sensor.write(0xf0);

  sensor.read();

  delayMicroseconds(100);
  lcd.setCursor(0, 1);
  lcd.print("");
}

void checkDirection()
{
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(" Change Dir?");
  lcd.setCursor(0, 1);
  lcd.print("Hold to Confirm");
  delay(2000);

  lcd.setCursor(0,1);
  lcd.print("");

  boolean resetPressed = false;
  int countdown = 3;

  while(!resetPressed && countdown >= 0) {
    tValue = digitalRead(RESET_BTN_PIN);

    if(tValue == LOW) {
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Direction");
      lcd.setCursor(0, 1);
      lcd.print("Changed");
      delay(1000);
      resetPressed = true;
      direct = false;
      lcd.clear();
    }

    countdown--;

    if(countdown < 0) {
      lcd.clear();
      lcd.setCursor(0,0);
    }

    if(resetPressed) {
      lcd.clear();
      lcd.setCursor(0,0);
    }

    delay(1000);
    
  }  
}

void setup() {
  // put your setup code here, to run once:

  lcd.begin(16, 2);
  lcd.setCursor(1, 0);
  lcd.print("Filament Meter");
  lcd.setCursor(0, 1);
  lcd.print("..Initializing..");
  
  pinMode(RESET_BTN_PIN, INPUT_PULLUP);
  initializeSensor();
  checkDirection();
  
  delay(2000);

  renderToLCD();
  
}

void loop() {
  // put your main code here, to run repeatedly:

  char mstat;
  char mx;
  char my;

  sensor.write(0xeb);
  sensor.read();

  mstat = sensor.read();
  mx = sensor.read();
  my = sensor.read();

  if(direct) {
    mx = mx * -1;
    my = my * -1;
  }

  buttonState = digitalRead(RESET_BTN_PIN);

  if(buttonState == LOW) {
    delay(200);

    if(clicks == 0) {
      timePress = millis();
      timePressLimit = timePress + 500;
      clicks++;
    } else if(clicks == 1 && millis() < timePressLimit) {
      newmx = 0;
      total = 0;
      timePress = 0;
      timePressLimit = 0;
      clicks = 0;
    }

    if(clicks == 1 && timePressLimit != 0 && millis() > timePressLimit) {
      timePress = 0;
      timePressLimit = 0;
      clicks = 0;

      newmx = 0;
    }
  }

  if(abs(mx) > 0 || abs(my) > 0 || (newmx == 0)) {
    newmx = newmx + mx;
    newmy = newmy + my;

    renderToLCD();
  }
  delay(100);
  
}

void renderToLCD()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("U = ");
    lcd.print(newmx / CALIBRATION_FACTOR);
    lcd.print(" mm");
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("T = ");
    lcd.print(total / CALIBRATION_FACTOR);
    lcd.print(" mm");
}

