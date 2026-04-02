// Pin definitions
int Bell1 = 2;
int Bell2 = 3;
int Bell3 = 4;
int Bell4 = 5;
int Button = 7;

// Song definitions (note:duration format)
const char* Mcdonalds = "1:125,2:250,3:250,4:250,3:1000";
const char* Original = "4:750,2:750,3:750,1:1500,1:750,3:750,4:750,2:750";
const char* Lamb = "3:500,2:500,1:500,2:500";
const char* Phone = "1:250,2:250,4:250,3:500,2:500";
const char* Chime = "1:100,2:100,3:100,4:100";
const char* Updown = "1:250,4:250,1:250,4:250";

const char* Songs[] = {Chime, Updown, Mcdonalds, Original, Lamb, Phone};
const int ArraySize = sizeof(Songs) / sizeof(Songs[0]);

const int BellsCount = 4;
const int Bells[BellsCount] = {Bell1, Bell2, Bell3, Bell4};

int nextSong = 0;

void setup()
{
    pinMode(Bell1, OUTPUT);
    pinMode(Bell2, OUTPUT);
    pinMode(Bell3, OUTPUT);
    pinMode(Bell4, OUTPUT);
    pinMode(Button, INPUT_PULLUP);
}

void loop()
{
   int buttonState = digitalRead(Button); // Read the state of the button
   if (buttonState == LOW) 
   {
      auto song = Songs[nextSong];
      play(song);      
      nextSong++;
      if (nextSong >= ArraySize)
      {
        nextSong = 0;
      }
   }
   delay(50);
}

void setBellsState(const char* notes, int state) {
    for (int i = 0; i < strlen(notes); i++) {
        char c = notes[i];
        int note = c - '0'; // Convert char to int
        if (note < 1 || note > 4) continue;

        Serial.print("Bell ");
        Serial.print(note);
        Serial.println(state == HIGH ? " ON" : " OFF");

        if (note < 1 || note > BellsCount) continue; // Validate note number
        int bellPin = Bells[note - 1];
        digitalWrite(bellPin, state);        
    }
}

void play(const char* song) {
    // Parse the song string
    char notes[10]; // Allocate buffer for notes string
    int duration = 0;
    char* song_copy = strdup(song);
    char* token = strtok(song_copy, ",");

    while (token != NULL) {
        // Parse note:duration format
        if (sscanf(token, "%9[^:]:%d", notes, &duration) != 2) {
            token = strtok(NULL, ",");
            continue;
        }

        // Turn on all bells in the sequence at the same time
        setBellsState(notes, HIGH);

        delay(50); // Keep bells on for 50ms

        // Turn off all bells in the sequence at the same time
        setBellsState(notes, LOW);

        delay(duration); // Use total duration for the entire note sequence
        token = strtok(NULL, ",");
    }
    free(song_copy);
}
