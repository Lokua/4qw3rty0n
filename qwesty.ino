#include <LiquidCrystal.h>
#include <PS2KeyAdvanced.h>
#include "scales.h"

#define DEBUG_PS2 1
#define NOTE_OFF 128
#define NOTE_ON 144
#define DEFAULT_VELOCITY 127
#define DATA_PIN 4
#define IRQ_PIN  3
#define N_KEYS 50
#define PRINT_MODE_DEBUG 0
#define PRINT_MODE_LIVE 1
#define PRINT_MODE_MIDI 2
#define PRINT_MODE_HELP 3

LiquidCrystal lcd(12, 11, 5, 8, 9, 2);
PS2KeyAdvanced keyboard;

uint8_t lockPanic = 0;

struct programState
{
  uint8_t channel = 0;
  bool heldNotes[128] = {};
  bool hold = false;
  uint8_t scaleIndex = 0;
  uint8_t octaveOffset = 3;
  uint8_t root = 0;
  uint8_t rowOffsets[4] = {0, 0, 0, 0};
  uint8_t printMode = PRINT_MODE_LIVE;
  uint8_t lastNote = 0;

  uint8_t keys[N_KEYS][2] = {
    // row 1
    {PS2_KEY_Z, 0},
    {PS2_KEY_X, 1},
    {PS2_KEY_C, 2},
    {PS2_KEY_V, 3},
    {PS2_KEY_B, 4},
    {PS2_KEY_N, 5},
    {PS2_KEY_M, 6},
    {PS2_KEY_COMMA, 7},
    {PS2_KEY_DOT, 8},
    {PS2_KEY_DIV, 9},
    {PS2_KEY_R_SHIFT, 10},
    // row 2
    {PS2_KEY_A, 2},
    {PS2_KEY_S, 3},
    {PS2_KEY_D, 4},
    {PS2_KEY_F, 5},
    {PS2_KEY_G, 6},
    {PS2_KEY_H, 7},
    {PS2_KEY_J, 8},
    {PS2_KEY_K, 9},
    {PS2_KEY_L, 10},
    {PS2_KEY_SEMI, 11},
    {PS2_KEY_APOS, 12},
    {PS2_KEY_ENTER, 13},
    // row 3
    {PS2_KEY_Q, 4},
    {PS2_KEY_W, 5},
    {PS2_KEY_E, 6},
    {PS2_KEY_R, 7},
    {PS2_KEY_T, 8},
    {PS2_KEY_Y, 9},
    {PS2_KEY_U, 10},
    {PS2_KEY_I, 11},
    {PS2_KEY_O, 12},
    {PS2_KEY_P, 13},
    {PS2_KEY_OPEN_SQ, 14},
    {PS2_KEY_CLOSE_SQ, 15},
    {PS2_KEY_BACK, 16},
    // row 4
    {PS2_KEY_SINGLE, 6},
    {PS2_KEY_1, 7},
    {PS2_KEY_2, 8},
    {PS2_KEY_3, 9},
    {PS2_KEY_4, 10},
    {PS2_KEY_5, 11},
    {PS2_KEY_6, 12},
    {PS2_KEY_7, 13},
    {PS2_KEY_8, 14},
    {PS2_KEY_9, 15},
    {PS2_KEY_0, 16},
    {PS2_KEY_MINUS, 17},
    {PS2_KEY_EQUAL, 18},
    {PS2_KEY_BS, 19}
  };
};
typedef struct programState ProgramState;
ProgramState state;

void setup() {
  Serial.begin(DEBUG_PS2 ? 9600 : 31250);

  keyboard.begin(DATA_PIN, IRQ_PIN);
  keyboard.setNoRepeat(1);
  keyboard.setLock(PS2_LOCK_NUM);

  lcd.begin(16, 2);
  lcd.print("Hello, world");

  setScale(PS2_KEY_F1);
}

void loop() {
  if (!keyboard.available()) {
    return;
  }

  uint16_t key = keyboard.read();
  uint8_t keyCode = key & 0xFF;
  int8_t keyIndex = getKeyIndex(keyCode);
  bool isNote = keyIndex > -1;
  int8_t note = isNote ? getMIDINote(keyIndex) : -1;

  if (DEBUG_PS2 && !isKeyUp(key)) {
    Serial.print("keyCode:");
    Serial.println(keyCode);
  }

  if (isKeyUp(key)) {
    if (isNote && !state.hold) {
      sendNoteOff(note);
      state.heldNotes[note] = false;
    }
    
    return;
  }

  if (isNote && !state.heldNotes[note]) {
    sendNoteOn(note);
    state.heldNotes[note] = true;
    state.lastNote = note;
  } else if (isScale(keyCode)) {
    setScale(keyCode);
  } else if (isHold(keyCode)) {
    state.hold = !state.hold;
  } else if (isOctave(keyCode)) {
    setOctave(keyCode);
  } else if (isRoot(keyCode)) {
    setRoot(keyCode);
  } else if (isPanicButton(keyCode)) {
    sendAllNotesOff();
    initHeldKeys();
  }

  if (!isNumLockOn()) {
    keyboard.setLock(PS2_LOCK_NUM);
    state.root = 9;
  }
  
// to prevent taxing the device, only update LCD on octave/root/scale change 
  if (isRoot(keyCode) || isScale(keyCode) ||  isOctave(keyCode)) {
     lcdPrint(keyCode);
   }

}

