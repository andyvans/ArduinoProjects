#include <Gamepad.h>

int rightXcenter = 500;
int rightYcenter = 500;
int leftXcenter = 500;
int leftYcenter = 500;
double multiplierRX = 0.254; //127 / 500
double multiplierRY = 0.254;
double multiplierLX = 0.254;
double multiplierLY = 0.254;

Gamepad gp;

void setup() {

  Serial.begin(9600);

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(9,  INPUT_PULLUP); //UPPER RIGHT
  pinMode(8,  INPUT_PULLUP); //UPPER LEFT
  pinMode(7,  INPUT_PULLUP); //LEFTBUTTON
  pinMode(6,  INPUT_PULLUP); //RIGHTBUTTON
  pinMode(10,  INPUT); //UP
  pinMode(11,  INPUT); //DOWN
  pinMode(12,  INPUT); //LEFT
  pinMode(13,  INPUT); //RIGHT
  pinMode(2,  INPUT); //Y
  pinMode(5,  INPUT); //X
  pinMode(4,  INPUT); //A
  pinMode(3,  INPUT); //B  
  
  calibrate();
}

void loop() {
  int lx, ly, rx, ry;
  lx = analogRead(A1);
  ly = analogRead(A0);
  rx = analogRead(A2);
  ry = analogRead(A3);
  //we need to convert a 0-1000 to -127 - 127
  lx = floor((lx - leftXcenter) * multiplierLX);
  ly = floor((ly - leftYcenter) * multiplierLY);
  rx = floor((rx - rightXcenter) * multiplierRX);
  ry = floor((ry - rightYcenter) * multiplierRY);
  if(lx > 127) lx = 127;
  if(ly > 127) ly = 127;
  if(rx > 127) rx = 127;
  if(ry > 127) ry = 127;
  ly = ly * -1;
  gp.setLeftXaxis(lx);
  gp.setLeftYaxis(ly);
  gp.setRightXaxis(rx);  
  gp.setRightYaxis(ry);  
  
  Serial.print("Left X: ");
  Serial.println(lx);
  Serial.print("Left Y: ");
  Serial.println(ly);
  Serial.print("Right X: ");
  Serial.println(rx);
  Serial.print("Right Y: ");
  Serial.println(ry+7);
  
  
  int UPLEFT, UPRIGHT, UP, DOWN, LEFT, RIGHT, RIGHTBUTTON, LEFTBUTTON, X, Y, A, B;
  UPLEFT = digitalRead(8);
  UPRIGHT = digitalRead(9);
  RIGHTBUTTON = digitalRead(6);
  LEFTBUTTON = digitalRead(7);
  UP = !digitalRead(10);
  DOWN = !digitalRead(11);
  LEFT = !digitalRead(12);
  RIGHT = !digitalRead(13);
  X = !digitalRead(5);
  Y = !digitalRead(2);
  A = !digitalRead(4);
  B = !digitalRead(3);
  
      
  
  if(UPLEFT == LOW) {
    gp.setButtonState(0, true);
    //Serial.println("UPLEFT");
  }
  else
    gp.setButtonState(0, false);

  if(UPRIGHT == LOW) {
    gp.setButtonState(1, true);
    //Serial.println("UPRIGHT");
  }
  else
    gp.setButtonState(1, false);
 /*
  if(UP == LOW)
    gp.setButtonState(2, true);
  else
    gp.setButtonState(2, false);

  if(DOWN == LOW)
    gp.setButtonState(3, true);
  else
    gp.setButtonState(3, false);    

  if(LEFT == LOW)
    gp.setButtonState(4, true);
  else
    gp.setButtonState(4, false); 

  if(RIGHT == LOW)
    gp.setButtonState(5, true);
  else
    gp.setButtonState(5, false);     
*/
  if(RIGHTBUTTON == LOW) {
    gp.setButtonState(6, true);
    //Serial.println("RIGHTBUTTON");
  }
  else
    gp.setButtonState(6, false);   

  if(LEFTBUTTON == LOW) {
    gp.setButtonState(7, true);
    //Serial.println("LEFTBUTTON");
  }
  else
    gp.setButtonState(7, false);     

  if(X == LOW) {
    gp.setButtonState(8, true);
    //Serial.println("X");
  }
  else
    gp.setButtonState(8, false);

  if(Y == LOW) {
    gp.setButtonState(9, true);
    //Serial.println("Y");
  }
  else
    gp.setButtonState(9, false);

  if(A == LOW) {
    gp.setButtonState(10, true);
    //Serial.println("A");
  }
  else
    gp.setButtonState(10, false);  

  if(B == LOW) {
    gp.setButtonState(11, true);
    //Serial.println("B");
  }
  else
    gp.setButtonState(11, false);       

  delay(20);
}

void calibrate()
{
  int lx, ly, rx, ry;
  int i = 0;
  while(i < 13)
  {
    lx = analogRead(A3);
    ly = analogRead(A2);
    rx = analogRead(A1);
    ry = analogRead(A0);
    bool validLX = lx > (leftXcenter - 100) && lx < (leftXcenter + 100);
    bool validLY = ly > (leftYcenter - 100) && ly < (leftYcenter + 100);
    bool validRX = rx > (rightXcenter - 100) && rx < (rightXcenter + 100);
    bool validRY = ry > (rightYcenter - 100) && ry < (rightYcenter + 100);
    if(validLX && validLY && validRX && validRY)
    {
      i++;
      //nothing to do here!
    }
    else i = 0;
    delay(20);
  }
  leftXcenter = lx;
  leftYcenter = ly;
  rightXcenter = rx;
  rightYcenter = ry;
  multiplierLX = (double)127 / (double)lx;
  multiplierLY = (double)127 / (double)ly;
  multiplierRX = (double)127 / (double)rx;
  multiplierRY = (double)127 / (double)ry;
}
