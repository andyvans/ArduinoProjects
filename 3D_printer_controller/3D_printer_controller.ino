#include <PID_v1.h>

// --- CONFIGURATION ---
const int THERMISTOR_PIN = A2;
const int MOSFET_PIN = 7;       // Must be PWM-capable
const float FIXED_RESISTOR = 4700.0; // Changed to 4.7k for better 210C resolution

// PID Variables
double Setpoint = 210.0; 
double Input, Output;
/* Tuning: Kp (Proportional), Ki (Integral), Kd (Derivative)
Kp: Increase if it's too slow to react; decrease if it overshoots too much.
Ki: Increase if it stabilizes slightly below your target.
Kd: Increase if the temperature is jittery or oscillates fast.
*/
double Kp=20, Ki=0.5, Kd=2; 

PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

void setup() {
  pinMode(MOSFET_PIN, OUTPUT);
  Serial.begin(9600);
  Serial.setTimeout(10); // Short timeout for faster serial response
  
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0, 255); // Arduino PWM range
  Serial.println("System Ready. Target: 210C. Type new temp and hit Enter.");
}

void loop() {
  // 1. Check for Serial Input to change temperature
  if (Serial.available() > 0) {
    float val = Serial.parseFloat();
    if (val > 20 && val < 250) { // Safety range check
      Setpoint = val;
      Serial.print(">>> New Setpoint: "); Serial.println(Setpoint);
    }
  }

  // 2. Read Temperature
  int adcValue = analogRead(THERMISTOR_PIN);
  float resistance = FIXED_RESISTOR * (1023.0 / (float)adcValue - 1.0);
  
  // Beta Equation (Assumes Beta 3950)
  float steinhart = resistance / 100000.0;     
  steinhart = log(steinhart) / 3950.0;         
  steinhart += 1.0 / (25.0 + 273.15);          
  Input = (1.0 / steinhart) - 273.15;          

  // 3. Safety: Emergency Shutoff if sensor fails or gets too hot
 if (Input > 260 || Input < 5) {
    analogWrite(MOSFET_PIN, 0);
    while(1) { Serial.println("ERROR: SENSOR FAULT"); delay(1000); }
 }

  // 4. Compute PID & Drive MOSFET
  myPID.Compute();
  analogWrite(MOSFET_PIN, Output);

  // 5. Periodic Debugging
  static unsigned long lastUpdate;
  if (millis() - lastUpdate > 1000) {
    Serial.print("Temp: "); Serial.print(Input);
    Serial.print(" C | Power: "); Serial.print((Output/255)*100); Serial.println("%");
    lastUpdate = millis();
  }
}
