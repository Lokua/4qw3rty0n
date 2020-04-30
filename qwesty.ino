#include <PS2Keyboard.h>

const uint8_t CHANNEL = 0;
const uint8_t NOTE_OFF = 128;
const uint8_t NOTE_ON = 144;
const uint8_t DEFAULT_VELOCITY = 127;
const uint8_t KEYS[] = {65, 71, 53, 66, 68, 73, 69, 57, 63, 64, 49, 67, 52, 54, 55, 56, 58, 59, 60, 74, 72, 51, 70, 50, 62, 61};

// Arduino cannot print to the Serial Monitor when using MIDI Baud (bits per second)?
const bool DEBUG_PS2 = true;
const uint16_t BAUD = DEBUG_PS2 ? 9600 : 31250;

const uint8_t PS2_CLOCK_PIN = 3;
const uint8_t PS2_DATA_PIN = 4;

PS2Keyboard keyboard;
bool noteOns[128] = {}; 

void setup() {
  Serial.begin(BAUD);
  keyboard.begin(PS2_DATA_PIN, PS2_CLOCK_PIN);
  
  if (!DEBUG_PS2) {
    sendAllNotesOff(CHANNEL);
  }
}

void loop() {
  if (keyboard.available()) {
    byte value = keyboard.read() - '0';

    if (!DEBUG_PS2 && isAlphaKey(value)) {
      sendNoteOn(CHANNEL, value);
      // we will obviously not do this delay in real implementation 
      delay(118);
      sendNoteOff(CHANNEL, value);
    } else if (DEBUG_PS2) {
      Serial.println(value);
    }
  }
}

void sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
  sendMIDI(NOTE_ON + channel, note, velocity);
}

void sendNoteOn(uint8_t channel, uint8_t note) {
  sendMIDI(NOTE_ON + channel, note, DEFAULT_VELOCITY);
}

void sendNoteOff(uint8_t channel, uint8_t note) {
  sendMIDI(NOTE_OFF + channel, note, 0);
}

void sendAllNotesOff(uint8_t channel) {
  for (uint8_t i = 0; i < 128; i++) {
    sendNoteOff(channel, i);
  }
}

void sendMIDI(uint8_t status, uint8_t data1, uint8_t data2) {
  //  debugMIDI(status, data1, data2);
  Serial.write(status);
  Serial.write(data1);
  Serial.write(data2);
}

void debugMIDI(uint8_t status, uint8_t data1, uint8_t data2) {
  Serial.print(status);
  Serial.print(", ");
  Serial.print(data1);
  Serial.print(", ");
  Serial.print(data2);
  Serial.print("\n");
}

bool isAlphaKey(byte key) {
  return key > 48 && key < 75;
}
