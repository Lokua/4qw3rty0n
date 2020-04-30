const uint8_t CHANNEL = 0;
const uint8_t NOTE_OFF = 128;
const uint8_t NOTE_ON = 144;
const uint8_t DEFAULT_VELOCITY = 127;

uint8_t demoNotes[] = {63, 67, 69, 70, 74, 75};
uint16_t demoDuration = 118;
uint8_t demoClock = 0;
uint8_t note = demoNotes[0];
uint8_t endPoint = 0;

void setup() {
  Serial.begin(31250);
  sendAllNotesOff(CHANNEL);
}

void loop() {
  sendNoteOn(CHANNEL, note, DEFAULT_VELOCITY);
  delay(demoDuration);

  sendNoteOff(CHANNEL, note);
  delay(demoDuration);

  demoClock = (demoClock + 1) % sizeof(demoNotes);
  note = demoNotes[demoClock];
}

void sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
  sendMIDI(NOTE_ON + channel, note, velocity);
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
