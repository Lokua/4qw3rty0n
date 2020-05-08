const fs = require('fs')

const NOTES = ['C', 'C#', 'D', 'D#', 'E', 'F', 'F#', 'G', 'G#', 'A', 'A#', 'B']
const trim = (s) => s.trim()

const text = fs.readFileSync('./scratch.txt', 'utf-8')

const data = text
  .split('\n')
  .filter(Boolean)
  .map(trim)
  .reduce(
    (acc, line) => {
      const [key, name, notes] = line.split('-').map(trim)
      const caps = name.replace(' ', '_').toUpperCase()
      const scale = notes
        .split(',')
        .map(trim)
        .map((note) => NOTES.indexOf(note))

      return {
        scales: [...acc.scales, scale],
        names: acc.names.concat(name),
        caps: acc.caps.concat(caps),
        // scales: acc.scales.concat(
        //   `extern const uint8_t ${caps}[${scale.length}] =` +
        //     ` {${scale.join(', ')}};`,
        // ),
        // names: acc.names.concat(`"${name}"`),
      }
    },
    {
      scales: [],
      names: [],
      caps: [],
    },
  )

// console.log(data.scales.join('\n'))
// console.log(data.names.join(',\n'))
// console.log(data.scales.length, data.names.length)
data.scales.forEach((scale, i) => {
  console.log(`Scale("${data.names[i]}", ${data.caps[i]}, ${scale.length}),`)
})

