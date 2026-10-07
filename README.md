# Student Information System (SIS) in C

A modular, terminal-based University Student Information System implemented in C with persistent CSV storage, comprehensive test suites, and strict separation of concerns.

---

## Key Features

- **Student Records Management**: Register students, manage personal details, track academic standing.
- **Course & Curriculum Tracking**: Create courses, define prerequisites, assign credit hours, and manage semester curricula.
- **Enrollment & Grading Engine**: Enroll students in courses, record midterm/final exam scores, and dynamically compute term and cumulative GPAs.
- **Faculty & Professor Management**: Assign instructors to departments and courses.
- **CSV Data Persistence**: Clean, human-readable CSV storage layer with transactional load and save operations (`data/` directory).
- **Automated Test Suite**: Unit and integration test suite verifying every submodule (`tests/` directory).
- **Full Documentation**: Complete user manuals, quick start guides, error code references, and architecture reports (`docs/` directory).

---

## Repository Structure

```text
student-information-system-c/
├── data/              # CSV database files (students, courses, grades, professors, etc.)
├── docs/              # User manual, quick-start guide, and technical reports
├── src/               # Core source code and headers
│   ├── course.[c|h]
│   ├── curriculum.[c|h]
│   ├── enrollment.[c|h]
│   ├── grade.[c|h]
│   ├── menu.[c|h]
│   ├── professor.[c|h]
│   ├── student.[c|h]
│   └── utils.[c|h]
├── tests/             # Automated test runners for each module
├── Makefile           # Unix / Linux build script
├── build.bat          # Windows build script
└── README.md
```

---

## Getting Started

### Build on Linux / macOS
```bash
make
./sis
```

### Build on Windows
```cmd
build.bat
sis.exe
```

### Running Tests
```bash
make test
```

---

## Author
- **İbrahim Halil Doğan** ([@eyyoibo](https://github.com/eyyoibo))
