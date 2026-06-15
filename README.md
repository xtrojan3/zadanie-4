# 🎲 Mini Monopoly (C)

A simplified text-based Monopoly simulation written in C. Players take turns rolling dice, buying properties, paying rent, and going to jail — until one player goes bankrupt.

---

## Features

- 2–4 players supported
- 24-space board with 16 buyable properties across 8 color groups
- Monopoly bonus: doubled rent when a player owns both properties of a color
- Jail system with jail-pass cards
- Bankruptcy detection with automatic winner calculation
- Optional debug flags for step-by-step board/player state output

---

## Build

Requires CMake 3.26+ and a C11-compatible compiler.

```bash
cmake -S . -B build
cmake --build build
```

---

## Usage

```
./z4 [-n NUM_PLAYERS] [-s] [-p] [-g]
```

| Flag | Description |
|------|-------------|
| `-n` | Number of players (default: 2, max: 4) |
| `-s` | Print board state after each turn |
| `-p` | Print player states after each turn |
| `-g` | Print full game state after each turn |

The program reads dice rolls from **stdin**, one integer per line.

```bash
echo -e "3\n5\n2\n6" | ./z4 -n 2 -g
```

---

## Game Rules

- All players start on space 1 (Start) with a fixed amount of cash based on player count
- Each turn a player moves forward by the dice roll value
- **Landing on an unowned property** → must buy it
- **Landing on an opponent's property** → pay rent (doubled if they have a monopoly)
- **Passing or landing on Start** → receive 2 coins
- **Landing on Go to Jail** → move to jail (or use a jail pass)
- **In jail** → pay 1 coin fine before moving on your next turn
- **Bankruptcy** → game ends, winner is the player with the highest total value (cash + property prices)

---

## Project Structure

```
z4/
├── src/
│   ├── z4.c          # main game loop and logic
│   └── monopoly.c    # board, properties, and data definitions
├── include/
│   └── monopoly.h    # structs, enums, macros, extern declarations
└── CMakeLists.txt
```

---

## Notes

- `monopoly.h` must not be modified (fixed interface)
- Board has 24 spaces; wrapping is handled automatically
- Winner is undetermined (`?`) in case of a score tie
