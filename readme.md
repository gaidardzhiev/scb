# Source Code Builder

This is a minimalist build system implemented in C that interprets a custom build definition file (named `builder`) similar to Makefiles. It parses variable assignments, build targets with dependencies, and commands to run. The build system invokes the C compiler or other tools via system calls to build your project.

---

## Features and Inner Workings

- **Variables:**  
  Support variable assignment with syntax `VAR = value` and variable substitution using `$(VAR)` notation in command lines. Variables are stored in a table and replaced dynamically before executing build commands.

- **Rules (Targets):**  
  Each rule defines a *target* (like an executable or object file), its *dependencies*, and one or more *commands* to build the target. For example:

```
main: main.o lib.o
gcc -o main main.o lib.o
```

- **Command Parsing:**  
Commands must be indented by a tab or space on lines following a target line. They are collected and stored for each rule.

- **Build Process:**  
The system recursively builds dependencies first, then runs the commands for a target.  
It substitutes variables in commands before running them via `system()` calls.

- **Special Targets:**  
The first target in the build file is built by default. You can specify a target on the command line (e.g., `./build clean`) to run specific targets like cleaning.

---

## Supported Build File Syntax (`builder`)

The build file uses a simple syntax that supports variables, targets with dependencies, commands, and comments.

### Variables

Define variables to store values like compiler, flags, or file names. No special export needed:

```
CC = musl-gcc
CFLAGS = -Wall -g
SRC_MAIN = main.c
SRC_FOO = foo.c
OBJ_MAIN = main.o
OBJ_FOO = foo.o
BIN = main

```

Use variables in command lines or dependencies with `$(VAR)` syntax. For example:

```
$(CC) $(CFLAGS) -c $(SRC_MAIN) -o $(OBJ_MAIN)
```


### Targets and Dependencies

Define a target followed by dependencies separated by spaces:

```
$(BIN): $(OBJ_MAIN) $(OBJ_FOO)
```

Commands to build the target can be indented (with a tab or space) on lines immediately after the target line:

```
    $(CC) $(CFLAGS) $(OBJ_MAIN) $(OBJ_FOO) -o $(BIN)
```


### Multiple commands per target

Specify more than one command as multiple indented lines:

```
$(OBJ_MAIN): $(SRC_MAIN)
printf "compiling sourcecode\n"
$(CC) $(CFLAGS) -c $(SRC_MAIN) -o $(OBJ_MAIN)
```


### Clean target example

Define a `clean` target to delete generated files using variables:

```
clean:
rm -f $(BIN) $(OBJ_MAIN) $(OBJ_FOO) $(OBJ_BAR)
```


### Comments

Lines starting with `#` are comments ignored by the build system

---

## How To Build SCB

```
git clone https://github.com/gaidardzhiev/scb
cd scb
make
make strip
make install
```

---

## How to Test SCB
Go to the tests directory:
```
cd tests
```

Build the default target (first target in the `builder` file) by running:
```
scb
```

Expected output:
```
CWD: /home/src/1v4n/scb/tests
run: gcc -Wall -g -c main.c -o main.o
ret: 0
CWD: /home/src/1v4n/scb/tests
run: gcc -Wall -g -c hello.c -o hello.o
ret: 0
CWD: /home/src/1v4n/scb/tests
run: gcc -Wall -g main.o hello.o -o hello
ret: 0
success
```

Test the executable:
```
./hello
Hello, build system!
```

---

## Example `builder` File for a Hello World C program

```
CC = musl-gcc
CFLAGS = -Wall -g
TARGET = hello
OBJ_MAIN = main.o
OBJ_HELLO = hello.o
SRC_MAIN = main.c
SRC_HELLO = hello.c
HDR_HELLO = hello.h

$(TARGET): $(OBJ_MAIN) $(OBJ_HELLO)
    $(CC) $(CFLAGS) $(OBJ_MAIN) $(OBJ_HELLO) -o $(TARGET)

$(OBJ_MAIN): $(SRC_MAIN) $(HDR_HELLO)
    $(CC) $(CFLAGS) -c $(SRC_MAIN) -o $(OBJ_MAIN)

$(OBJ_HELLO): $(SRC_HELLO) $(HDR_HELLO)
    $(CC) $(CFLAGS) -c $(SRC_HELLO) -o $(OBJ_HELLO)

clean:
    rm -f $(TARGET) $(OBJ_MAIN) $(OBJ_HELLO)
```

---

## Limitations & Possible Improvements

- Does not track file modification times; always rebuilds dependencies.
- Simple parser and syntax; no advanced Make features (pattern rules, automatic variables).
- No parallel compilation support.
- Assumes commands succeed if `system()` returns zero.
- Can be extended to support more features like incremental builds, error handling, and enhanced syntax.

---

This build system provides a clear, small, and understandable foundation for building C projects without the complexity of traditional Makefiles or external tools. It helps learn how build systems work internally by implementing one in pure C.

---

## License

This project is provided under the GPL3 License.

---
