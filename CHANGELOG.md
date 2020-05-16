# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](c),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [v0.1.0] - 2020-05-15

### Added

### Changed
- Removed right shift from key map; treat both shifts as shift.
- Removed numlock overrides in main loop.
- Remapped ROOT_KEYS to [0...9] and [/, *] (0 = C, * = B).
- Removed support for non-i2c LCD used in prior prototype
- Reordered scales
  - Added Phrygian Dominant
  - Removed Blues Heptatonic
- Increase "screen saver" dim to 5 minutes