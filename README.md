# Citrus

This project is intended to become a fully-featured chess engine.
It is being worked on as the subject of my Honors Capstone Project.

## Status

Currently, Citrus is in early development. It is not currently functional.

Current progress towards functionality:
- [x] Basic project structure
- [x] Board representation
- [x] Make/unmake moves
- [x] Move generation
- [ ] Search
- [ ] Evaluation
- [ ] UCI Support

## Build Instructions

The project does not fully build/run yet.
Eventually, the build commands will be:

```bash
cmake -S . -B build
cmake --build build
```

This will require installing cmake.

## Documentation

Citrus uses Doxygen to generate documentation.
Eventually, the command to do this will be:

```bash
cmake --build build --target docs
```
