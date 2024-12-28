#define SWITCH_1 6      // Pin for Switch 1
#define SWITCH_2 10       // Pin for Switch 2
#define SWITCH_3 0      // Pin for Switch 3

#define LED_1 6           // PWM pin for LED 1
#define LED_2 10            // PWM pin for LED 2
#define LED_3 0           // PWM pin for LED 3

#define PIEZO_PIN A0       // Analog pin for Piezo input

#define MIDI_CHANNEL 1
#define NOTE_1 60          // C4
#define NOTE_2 62          // D4
#define NOTE_3 64          // E4

#define CC_PIEZO 1         // Modulation (CC 1)

const int PIEZO_THRESHOLD = 50;  // Minimum analog value to register a tap
const int PIEZO_MAX = 1023;      // Maximum analog value for the piezo (ADC range)

// LED pulsating variables
int brightness_1 = 0, brightness_2 = 0, brightness_3 = 0;
int fadeDirection_1 = 1, fadeDirection_2 = 1, fadeDirection_3 = 1; // 1 = up, -1 = down
const int fadeAmount = 5;       // Brightness change per step
const int brightnessMax = 120;  // Maximum brightness for soft glow
const int brightnessMin = 20;   // Minimum brightness for soft glow
const int fadeDelay = 30;       // Delay between brightness changes (ms)

// Variables
int lastState_1 = HIGH, lastState_2 = HIGH, lastState_3 = HIGH;

void setup() {
  pinMode(SWITCH_1, INPUT_PULLUP);
  pinMode(SWITCH_2, INPUT_PULLUP);
  pinMode(SWITCH_3, INPUT_PULLUP);

  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);

  Serial.begin(115200); // MIDI communication
}

void loop() {
  // Handle Switch 1 and LED 1
  checkSwitch(SWITCH_1, NOTE_1, LED_1, brightness_1, fadeDirection_1);

  // Handle Switch 2 and LED 2
  checkSwitch(SWITCH_2, NOTE_2, LED_2, brightness_2, fadeDirection_2);

  // Handle Switch 3 and LED 3
  checkSwitch(SWITCH_3, NOTE_3, LED_3, brightness_3, fadeDirection_3);

  // Pulsate LEDs
  pulseLED(LED_1, brightness_1, fadeDirection_1);
  pulseLED(LED_2, brightness_2, fadeDirection_2);
  pulseLED(LED_3, brightness_3, fadeDirection_3);

  delay(fadeDelay); // Smooth the pulsation
}

// Function to check switch state and send MIDI notes
void checkSwitch(int switchPin, int note, int ledPin, int &brightness, int &fadeDirection) {
  int switchState = digitalRead(switchPin);

  if (switchState == LOW) {  // Switch turned ON
    sendMIDIMessage(0x90, note, 127);  // Note On
    analogWrite(ledPin, brightnessMax); // Turn LED fully ON
    delay(100);                        // Debounce
  } else {  // Switch turned OFF
    sendMIDIMessage(0x80, note, 0);    // Note Off
    delay(100);                        // Debounce
  }
}

// Function to handle LED pulsating
void pulseLED(int ledPin, int &brightness, int &fadeDirection) {
  // Adjust brightness
  brightness += fadeDirection * fadeAmount;

  // Reverse direction at boundaries
  if (brightness >= brightnessMax) {
    fadeDirection = -1; // Start fading out
  } else if (brightness <= brightnessMin) {
    fadeDirection = 1;  // Start fading in
  }

  // Set the LED brightness
  analogWrite(ledPin, brightness);
}

// Function to send MIDI Note On/Off messages
void sendMIDIMessage(byte command, byte pitch, byte velocity) {
  Serial.write(command | (MIDI_CHANNEL - 1));  // MIDI Command + Channel
  Serial.write(pitch);                         // MIDI Note
  Serial.write(velocity);                      // Velocity
}
