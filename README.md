# NumberStore - Named Pipes IPC

This project implements high-performance, thread-safe number storage system using Named Pipes for IPC communication on Windows.
It contains a daemon and CLI application that communicate using Named Pipes IPC on Windows.
The daemon stores numbers with timestamps, preventing duplicates, while the CLI provides an interactive interface for users to manage the data.

## Features Implemented
- **Daemon Process**: Background service that manages number storage
- **CLI Application**: Interactive command-line interface for users
- **Named Pipes IPC**: Communication between daemon and CLI
- **Thread Safety**: Multiple CLI instances can connect simultaneously
- **Snapshot Optimizations**: Read operations use cached snapshots to minimize lock contention
- **Lock Optimization**: Scoped lock management with minimal critical sections
- **Data Persistence**: Numbers stored with Unix timestamps
- **Input Validation**: Prevents duplicate numbers and invalid input
- **Graceful Shutdown**: Proper cleanup on exit signals

## Compiler and Build System
- **Compiler**: Microsoft Visual C++ (MSVC) with C++17 standard
- **Build System**: CMake 3.15+ generating Visual Studio 2019 project files
- **Platform**: Windows 10/11 (x64 architecture)

## Data Structure Choice and Reasoning

**Primary Storage**: `std::map<uint64_t, int64_t>`

**Why std::map was chosen:**

**Assumptions**: The application was assumed to be read-heavy, with print operations (displaying all numbers) occurring more frequently than insert/delete operations in typical usage scenarios.

1. **Automatic Sorting**: The requirement is to display numbers in ascending order. std::map keeps elements sorted by key automatically, eliminating the need to sort during retrieval.

2. **Fast Lookups**: O(log n) time for insert, delete, and find operations. This is efficient for checking duplicates and removing specific numbers.

3. **Memory Efficiency**: Only stores the required data (number + timestamp pairs) without extra overhead.

4. **Thread Safety**: Combined with std::shared_mutex, it allows multiple readers (CLI print operations) while ensuring exclusive access for writers (insert/delete operations).

**Thread Safety Implementation**: `std::shared_mutex`
- Multiple CLI instances can read simultaneously (print all numbers)
- Write operations (insert/delete) get exclusive access
- Prevents data corruption and race conditions

**Snapshot Optimization Strategy**:
The system uses snapshot-based read optimization based on the assumption that read operations (printing all numbers) are more frequent than write operations (insert/delete). During read operations, a snapshot copy of the data is created while holding a shared lock, then the lock is released immediately. This allows multiple readers to access their own snapshots concurrently without blocking each other or writers. The trade-off is additional memory usage for snapshots, but this significantly reduces lock contention in read-heavy workloads.

**Alternative Considered**: std::unordered_map could be considered for O(1) operations, but the sorting requirement made std::map the better choice since sorting an unordered_map on every print would be O(n log n).

**Hybrid Approach Considered**: A combination of ordered and unordered maps could have been implemented where the snapshot is stored as an ordered map (std::map) for fast sorted retrieval during print operations, while the original data structure remains as an unordered map (std::unordered_map) for O(1) insert/delete operations. This would provide the best of both worlds - fast modifications and efficient sorted access. However, this approach would require maintaining synchronization between two data structures and additional memory overhead, making the single std::map solution more straightforward and maintainable for this application's requirements.


## Build Instructions

### Prerequisites
- Windows 10/11
- CMake 3.15 or later
- Visual Studio 2019 or later with C++ desktop development workload

### Installing CMake
1. Download CMake from https://cmake.org/download/
2. Run the installer and select "Add CMake to system PATH"
3. Verify installation by opening Command Prompt and running:
   ```cmd
   cmake --version
   ```

### Building the Project

#### Method 1: Using Visual Studio (Recommended)
1. **Download/Extract the project** to your desired location
2. **Right-click** on the project folder in Windows Explorer
3. **Select "Open with Visual Studio"**
4. **Open CMakeLists.txt** when prompted (or navigate to it in Solution Explorer)
5. **Press Ctrl+S** to save and trigger the build process
6. Visual Studio will automatically configure and build the project in the `out` folder

#### Method 2: Using Command Line
1. **Open Command Prompt or PowerShell**
2. **Navigate to the project directory**:
   ```cmd
   cd path\to\numberstore1
   ```

3. **Create output directory**:
   ```cmd
   mkdir out
   cd out
   ```

4. **Generate build files**:
   ```cmd
   cmake .. -G "Visual Studio 16 2019" -A x64
   ```

   > **Note:**
   > If you have a different version of Visual Studio installed, change the generator name accordingly. For example:
   >
   > - Visual Studio 2022:
   >   ```cmd
   >   cmake .. -G "Visual Studio 17 2022" -A x64
   >   ```
   > - Visual Studio 2017:
   >   ```cmd
   >   cmake .. -G "Visual Studio 15 2017" -A x64
   >   ```
   >
   > To see all available generators for your system, run:
   > ```cmd
   > cmake --help
   > ```

5. **Build the project**:
   ```cmd
   cmake --build . --config Debug
   ```
   
   Or for optimized release build:
   ```cmd
   cmake --build . --config Release
   ```

### Project Files Included
- **CMakeLists.txt**: Main build configuration
- **All source files**: Complete implementation in C++
- **Header files**: Interface definitions and declarations
- **Build scripts**: Automated build process via CMake

## Running the Application

### Start the Daemon
```cmd
cd build\bin\Debug
numberstore-daemon.exe
```

You should see:
```
Number Store Daemon starting up...
Daemon server started. Waiting for client connections...
Press Ctrl+C to stop the daemon.
```

### Run CLI Client(s)
```cmd
cd build\bin\Debug
numberstore-cli.exe
```

Multiple CLI instances can connect to the same daemon simultaneously.

## Usage Example
```
Number Store - Main Menu
========================================
1. Insert a number
2. Delete a number  
3. Print all numbers
4. Delete all numbers
5. Exit
========================================
Enter your choice (1-5): 1

--- Insert Number ---
Enter a positive integer to insert: 42
✓ Number 42 inserted at timestamp 1755550800

Enter your choice (1-5): 3

--- All Stored Numbers ---
Number:Timestamp
------------------------------
42:1755550800
```

## Architecture Design

**Daemon Components**:
- Connection Manager: Handles multiple client connections
- Command Processor: Processes client requests
- Number Store: Thread-safe data storage
- Signal Handler: Graceful shutdown management

**CLI Components**:
- User Interface: Interactive menu system
- Daemon Client: Communication with daemon
- Input Validation: Ensures data integrity

**IPC Protocol**:
- Named Pipes for reliable Windows IPC
- Message serialization for structured communication
- Error handling and connection management

All operations maintain data consistency and thread safety across concurrent access.
