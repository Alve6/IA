# Ricochet Robots Solver

This project is a **C++ + Raylib** implementation of a Ricochet Robots-style game with built-in search algorithms to solve levels automatically.

The program lets you:
- load a level from the `lvls/` folder,
- play the puzzle manually,
- request a hint,
- auto-solve the current board,
- compare all implemented algorithms,
- export solver results to text files.

## Project structure

```text
IA/
├── Makefile
├── game                  # compiled executable (if already built)
├── include/
│   ├── game.h
│   ├── levels.h
│   └── solver.h
├── lvls/                 # level files
└── src/
    ├── game.cpp
    ├── levels.cpp
    ├── main.cpp
    └── solver.cpp
```

## Requirements

To compile the project you need:
- **g++** with C++17 support
- **Raylib** installed locally
- Linux libraries used by Raylib: `GL`, `pthread`, `dl`, `rt`, and `X11`

The `Makefile` expects Raylib to be installed in:

```bash
$HOME/.local
```

Specifically:
- headers in `~/.local/include`
- libraries in `~/.local/lib`

If your Raylib installation is in a different place, edit this line in the `Makefile`:

```makefile
RAYLIB_PREFIX := $(HOME)/.local
```

and replace it with your own Raylib path.

## How to compile

Open a terminal inside the project folder and run:

```bash
make
```

This builds the executable named:

```bash
game
```

## How to run

You can run the program in either of these ways:

```bash
make run
```

or:

```bash
./game
```

## How to clean compiled files

To remove the executable:

```bash
make clean
```

## How to use the program

### 1. Choose a level
When the program starts, it opens a **menu screen** with one button for each file inside the `lvls/` folder.

- Click a level button such as `1.lvl`, `2.lvl`, etc.
- The selected level is loaded immediately.

### 2. Manual play
Inside the game screen:

- Click a **robot** to select it.
- Then click a **tile in the same row or column** as that robot.
- The robot will slide in that direction until it hits a wall or another robot.

The active robot can be one of:
- Blue
- Red
- Green
- Orange

### 3. Win condition
You win when **any robot reaches the goal tile**.

### 4. Reset the board
Press:

```text
R
```

to restore the current level to its initial state.

## Solver features

The right side of the game window contains solver controls.

### Hint
- Click **Hint**
- Choose one algorithm
- The program shows the **first move** of the solution found

### Solve
- Click **Solve**
- Choose one algorithm
- The program automatically performs the full solution step by step

### Solve to File
- Click **Solve to File**
- Choose one algorithm
- The solution and statistics are saved to a text file inside:

```text
./results/
```

### Compare All
- Click **Compare All**
- The program runs all algorithms on the current state
- It displays performance information on screen

### Compare to File
- Click **Compare to File**
- The program runs all algorithms and saves the comparison to:

```text
./results/
```

## Implemented algorithms

The project currently includes:
- BFS
- DFS
- IDS
- Greedy v1
- Greedy v2
- A*
- Weighted A*

The solver output includes information such as:
- whether the level was solved,
- elapsed time,
- generated states,
- expanded states,
- repeated states,
- visited states,
- maximum frontier size,
- approximate memory usage,
- move count / solution cost.

## Output files

When you export a solution or comparison, the program automatically creates a folder named:

```text
results/
```

Generated filenames include the level name, algorithm, and a timestamp.

Example:

```text
results/4_A__1712950000.txt
```

## Level file format

Each level file in `lvls/` follows this structure:

```text
<width> <height>
<blue_x> <blue_y>
<red_x> <red_y>
<green_x> <green_y>
<orange_x> <orange_y>
<row 1>
<row 2>
...
<row height>
```

### Tile symbols
Each character in the board represents one tile:

- `N` = empty tile
- `S` = south wall
- `E` = east wall
- `L` = south wall + east wall
- `G` = goal
- `T` = goal + south wall
- `D` = goal + east wall
- `W` = goal + south wall + east wall

### Example

```text
4 4
0 0
0 3
3 0
3 3
NNLN
NSNS
ETNS
NNNN
```

This means:
- board size is `4 x 4`
- the four robot starting positions are given in the next four lines
- the remaining lines describe the board layout

## Notes

- Large levels may take much longer with uninformed search algorithms such as **BFS**.
- According to the current project notes, level `7` is much slower with BFS, while **Greedy** is faster but may be suboptimal, and **A*** gives fast optimal solutions.

## Quick start

```bash
make
./game
```

Then:
1. click a level,
2. play manually or use the solver buttons,
3. press `R` to reset whenever needed.
