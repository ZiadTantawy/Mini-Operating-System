# Mini Operating System Simulator

## Overview
This project is a **Mini Operating System (OS)** implemented in **C** as part of the CSEN 602 – Operating Systems course at GUC.  
The simulator demonstrates key OS concepts, including **process scheduling, memory management, and concurrency control** with mutexes.  
It also features a **GUI for real-time visualization** of processes, queues, memory, and system resources.

---

## Features
- **Process Scheduling**
  - First Come First Serve (FCFS)
  - Round Robin (with user-defined quantum)
  - Multilevel Feedback Queue (MLFQ) with dynamic priorities
- **Memory Management**
  - 60-word fixed-size memory model
  - Stores program code, variables, and Process Control Blocks (PCBs)
  - Each process allocated space for instructions and 3 variables
- **Process Control Blocks (PCBs)**
  - Process ID, State, Priority, Program Counter, Memory Boundaries
- **Concurrency & Synchronization**
  - Implemented **mutexes** for:
    - File access (read/write)
    - User input
    - User output
  - `semWait` / `semSignal` instructions for mutual exclusion
- **Interpreter**
  - Executes custom program instructions:
    - `print`, `assign`, `printFromTo`
    - `writeFile`, `readFile`
    - `semWait`, `semSignal`
- **GUI Visualization**
  - Displays:
    - Process states (Ready, Running, Blocked)
    - Ready & Blocked Queues
    - Memory allocation
    - Mutex/resource status
  - Real-time execution logs and step-by-step simulation
