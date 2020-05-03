extern const uint8_t MAJOR[7] = {0, 2, 4, 5, 7, 9, 11};
extern const uint8_t HARMONIC_MAJOR[7] = {0, 2, 4, 5, 7, 8, 11};
extern const uint8_t MIXOLYDIAN[7] = {0, 2, 4, 5, 7, 9, 10};
extern const uint8_t MAJOR_PENTATONIC[5] = {0, 2, 4, 7, 9};

extern const uint8_t MINOR[7] = {0, 2, 3, 5, 7, 8, 10};
extern const uint8_t HARMONIC_MINOR[7] = {0, 2, 4, 5, 7, 8, 11};
extern const uint8_t BLUES_MINOR[6] = {0, 3, 5, 6, 7, 10};
extern const uint8_t MINOR_PENTATONIC[5] = {0, 3, 5, 7, 10};

extern const uint8_t DORIAN[7] = {0, 2, 3, 5, 7, 9, 10};
extern const uint8_t PHRYGIAN[7] = {0, 1, 3, 5, 7, 8, 10};
extern const uint8_t PHRYGIAN_DOMINANT[7] = {0, 1, 4, 5, 7, 8, 10};
extern const uint8_t LYDIAN_DOMINANT[7] = {0, 2, 4, 6, 7, 9, 10};

extern const uint8_t KEY_MAP[][2] = {
  // 1
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
  // 2
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
  // 3
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
  // 4
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