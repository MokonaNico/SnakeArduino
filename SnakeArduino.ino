#include <LiquidCrystal.h>
#include <LedControl.h>

//10 -> CLK, 11 -> CS, 12 -> DIN
LedControl lc=LedControl(12,10,11,1);
//39 -> D7, 41 -> D6, 43 -> D5, 45 -> D4, 47 -> E, 49 -> RS 
LiquidCrystal lcd = LiquidCrystal(49, 47, 45, 43, 41, 39);

typedef struct Vector Vector;
struct Vector{
  int x;
  int y;
};

boolean vectorEquals(Vector v1, Vector v2){
  return v1.x == v2.x and v1.y == v2.y;

}

const Vector UP = {0,-1};
const Vector DOWN = {0,1};
const Vector RIGHT = {1,0};
const Vector LEFT = {-1,0};
const Vector ZERO =  {0,0};

const int BOARDSIZE = 8;

const int X_pin = A0;
const int Y_pin = A1;

boolean gameIsOn = true;

long moveTime = 0;
const int MOVE_REFRESH_TIME = 200;

Vector playerPos = {2,3};
Vector tail[63]; 
int tailSize = 0;
Vector playerCurrentDirection = DOWN;

Vector applePos;
Vector inputPlayer;

void refreshDisplay(){
  lc.clearDisplay(0);
  lc.setLed(0, playerPos.y, playerPos.x, true);
  if(millis()%1000 > 300)
    lc.setLed(0, applePos.y, applePos.x, true);
  for(int i = 0; i < tailSize; i++){
    lc.setLed(0, tail[i].y, tail[i].x, true); 
  }
}

void randomizeApple(){
  applePos.x = (int) random(8);
  applePos.y = (int) random(8);
}

void movePlayer(){
  for(int i = tailSize -1; i >= 1 ; i--){
    tail[i] = tail[i-1];
  }
  tail[0] = playerPos;
  
  playerPos.x = playerPos.x + playerCurrentDirection.x;
  if(playerPos.x < 0) playerPos.x = BOARDSIZE - 1;
  else if(playerPos.x >= BOARDSIZE) playerPos.x = 0;
  playerPos.y = playerPos.y + playerCurrentDirection.y;
  if(playerPos.y < 0) playerPos.y = BOARDSIZE - 1;
  else if(playerPos.y >= BOARDSIZE) playerPos.y = 0;

  for(int i = 0; i < tailSize; i++){
    if(vectorEquals(playerPos, tail[i])) gameIsOn = false;
  }
}

Vector getInputDirection(){
  Vector input;
  input.x = analogRead(X_pin);
  input.y = analogRead(Y_pin);
  if(input.x >= 768 and input.y >= 256 and input.y <= 768) return UP;
  if(input.x <= 256 and input.y >= 256 and input.y <= 768) return DOWN; 
  if(input.y >= 768 and input.x >= 256 and input.x <= 768) return RIGHT;
  if(input.y <= 256 and input.x >= 256 and input.x <= 768) return LEFT; 
  return ZERO;
}

void updatePlayerNextDirection(){
  Vector inputDirection = getInputDirection();
  if(vectorEquals(playerCurrentDirection, UP) or vectorEquals(playerCurrentDirection, DOWN)){
    if(vectorEquals(inputDirection, RIGHT) or vectorEquals(inputDirection, LEFT)) playerCurrentDirection = inputDirection;
  } else {
    if(vectorEquals(inputDirection, UP) or vectorEquals(inputDirection, DOWN)) playerCurrentDirection = inputDirection;
  }
}

void growTail(){
  Vector tailPos = applePos;
  tail[tailSize] = tailPos;
  tailSize++;
}

void updateApple(){
  if(vectorEquals(applePos,playerPos)){
    growTail();
    randomizeApple();
  }
}

void updateLCD(){
    lcd.setCursor(7,0);
    lcd.print(tailSize);
    lcd.setCursor(8,1);
    if(gameIsOn)
      lcd.print("InGame");
    else 
      lcd.print("GameOver");
}

void setup() {
  lc.shutdown(0,false);
  lc.setIntensity(0,8);
  lc.clearDisplay(0);

  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("Score: 0");
  lcd.setCursor(0,1);
  lcd.print("Status: InGame"); 
  
  randomSeed(analogRead(A3));
  randomizeApple();
  moveTime = millis();
}

void loop() {
  if(gameIsOn){
    updatePlayerNextDirection();
      if((moveTime + MOVE_REFRESH_TIME) <= millis()) {
          moveTime = millis();
          movePlayer();
          refreshDisplay();
      }
      updateApple(); 
      updateLCD();
  }
}
