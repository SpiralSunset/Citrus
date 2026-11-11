# Citrus

This project is intended to become a fully-featured chess engine.
It is being worked on as the subject of my Honors Capstone Project.

## Status

Citrus is on hiatus while I finish my capstone report.

Here are some planned additions for the future, in no particular order:
- [x] Iterative deepening and transposition table
- [x] Quiescence search
- [ ] Mate search
- [ ] Magic bitboards
- [ ] Time managing
- [ ] Opening book
- [ ] Improved UCI Support

## Build Instructions

To build Citrus from source, run these commands from the root directory:

```bash
cmake -S . -B build
cmake --build build
```

This will require installing cmake.

## Documentation

Citrus uses Doxygen to generate documentation.
To build the html documentation, run this:

```bash
cmake --build build --target docs
```
