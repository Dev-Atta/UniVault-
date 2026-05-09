# UniVault — University Campus Management System

A console-based C++ application that simulates a university campus using dynamic memory allocation. It manages student enrollment, room assignment, GPA tracking, and semester simulation — all built from scratch using raw pointers and manual heap management.

---

## Features

- **3D Campus Grid** — Departments × Floors × Rooms, fully heap-allocated using `int***`
- **Student Enrollment** — Dynamically growing parallel arrays for names, IDs, GPAs, and statuses
- **Room Assignment** — Random room picker with linear fallback search if the chosen room is occupied
- **Semester Simulation (Time Step)** — Dice-roll based events: GPA progress, steady, warning, or graduation
- **Live Dashboard** — Displays real memory addresses and student info for the current dept/floor view
- **Jump View** — Navigate to any department and floor without running a time step
- **Student Search** — Linear search by student ID with full record and room location display
- **Persistent Storage** — Save and load all data to/from `database.txt`; config stored in `config.txt`

---

## How to Build & Run

```bash
g++ -o univault main.cpp
./univault
```

> On Windows, use `univault.exe` after compiling with MinGW or MSVC.

---

## Configuration

On first run, the program looks for `config.txt`. If not found, it prompts you to enter:

```
Number of Departments
Number of Floors
Number of Rooms per Floor
```

It then saves these values so future runs load them automatically.

**Example `config.txt`:**
```
3
4
5
```
This creates a campus with 3 departments, 4 floors, and 5 rooms each (60 rooms total).

---

## Menu Commands

| Key | Action |
|-----|--------|
| `E` | Enroll a new student |
| `T` | Run one time step (simulate a semester) |
| `J` | Jump to a different dept/floor view |
| `F` | Find a student by ID |
| `S` | Save all data to `database.txt` |
| `X` | Exit the program |

---

## How It Works

### Memory Model

The campus grid is a manually allocated 3D array:
```
int*** grid  →  grid[dept][floor][room]  →  stores Student ID (0 = vacant)
```

Student data is stored in four parallel heap arrays:
```
char** names   →  student names
int*   ids     →  student IDs (starting at 1001)
float* gpas    →  GPAs (starting at 2.00)
int*   statuses →  0 = STUDYING | 1 = WARNING | 2 = GRADUATING
```
Arrays grow with `new` on enrollment and shrink with `delete[]` on graduation.

### Time Step Logic

Each semester, every student rolls a value from 1–10:

| Roll | Outcome |
|------|---------|
| 1–5  | GPA +0.1 (capped at 4.00), status → STUDYING |
| 6–8  | No change (STEADY) |
| 9    | GPA −0.2 (floored at 0.00), status → WARNING |
| 10   | Student graduates, room is freed, arrays shrink |

Bitwise flags track each event per student during the step.

### Save / Load Format

`database.txt` stores one student per line:
```
<num_students>
<id> <name_with_underscores> <gpa> <status> <dept> <floor> <room>
```
Spaces in names are saved as underscores and restored on load.

---

## Project Structure

```
.
├── main.cpp          # Full source code
├── config.txt        # Auto-generated campus configuration
├── database.txt      # Auto-generated student save file
└── README.md
```

---

## Concepts Demonstrated

- Triple pointers and 3D heap allocation
- Manual `new` / `delete[]` memory management
- Dynamic array resizing (grow and shrink)
- Pointer arithmetic instead of array indexing
- Bitwise flags for event tracking
- File I/O with `ifstream` / `ofstream`
- Linear search algorithms
