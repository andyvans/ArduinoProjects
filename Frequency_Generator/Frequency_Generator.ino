/*
ESP32 1Hz-1MHz PWM generator with serial input, PWM at pin D18
Usage: Enter frequency or dutycycle in the Serial monitor @ 115200 baud
  #Frequency format : f1000, f1M, f30k, F30k
  #Dutycycle format : 45%
Note: PWM resolution is 10bit but reduces to 6 bit at higher frequencies.
      Resulution and actual PWM% is printed in the serial monitor
Mr.T's Design Graveyard: https://www.youtube.com/@smartpowerelectronics8779
*/

// PWM Configuration
int PWM_CHANNEL = 0;       // LEDC channel 0
int PWM_FREQ = 1000;       // Default 1kHz
int PWM_RESOLUTION = 10;   // Initial resolution (adjusted automatically)
int LED_OUTPUT_PIN = 18;   // GPIO pin for PWM output
int LED_OUTPUT_PIN_INVERT = 19; // GPIO pin for inverted PWM output

// Global variables
float max_duty_cycle;
float current_duty_percent = 50.0; // Track duty cycle as percentage
String inputString = "";    // For serial input
bool stringComplete = false;

void printManual()
{
  Serial.println("Please enter frequency or dutycycle");
  Serial.println("   #Frequency 1Hz-1MHz, for example: f1000, f1M, f30k)");
  Serial.println("   #Dutycycle 0-100%,   for example: 45%");
}

void setup() {
  Serial.begin(115200);
  delay(200);
  // Initialize PWM
  updatePWM();

  // Set initial duty to 50%
  ledcWrite(LED_OUTPUT_PIN, (current_duty_percent * max_duty_cycle) / 100.0);
  ledcWrite(LED_OUTPUT_PIN_INVERT, (current_duty_percent * max_duty_cycle) / 100.0);
  printManual();
  printStatus();
}

void loop() {
  if (stringComplete) {
    processInput();
    inputString = "";
    stringComplete = false;
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      inputString += inChar;
    }
  }
}

void processInput() {
  inputString.trim();

  // Frequency command
  if (inputString.startsWith("f") || inputString.startsWith("F")) {
    String freqStr = inputString.substring(1);
    freqStr.toUpperCase();

    long newFreq = 0;

    if (freqStr.endsWith("K")) {
      newFreq = freqStr.substring(0, freqStr.length() - 1).toInt() * 1000;
    }
    else if (freqStr.endsWith("M")) {
      newFreq = freqStr.substring(0, freqStr.length() - 1).toInt() * 1000000;
    }
    else {
      newFreq = freqStr.toInt();
    }

    if (newFreq >= 1 && newFreq <= 1000000) {
      PWM_FREQ = newFreq;
      updatePWM();
      // Maintain same % duty after frequency change
      ledcWrite(LED_OUTPUT_PIN, (current_duty_percent * max_duty_cycle) / 100.0);
      ledcWrite(LED_OUTPUT_PIN_INVERT, (current_duty_percent * max_duty_cycle) / 100.0);
      printStatus();
    } else {
      Serial.println("Error: Frequency must be 1Hz to 1MHz");
    }
  }
  // Duty cycle command
  else if (inputString.endsWith("%")) {
    String dutyStr = inputString.substring(0, inputString.length() - 1);
    float dutyPercent = dutyStr.toFloat();

    if (dutyPercent >= 0 && dutyPercent <= 100) {
      // Calculate the minimum non-zero duty cycle
      float min_duty_percent = 100.0 / max_duty_cycle;
      // Round the input duty percentage to the nearest multiple of min_duty_percent
      dutyPercent = round(dutyPercent / min_duty_percent) * min_duty_percent;

      if (dutyPercent > 0 && dutyPercent < min_duty_percent) {
        dutyPercent = min_duty_percent;
        Serial.print("Warning: Minimum achievable duty cycle is ");
        Serial.print(min_duty_percent, 4);
        Serial.println("%. Setting to this value.");
      }
      current_duty_percent = dutyPercent;
      ledcWrite(LED_OUTPUT_PIN, (current_duty_percent * max_duty_cycle) / 100.0);
      ledcWrite(LED_OUTPUT_PIN_INVERT, (current_duty_percent * max_duty_cycle) / 100.0);
      printStatus();
    } else {
      Serial.println("Error: Duty cycle must be 0 - 100%");
    }
  }
  else {
    //Serial.println("Invalid command. Use f1000 or 50% format");
      printManual();
  }
}

void updatePWM() {
  // Set resolution based on frequency
  if (PWM_FREQ >= 500000) {      // 500kHz - 1MHz: 6-bit
    PWM_RESOLUTION = 6;
  } else if (PWM_FREQ >= 250000) { // 250-500kHz: 7-bit
    PWM_RESOLUTION = 7;
  } else if (PWM_FREQ >= 125000) { // 125-250kHz: 8-bit
    PWM_RESOLUTION = 8;
  } else if (PWM_FREQ >= 60000) {  // 60-125kHz: 9-bit
    PWM_RESOLUTION = 9;
  } else {                        // <60kHz: 10-bit
    PWM_RESOLUTION = 10;
  }

  max_duty_cycle = (1 << PWM_RESOLUTION) - 1;
  
  // Arduino ESP32 Core 3.x API
  ledcDetach(LED_OUTPUT_PIN);  // Detach first if already attached
  ledcDetach(LED_OUTPUT_PIN_INVERT);
  ledcAttach(LED_OUTPUT_PIN, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(LED_OUTPUT_PIN_INVERT, PWM_FREQ, PWM_RESOLUTION);
  ledcOutputInvert(LED_OUTPUT_PIN_INVERT, true);  // Invert the output
}

void printStatus() {
  // Format frequency string
  String freqStr;
  if (PWM_FREQ >= 1000000) {
    freqStr = String(PWM_FREQ / 1000000.0, 1) + "MHz";
  } else if (PWM_FREQ >= 1000) {
    freqStr = String(PWM_FREQ / 1000.0, 1) + "kHz";
  } else {
    freqStr = String(PWM_FREQ) + "Hz";
  }

  Serial.print("Current Frequency is: ");
  Serial.print(freqStr);
  Serial.print(" (");
  Serial.print(PWM_RESOLUTION);
  Serial.print("-bit)");
  Serial.print("; Dutycycle: ");
  Serial.print(current_duty_percent, 1);
  Serial.println("%");
}