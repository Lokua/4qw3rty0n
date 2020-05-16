class Scale {
  public:
    uint8_t size;
    uint8_t *scale;
    char *name;
    uint8_t keyCode;
    Scale(char * name_, uint8_t * scale_, uint8_t size_, uint8_t keyCode_) {
      name = name_;
      scale = scale_;
      size = size_;
      keyCode = keyCode_;
    }
};

extern const uint8_t CHROMATIC[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
extern const uint8_t MAJOR[7] = {0, 2, 4, 5, 7, 9, 11};
extern const uint8_t HARMONIC_MAJOR[7] = {0, 2, 4, 5, 7, 8, 11};
extern const uint8_t MAJOR_PENTATONIC[5] = {0, 2, 4, 7, 9};
extern const uint8_t MINOR_PENTATONIC[5] = {0, 3, 5, 7, 10};
extern const uint8_t MINOR[7] = {0, 2, 3, 5, 7, 8, 10};
extern const uint8_t HARMONIC_MINOR[7] = {0, 2, 3, 6, 7, 8, 11};
extern const uint8_t MELODIC_MINOR[7] = {0, 2, 3, 5, 7, 9, 11};
extern const uint8_t MIXOLYDIAN[7] = {0, 2, 4, 5, 7, 9, 10};
extern const uint8_t MIXOLYDIAN_B6[7] = {0, 2, 4, 5, 7, 8, 10};
extern const uint8_t LYDIAN[7] = {0, 2, 4, 6, 7, 9, 11};
extern const uint8_t LYDIAN_DOMINANT[7] = {0, 2, 4, 6, 7, 9, 10};
extern const uint8_t PHRYGIAN[7] = {0, 1, 3, 5, 7, 8, 10};
extern const uint8_t FOLK[8] = {0, 1, 3, 4, 5, 7, 8, 10};
extern const uint8_t BLUES_MAJOR[6] = {0, 2, 3, 4, 7, 9};
extern const uint8_t BLUES_SCALE[6] = {0, 3, 5, 6, 7, 10};
extern const uint8_t BLUES_HEPTATONIC[7] = {0, 2, 3, 5, 6, 7, 10};
extern const uint8_t BLUES_KC[5] = {0, 2, 3, 7, 9};
extern const uint8_t WHOLE_TONE[6] = {0, 2, 4, 6, 8, 10};
extern const uint8_t ALTERED[7] = {0, 1, 3, 4, 6, 8, 10};
extern const uint8_t OCTATONIC[8] = {0, 2, 3, 5, 6, 8, 9, 11};
extern const uint8_t DIMINISHED[8] = {0, 1, 3, 4, 6, 7, 9, 10};
extern const uint8_t BALINESE[5] = {0, 2, 3, 7, 8};
extern const uint8_t CHINESE[5] = {0, 4, 6, 7, 11};
extern const uint8_t EGYPTIAN[5] = {0, 2, 5, 7, 10};
extern const uint8_t JAPANESE[5] = {0, 1, 5, 7, 8};

Scale scales[] = {
  Scale("Chromatic", CHROMATIC, 12, PS2_KEY_ESC),
  Scale("Major", MAJOR, 7, PS2_KEY_F1),
  Scale("Harmonic Major", HARMONIC_MAJOR, 7, PS2_KEY_F2),
  Scale("Major Pentatonic", MAJOR_PENTATONIC, 5, PS2_KEY_F3),
  Scale("Minor Pentatonic", MINOR_PENTATONIC, 5, PS2_KEY_F4),
  Scale("Minor", MINOR, 7, PS2_KEY_F5),
  Scale("Harmonic Minor", HARMONIC_MINOR, 7, PS2_KEY_F6),
  Scale("Melodic Minor", MELODIC_MINOR, 7, PS2_KEY_F7),
  Scale("Mixolydian", MIXOLYDIAN, 7, PS2_KEY_F8),
  Scale("Mixolydian b6", MIXOLYDIAN_B6, 7, PS2_KEY_F9),
  Scale("Lydian", LYDIAN, 7, PS2_KEY_F10),
  Scale("Lydian Dominant", LYDIAN_DOMINANT, 7, PS2_KEY_F11),
  Scale("Phrygian", PHRYGIAN, 7, PS2_KEY_F12),

  // "shift" scales:
  Scale("Folk", FOLK, 8, PS2_KEY_ESC),
  Scale("Blues Major", BLUES_MAJOR, 6, PS2_KEY_F1),
  Scale("Blues Scale", BLUES_SCALE, 6, PS2_KEY_F2),
  Scale("Blues Heptatonic", BLUES_HEPTATONIC, 7, PS2_KEY_F3),
  Scale("Blues KC", BLUES_KC, 5, PS2_KEY_F4),
  Scale("Whole Tone", WHOLE_TONE, 6, PS2_KEY_F5),
  Scale("Altered", ALTERED, 7, PS2_KEY_F6),
  Scale("Octatonic", OCTATONIC, 8, PS2_KEY_F7),
  Scale("Diminished", DIMINISHED, 8, PS2_KEY_F8),
  Scale("Balinese", BALINESE, 5, PS2_KEY_F9),
  Scale("Chinese", CHINESE, 5, PS2_KEY_F10),
  Scale("Egyptian", EGYPTIAN, 5, PS2_KEY_F11),
  Scale("Japanese", JAPANESE, 5, PS2_KEY_F12)
};

extern const uint8_t ROOT_KEYS[12] = {
  PS2_KEY_KP0,
  PS2_KEY_KP1,
  PS2_KEY_KP2,
  PS2_KEY_KP3,
  PS2_KEY_KP4,
  PS2_KEY_KP5,
  PS2_KEY_KP6,
  PS2_KEY_KP7,
  PS2_KEY_KP8,
  PS2_KEY_KP9,
  PS2_KEY_KP_DIV,
  PS2_KEY_KP_TIMES
};

extern const char * NOTES[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
