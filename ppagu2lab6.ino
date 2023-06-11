/*
   1- Author1: Francis Pagulayan - 665900279
    - Author2: Osman Khan  - 658639478
   2- Lab: Lab 6 - Serial Communication 
   3- Description: 
   a) In your own words, what is this code supposed to do?
    This code is for player one of a two player racing game that utilizes two arduinos communicating through serial communcation.
    
   b) Explain the division of labor between the teammates.
    We each wrote our own code ad built our own arduino then came into lab together and figured out how to make them communicate with each other

   4- Serial Communication: describe how serial communication works in this lab. (include in this description the pin numbers you used)
    In this lab, we write an "a" to designate a button push on my arduino and a "B" to designate a button push on the other arduino. First we check if there is anything available
    in the serial and if so we assign a variable with what it is. Then, we check if the value is the corresponding letter for each player and do their resective action.
    We are using pins 0 and 1 for serial communication. 
    pin 7 is the button
    Pin 8 and 9 are the green and red led respectively
    pin 10 is the buzzer



   5- References: https://robotic-controls.com/learn/arduino/arduino-arduino-serial-communication
   https://iot-guider.com/arduino/serial-communication-between-two-arduino-boards/
   https://www.arduino.cc/reference/en/language/functions/communication/serial/
   6- Demo: In person demo With Michael
*/

#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); // Initialize the LCD

int roundp1 = 1;
int roundp2 = 1;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 500;

const int button = 7;
int lastButtonState = LOW;
int buttonState = LOW;

const int GREEN_LED = 8;
const int RED_LED = 9;

const int buzzer = 10;

int playerscore = 0;
bool readyState = false;
bool p2readyState = false;

char incomingByte;

int player1Pos = 0; // Initialize player 1 position to the start
int player2Pos = 0; // Initialize player 2 position to the start

void setup()
{
  lastButtonState = LOW;
  buttonState = LOW;
  lcd.begin(16, 2); // Initialize the LCD
  pinMode(button, INPUT);
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(GREEN_LED, LOW);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(RED_LED, LOW);
  pinMode(buzzer, OUTPUT);
  lcd.print("Player 1, Press"); // Display message to player 1
  lcd.setCursor(0, 1);
  lcd.print("button to start");
  Serial.begin(9600);
}

void loop() {
  unsigned long currentMillis = millis();  // Get current time
  buttonState = digitalRead(button);       // Read button state

  if (Serial.available() > 0) {  // If there's incoming serial data
    incomingByte = Serial.read();
    if (incomingByte == 'b') {  // If player 2 is ready
      p2readyState = true;
    }
  }
  if (!readyState && buttonState == HIGH && (millis() - lastDebounceTime) > debounceDelay) {
      lastButtonState = buttonState;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.write("Ready, Waiting");
      lcd.setCursor(0, 1);
      lcd.write("for Player 2");
      readyState = true;
      Serial.write('a');  // Send signal to player 2 to indicate readiness
    }
  if (readyState && p2readyState) {  // If both players are ready
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write("Ready");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write("3");
    tone(buzzer, 1000, 500);  // Beep the buzzer
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write("2");
    tone(buzzer, 1000, 500);  // Beep the buzzer
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write("1");
    tone(buzzer, 1000, 500);  // Beep the buzzer
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write("Go!!!");
    delay(500);
    lcd.clear();

    while (readyState && p2readyState) {  // Start the game loop
      buttonState = digitalRead(button);
      if (Serial.available() > 0) {  // If there's incoming serial data
        incomingByte = Serial.read();
        if (incomingByte == 'b') {  // If player 2 moves
          player2Pos++;             // Move player 2 one position to the right
        }
        Serial.flush();
      }
      if (buttonState != lastButtonState && ((millis() - lastDebounceTime) > debounceDelay)) {        
        lastButtonState = buttonState;
        player1Pos++;  // Move player 1 one position to the right
        Serial.write('a');
        lastDebounceTime = millis();  // Reset debounce timer
      }
      if (player1Pos == 15) {
        player1Pos = 0;
        roundp1++;
        lcd.setCursor(0, 0);
        lcd.print("               #");
        lcd.setCursor(0, 0);
      }
      if (player2Pos == 15) {
        player2Pos = 0;
        roundp2++;
        lcd.setCursor(0, 1);
        lcd.print("               #");
        lcd.setCursor(0, 1);
      }
      lcd.setCursor(0, 0);
      if (roundp1 == 2) {
        lcd.print("               #");
      } else {
        lcd.print("                ");
      }
      lcd.setCursor(player1Pos, 0);
      lcd.write("X");  // Display player 1's character

      lcd.setCursor(0, 1);
      if ( roundp2 == 2) {
        lcd.print("               #");
      } else {
        lcd.print("                ");
      }
      lcd.setCursor(player2Pos, 1);
      lcd.write("O");


      if (roundp1 == 3) {               // If player 1 reaches the end
        digitalWrite(GREEN_LED, HIGH);  // Turn on the green LED
        readyState = false;
        p2readyState = false;
        incomingByte = 'c';
        player1Pos = 0;
        player2Pos = 0;
        roundp1 = 1;
        roundp2 = 1;
        delay(1000);
        Serial.flush();
        digitalWrite(GREEN_LED, LOW);
        setup();  // End the game loop
      } else if (roundp2 == 3) {
        digitalWrite(RED_LED, HIGH);  // Turn on the green LED
        readyState = false;
        p2readyState = false;
        incomingByte = 'c';
        player1Pos = 0;
        player2Pos = 0;
        roundp1 = 1;
        roundp2 = 1;
        delay(1000);
        Serial.flush();
        digitalWrite(RED_LED, LOW);
        setup();  // End the game loop
      }
    }
  }
  lastButtonState = buttonState;  // Save last button state
}
