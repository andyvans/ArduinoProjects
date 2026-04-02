int Relay1 = ;
int Relay2 = D6;

void setup()
{
  pinMode (Relay2, OUTPUT);
  pinMode(Relay1, OUTPUT); //Set Pin14 as output

}

void loop()                                   
{
  digitalWrite(Relay1, HIGH); //Turn on relay
  digitalWrite(Relay2, HIGH); //Turn off relay
  delay(100);
  digitalWrite(Relay1, LOW); //Turn on relay
  digitalWrite(Relay2, LOW); //Turn off relay
  delay(100);
  
}
