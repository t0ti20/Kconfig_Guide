# Kconfig System: Complete Guide & Illustration

> A comprehensive guide to understanding and using the Kconfig configuration system in C++ projects with CMake.

---

## Table of Contents

1. [What is Kconfig?](#1-what-is-kconfig)
2. [User Interaction Layer](#2-user-interaction-layer)
3. [Kconfig File Structure](#3-kconfig-file-structure)
4. [Configuration Output](#4-configuration-output)
5. [Integration with Build System (CMake)](#5-integration-with-build-system-cmake)
6. [Application Usage](#6-application-usage)
7. [End-to-End Flow](#7-end-to-end-flow-diagram)
8. [Key Concepts](#8-key-concepts)
9. [Complete Example Walkthrough](#9-complete-example-walkthrough)

---

## 1. What is Kconfig?

**Kconfig** is a configuration system originally developed for the Linux kernel, now used in many embedded and systems projects. It provides a structured, hierarchical way to manage build-time configuration options.

```
┌─────────────────────────────────────────────────────────────┐
│                      Kconfig System                         │
│                                                             │
│  "A configuration layer between user input and the build    │
│   system that manages compile-time options"                 │
│                                                             │
│  ✓ Type-safe configuration (bool, int, string, hex, etc.)  │
│  ✓ Dependency management (auto-enable/disable options)     │
│  ✓ Default values with override capability                 │
│  ✓ Interactive menu-driven interface                       │
│  ✓ Build-time (not runtime) configuration                  │
└─────────────────────────────────────────────────────────────┘
```

### Why Use Kconfig?

| Feature | Benefit |
|---------|---------|
| **Type Safety** | Validates input at configuration time (e.g., integers within range) |
| **Dependencies** | Automatically shows/hides options based on other settings |
| **Documentation** | Built-in help text for every option |
| **Consistency** | Single source of truth for all build configurations |
| **Scalability** | Used in Linux kernel (10,000+ options), scales to any project size |

---

## 2. User Interaction Layer

Users interact with Kconfig through terminal-based UI tools:

```
┌─────────────────────────────────────────────────────────────┐
│                    USER INTERACTION                         │
└─────────────────────────────────────────────────────────────┘

    User runs:  cmake --build build --target menuconfig
                       │
                       ▼
    ┌──────────────────────────────────────────────┐
    │  Professional Application Configuration      │ ◄─── menuconfig TUI
    │  ──────────────────────────────────────────  │
    │  → Output Settings                           │
    │  → Timing & Performance                      │
    │  → Advanced Features                         │
    │  → Application Metadata                      │
    │  → Debugging & Development                   │
    │                                              │
    │  <Select>  <Exit>  <Help>  <Save>           │
    └──────────────────────────────────────────────┘
                       │
                       ▼
         User navigates menus, selects options
                       │
                       ▼
              Saves configuration
                       │
                       ▼
           Generates .config file
```

### Configuration Data Types

Kconfig supports several data types:

| Type | Syntax | Example | Use Case |
|------|--------|---------|----------|
| **bool** | `bool "Option name"` | Enable/disable feature | Feature toggles |
| **tristate** | `tristate "Option name"` | y/m/n (yes/module/no) | Kernel modules (less common in userspace) |
| **string** | `string "Option name"` | Text input | Messages, file paths |
| **int** | `int "Option name"` | Integer value | Counts, sizes |
| **hex** | `hex "Option name"` | Hexadecimal value | Memory addresses, buffer sizes |
| **choice** | `choice ... endchoice` | One of many options | Color selection, modes |

---

## 3. Kconfig File Structure

### Basic Anatomy

```kconfig
mainmenu "Application Configuration"    ◄─── Top-level title

menu "Output Settings"                  ◄─── Menu grouping
    
    config ENABLE_OUTPUT                ◄─── Configuration symbol
        bool "Enable console output"    ◄─── Type + prompt text
        default y                       ◄─── Default value
        help                            ◄─── Documentation
          Enable or disable all console output.
          
    config GREETING_MESSAGE
        string "Primary greeting message"
        default "Hello, World!"
        depends on ENABLE_OUTPUT        ◄─── Dependency (only shown if parent is enabled)
        help
          The main greeting text.

endmenu                                 ◄─── Close menu
```

### Hierarchical Structure

```
Kconfig (root)
│
├─ menu "Output Settings"
│   ├─ config ENABLE_OUTPUT (bool)
│   ├─ config GREETING_MESSAGE (string) ──► depends on ENABLE_OUTPUT
│   ├─ config USE_COLORS (bool)
│   └─ choice "Text color scheme"
│       ├─ config COLOR_RED (bool)
│       ├─ config COLOR_GREEN (bool)
│       └─ config COLOR_BLUE (bool)
│
├─ menu "Timing & Performance"
│   ├─ config REPEAT_COUNT (int, range 1-100)
│   └─ config DELAY_MS (int) ──────────────► depends on REPEAT_COUNT > 1
│
└─ menu "Advanced Features"
    └─ config ENABLE_LOGGING (bool)
        └─ config LOG_FILENAME (string) ──► depends on ENABLE_LOGGING
```

### Key Kconfig Keywords

| Keyword | Purpose | Example |
|---------|---------|---------|
| `config` | Define a configuration symbol | `config MY_FEATURE` |
| `bool` / `int` / `string` / `hex` | Data type + prompt | `bool "Enable feature"` |
| `default` | Default value | `default y` or `default "Hello"` |
| `depends on` | Show only if condition is true | `depends on PARENT_FEATURE` |
| `select` | Auto-enable another option | `select DEPENDENCY` |
| `range` | Valid range for int/hex | `range 1 100` |
| `choice` / `endchoice` | Mutually exclusive options | Color selection |
| `menu` / `endmenu` | Organize options into groups | `menu "Settings"` |
| `if` / `endif` | Conditional section | `if DEBUG_MODE ... endif` |
| `help` | Documentation (indented) | Multi-line help text |

### Dependency Examples

```kconfig
# Example 1: Simple dependency
config PARENT
    bool "Parent feature"

config CHILD
    bool "Child feature"
    depends on PARENT        # Only shown if PARENT=y

# Example 2: Complex dependency
config DELAY_MS
    int "Delay (ms)"
    depends on REPEAT_COUNT > 1   # Conditional on value

# Example 3: Multiple dependencies
config ADVANCED_LOGGING
    bool "Advanced logging"
    depends on ENABLE_LOGGING && DEBUG_MODE

# Example 4: Auto-selection
config USE_COLORS
    bool "Enable colors"
    select ANSI_SUPPORT      # Automatically enables ANSI_SUPPORT
```

---

## 4. Configuration Output

Kconfig generates two critical output files:

### 4.1 `.config` File

**Location**: Project root (`.config`)  
**Format**: Shell-style variable assignments  
**Purpose**: Human-readable configuration storage

```bash
#
# Output Settings
#
CONFIG_ENABLE_OUTPUT=y
CONFIG_GREETING_MESSAGE="Hello, World!"
CONFIG_SECONDARY_MESSAGE=""
CONFIG_USE_COLORS=y
CONFIG_COLOR_GREEN=y
# CONFIG_COLOR_RED is not set
# CONFIG_COLOR_BLUE is not set
CONFIG_BOLD_TEXT=n

#
# Timing & Performance
#
CONFIG_REPEAT_COUNT=1
CONFIG_BUFFER_SIZE=0x1000
```

**Key Points:**
- Boolean `y` means enabled, `n` or `# CONFIG_X is not set` means disabled
- Strings use double quotes: `"Hello"`
- Integers can be decimal or hex: `1` or `0x1000`
- Comments show disabled options

### 4.2 `autoconf.h` Header File

**Location**: `build/include/generated/autoconf.h`  
**Format**: C preprocessor macros  
**Purpose**: Used by C/C++ source code

```c
/* Generated by kconfiglib - DO NOT EDIT */
#define CONFIG_ENABLE_OUTPUT 1
#define CONFIG_GREETING_MESSAGE "Hello, World!"
#define CONFIG_SECONDARY_MESSAGE ""
#define CONFIG_USE_COLORS 1
#define CONFIG_COLOR_GREEN 1
#define CONFIG_BOLD_TEXT 0
#define CONFIG_REPEAT_COUNT 1
#define CONFIG_BUFFER_SIZE 0x1000
#define CONFIG_APP_VERSION "1.0.0"
```

**Key Points:**
- Boolean `y` → `#define CONFIG_X 1`
- Boolean `n` → `#define CONFIG_X 0` (or undefined)
- Strings preserved as string literals
- All symbols prefixed with `CONFIG_`

### Transformation Flow

```
┌─────────────────┐
│  Kconfig        │  Defines structure, types, defaults
│  (input spec)   │
└────────┬────────┘
         │
         ▼
    [menuconfig]  ◄────── User edits values
         │
         ▼
┌─────────────────┐
│  .config        │  Stores user selections
│  (shell format) │  CONFIG_FEATURE=y
└────────┬────────┘
         │
         ▼
    [genconfig]  ◄────── Kconfig → C header generator
         │
         ▼
┌─────────────────┐
│  autoconf.h     │  C preprocessor macros
│  (C header)     │  #define CONFIG_FEATURE 1
└─────────────────┘
```

---

## 5. Integration with Build System (CMake)

### CMake Integration Architecture

```
┌──────────────────────────────────────────────────────────────┐
│                    CMakeLists.txt                            │
└──────────────────────────────────────────────────────────────┘

Step 1: Bootstrap (Auto-generate .config if missing)
   ┌───────────────────────────────────────────────┐
   │ if(NOT EXISTS ".config")                      │
   │     execute_process(                          │
   │         COMMAND alldefconfig Kconfig          │
   │     )                                         │
   │ endif()                                       │
   └───────────────────────────────────────────────┘
                         │
                         ▼
                    .config created

Step 2: Add menuconfig target (Interactive configuration)
   ┌───────────────────────────────────────────────┐
   │ add_custom_target(menuconfig                  │
   │     COMMAND menuconfig Kconfig                │
   │     USES_TERMINAL                             │
   │ )                                             │
   └───────────────────────────────────────────────┘
                         │
   User runs: cmake --build build --target menuconfig
                         │
                         ▼
                .config updated by user

Step 3: Generate autoconf.h (Build-time generation)
   ┌───────────────────────────────────────────────┐
   │ add_custom_command(                           │
   │     OUTPUT autoconf.h                         │
   │     COMMAND genconfig --header-path autoconf.h│
   │     DEPENDS .config Kconfig                   │
   │ )                                             │
   │                                               │
   │ add_custom_target(kconfig_headers             │
   │     DEPENDS autoconf.h                        │
   │ )                                             │
   └───────────────────────────────────────────────┘
                         │
          Runs whenever .config or Kconfig changes
                         │
                         ▼
              autoconf.h generated

Step 4: Link to main executable
   ┌───────────────────────────────────────────────┐
   │ add_executable(app src/main.cpp)              │
   │ add_dependencies(app kconfig_headers)         │
   │ target_include_directories(app PRIVATE        │
   │     ${CMAKE_BINARY_DIR}/include/generated     │
   │ )                                             │
   └───────────────────────────────────────────────┘
                         │
                         ▼
            main.cpp can #include "autoconf.h"
```

### Key CMake Concepts

| CMake Command | Purpose |
|---------------|---------|
| `execute_process(COMMAND alldefconfig)` | Generate `.config` with defaults at configure time |
| `add_custom_target(menuconfig)` | Create phony target to run menuconfig UI |
| `add_custom_command(OUTPUT autoconf.h)` | Define how to generate header from `.config` |
| `add_dependencies(app kconfig_headers)` | Ensure header is generated before compiling app |
| `target_include_directories()` | Add generated header location to include path |

### Build Commands

```bash
# Initial setup (first time only)
cmake -S . -B build

# Configure options (opens menuconfig UI)
cmake --build build --target menuconfig

# Build application (regenerates autoconf.h if .config changed)
cmake --build build

# Run application
./build/app
```

---

## 6. Application Usage

### How C++ Code Uses Kconfig

```cpp
#include "autoconf.h"  // Generated header with CONFIG_* macros

int main() {
    // ═══════════════════════════════════════════════════════
    // 1. Conditional Compilation (Feature Toggle)
    // ═══════════════════════════════════════════════════════
    
    #ifdef CONFIG_ENABLE_OUTPUT
        printf("Output is enabled!\n");
    #endif
    
    // ═══════════════════════════════════════════════════════
    // 2. Use String Configuration
    // ═══════════════════════════════════════════════════════
    
    printf("%s\n", CONFIG_GREETING_MESSAGE);  // "Hello, World!"
    
    // ═══════════════════════════════════════════════════════
    // 3. Use Integer Configuration
    // ═══════════════════════════════════════════════════════
    
    for (int i = 0; i < CONFIG_REPEAT_COUNT; i++) {
        // Loop configured number of times
    }
    
    // ═══════════════════════════════════════════════════════
    // 4. Choice Groups (Mutually Exclusive Options)
    // ═══════════════════════════════════════════════════════
    
    #ifdef CONFIG_COLOR_RED
        #define ANSI_COLOR "\033[31m"
    #elif defined(CONFIG_COLOR_GREEN)
        #define ANSI_COLOR "\033[32m"
    #elif defined(CONFIG_COLOR_BLUE)
        #define ANSI_COLOR "\033[34m"
    #endif
    
    // ═══════════════════════════════════════════════════════
    // 5. Nested Dependencies (Only defined if parent enabled)
    // ═══════════════════════════════════════════════════════
    
    #ifdef CONFIG_DELAY_MS  // Only exists if REPEAT_COUNT > 1
        sleep(CONFIG_DELAY_MS);
    #endif
    
    // ═══════════════════════════════════════════════════════
    // 6. Buffer Sizing (Hex Values)
    // ═══════════════════════════════════════════════════════
    
    char buffer[CONFIG_BUFFER_SIZE];  // Size from Kconfig
    
    return 0;
}
```

### Real-World Application Patterns

```cpp
// ═══════════════════════════════════════════════════════
// Pattern 1: Debug vs Release Builds
// ═══════════════════════════════════════════════════════

void log_debug(const char* msg) {
    #ifdef CONFIG_DEBUG_MODE
        fprintf(stderr, "[DEBUG] %s\n", msg);
    #endif
}

// ═══════════════════════════════════════════════════════
// Pattern 2: Feature Selection
// ═══════════════════════════════════════════════════════

void save_data(const char* data) {
    #ifdef CONFIG_ENABLE_LOGGING
        std::ofstream file(CONFIG_LOG_FILENAME, std::ios::app);
        file << data;
    #endif
}

// ═══════════════════════════════════════════════════════
// Pattern 3: Platform-Specific Code
// ═══════════════════════════════════════════════════════

void initialize() {
    #ifdef CONFIG_USE_COLORS
        // Enable ANSI color support
        enable_ansi_colors();
    #endif
}

// ═══════════════════════════════════════════════════════
// Pattern 4: Compile-Time Assertions
// ═══════════════════════════════════════════════════════

#ifdef CONFIG_ASSERT_ENABLED
    if (len > CONFIG_MAX_MESSAGE_LENGTH) {
        fprintf(stderr, "Buffer overflow detected!\n");
        abort();
    }
#endif
```

---

## 7. End-to-End Flow Diagram

### Complete System Architecture

```
┌─────────────────────────────────────────────────────────────────────────┐
│                      KCONFIG SYSTEM: END-TO-END FLOW                    │
└─────────────────────────────────────────────────────────────────────────┘


 ╔═══════════════════════╗
 ║   DEVELOPER           ║
 ║   (Configuration      ║
 ║    Designer)          ║
 ╚═══════════════════════╝
           │
           │ Creates/edits
           ▼
 ┌───────────────────────┐
 │  Kconfig              │ ◄──── Configuration Schema
 │  ─────────────────    │       • Types (bool, int, string, hex)
 │  mainmenu "..."       │       • Defaults
 │  config FEATURE       │       • Dependencies (depends on)
 │    bool "..."         │       • Help text
 │    default y          │       • Ranges
 │    depends on X       │
 └───────────────────────┘
           │
           │ Read by
           ▼
 ╔═══════════════════════╗
 ║   USER / BUILDER      ║
 ║   (Configures build)  ║
 ╚═══════════════════════╝
           │
           │ Runs: cmake --build build --target menuconfig
           ▼
 ┌───────────────────────┐
 │  menuconfig (TUI)     │ ◄──── Interactive Terminal UI
 │  ─────────────────    │       • Navigate menus
 │  [*] Enable output    │       • Toggle options
 │  (Hello) Greeting     │       • Enter values
 │  [*] Use colors       │       • Read help
 │    (X) Green          │       • Save configuration
 │    ( ) Red            │
 └───────────────────────┘
           │
           │ Saves selections
           ▼
 ┌───────────────────────┐
 │  .config              │ ◄──── User Configuration (Shell format)
 │  ─────────────────    │
 │  CONFIG_ENABLE=y      │       • Human-readable
 │  CONFIG_GREETING="Hi" │       • Version controlled
 │  CONFIG_USE_COLORS=y  │       • Persistent storage
 │  CONFIG_COLOR_GREEN=y │
 └───────────────────────┘
           │
           │ Processed by
           ▼
 ┌───────────────────────┐
 │  genconfig            │ ◄──── Kconfig→C Header Generator
 │  (kconfiglib tool)    │
 └───────────────────────┘
           │
           │ Generates
           ▼
 ┌───────────────────────┐
 │  autoconf.h           │ ◄──── Generated C Header
 │  ─────────────────    │
 │  #define CONFIG_      │       • Preprocessor macros
 │    ENABLE_OUTPUT 1    │       • Used by #ifdef
 │  #define CONFIG_      │       • Values embedded in code
 │    GREETING "Hi"      │
 └───────────────────────┘
           │
           │ Included in
           ▼
 ┌───────────────────────┐
 │  CMakeLists.txt       │ ◄──── Build System Integration
 │  ─────────────────    │
 │  add_custom_command(  │       • Triggers genconfig
 │    OUTPUT autoconf.h  │       • Sets up dependencies
 │    DEPENDS .config    │       • Configures include paths
 │  )                    │
 │  add_executable(app)  │
 └───────────────────────┘
           │
           │ Builds
           ▼
 ┌───────────────────────┐
 │  src/main.cpp         │ ◄──── Application Source Code
 │  ─────────────────    │
 │  #include "autoconf.h"│       • Reads CONFIG_* macros
 │  #ifdef CONFIG_ENABLE │       • Conditional compilation
 │    printf("%s",       │       • Embeds configured values
 │      CONFIG_GREETING);│
 │  #endif               │
 └───────────────────────┘
           │
           │ Compiled by
           ▼
 ┌───────────────────────┐
 │  C++ Compiler         │ ◄──── Build Process
 │  (g++/clang++)        │
 └───────────────────────┘
           │
           │ Produces
           ▼
 ┌───────────────────────┐
 │  app (executable)     │ ◄──── Final Binary
 │  ─────────────────    │
 │  • Config baked in    │       • No runtime parsing
 │  • No .config needed  │       • Optimized by compiler
 │  • Ready to ship      │       • Single artifact
 └───────────────────────┘
           │
           │ Executed
           ▼
 ╔═══════════════════════╗
 ║   RUNTIME OUTPUT      ║
 ║   ─────────────────   ║ ◄──── User Sees Results
 ║   $ ./app             ║
 ║   Hi                  ║       • Configured behavior
 ║                       ║       • No config files needed
 ╚═══════════════════════╝


┌─────────────────────────────────────────────────────────────────────────┐
│  KEY POINTS:                                                            │
│                                                                         │
│  ✓ Configuration happens at BUILD TIME, not runtime                    │
│  ✓ Final binary contains all config values (no .config needed)         │
│  ✓ Changes require rebuild (edit .config → regenerate → recompile)     │
│  ✓ Type-safe: invalid configs caught before compilation                │
│  ✓ Optimized: compiler can inline/eliminate code based on config       │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## 8. Key Concepts

### 8.1 Build-Time vs Runtime Configuration

```
┌────────────────────────────────────┬────────────────────────────────────┐
│     BUILD-TIME (Kconfig)           │      RUNTIME (Config Files)        │
├────────────────────────────────────┼────────────────────────────────────┤
│ Configuration baked into binary    │ Configuration read at startup      │
│ Change requires recompilation      │ Change requires app restart        │
│ #ifdef eliminates unused code      │ All code paths included            │
│ Type-checked at menuconfig time    │ Type errors at runtime             │
│ Optimized by compiler              │ Runtime parsing overhead           │
│ Smaller binary (dead code removed) │ Larger binary (all code present)   │
│                                    │                                    │
│ Example:                           │ Example:                           │
│   #ifdef CONFIG_DEBUG              │   if (config["debug"] == true) {   │
│     printf("Debug: %s", msg);      │     printf("Debug: %s", msg);      │
│   #endif                           │   }                                │
│                                    │                                    │
│ Use when:                          │ Use when:                          │
│ • Feature toggling                 │ • User preferences                 │
│ • Platform variants                │ • Server addresses                 │
│ • Debug vs release                 │ • Runtime behavior                 │
│ • Different builds for customers   │ • No rebuild possible              │
└────────────────────────────────────┴────────────────────────────────────┘
```

### 8.2 Dependency Management

Kconfig automatically manages option visibility and dependencies:

```kconfig
config PARENT_FEATURE
    bool "Parent feature"
    default y

config CHILD_FEATURE
    bool "Child feature"
    depends on PARENT_FEATURE  # Only shown if PARENT_FEATURE=y

config GRANDCHILD_FEATURE
    bool "Grandchild feature"
    depends on CHILD_FEATURE   # Requires both PARENT and CHILD
```

**Behavior:**
- If `PARENT_FEATURE=n`, `CHILD_FEATURE` is hidden in menuconfig
- If `CHILD_FEATURE=n`, `GRANDCHILD_FEATURE` is hidden
- Disabling parent automatically disables children
- Enabling child automatically prompts to enable parent

### 8.3 Default Values

```kconfig
config MY_STRING
    string "Some text"
    default "hello"              # Static default

config MY_CONDITIONAL_DEFAULT
    int "Some number"
    default 10 if DEBUG_MODE     # Conditional default
    default 5                    # Fallback default

config MY_INHERITED_DEFAULT
    bool "Feature"
    default y if PLATFORM_X      # Platform-specific default
    default n
```

### 8.4 Scalability

Kconfig scales from tiny embedded projects to massive systems:

| Project | Options | Kconfig Files | Complexity |
|---------|---------|---------------|------------|
| **This Demo** | ~30 | 1 | Single file |
| **Embedded Firmware** | 100-500 | 5-20 | Multiple files with `source` |
| **Zephyr RTOS** | ~2,000 | 100+ | Hierarchical architecture |
| **Linux Kernel** | ~10,000 | 1,000+ | Highly modular system |

**Scaling Techniques:**

```kconfig
# Main Kconfig
mainmenu "My Project"

source "drivers/Kconfig"      # Include driver options
source "network/Kconfig"      # Include network options
source "filesystem/Kconfig"   # Include filesystem options

# drivers/Kconfig
menu "Driver Options"
    source "drivers/usb/Kconfig"
    source "drivers/serial/Kconfig"
endmenu
```

### 8.5 Common Patterns

#### Pattern 1: Feature Flags

```kconfig
config ENABLE_FEATURE_X
    bool "Enable experimental feature X"
    default n
    help
      Enable the new feature X (unstable).
```

```cpp
#ifdef CONFIG_ENABLE_FEATURE_X
    use_feature_x();
#endif
```

#### Pattern 2: Mutually Exclusive Options

```kconfig
choice
    prompt "Build type"
    default BUILD_RELEASE

config BUILD_DEBUG
    bool "Debug"

config BUILD_RELEASE
    bool "Release"

config BUILD_PROFILE
    bool "Profile"

endchoice
```

```cpp
#ifdef CONFIG_BUILD_DEBUG
    set_optimization("-O0 -g");
#elif defined(CONFIG_BUILD_RELEASE)
    set_optimization("-O3");
#elif defined(CONFIG_BUILD_PROFILE)
    set_optimization("-O2 -pg");
#endif
```

#### Pattern 3: Buffer Sizing

```kconfig
config MAX_CONNECTIONS
    int "Maximum simultaneous connections"
    range 1 1000
    default 100

config BUFFER_SIZE
    hex "Network buffer size"
    default 0x1000
```

```cpp
Connection connections[CONFIG_MAX_CONNECTIONS];
uint8_t buffer[CONFIG_BUFFER_SIZE];
```

#### Pattern 4: String Configuration

```kconfig
config DEFAULT_USERNAME
    string "Default username"
    default "admin"

config LOG_PREFIX
    string "Log message prefix"
    default "[APP]"
```

```cpp
printf("%s User: %s\n", CONFIG_LOG_PREFIX, CONFIG_DEFAULT_USERNAME);
```

---

## 9. Complete Example Walkthrough

Let's walk through configuring and building this demo project.

### Step 1: Initial Project State

```bash
$ ls
CMakeLists.txt  Kconfig  src/

$ cat Kconfig
mainmenu "Professional Application Configuration"
menu "Output Settings"
config ENABLE_OUTPUT
    bool "Enable console output"
    default y
...
```

### Step 2: Initial Configuration

```bash
# Generate default .config
$ cmake -S . -B build
-- [Kconfig] No .config found — generating defaults with alldefconfig
-- Configuring done
-- Generating done

$ cat .config
CONFIG_ENABLE_OUTPUT=y
CONFIG_GREETING_MESSAGE="Hello, World!"
CONFIG_USE_COLORS=y
CONFIG_COLOR_GREEN=y
CONFIG_REPEAT_COUNT=1
CONFIG_BUFFER_SIZE=0x1000
```

### Step 3: Customize Configuration

```bash
# Open menuconfig
$ cmake --build build --target menuconfig

# ┌─ Professional Application Configuration ─┐
# │ → Output Settings                         │
# │ → Timing & Performance                    │
# │ → Advanced Features                       │
# └───────────────────────────────────────────┘

# Navigate to "Output Settings" → Enter

# ┌─ Output Settings ─────────────────────────┐
# │ [*] Enable console output                 │
# │ (Hello, World!) Primary greeting message  │
# │ () Secondary message (optional)           │
# │ [*] Use colored output                    │
# │   (X) Green     ◄── Selected              │
# │   ( ) Red                                 │
# │   ( ) Blue                                │
# │ [ ] Use bold/bright text                  │
# └───────────────────────────────────────────┘

# User changes:
# - Greeting: "Welcome to Kconfig Demo!"
# - Secondary: "Learning configuration management"
# - Color: Cyan
# - Bold: Yes

# Save and exit

$ cat .config
CONFIG_ENABLE_OUTPUT=y
CONFIG_GREETING_MESSAGE="Welcome to Kconfig Demo!"
CONFIG_SECONDARY_MESSAGE="Learning configuration management"
CONFIG_USE_COLORS=y
CONFIG_COLOR_CYAN=y      # Changed
CONFIG_BOLD_TEXT=y       # Changed
```

### Step 4: Build Process

```bash
# Build (automatically regenerates autoconf.h)
$ cmake --build build

# CMake detects .config changed → runs genconfig
[Kconfig] Generating autoconf.h from .config
[ 50%] Building CXX object CMakeFiles/app.dir/src/main.cpp.o
[100%] Linking CXX executable app

$ cat build/include/generated/autoconf.h
#define CONFIG_ENABLE_OUTPUT 1
#define CONFIG_GREETING_MESSAGE "Welcome to Kconfig Demo!"
#define CONFIG_SECONDARY_MESSAGE "Learning configuration management"
#define CONFIG_USE_COLORS 1
#define CONFIG_COLOR_CYAN 1
#define CONFIG_BOLD_TEXT 1
#define CONFIG_REPEAT_COUNT 1
#define CONFIG_BUFFER_SIZE 0x1000
```

### Step 5: Application Behavior

```bash
$ ./build/app
Welcome to Kconfig Demo!            # ◄── In cyan + bold
Learning configuration management   # ◄── In cyan + bold
```

```cpp
// How it works internally:

#include "autoconf.h"  // Includes generated header

int main() {
    // ANSI_COLOR resolves to "\033[36m" (cyan)
    // ANSI_BOLD resolves to "\033[1m"
    // Because CONFIG_USE_COLORS=1, CONFIG_COLOR_CYAN=1, CONFIG_BOLD_TEXT=1
    
    printf("%s%s%s%s\n",
           ANSI_BOLD,                         // "\033[1m"
           ANSI_COLOR,                        // "\033[36m"
           CONFIG_GREETING_MESSAGE,            // "Welcome to Kconfig Demo!"
           ANSI_RESET);                       // "\033[0m"
    
    printf("%s%s%s%s\n",
           ANSI_BOLD,
           ANSI_COLOR,
           CONFIG_SECONDARY_MESSAGE,           // "Learning configuration..."
           ANSI_RESET);
}
```

### Step 6: Advanced Configuration

```bash
# Enable more features
$ cmake --build build --target menuconfig

# Navigate to "Timing & Performance"
# - REPEAT_COUNT: 3
# - DELAY_MS: 500  (only appears because REPEAT_COUNT > 1)

# Navigate to "Advanced Features"
# - [*] Enable file logging
# - LOG_FILENAME: "demo.log"

# Navigate to "Debugging & Development"
# - [*] Enable debug mode
# - [*] Print all config values at startup

# Save and rebuild
$ cmake --build build
$ ./build/app

=== Configuration Dump ===
ENABLE_OUTPUT         = true
GREETING_MESSAGE      = "Welcome to Kconfig Demo!"
SECONDARY_MESSAGE     = "Learning configuration management"
USE_COLORS            = true
BOLD_TEXT             = true
REPEAT_COUNT          = 3
DELAY_MS              = 500
BUFFER_SIZE           = 0x1000 (4096 bytes)
ENABLE_LOGGING        = true
LOG_FILENAME          = "demo.log"
DEBUG_MODE            = true
==========================

Welcome to Kconfig Demo!
Learning configuration management
[wait 500ms]
Welcome to Kconfig Demo!
Learning configuration management
[wait 500ms]
Welcome to Kconfig Demo!
Learning configuration management

$ cat demo.log
Welcome to Kconfig Demo!
Learning configuration management
Welcome to Kconfig Demo!
Learning configuration management
Welcome to Kconfig Demo!
Learning configuration management
```

---

## 10. Comparison with Other Configuration Systems

| System | Scope | Type Safety | UI | Generated Output |
|--------|-------|-------------|----|--------------------|
| **Kconfig** | Build-time | Strong | Terminal (menuconfig) | C headers + .config |
| **CMake options** | Build-time | Weak | Command-line `-D` | CMake variables |
| **JSON/YAML** | Runtime | None | Text editor | Parsed at runtime |
| **Environment vars** | Runtime | None | Shell export | Read at runtime |
| **Command-line args** | Runtime | Weak | CLI flags | Parsed at runtime |

---

## 11. Best Practices

### ✅ DO

- **Use meaningful names**: `CONFIG_ENABLE_LOGGING` not `CONFIG_LOG`
- **Provide defaults**: Every option should have a sensible default
- **Write help text**: Explain what, why, and how for every option
- **Group related options**: Use `menu` to organize hierarchically
- **Use dependencies**: Hide irrelevant options with `depends on`
- **Use ranges**: Constrain integers to valid ranges (`range 1 100`)
- **Version control .config**: Check in default .config for your project

### ❌ DON'T

- **Don't use runtime parsing**: Defeats the purpose of build-time config
- **Don't skip help text**: Future you will forget what options do
- **Don't use overly generic names**: `CONFIG_ENABLE` is too vague
- **Don't forget dependencies**: Options without proper deps confuse users
- **Don't mix runtime and build-time config**: Keep them separate
- **Don't hardcode values**: Use Kconfig instead of `#define` in code

---

## 12. Troubleshooting

### Problem: `alldefconfig: command not found`

**Solution**: Install kconfiglib

```bash
pip install kconfiglib
```

### Problem: Changes not reflected in binary

**Solution**: Rebuild after changing .config

```bash
cmake --build build --target menuconfig  # Change settings
cmake --build build                       # Must rebuild
```

### Problem: `CONFIG_X` undeclared in C++ code

**Solution**: Check if option has dependencies

```kconfig
config DELAY_MS
    int "Delay"
    depends on REPEAT_COUNT > 1  # Only defined if condition met
```

```cpp
// Guard usage:
#ifdef CONFIG_DELAY_MS
    sleep(CONFIG_DELAY_MS);
#endif
```

### Problem: Circular dependency error

**Cause**: Option A depends on B, B depends on A

**Solution**: Restructure dependencies or use `select` instead

---

## 13. Further Reading

- **Kconfig Language**: [https://www.kernel.org/doc/html/latest/kbuild/kconfig-language.html](https://www.kernel.org/doc/html/latest/kbuild/kconfig-language.html)
- **kconfiglib Documentation**: [https://github.com/ulfalizer/Kconfiglib](https://github.com/ulfalizer/Kconfiglib)
- **Linux Kernel Kconfig**: Browse `linux/Kconfig` for real-world examples
- **Zephyr RTOS**: [https://docs.zephyrproject.org/latest/build/kconfig/index.html](https://docs.zephyrproject.org/latest/build/kconfig/index.html)

---

## Summary

**Kconfig is a powerful build-time configuration system that:**

1. ✅ Provides type-safe, validated configuration options
2. ✅ Offers an interactive menu-driven interface (menuconfig)
3. ✅ Manages dependencies automatically
4. ✅ Generates C headers for compile-time use
5. ✅ Scales from small projects to massive systems (Linux kernel)
6. ✅ Eliminates unused code paths through conditional compilation
7. ✅ Documents configuration through integrated help text

**In this project, Kconfig enables:**

- Professional-grade configuration management
- Compile-time optimization (dead code elimination)
- Type-safe option validation
- Automatic dependency handling
- Self-documenting configuration options
- Reproducible builds through version-controlled .config

---

**Ready to experiment?**

```bash
cmake --build build --target menuconfig
cmake --build build
./build/app
```

Explore all the options and see how your configuration affects the application behavior!