int8_t getKeyIndex(uint8_t keyCode) {
  for (uint8_t i = 0; i < N_KEYS; i++) {
    if (state.keys[i][0] == keyCode) {
      return state.keys[i][1];
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
  uint8_t scaleLength = getScaleLength(state.scaleIndex);
  updateRowOffsets(scaleLength);

  uint8_t key = 0;
  uint8_t row = 0;
  uint8_t j = 0;

  for (uint8_t i = 0; i < N_KEYS; i++) {
    key = state.keys[i][0];

    if (key == PS2_KEY_A) {
      row = 1;
      j = 0;
    } else if (key == PS2_KEY_Q) {
      row = 2;
      j = 0;
    } else if (key == PS2_KEY_SINGLE) {
      row = 3;
      j = 0;
    }

    state.keys[i][1] = j + state.rowOffsets[row];
    j++;
  }
}

void setOctave(uint8_t keyCode) {
  uint8_t x = state.octaveOffset + (keyCode == PS2_KEY_UP_ARROW ? 1 : -1);
  state.octaveOffset = x < 0 ? 0 : x > 5 ? 5 : x;
}

void setRoot(uint8_t keyCode) {
  for (uint8_t i = 0; i < 12; i++) {
    if (keyCode == ROOT_KEYS[i]) {
      state.root = i;
      return;
    }
  }
}

uint8_t getScaleLength(uint8_t scaleIndex) {
  switch (scaleIndex) {
    case 0:
    case 1:
    case 2:
    case 4:
    case 5:
    case 8:
    case 9:
    case 10:
    case 11:
      return 7;
    case 3:
    case 7:
      return 5;
    case 6:
      return 6;
  }
}

void updateRowOffsets(uint8_t scaleLength) {
  switch (scaleLength) {
    case 7:
      state.rowOffsets[0] = 0;
      state.rowOffsets[1] = 2;
      state.rowOffsets[2] = 4;
      state.rowOffsets[3] = 6;
      break;

    // blues
    case 6:
      state.rowOffsets[0] = 0;
      state.rowOffsets[1] = 2;
      state.rowOffsets[2] = 4;
      state.rowOffsets[3] = 5;
      break;

    // pentatonics
    case 5:
      state.rowOffsets[0] = 0;
      state.rowOffsets[1] = 2;
      state.rowOffsets[2] = 4;
      state.rowOffsets[3] = 4;
      break;
  }
}

uint8_t getMIDINote(uint8_t keyCodeIndex) {
  uint8_t scaleLength = getScaleLength(state.scaleIndex);
  uint8_t pitchClassIndex = keyCodeIndex % scaleLength;
  uint8_t pitch = (getPitchClass(pitchClassIndex) + state.root) % 12;
  uint8_t octave = (floor(keyCodeIndex / scaleLength) * 12) + (state.octaveOffset * 12);

  return octave + pitch;
}

uint8_t getPitchClass(uint8_t index) {
  switch (state.scaleIndex) {
    // F1-F4
    case 0:
      return MAJOR[index];
    case 1:
      return HARMONIC_MAJOR[index];
    case 2:
      return MIXOLYDIAN[index];
    case 3:
      return MAJOR_PENTATONIC[index];

    // F5-F8
    case 4:
      return MINOR[index];
    case 5:
      return HARMONIC_MINOR[index];
    case 6:
      return BLUES_MINOR[index];
    case 7:
      return MINOR_PENTATONIC[index];

    // F9-F12
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
  return keyCode == PS2_KEY_HOME || keyCode == PS2_KEY_KP_ENTER;
}

bool isOctave(uint8_t keyCode) {
  return keyCode == PS2_KEY_UP_ARROW ||
         keyCode == PS2_KEY_DN_ARROW ||
         keyCode == PS2_KEY_KP_PLUS ||
         keyCode == PS2_KEY_KP_MINUS;
}

bool isIncrementalTranspose(uint8_t keyCode) {
  return keyCode == PS2_KEY_L_ARROW || keyCode == PS2_KEY_R_ARROW;
}

bool isRoot(uint8_t keyCode) {
  return keyCode == PS2_KEY_KP1 || // C
         keyCode == PS2_KEY_KP2 || // C#
         keyCode == PS2_KEY_KP3 || // D
         keyCode == PS2_KEY_KP4 || // D#
         keyCode == PS2_KEY_KP5 || // E
         keyCode == PS2_KEY_KP6 || // F
         keyCode == PS2_KEY_KP7 || // F#
         keyCode == PS2_KEY_KP8 || // G
         keyCode == PS2_KEY_KP9 || // G#
         keyCode == PS2_KEY_NUM || // A
         keyCode == PS2_KEY_KP_DIV || // A#
         keyCode == PS2_KEY_KP_TIMES; // B
}

bool isPanicButton(uint8_t keyCode) {
  return keyCode == PS2_KEY_DELETE || keyCode == PS2_KEY_KP_DOT;
}

bool isNumLockOn() {
  return !!(keyboard.getLock() & PS2_LOCK_NUM);
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

void lcdPrint(uint8_t keyCode) {
  if (state.printMode == PRINT_MODE_LIVE) {
    lcd.clear();

    lcd.print(NOTES[state.root]);
    lcd.setCursor(3, 0);
    lcd.print(SCALE_NAMES[state.scaleIndex]);

    lcd.setCursor(0, 1);
    lcd.print(NOTES[state.lastNote % 12]);
    lcd.print((uint8_t) floor(state.lastNote / 12));
    lcd.print(" ");

    lcd.print("Octave:");
    lcd.print(state.octaveOffset);
    lcd.print(" ");

    lcd.setCursor(15, 1);
    lcd.print(state.hold ? "H" : "");
  } else if (state.printMode == PRINT_MODE_DEBUG) {
    lcd.clear();
    lcd.print(keyCode);
  }
}
