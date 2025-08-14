# Hoare Verifier

A small Hoare-logic verifier for simple imperative programs (C-like syntax).
It parses annotated programs (assignments, `if`/`else`, `while` with `INVARIANT`/`VARIANT`), builds an AST, generates verification conditions (VCs) using a Hoare-style prover, and discharges them with the Z3 SMT solver.

This README explains what the tool does, how it’s structured, how verification works, how to build and run tests, and a short guide for writing loop invariants.

---

## Overview & goals

**Purpose.**
The verifier helps you mechanically check correctness of small imperative programs expressed with pre/postconditions and loop invariants. It’s intended for education, prototyping verification ideas, and testing invariants against an SMT backend.

**Key capabilities.**

* Parse small imperative programs annotated with `PRECONDITION:` / `POSTCONDITION:` and loops annotated with `INVARIANT(...)` and `VARIANT(...)`.
* Translate the program into an AST.
* Compute verification conditions using weakest-precondition style rules.
* Translate generated VCs into Z3 formulas and check validity using the Z3 C API.
* Support basic arithmetic and boolean operators, `min`, `max`, and a `fact` function modeled in Z3.

**What it is not.**

* Not a full programming language verifier (no pointers, arrays, concurrency).
* Not an automatic invariant generator — invariants must be provided by the user.
* An educational / prototyping tool — performance and completeness are limited by language support and the underlying SMT solver.

---

## Architecture

Project layout (important directories and their roles):

```
.
├── Ast/           # AST node definitions and helpers
├── Hashmap/       # small hashmap used for variable caching
├── Hoare/         # Hoare prover (VC generation)
├── Lexer/         # flex lexer (lexer.l + generated lex.yy.c)
├── Parser/        # bison parser (parser.y + generated parser.tab.c/h)
├── Z3/            # z3_helpers: mapping AST -> Z3 AST and helpers
├── tests/         # test cases (correct/incorrect)
└── myparser       # built executable
```

* **Lexer (Flex)**: tokenizes input and recognizes keywords, operators, numbers and identifiers.
* **Parser (Bison)**: constructs the AST representing assignments, control flow, invariants, etc.
* **AST**: a simple tree representation used by the Hoare prover.
* **Hoare**: code that walks the AST and computes verification conditions (VCs).
* **Z3 helpers**: translate VC AST to Z3 `Z3_ast`s, assert the negation of the VC and query Z3 for satisfiability.

---

## How verification works (high-level)

1. **Parse program** into AST.
2. **Attach pre/post**: capture `PRECONDITION:` and `POSTCONDITION:` as AST nodes.
3. **Hoare prover** computes the VC, typically of the shape `precondition -> wp(program, postcondition)`.
4. **Z3 translation**: convert the VC into Z3 AST nodes.
5. **Check validity**: the tool asserts the *negation* of the VC and asks Z3:

   * `unsat` ⇒ VC valid ⇒ program **correct**.
   * `sat` ⇒ counterexample exists ⇒ program **not correct** (a model can be inspected).
   * `unknown` ⇒ Z3 could not decide.

---

## Requirements

* Flex (lex)
* Bison (yacc)
* gcc or clang
* libz3 and Z3 C headers (Z3 C API)
* GNU make (optional)

On Debian/Ubuntu:

```bash
sudo apt-get install flex bison build-essential libz3-dev
```

---

## Build

From the repository root:

```bash
# generate lexer (if needed)
flex -o Lexer/lex.yy.c Lexer/lexer.l

# generate parser (if needed)
bison -t -d -o Parser/parser.tab.c Parser/parser.y

# compile
gcc \
  -I. -IAst -IHashmap -IHoare -IZ3 -IParser -ILexer \
  -o myparser Parser/parser.tab.c Lexer/lex.yy.c \
         Ast/ast.c Hashmap/hashmap.c Hoare/hoare.c \
         Z3/z3_helpers.c -lz3 -lfl
```

If a `makefile` is present, you can also run:

```bash
make all
```

---

## Usage

Run the verifier by piping a `.t` test file into the binary:

```bash
./myparser < tests/correct/sum_odd.t
```

Output includes:

* a short AST dump of parsed statements,
* messages from the verifier,
* the Z3 verdict: `The program is correct!`, `The program is NOT correct!`, or `Unknown result.`

---

## Test format (example)

A test file is plain text consisting of program statements followed by `PRECONDITION:` and `POSTCONDITION:`. Example:

```c
n = 7;
sumodd = 0;
i = 1;

while (i <= n) INVARIANT (
    sumodd == (i / 2) * (i / 2) and
    i >= 1 and i <= n + 1
) VARIANT (n - i + 1) {
    if (i % 2 != 0) {
        sumodd = sumodd + i;
    }
    i = i + 1;
}

PRECONDITION: true
POSTCONDITION: sumodd == ((n + 1) / 2) * ((n + 1) / 2)
```

Supported constructs include:

* `x = expr;`
* `if (cond) { ... } else { ... }` (else optional)
* `while (cond) INVARIANT (...) VARIANT (...) { ... }`
* `min(x,y)`, `max(x,y)`, `fact(x)` (fact is modeled in Z3 helpers)
* arithmetic `+ - * / %` and comparisons `< <= > >= == !=`
* boolean `and/or/not` (case-insensitive variants accepted)
* identifiers with underscores: `[a-zA-Z_][a-zA-Z0-9_]*`

---

## Writing loop invariants (short guide)

* **Invariant must hold at loop entry** and be preserved by every iteration.
* Express invariants relative to the loop counter at the loop head (for example, “processed 1..i-1”).
* For accumulation loops, a common pattern is `acc == f(i)` where `i` represents the next element to process.
* Be precise about integer division: `i / 2` is floor division in this verifier.
* The `VARIANT` expression must be non-negative while the guard holds and strictly decrease each iteration (used to argue termination).

---

## Tests

Add `.t` files under `tests/correct/` (expected provable) and `tests/incorrect/` (expected failing). Run any test by:

```bash
./myparser < tests/correct/sum.t
```

Use the included test files as templates for writing your own.
