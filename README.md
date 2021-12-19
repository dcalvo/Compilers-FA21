# Compiler Project
This is a course project of 601.428/628 (F21): Compilers and Interpreters, Fall 2021

- [Compiler Project](#compiler-project)
  - [1. Overview](#1-overview)
  - [2. Testing & Results](#2-testing--results)
  - [3. How to run this code?](#3-how-to-run-this-code)
  - [4. Pascal-Like Language Specification](#4-pascal-like-language-specification)
    - [4.1 Lexical structure](#41-lexical-structure)
    - [4.2 Syntax](#42-syntax)
      - [4.2.1 Declarations and types](#421-declarations-and-types)
      - [4.2.2 Expressions](#422-expressions)
      - [4.2.3 Multidimensional arrays](#423-multidimensional-arrays)
    - [4.3 Semantics, type checking](#43-semantics-type-checking)
    - [4.4 Further Runtime Semantics](#44-further-runtime-semantics)
    - [4.5 Optimization Utilities & Intermediate Representations](#45-optimization-utilities--intermediate-representations)
      - [4.5.1 Live variables analysis](#451-live-variables-analysis)
      - [4.5.2 Control Flow Graph Transformation](#452-control-flow-graph-transformation)

## 1. Overview
In this project, I build a compiler for a simple Pascal-like programming language. The language description in detail is stated [in this section](#4-pascal-like-language-specification)

* Front end lexical analyzer and parser
* AST-based intermediate representation to represent the source program.
* Semantic analysis phase that builds symbol tables representing the data types and storage requirements.
* High-level (machine independent) code & Low-level(X86_64 assembly language) translation.
* Code Optimization: Register allocation, Constant Propagation, High-level Peephole optimization and Storage Allocation


## 2. Testing & Results
**case 1:** Unoptimized Compiler\
**case 2:** Register Alloc\
**case 3:** Register Alloc + High-level Peephole\
**case 4:** Register Alloc + High-level Peephole + Storage Alloc

|                 | case 1 | case 2 | case 3 | case 4 |
|:---------------:|:------:|:------:|:------:|:------:|
|  `array20.in`   | 3.550s | 1.660s | 1.996s | 1.301s |
| `multidim20.in` | 2.435s | 1.348s | 1.487s | 1.177s |

Times are on Linux, using a Core i5-7200U processor.

## 3. How to run this code?
1) **No optimization:**\
./compiler [input_filename]
2) **Register allocation + High-level peephole optimization + Storage allocation:**\
./compiler [input_filename] -o
3) **Check AST Tree:**\
./compiler [input_filename] -p\
or use *graphvis* to visualize the graph\
./compiler [input_filename] -g
4) **Check Symbol table:**\
./compiler [input_filename] -s
5) **Check High-level intermediate code:**\
./compiler [input_filename] -i

## 4. Pascal-Like Language Specification
### 4.1 Lexical structure
The language has the following kinds of tokens:

Keywords: `PROGRAM` `BEGIN` `END` `CONST` `TYPE` `VAR` `ARRAY` `OF` `RECORD` `IF` `THEN` `ELSE` `REPEAT` `UNTIL` `WHILE` `DO` `READ` `WRITE`\
Operators: `:=` `=` `+` `-` `*` `DIV` `MOD` `#` `<` `>` `<=` `:=`\
Brackets/grouping: `(` `)` `[` `]`\
Misc/punctuation: `:` `;` `.` `,`\
Identifers: a letter or underscore (`_`), followed by 0 or more letters, underscores, and/or digits\
Integer literals: a sequence of one or more digit characters\
Comments: start with `--` and continue to the end of the source line\
Tokens may be separated by whitespace.

### 4.2 Syntax
The overall input is a *program*, which has the form
```
PROGRAM identifier ; opt-declarations BEGIN opt-instructions END .
```
*opt-declarations* is a sequence of 0 or more *declarations*.

*opt-instructions* is a sequence of 0 or more *instructions*.

#### 4.2.1 Declarations and types
A *declaration* is a *const-declaration*, *var-declaration*, or *type-declaration*.

A *const-declaration* has the form
```
CONST const-definitions
```
*const-definitions* is a sequence of one or more occurrences of *const-definition*.

A *const-definition* has the form
```
identifier = expression ;
```

A *var-declaration* has the form
```
VAR var-definitions
```
*var-definitions* is a sequence of one or more occurrences of *var-definition*.

A *var-definition* has the form
```
identifier-list : type ;
```
An *identifier-list* is a sequence of 1 or more identifiers, separated by commas (`,`).

A *type-declaration* has the form
```
TYPE type-definitions
```
*type-definitions* is a sequence of one or more occurrences of *type-definition*.

A *type-definition* has the form
```
identifier = type ;
```
A *type* is a *named-type*, *array-type*, or *record-type*.

A *named-type* is simply an *identifier*.

An *array-type* has the form
```
ARRAY expression OF type
```

A *record-type* has the form
```
RECORD var-definitions END
```

#### 4.2.2 Expressions
A *primary-expression* is an integer literal, designator, or parenthesized expression.

A *unary-expression* is either a primary-expression, or a unary-expression preceded by either `-` or `+`.

The following infix operators may be used in expressions, with the operands being *unary-expressions*:

|    Operator     | Precedence | Associativity |
|:---------------:|:----------:|:-------------:|
|     `+`,`-`     |   lower    |     left      |
| `*`,`DIV`,`MOD` |   higher   |     left      |

Note that relational operators may **not** appear in expressions. (They are only used in *conditions*.)

Instructions
An instruction is an *assignment-statement*, *if-statement*, *repeat-statement*, *while-statement*, *read-statement*, or *write-statement*. Each *instruction* must be followed by a semicolon (`;`).

An *assignment-statement* has the form
```
designator := expression
```

An if-statement can have either of the following forms:
```
IF condition THEN opt-instructions END
IF condition THEN opt-instructions ELSE opt-instructions END
```

A repeat-statement has the form
```
REPEAT opt-instructions UNTIL condition END
```

A while-statement has the form
```
WHILE condition DO opt-instructions END
```

A write-statement has the form
```
WRITE expression
```

A read-statement has the form
```
READ designator
```

A condition has the form
```
expression relational-op expression
```

where relational-op is one of `<`, `<=`, `>`, `>=`, `=`, or `#` (not equals).

A *designator* has one of the following forms:
```
identifier
designator [ expression-list ]
designator . identifier
```

These three forms of designators correspond to variable references, array element references, and record field references, respectively.

An *expression-list* is a sequence of one or more expressions, separated by commas (`,`).

#### 4.2.3 Multidimensional arrays
```
PROGRAM multidim;
  TYPE Grid = ARRAY 10 OF ARRAY 10 OF INTEGER;
  VAR mygrid : Grid;

BEGIN
  mygrid[0][0] := 2;
  mygrid[1, 1] := 3;
END.
```

### 4.3 Semantics, type checking
The source language is a Pascal-like imperative programming language.

There are two primitive data types, `INTEGER` and `CHAR`. These are the integral types.

The two kinds of composite data types are arrays and records. An array type specifies an integer size and an element type, which can be an arbitrary type. A record type consists of fields, each of which has a name and type.

Within a lexical scope in the program, a name may only refer to a single variable, constant, or type. For example, the program:

```
PROGRAM names;
  TYPE a = INTEGER;
  VAR a : ARRAY 10 OF CHAR;

BEGIN
  READ a;
END.
```

is not legal because the name `a`, originally referring to a type, is reused to name a variable.

`RECORD` types are a lexical scope distinct from the enclosing scope, so it is legal for field names to shadow names used in the outer scope.

The operands of numeric operators (`+`, `-`, `*`, `DIV`, etc.) must be integral. Mixed type expressions (consisting of one `INTEGER` and one `CHAR`) are allowed, and the result of a mixed type expression is `INTEGER`. For non-mixed expressions, the result type is the same as the operand types.

The operands of comparison operators (`<`, `>`, etc.) must be integral.

Integer literals have type `INTEGER`.

Variable references have the type specified by the corresponding variable declaration.

For an array subscript reference, the designator must have an array type, and the expression(s) designating the index(es) must have an integral type (`INTEGER` or `CHAR`.) The type of the subscript reference is the array type’s element type.

For a field reference, the designator must have a record type, and the type of the field reference is the type of the field named by the identifier on the right hand side of the `.`.

`WRITE` statements are used for output, and the expression specifying the value to output must be either `INTEGER`, `CHAR`, or array of `CHAR` (any length).

`READ` statements are used for input, and the designator specifying the variable where the input should be stored must be either `INTEGER`, `CHAR`, or array of `CHAR`.

For this assignment, the follow type checking rules should be applied for assignments:

Only `INTEGER` and `CHAR` values may be assigned
The type of the designator on the left hand side of the assignment must be exactly the same as the type of the right hand side
These rules may be relaxed for subsequent assignments.


### 4.4 Further Runtime Semantics
The `INTEGER` data type must be a signed integer type with at least 32 bits of precision. For simplicity, a 64 bit type `INTEGER` is recommended.

A `READ` statement should work as follows:

* For reading into an `INTEGER` variable (or array element, or field), the compiled program should make a call to the `scanf` function to read a single integer value
* For reading into a `CHAR` variable (or array element, or field), the compiled program should make a call equivalent to
  ```
  char ch;
  scanf(" %c", &ch);
  ```
  where `ch` will contain the single, non-space character read by `scanf`

* For reading into an array of `CHAR` elements, the compiled program should make a call equivalent to
  ```
  scanf("%s", arr);
  ```
  where `arr` is the array into which the input is being read

A `WRITE` statement should work as follows:

* For writing an `INTEGER` value, the compiled program should print its decimal representation, followed by a single newline (`\n`) character; you should use printf to generate the output
For writing a single `CHAR` value, the compiled program should make a call equivalent to
  ```
  printf("%c\n", ch);
  ```
  where `ch` contains the character value to be printed

* For writing an array of `CHAR` values, the compiled program should make a call equivalent to
  ```
  printf("%s\n", arr);
  ```
  where `arr` is the array being printed

Note that the behaviors for reading and writing `CHAR` arrays imply that they operand on NUL-terminated character strings, i.e., the same representation that C uses.

Array subscript references do not need to be bounds-checked.

### 4.5 Optimization Utilities & Intermediate Representations

#### 4.5.1 Live variables analysis
The `live_vregs.h` and `live_vregs.cpp` source files implement global live variables analysis for virtual registers. They're useful for determining which instructions are safe to eliminate after local optimizations are applied. Note that you will need to add the following member functions to the `InstructionSequence` class:
```
const_reverse_iterator crbegin() const { return m_instr_seq.crbegin(); }
const_reverse_iterator crend() const { return m_instr_seq.crend(); }
```
This code also assumes the existence of `HighLevel::is_def` and `HighLevel::is_use` functions, which determine (respectively)
* whether an instruction is a def (assignment to a virtual register), and
* whether an operand of an instruction is a use of a virtual register

Here are possible implementations of print_basic_block and format_instruction for the HighLevelControlFlowGraphBuilder class:
```
void HighLevelControlFlowGraphPrinter::print_basic_block(BasicBlock *bb) {
  for (auto i = bb->cbegin(); i != bb->cend(); i++) {
    Instruction *ins = *i;
    std::string s = format_instruction(bb, ins);
    printf("\t%s\n", s.c_str());
  }
}

std::string HighLevelControlFlowGraphPrinter::format_instruction(BasicBlock *bb,
                                                                 Instruction *ins) {
  PrintHighLevelInstructionSequence p;
  return p.format_instruction(ins);
}
```

#### 4.5.2 Control Flow Graph Transformation
You can implement your optimization passes as transformations of `InstructionSequence` (linear IR) or `ControlFlowGraph` (control flow graph IR).

If you do transformations of an `InstructionSequence`, you will need to take control flow into account. Any instruction that either

* is a branch, or
* has an immediate successor that is a labeled control-flow target
should be considered the end of a basic block.

This implementation uses the `ControlFlowGraph` intermediate representation, you can create it from an `InstructionSequence` as follows. For high-level code:
```
InstructionSequence iseq = /* InstructionSequence containing high-level code... */
HighLevelControlFlowGraphBuilder cfg_builder(iseq);
ControlFlowGraph *cfg = cfg_builder.build();    
```
For low-level (x86-64) code:
```
InstructionSequence *ll_iseq = /* InstructionSequence containing low-level code... */
X86_64ControlFlowGraphBuilder xcfg_builder(ll_iseq);
ControlFlowGraph *xcfg = xcfg_builder.build();
```
To convert a `ControlFlowGraph` back to an `InstructionSequence`:
```
ControlFlowGraph *cfg = /* a ControlFlowGraph */
InstructionSequence *result_iseq = cfg->create_instruction_sequence();
```

Note that the `create_instruction_sequence()` method is not guaranteed to work if the structure of the ControlFlowGraph was modified. 

The idea of transforming each basic block is to override the transform_basic_block member function in `cfg_transform.h` and `cfg_transform.cpp`. In general, this class should be useful for implementing any local (basic block level) optimization.
