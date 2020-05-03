#include <PS2KeyAdvanced.h>
#include "constants.h"

// when on, will print MIDI statements to the Serial Monitor instead of
// actually sending MIDI since Arduino Uno only has a single hardware Serial and cannot
// println under the MIDI BUAD rate
#define DEBUG_PS2 1

#define NOTE_OFF 128
#define NOTE_ON 144
#define DEFAULT_VELOCITY 127
#define DATA_PIN 4
#define IRQ_PIN  3

PS2KeyAdvanced keyboard;

const uint16_t BAUD = DEBUG_PS2 ? 9600 : 31250;

struct programState
{
  uint8_t channel = 0;
  bool heldKeys[255] = {};
  bool hold = false;
  uint8_t scaleIndex = 0;
};
typedef struct programState ProgramState;
ProgramState state;

void setup() {
  Serial.begin(BAUD);
  keyboard.begin(DATA_PIN, IRQ_PIN);
  keyboard.setNoRepeat(1);

  if (!DEBUG_PS2) {
    sendAllNotesOff(state.channel);
  }

  /* see `util.js` for why this commented out code is here */
  //  for (uint8_t i = 0; i < sizeof(KEYS); i++) {
  //    Serial.println(KEYS[i]);
  //  }
}

void loop() {
  if (!keyboard.available()) {
    return;
  }

  uint16_t key = keyboard.read();
  uint8_t keyCode = key & 0xFF;

  Serial.print("keyCode");
  Serial.print(isKeyUp(key) ? "(keyup): " : ": ");
  Serial.println(keyCode);

  if (isKeyUp(key)) {
    if (isNote(keyCode) && !state.hold) {
      sendNoteOff(keyCode);
      state.heldKeys[keyCode] = false;
    }
  } else if (isNote(keyCode) && !state.heldKeys[keyCode]) {
    Serial.print("index: ");
    Serial.println(KEY_CODE_TO_INDEX[keyCode]);
    sendNoteOn(keyCode);
    state.heldKeys[keyCode] = true;
  } else if (isScale(keyCode)) {
    Serial.println("scale");
  } else if (isHold(keyCode)) {
    Serial.println("hold");
  } else if (isOctave(keyCode)) {
    Serial.println("octave");
  }
}

bool isKeyUp(uint16_t key) {
  //  Serial.print("keyup debug: ");
  //  Serial.print(key >> 8, HEX);
  //  Serial.print(" ");
  uint16_t status_ = key >> 8;
  return status_ == 0x80 || status_ == 0x81;
}

// see createKeyToIndex.js for generating these conditions
bool isNote(uint8_t keyCode) {
  return keyCode == 28 ||
         keyCode == 30 ||
         keyCode == 48 ||
         keyCode == 49 ||
         keyCode == 50 ||
         keyCode == 51 ||
         keyCode == 52 ||
         keyCode == 53 ||
         keyCode == 54 ||
         keyCode == 55 ||
         keyCode == 56 ||
         keyCode == 57 ||
         keyCode == 59 ||
         keyCode == 60 ||
         keyCode == 61 ||
         keyCode == 62 ||
         keyCode == 64 ||
         keyCode == 65 ||
         keyCode == 66 ||
         keyCode == 67 ||
         keyCode == 68 ||
         keyCode == 69 ||
         keyCode == 7 ||
         keyCode == 70 ||
         keyCode == 71 ||
         keyCode == 72 ||
         keyCode == 73 ||
         keyCode == 74 ||
         keyCode == 75 ||
         keyCode == 76 ||
         keyCode == 77 ||
         keyCode == 78 ||
         keyCode == 79 ||
         keyCode == 80 ||
         keyCode == 81 ||
         keyCode == 82 ||
         keyCode == 83 ||
         keyCode == 84 ||
         keyCode == 85 ||
         keyCode == 86 ||
         keyCode == 87 ||
         keyCode == 88 ||
         keyCode == 89 ||
         keyCode == 90 ||
         keyCode == 91 ||
         keyCode == 92 ||
         keyCode == 93 ||
         keyCode == 94 ||
         keyCode == 95;
}

bool isAlphaKey(uint8_t keyCode) {
  return keyCode >= PS2_KEY_A && keyCode <= PS2_KEY_Z;
}

bool isNumericKey(uint8_t keyCode) {
  return keyCode >= PS2_KEY_0 && keyCode <= PS2_KEY_9;
}

bool isScale(uint8_t keyCode) {
  return keyCode >= PS2_KEY_F1 && keyCode <= PS2_KEY_F12;
}

bool isHold(uint8_t keyCode) {
  return keyCode == PS2_KEY_CAPS;
}

bool isOctave(uint8_t keyCode) {
  return keyCode == PS2_KEY_UP_ARROW || keyCode == PS2_KEY_DN_ARROW;
}

void sendNoteOn(uint8_t note) {
  sendNoteOn(state.channel, note, DEFAULT_VELOCITY);
}

void sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
  sendMIDI(NOTE_ON + channel, note, velocity);
}

void sendNoteOff(uint8_t note) {
  sendMIDI(NOTE_OFF + state.channel, note, 0);
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
  if (DEBUG_PS2) {
    Serial.print(status);
    Serial.print(", ");
    Serial.print(data1);
    Serial.print(", ");
    Serial.print(data2);
    Serial.print("\n---\n");
  } else {
    Serial.write(status);
    Serial.write(data1);
    Serial.write(data2);
  }
}
