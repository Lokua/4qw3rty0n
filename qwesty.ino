#include <PS2KeyAdvanced.h>
#include "constants.h"

#define DEBUG_PS2 0
#define NOTE_OFF 128
#define NOTE_ON 144
#define DEFAULT_VELOCITY 127
#define DATA_PIN 4
#define IRQ_PIN  3

PS2KeyAdvanced keyboard;

struct programState
{
  uint8_t channel = 0;
  bool heldNotes[128] = {};
  bool hold = false;
  uint8_t scaleIndex = 0;
  uint8_t octaveOffset = 3;
};
typedef struct programState ProgramState;
ProgramState state;

void setup() {
  Serial.begin(DEBUG_PS2 ? 9600 : 31250);
  keyboard.begin(DATA_PIN, IRQ_PIN);
  keyboard.setNoRepeat(1);
}

void loop() {
  if (!keyboard.available()) {
    return;
  }

  uint16_t key = keyboard.read();
  uint8_t keyCode = key & 0xFF;
  bool isNoteKey = isNote(keyCode);
  int8_t note = isNoteKey ? getMIDINote(getKeyIndex(keyCode)) : -1;

  if (DEBUG_PS2) {
    if (!isKeyUp(key)) {
      Serial.print("keyCode (keyDown): ");
      Serial.println(keyCode);
    }
  }

  if (isKeyUp(key)) {
    if (isNoteKey && !state.hold) {
      sendNoteOff(note);
      state.heldNotes[note] = false;
    }
    return;
  }

  if (isNoteKey && !state.heldNotes[note]) {
    sendNoteOn(note);
    state.heldNotes[note] = true;
    return;
  }

  if (isScale(keyCode)) {
    setScale(keyCode);
    return;
  }

  if (isHold(keyCode)) {
    state.hold = !state.hold;
    return;
  }

  if (isOctave(keyCode)) {
    setOctave(keyCode);
    return;
  }

  if (isPanicButton(keyCode)) {
    sendAllNotesOff();
    initHeldKeys();
    return;
  }
}

int8_t getKeyIndex(uint8_t keyCode) {
  for (uint8_t i = 0; i < sizeof(KEY_MAP) / sizeof(KEY_MAP[0]); i++) {
    if (KEY_MAP[i][0] == keyCode) {
      return KEY_MAP[i][1];
    }
  }

  return -1;
}

void initHeldKeys() {
  for (uint8_t i = 0; i < 128; i++) {
    state.heldNotes[i] = false;
  }
}

void setScale(uint8_t keyCode) {
  state.scaleIndex = keyCode - PS2_KEY_F1;
}

void setOctave(uint8_t keyCode) {
  uint8_t x = state.octaveOffset + (keyCode == PS2_KEY_UP_ARROW ? 1 : -1);
  state.octaveOffset = x < 0 ? 0 : x > 5 ? 5 : x;
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

bool isNote(int8_t keyCode) {
  return getKeyIndex(keyCode) > -1;
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
  return keyCode == PS2_KEY_HOME;
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
