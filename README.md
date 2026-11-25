# Testing System

A desktop exam application in C++/CLI (Windows Forms) with a SQLite database. Students register, take tests by section and category, pause and resume them, and see their grades; an administrator manages users, tests and statistics.

Final exam project of the C++ course at IT-Step Academy, 2025.

## Features

**Students**
- Register with full name, address, phone, login and password; passwords are stored as SHA-256 hashes.
- Pick a section and a category (for example *Mathematics → Algebra*) and answer the questions one by one.
- **Save & Exit** in the middle of a test and **Resume** later from the same question with the same score.
- Results show correct answers, percentage and a grade on a 12-point scale, with the full history of attempts.

**Administrator**
- The first run asks to create the admin account; admin credentials can be changed later.
- View, edit and delete users.
- Add sections, categories and questions, or **import** and **export** whole tests as text files.
- Statistics per test (average grade, attempts, correct answers) and per user, with export of all results.

## Data model

| Table | Holds |
|---|---|
| `Users` | Students and their hashed passwords |
| `Admin` | The administrator account |
| `Tests` | Sections and categories |
| `Questions` | Numbered questions with correct answers |
| `Progress` | Where a paused test stopped, per user and test |
| `Results` | Finished attempts: correct, total, grade, date |

All queries with user input use prepared statements. The database file `students.db` is created next to the executable on first run and filled with sample tests.

## Build and run

Requirements: Windows and Visual Studio 2022 with the **Desktop development with C++** workload and **C++/CLI support**.

1. Open `Exam C++.sln`.
2. Select **Debug | x64** or **Release | x64** (the bundled SQLite library is 64-bit).
3. Build and run (F5). `sqlite3.dll` is copied next to the executable automatically.

## Import format

```
[Section]
{Category}
Question text
Correct answer
Question text
Correct answer
```

## Stack

C++/CLI, Windows Forms, .NET Framework 4.8, SQLite 3 (header, import library and DLL in `sqlite/`; SQLite is in the public domain).
