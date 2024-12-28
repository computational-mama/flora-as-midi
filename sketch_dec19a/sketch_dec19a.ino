#define SWITCH_1 6      // Pin for Switch 1
#define SWITCH_2 10       // Pin for Switch 2
#define SWITCH_3 0      // Pin for Switch 3

#define MIDI_CHANNEL 1   // MIDI Channel (1-16)

// Notes for each switch
#define NOTE_1 60        // Middle C
#define NOTE_2 62        // D4
#define NOTE_3 64        // E4

// CC Parameters for each switch
#define CC_1 1           // Modulation Wheel
#define CC_2 7           // Volume
#define CC_3 10          // Pan

int lastState_1 = HIGH, lastState_2 = HIGH, lastState_3 = HIGH;  // Store previous states
bool ccSent_1 = false, ccSent_2 = false, ccSent_3 = false;       // Flags to send CC once

void setup() {
  pinMode(SWITCH_1, INPUT_PULLUP);
  pinMode(SWITCH_2, INPUT_PULLUP);
  pinMode(SWITCH_3, INPUT_PULLUP);

  Serial.begin(115200); // MIDI baud rate
}

void loop() {
  // Handle Switch 1
  checkSwitch(SWITCH_1, lastState_1, NOTE_1, CC_1, ccSent_1);

  // Handle Switch 2
  checkSwitch(SWITCH_2, lastState_2, NOTE_2, CC_2, ccSent_2);

  // Handle Switch 3
  checkSwitch(SWITCH_3, lastState_3, NOTE_3, CC_3, ccSent_3);
}

// Function to check switch state and send MIDI messages
void checkSwitch(int switchPin, int &lastState, int note, int ccNumber, bool &ccSent) {
  int switchState = digitalRead(switchPin);

  if (switchState == LOW && lastState == HIGH) {  // Switch turned ON
    sendMIDIMessage(0x90, note, 127);            // Note On
    sendCCMessage(ccNumber, random(50, 127));    // Send random CC value
    ccSent = false;                              // Reset flag
    delay(100);                                  // Debounce
  }
  else if (switchState == HIGH && lastState == LOW) {  // Switch turned OFF
    sendMIDIMessage(0x80, note, 0);              // Note Off
    delay(100);                                  // Debounce
  }

  // Send a CC message only once while holding the switch
  if (switchState == LOW && !ccSent) {
    sendCCMessage(ccNumber, 100);                // Send a static CC value
    ccSent = true;                               // Prevent flooding
  }

  lastState = switchState;  // Update last state
}

// Function to send MIDI Note On/Off messages
void sendMIDIMessage(byte command, byte pitch, byte velocity) {
  Serial.write(command | (MIDI_CHANNEL - 1));  // MIDI Command + Channel
  Serial.write(pitch);                         // Note Number
  Serial.write(velocity);                      // Velocity
}

// Function to send MIDI CC messages
void sendCCMessage(byte controller, byte value) {
  Serial.write(0xB0 | (MIDI_CHANNEL - 1));     // CC Message on MIDI Channel
  Serial.write(controller);                    // Controller Number
  Serial.write(value);                         // Controller Value
}
