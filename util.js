let array = Array(100).fill(null)

// uncomment for-loop in `qwesty.ino` to get an updated list of these values
const keys = [
  90,
  88,
  67,
  86,
  66,
  78,
  77,
  59,
  61,
  62,
  7,
  65,
  83,
  68,
  70,
  71,
  72,
  74,
  75,
  76,
  91,
  64,
  30,
  81,
  87,
  69,
  82,
  84,
  89,
  85,
  73,
  79,
  80,
  93,
  94,
  92,
  49,
  50,
  51,
  52,
  53,
  54,
  55,
  56,
  57,
  48,
  60,
  95,
  28,
]

keys.forEach((number, index) => {
  array[number] = index
})

const sortedKeys = keys.slice().sort()

// this creates the KEYS_TO_INDEX array in `constants.h`
console.log(array.map((n) => (n === null ? 0 : n)).join(',\n'))

// console.log(sortedKeys)

// this is used to create the conditions inside `isNoteOn`
console.log(sortedKeys.map(k => `keyCode == ${k}`).join(' ||\n'))
