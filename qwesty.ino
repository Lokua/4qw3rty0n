#include <PS2KeyAdvanced.h>
#include "constants.h"

// when on, will print MIDI statements to the Serial Monitor instead of
// actually sending MIDI since Arduino Uno only has a single hardware Serial and cannot
// println under the MIDI BUAD rate
#define DEBUG_PS2 0

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
  uint8_t octaveOffset = 2;
};
typedef struct programState ProgramState;
ProgramState state;

void setup() {
  Serial.begin(BAUD);
  keyboard.begin(DATA_PIN, IRQ_PIN);
  keyboard.setNoRepeat(1);

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

  if (DEBUG_PS2) {
    if (!isKeyUp(key)) {
      Serial.print("keyCode (keyDown): ");
      Serial.println(keyCode);
    }
  }

  if (isKeyUp(key)) {
    if (isNote(keyCode) && !state.hold) {
      sendNoteOff(getMIDINote(KEY_CODE_TO_INDEX[keyCode]));
      state.heldKeys[keyCode] = false;
    }
    return;
  }

  if (isNote(keyCode) && !state.heldKeys[keyCode]) {
    sendNoteOn(getMIDINote(KEY_CODE_TO_INDEX[keyCode]));
    state.heldKeys[keyCode] = true;
    return;
  }

  if (isScale(keyCode)) {
    setScale(keyCode);
    return;
  }

  if (isHold(keyCode)) {
    DEBUG_PS2 && Serial.println("hold");
    return;
  }

  if (isOctave(keyCode)) {
    setOctave(keyCode);
    return;
  }

  if (isPanicButton(keyCode)) {
    sendAllNotesOff();
    return;
  }
}

void setScale(uint8_t keyCode) {
  state.scaleIndex = keyCode - PS2_KEY_F1;
}

void setOctave(uint8_t keyCode) {
  uint8_t x = keyCode == PS2_KEY_UP_ARROW ? 1 : -1;
  state.octaveOffset = x < 0 ? 0 : x > 3 ? 3 : state.octaveOffset + x;
}

uint8_t getMIDINote(uint8_t keyCodeIndex) {
  uint8_t scaleLength;

  switch (state.scaleIndex) {
    case 0:
    case 1:
    case 2:
    case 4:
    case 5:
    case 8:
    case 9:
    case 10:
    case 11:
      scaleLength = 7;
      break;
    case 3:
    case 7:
      scaleLength = 5;
      break;
    case 6:
      scaleLength = 6;
      break;
  }

  uint8_t pitchClassIndex = keyCodeIndex % scaleLength;
  uint8_t pitchClass = getPitchClass(pitchClassIndex);
  uint8_t octave = (floor(keyCodeIndex / scaleLength) * 12) + (state.octaveOffset * 12);

  return octave + pitchClass;
}

uint8_t getPitchClass(uint8_t index) {
  switch (state.scaleIndex) {
    case 0:
      return MAJOR[index];
    case 1:
      return HARMONIC_MAJOR[index];
    case 2:
      return MIXOLYDIAN[index];
    case 3:
      return MAJOR_PENTATONIC[index];
    case 4:
      return MINOR[index];
    case 5:
      return HARMONIC_MINOR[index];
    case 6:
      return BLUES_MINOR[index];
    case 7:
      return MINOR_PENTATONIC[index];
    case 8:
      return DORIAN[index];
    case 9:
      return PHRYGIAN[index];
    case 10:
      return PHRYGIAN_DOMINANT[index];
    case 11:
      return LYDIAN_DOMINANT[index];
  }
}


bool isKeyUp(uint16_t key) {
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

bool isPanicButton(uint8_t keyCode) {
  return keyCode == PS2_KEY_DELETE;
}

void sendNoteOn(uint8_t note) {
  sendMIDI(NOTE_ON + state.channel, note, DEFAULT_VELOCITY);
}

void sendNoteOff(uint8_t note) {
  sendMIDI(NOTE_OFF + state.channel, note, 0);
}

void sendAllNotesOff() {
  for (uint8_t i = 0; i < 128; i++) {
    sendNoteOff(i);
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
