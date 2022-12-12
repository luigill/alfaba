#include <SPI.h>
#include <MFRC522.h>

#define ARRAY_SIZE 4

int execs = 0;
String correctVector[ARRAY_SIZE] = {"ge","aa","te","oo"};
String insertedVector[ARRAY_SIZE] = {"aa","aa","te","oo"};
int diffVector[4] = {0,0,0,0};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  SPI.begin();

}

void loop() {
  // put your main code here, to run repeatedly:

}


void vectorCompare(){
  for(int i = 0; i < ARRAY_SIZE; i++)
    if(strcmp(insertedVector[i],correctVector[i] == 0)){
      diffVector[i] = 0;
    }else{
      diffVector[i] = -1;
    }
}

void logGenerator(){
  Serial.println()
}