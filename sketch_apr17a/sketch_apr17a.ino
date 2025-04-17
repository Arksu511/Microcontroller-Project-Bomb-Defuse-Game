#include <Keypad.h>
#include <stdio.h> 
#include <stdlib.h> 

int *ptr;
int size = 1;

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

void setup() {
  Serial.begin(9600);
  for (int i = 10; i<= 13; i++) {
    pinMode(i, OUTPUT);
  }
  randomSeed(analogRead(0));
  ptr = (int*)calloc(size, sizeof(int));
  ptr[0] = random(4);
  Serial.println(random(4));
}


void loop() {

  char customkey = customKeypad.getKey();
  Serial.println(random(4));
}

void updateNumber() {
  size++;
  int *temp = ptr;
  ptr = realloc(ptr, size * sizeof(int)); 
  if (!ptr) { 
      Serial.println("Memory Re-allocation failed."); 
      ptr = temp; 
  } 
  ptr[size-1] = random(4);
}


