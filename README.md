# CoreLox: The Core of Lox, Forged in C 🔥

Welcome to **CoreLox**, where the elegance of bytecode meets the simplicity and power of C. While RustyLox brought Lox to life in Rust, CoreLox strips it down to its essentials—minimalist, efficient, and purpose-driven. This is Lox interpretation, closer to the metal.

## 📜 **Project Overview**

In CoreLox, we’re not just interpreting Lox—we’re compiling it down to bytecode and executing it with a virtual machine (VM). Unlike Rustylox’s high-level safety nets, here, we handle memory ourselves and focus on creating a streamlined, efficient Lox interpreter that prioritizes speed and clarity.

Where Rustylox embraces Rust’s error handling and concurrency, CoreLox takes a different path, putting you in control of every byte of memory. It’s a little more dangerous, a little more fun, and a lot faster.

## 📑 **Table of Contents**

- [📜 Project Overview](#-project-overview)
- [🚀 Current Features](#-current-features)
- [🛠 Bytecode and Chunks](#-bytecode-and-chunks)
- [🛠 Virtual Machine](#-virtual-machine)
- [📜 Lox Grammar](#-lox-grammar)
- [🔗 Differences from Rustylox](#-differences-from-rustylox)
- [🤓 Getting Started](#-getting-started)
- [🤝 Contributing](#-contributing)

## 🚀 **Current Features**

CoreLox brings the following features to your interpreter toolbox:

- **Bytecode Compilation**: Transforms Lox code into bytecode that is processed at runtime, with each instruction mapped to efficient operations in C.
- **Efficient Memory Management**: Dynamic memory allocation and resizing for both the bytecode and constant pool, giving the interpreter room to grow as needed.
- **Constants Pool**: Manages literals and variables in a dedicated array to ensure fast access during execution.
- **Minimalistic, Fast Execution**: While Rustylox focuses on safety, CoreLox is all about speed and direct control of system resources.

## 🛠 **Bytecode and Chunks**

CoreLox compiles Lox source code into **bytecode**, represented as an array of instructions known as a **Chunk**. A chunk can contain operations ranging from arithmetic calculations to control flow and memory management.

Unlike Rustylox’s memory safety guarantees, CoreLox handles memory manually. This means each chunk dynamically grows as more instructions are added—if you hit the limit, we reallocate memory to keep things running smoothly.

### Memory Management

Memory allocation is a key difference from Rustylox, where Rust handles much of the heavy lifting for you. In CoreLox, we use manual memory management via **GROW_ARRAY** and **FREE_ARRAY** macros, making sure to free memory ourselves to avoid leaks.

## 🛠 **Virtual Machine**

Carbonlox’s **Virtual Machine (VM)** is at the heart of the project, interpreting bytecode instructions one at a time and executing them. While Rustylox’s VM leverages Rust’s concurrency features, Carbonlox’s VM is a single-threaded, streamlined machine that prioritizes performance and simplicity.

The VM operates on chunks of bytecode, processing instructions like arithmetic, logical comparisons, and function calls. With a focus on keeping the codebase clean and efficient, Carbonlox’s VM is built to be extensible and easily maintainable.

## 📜 **Lox Grammar**

CoreLox fully supports the core Lox language, following this simplified **EBNF** grammar:

```ebnf
program      = { declaration }, EOF ;

declaration  = varDecl 
             | funDecl
             | statement ;

funDecl      = "fun", function ;

function     = IDENTIFIER, "(", [ parameters ], ")", block ;

parameters   = IDENTIFIER, { ",", IDENTIFIER } ;

varDecl      = "var", IDENTIFIER, [ "=" expression ], ";" ;

statement    = exprStmt 
             | forStmt
             | ifStmt
             | printStmt 
             | returnStmt
             | whileStmt
             | switchStmt
             | block 
             | breakStmt
             | continueStmt ;

returnStmt   = "return", [ expression ], ";" ;

breakStmt    = "break", ";" ;

continueStmt = "continue", ";" ;

block        = "{", { declaration }, "}" ;

exprStmt     = expression, ";" ;

printStmt    = "print", expression, ";" ;

whileStmt    = "while", "(", expression, ")", statement ;

forStmt      = "for", "(", ( varDecl | exprStmt | ";" ),
                [expression], ";",
                [expression], ")", statement ;

switchStmt   = "switch", "(", expression, ")",
                 "{", { switchCase }, [ defaultCase ], "}" ;

switchCase   = "case", expression, ":", { statement } [ "fall" ];

defaultCase  = "default", ":", { statement } ;

ifStmt       = "if", "(", expression, ")" statement,
               { "elif", "(", expression, ")", statement },
               [ "else", statement ] 
             | "if", expression, "then", statement,
               { "elif", expression, "then", statement },
               [ "else", statement ] ; 

expression   = assignment ;

assignment   = IDENTIFIER, "=", assignment
             | ternary ;

ternary      = logic_or, "?", expression, ":" ternary
             | logic_or ;

logic_or     = logic_and, { "or", logic_and } ;

logic_and    = equality, { "and", equality } ;

equality     = comparison { ( "!=" | "==" ) comparison } ;

comparison   = term { ( ">" | ">=" | "<" | "<=" ) term } ;

term         = factor { ( "-" | "+" ) factor } ;

factor       = unary { ( "/" "*" "%" ) unary } ;

unary        = ( "!" | "-" ) unary
             | call ;

call         = primary, { "(", [ arguments ], ")" } ;

arguments    = expression, { ",", expression } ;

primary      = NUMBER 
             | STRING 
             | "true" 
             | "false" 
             | "nil"
             | "(" expression ")" 
             | IDENTIFIER ;
```

## 🔗 **Differences from Rustylox**

While Rustylox embraces the safety and concurrency of Rust, CoreLox takes a different approach. Here’s how they differ:

- **Memory Management**: In Rustylox, Rust’s ownership system prevents memory leaks and dangling pointers, but in CoreLox, manual memory management gives you full control and responsibility.
- **Language**: Rustylox is written in Rust, utilizing its concurrency model and error handling features, while CoreLox is written in C, focusing on simplicity and performance.
- **Error Handling**: Rustylox has Rust’s built-in error handling, whereas CoreLox requires careful error checking and handling in C, especially with memory management.
- **Concurrency**: Rustylox is built with Rust’s multi-threaded capabilities in mind, while CoreLox is currently single-threaded, optimized for sequential execution.

Both interpreters have their strengths, but CoreLox is for those who love to get their hands dirty with memory management and the low-level control that C offers.

## 🤓 **Getting Started**

### Prerequisites

To build and run CoreLox, you need:
- A C compiler (e.g., `gcc`)
- `make` for automating the build process

### Building the Project

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/carbonlox.git
   cd carbonlox
   ```

2. Build the project:
   ```bash
   make
   ```

### Running the Interpreter

After building, you can run the interpreter on a Lox file:
```bash
./carbonlox your_file.lox
```

### Cleaning the Project

To clean up the build artifacts:
```bash
make clean
```

## 🤝 **Contributing**

Got a cool idea for CoreLox? Found a bug? Want to help improve the VM? We’d love your contributions!

1. **Fork the Repository**: Start by forking the repo and creating a new branch.
2. **Write Your Code**: Implement your feature or bug fix.
3. **Submit a Pull Request**: Once you’re done, submit a pull request with a clear description of your changes.

Whether you’re new to C or an old hand, there’s always something to contribute. Let’s build CoreLox together!

## License

This project is licensed under the MIT License. See the LICENSE file for details.

---

CoreLox offers the power and flexibility of bytecode interpretation in C. Whether you’re here to learn, contribute, or dive deep into the inner workings of Lox, you’ll find plenty to explore in CoreLox. Happy coding! 🎉