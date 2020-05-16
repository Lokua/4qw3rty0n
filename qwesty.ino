#define DEBUG_PS2 0

#include <DFRobot_LCD.h>
#include <PS2KeyAdvanced.h>
#include "scales.h"

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

DFRobot_LCD lcd(16, 2);
PS2KeyAdvanced keyboard;

struct programState
{
  uint8_t channel = 0;
  uint8_t heldNotes[128];
  bool hold = false;
  uint8_t scaleIndex = 0;
  Scale *scale = &scales[1];
  int8_t octave = 3;
  uint8_t root = 0;
  uint8_t lastNote = 0;
  bool shift = false;
  uint8_t printMode = PRINT_MODE_LIVE;
  char lcdLine0[16];
  uint8_t pendingHeldNotesCount = 0;
  bool displayOn = false;

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

  lcd.init();
  lcd.setRGB(50, 50, 50);
  lcd.print("4qw3rty0n v0.0.1");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcdUpdateScale();
  lcdUpdateRootAndOctave();
  lcdUpdateNote();
  lcdUpdateHold();
}

const long dimDisplayAtInterval = 300000L;
unsigned long lastEventTime = 0L;

void loop() {
  unsigned long now = millis();

  if (!keyboard.available()) {
    if (state.displayOn && now - lastEventTime >= dimDisplayAtInterval) {
      lcd.noDisplay();
      lcd.setRGB(0, 0, 0);
      state.displayOn = false;
    }

    return;
  }

  lastEventTime = now;

  if (!state.displayOn) {
    lcd.display();
    lcd.setRGB(50, 50, 50);
    state.displayOn = true;
  }

  uint16_t key = keyboard.read();
  uint8_t keyCode = key & 0xFF;
  int8_t keyIndex = getKeyIndex(keyCode);
  bool isNote = keyIndex > -1;
  int8_t note = isNote ? getMIDINote(keyIndex) : -1;

#if (DEBUG_PS2)
  if (!isKeyUp(key)) {
    Serial.print("keyCode:");
    Serial.println(keyCode);
  }
#endif

  if (isKeyUp(key)) {
    if (isShift(keyCode)) {
      state.shift = false;
    } else if (isNote && !state.hold) {
      if (state.pendingHeldNotesCount) {
        int8_t heldNoteIndex = findHeldNoteIndexOfKeyCode(keyCode);

        if (heldNoteIndex > -1) {
          sendNoteOff(heldNoteIndex);
          state.heldNotes[heldNoteIndex] = 0;
          state.pendingHeldNotesCount--;
        }
      } else {
        sendNoteOff(note);
        state.heldNotes[note] = 0;
      }
    }

    return;
  }

  if (isNote && !state.heldNotes[note]) {
    sendNoteOn(note);
    state.heldNotes[note] = keyCode;
    state.lastNote = note;
    lcdUpdateNote();
  } else if (isScale(keyCode)) {
    setScale(keyCode);
    updatePendingHeldNotesCount();
    lcdUpdateScale();
  } else if (isHold(keyCode)) {
    state.hold = !state.hold;
    lcdUpdateHold();
  } else if (isOctave(keyCode)) {
    setOctave(keyCode);
    updatePendingHeldNotesCount();
    lcdUpdateRootAndOctave();
  } else if (isRoot(keyCode)) {
    setRoot(keyCode);
    updatePendingHeldNotesCount();
    lcdUpdateRootAndOctave();
  } else if (isIncrementalTranspose(keyCode)) {
    incrementRoot(keyCode);
    updatePendingHeldNotesCount();
    lcdUpdateRootAndOctave();
  } else if (isPanicButton(keyCode)) {
    sendAllNotesOff();
    initHeldNotes();
  } else if (isShift(keyCode)) {
    state.shift = true;
  }

  if (!isNumLockOn()) {
    keyboard.setLock(PS2_LOCK_NUM);
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

void initHeldNotes() {
  for (uint8_t i = 0; i < 128; i++) {
    state.heldNotes[i] = 0;
  }
}

void updatePendingHeldNotesCount() {
  for (uint8_t i = 0; i < 128; i++) {
    if (state.heldNotes[i]) {
      state.pendingHeldNotesCount++;
    }
  }
}

int8_t findHeldNoteIndexOfKeyCode(uint8_t keyCode) {
  for (uint8_t i = 0; i < 128; i++) {
    if (state.heldNotes[i] == keyCode) {
      return i;
    }
  }

  return -1;
}

void setScale(uint8_t keyCode) {
  uint8_t nScales = 13;
  uint8_t i = state.shift ? nScales : 0;
  uint8_t l = state.shift ? nScales * 2 : nScales;

  for (i; i < l; i++) {
    if (keyCode == scales[i].keyCode) {
      state.scale = &scales[i];
      break;
    }
  }

  uint8_t row4Offset = state.scale->size - 1;

  for (i = 36; i < N_KEYS; i++) {
    state.keys[i][1] = (i - 36) + row4Offset;
  }
}

void setOctave(uint8_t keyCode) {
  int8_t x = state.octave + (keyCode == PS2_KEY_UP_ARROW || keyCode == PS2_KEY_KP_PLUS ? 1 : -1);
  state.octave = x < -2 ? -2 : x > 8 ? 8 : x;
}

void setRoot(uint8_t keyCode) {
  for (uint8_t i = 0; i < 12; i++) {
    if (keyCode == ROOT_KEYS[i]) {
      state.root = i;
      return;
    }
  }
}

void incrementRoot(uint8_t keyCode) {
  if (keyCode == PS2_KEY_L_ARROW) {
    if (state.root == 0) {
      if (state.octave == -2) {
        return;
      }

      state.root = 11;
      state.octave--;
    } else {
      state.root--;
    }
  } else {
    if (state.root == 11) {
      if (state.octave == 8) {
        return;
      }

      state.octave++;
      state.root = 0;
    } else {
      state.root++;
    }
  }
}

uint8_t getMIDINote(uint8_t keyCodeIndex) {
  uint8_t pitchIndex = keyCodeIndex % state.scale->size;
  uint8_t pitch = (state.scale->scale[pitchIndex] % 12) + state.root;
  int8_t octave = ((keyCodeIndex / state.scale->size) * 12) + ((state.octave + 2) * 12);
  uint8_t note = octave + pitch;

  return note > 127 ? 127 : note;
}

bool isKeyUp(uint16_t key) {
  uint16_t status_ = key >> 8;
  return status_ == 0x80 || status_ == 0x81;
}

bool isScale(uint8_t keyCode) {
  return (keyCode >= PS2_KEY_F1 && keyCode <= PS2_KEY_F12) || keyCode == PS2_KEY_ESC;
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
  for (uint8_t i = 0; i < 12; i++) {
    if (keyCode == ROOT_KEYS[i]) {
      return true;
    }
  }

  return false;
}

bool isPanicButton(uint8_t keyCode) {
  return keyCode == PS2_KEY_DELETE || keyCode == PS2_KEY_KP_DOT;
}

bool isShift(uint8_t keyCode) {
  return keyCode == PS2_KEY_L_SHIFT || keyCode == PS2_KEY_R_SHIFT;
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
#if (DEBUG_PS2)
  Serial.print(status);
  Serial.print(", ");
  Serial.print(data1);
  Serial.print(", ");
  Serial.print(data2);
  Serial.print("\n---\n");
#else
  Serial.write(status);
  Serial.write(data1);
  Serial.write(data2);
#endif
}

void lcdUpdateScale() {
  uint8_t scaleNameLength = strlen(state.scale->name);

  for (uint8_t i = 0; i < 16; i++) {
    state.lcdLine0[i] = i < scaleNameLength ? state.scale->name[i] : ' ';
  }

  lcd.setCursor(0, 0);
  lcd.print(state.lcdLine0);
}

void lcdUpdateRootAndOctave() {
  char * note = NOTES[state.root];
  note = state.octave < 0 ? strlwr(note) : strupr(note);

  lcd.setCursor(0, 1);
  lcd.print(note);

  if (strlen(note) == 1) {
    lcd.print("-");
  }

  lcd.print(abs(state.octave));
}

void lcdUpdateNote() {
  char * note = NOTES[state.lastNote % 12];
  int8_t octave = (state.lastNote / 12) - 2;
  note = octave < 0 ? strlwr(note) : strupr(note);

  lcd.setCursor(6, 1);
  lcd.print(note);

  if (strlen(note) == 1) {
    lcd.print("-");
  }

  lcd.print(abs(octave));
}

void lcdUpdateHold() {
  lcd.setCursor(15, 1);
  lcd.print(state.hold ? "H" : " ");
}
