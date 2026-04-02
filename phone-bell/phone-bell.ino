//L293D
const int motorPin1  = 5;  // Pin 14 of L293
const int motorPin2  = 6;  // Pin 10 of L293


//This will run only one time.
void setup()
{
  Serial.begin(115200);

  //Set pins as outputs
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
}


void loop()
{    
  ring();  
  ring();
  delay (1500);  
}

void ring()
{
  unsigned long endtime = millis() + 400;  
  while (endtime > millis())
  {
    bell();
  }

  Serial.println("stop");
  // Stop
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  delay(400);
}

void bell()
{
  Serial.println("bell");

  // Left
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);    
  delay(15); 

  // Right
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  delay(15);
}