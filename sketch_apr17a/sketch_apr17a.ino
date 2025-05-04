#include <Keypad.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <arduino-timer.h>

int buzzPin = 10;
int interruptPin;
byte analogPin = A3;

int *ptr;
int size = 1;
int countdown = 60;
int level = 3;

bool start = false;
bool intro = false;
bool running  = false;

const int ROWS = 4;
const int COLS = 4;

char outputs[4] = {'1', '2', '3', '4'};

char Layout[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};
Keypad customKeypad = Keypad(makeKeymap(Layout), rowPins, colPins, ROWS, COLS);

auto timer = timer_create_default();

void setup() {
  Serial.begin(9600);
  for (int i = 11; i<= 13; i++) {
    pinMode(i, OUTPUT);
  }
  randomSeed(analogRead(analogPin));
  ptr = (int*)calloc(size, sizeof(int));
  random(7);
  ptr[0] = random(7) + 1;
  pinMode(buzzPin, OUTPUT);
  digitalWrite(buzzPin, HIGH);
  timer.every(1000, beep);
  Serial.println();
}


void loop() {
  Serial.print(countdown);
  while(!start){
    Serial.println("=====================================");
    Serial.println("This is a bomb defusing game");
    Serial.println("Please select the difficulties:");
    Serial.println("A ----> Easy");
    Serial.println("B ----> Normal");
    Serial.println("C ----> Hard");
    while(true){
      char customkey = customKeypad.getKey();
      if(customkey){
        if(customkey == 'A'){
          Serial.println("A pass");
          countdown = 60;
          level = 3;
          start = true;
          break;
        }
        else if (customkey == 'B'){
          Serial.println("B pass");
          countdown = 30;
          level = 4;
          start = true;
          break;
        }
        else if (customkey == 'C'){
          Serial.println("C pass");
          countdown = 15;
          level = 5;
          start = true;
          break;
        }
        else{
          Serial.println("Please select again");
        }
      }
    }
  }
  

  if(!intro){
    Serial.println("=====================================");
    Serial.println("About to start in");
    delay(1000);
    Serial.println("3");
    delay(1000);
    Serial.println("2");
    delay(1000);
    Serial.println("1");
    delay(1000);
    Serial.println("Startttttttttttttttttttttttttttttttttt");
    intro = true;
    running = true;
    longBeep();
  }


  
  int counter = 0;
  int round = 0;
  int wired = false;
  int mappedValue;

  while(running){

    // Need to fix
    if(counter >= size){
      if(!wired){
        wiredBeep();
        wired = true;
        mappedValue = map(ptr[counter-1], 1, 7, 50, 1023);
        if(abs(analogRead(analogPin) - mappedValue) <= 100) {
          if(mappedValue >= 500) {
            mappedValue = 50;
          }
          else {
            mappedValue = 1000;
          }
        }
        Serial.println(map(ptr[counter-1], 1, 7, 50, 1023));
      }

      int input = analogRead(analogPin);
      if(abs(input - mappedValue) <= 5) {
        updateNumber();
        round++;
        if(round == level){
          gameWin();
          break;
        }
        counter = 0;
        wired = false;
        Serial.println("Cut the wire");
        wiredBeep();
        delay(250);
      }
    }
    else {
      char test;
      switch(ptr[counter]){
        case 1:
          digitalWrite(11, HIGH);
          test = '1';
          break;
        case 2:
          digitalWrite(12, HIGH);
          test = '2';
          break;
        case 3:
          digitalWrite(11, HIGH);
          digitalWrite(12, HIGH);
          test = '3';
          break;
        case 4:
          digitalWrite(13, HIGH);
          test = '4';
          break;
        case 5:
          digitalWrite(11, HIGH);
          digitalWrite(13, HIGH);
          test = '5';
          break;
        case 6:
          digitalWrite(12, HIGH);
          digitalWrite(13, HIGH);
          test = '6';
          break;
        case 7:
          digitalWrite(11, HIGH);
          digitalWrite(12, HIGH);
          digitalWrite(13, HIGH);
          test = '7';
          break;
      }
      
      char customKey = customKeypad.getKey();


      if(customKey && customKey == test){
        Serial.println("Correct");
        for (int i = 11; i<= 13; i++) {
          digitalWrite(i, LOW);
        }
        counter++;
        delay(250);
      }
      else if (customKey){
        errorBeep();
      }
    }
    timer.tick();
  }
}

void beep(){
  if(countdown <= 5){
    for (int i = 0; i < 3; i++) {
      analogWrite(buzzPin, 150); // Medium volume
      timer.in(100, [](){
      analogWrite(buzzPin, 0);
      });
      timer.in(100, [](){
      analogWrite(buzzPin, 255);
      });
    }
  }
  else {
    analogWrite(buzzPin, 150);
    timer.in(250, [](){
      analogWrite(buzzPin, 255);
    });
  }

  Serial.println(countdown);
  countdown--;
  if(countdown == 0){
    timer.in(1000, gameOver);
  }
}

void longBeep(){
  digitalWrite(buzzPin, LOW);
  delay(750);
  digitalWrite(buzzPin, HIGH);
  delay(1000);
}

void wiredBeep(){
  analogWrite(buzzPin, 50);
  timer.in(1000, [](){
    analogWrite(buzzPin, 100);
  });
  timer.in(1000, [](){
    analogWrite(buzzPin, 255);
  });
}

void errorBeep() {
  for (int i = 0; i < 3; i++) {
    analogWrite(buzzPin, 255);
    delay(50);
    analogWrite(buzzPin, 0);
    delay(100);
  }
}

void victorySound() {
  for (int freq = 1000; freq <= 5000; freq += 100) {
    tone(buzzPin, freq); // Frequency sweep
    delay(30);
  }
  noTone(buzzPin);
}

void updateNumber() {
  size++;
  int *temp = ptr;
  ptr = realloc(ptr, size * sizeof(int)); 
  if (!ptr) { 
      Serial.println("Memory Re-allocation failed."); 
      ptr = temp; 
  } 
  ptr[size-1] = random(7) + 1;
}

void gameWin() {
  Serial.println("Congratulations");
  Serial.println("You have defused the bomb");
  victorySound();
  resetGame();
}

void gameOver() {
  Serial.println("!!!BOOM!!!");
  longBeep();
  resetGame();
}

void resetGame() {
  size = 1;
  ptr[0] = random(7) + 1;
  countdown = 60;
  level = 3;
  start = false;
  intro = false;
  running = false;
  digitalWrite(buzzPin, HIGH);
    // Turn off all LEDs, not working
  for (int i = 11; i <= 13; i++) {
    digitalWrite(i, LOW);
  }

}

