<!-- ========================================================= -->
<!-- PROJECT TITLE & BADGES                                    -->
<!-- Replace badges or remove them if you don't use these CI   -->
<!-- ========================================================= -->
# 🧠♟️  MyChessEngine
[![CMake](https://github.com/<user>/<repo>/actions/workflows/cmake.yml/badge.svg)](https://github.com/<user>/<repo>/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

---

<!-- ========================================================= -->
<!-- 1. PROJECT OVERVIEW                                       -->
<!-- ========================================================= -->
## Overview
MyChessEngine is a **from-scratch C++ chess engine**
It implements:

- **Minimax with α–β pruning**
- **Move ordering** (MVV/LVA + killer & history heuristics)
- **Quiescence search** to avoid the horizon effect
- **Zobrist hashing** + **transposition table** (1 Mi entries, replacement scheme = depth preferred)
- **Opening Tables** allowing the engine to play the main lines
- Castling / en-passant / promotion rules fully handled
- Simple **material+piece-square evaluation**
- FEN loader / perft for self-testing

<p align="center">
  <img height="280" src="docs/screenshot.gif" alt="engine in action"/>
</p>

---

<!-- ========================================================= -->
<!-- 2. WHY I BUILT IT (MOTIVATION)                            -->
<!-- ========================================================= -->
## Motivation
Since I had enjoyed the first project so much, I wanted to develop on what I had learned during my first chess engine project in my introductory C++ course. I decided to rebuild an engine from scratch allowing me to develop each aspect of the chess engine and learning from the previous errors we had made. I also wanted to reassess the C++ knowledge I had acquired during the course, making sure I was still familiar with all the concepts and able to use them effectively. Before I started the project, I wanted to set myself clear goals and guidelines for this project, some of which were:

1. **Zero external chess libraries** – build everything, including FEN parsing, on my own.  
2. **No bitboard** - I did not opt for a bit representation of the chess board, instead focused on heavily optimising a more readable and understandable board representation using vectors
3. **Target depth** - I set myself a target depth of 5 in under 10seconds. Considerably more than what we had previously been able to do.

---

<!-- ========================================================= -->
<!-- 3. Afterthoughts                           -->
<!-- ========================================================= -->
##Afterthoughts**
Like with the first chess engine project, I had a lot of fun creating this chess engine. I started off the project remarkably fast, however this followed with a long period of reflection and re-evaluation when I realised that the engine was not fast enough and unable to search deep enough in a reasonable amount of time. I also spent a large amount of time debugging errors and an important lesson I can take from the experience is to not try and go too fast when coding as a simple error can lead to hours lost debugging later on. On a more positive side, I appreciated how I had given myself concise, time-pressured and difficult goals. These goals continued to motivate me throughout the two weeks and made me enjoy the project even more.

<!-- ========================================================= -->
<!-- 4. QUICK START                                            -->
<!-- ========================================================= -->
## Quick Start

```bash
# 1. Clone
git clone https://github.com/<user>/MyChessEngine.git && cd MyChessEngine

# 2. Build (requires CMake ≥3.16 and a C++20 compiler)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j

# 3. Run a simple CLI game vs. engine
./build/mychessengine
