# Hoare Verifier — README

A small Hoare-logic verifier for a simple imperative programs annotated with `PRECONDITION:`, `POSTCONDITION:` and `while` loops with `INVARIANT(...)` and `VARIANT(...)`.  
Parses a toy imperative language, builds an AST, generates verification conditions (VCs) with a backward Hoare-style prover (`hoare_prover`) and discharges them with Z3 (via the Z3 C API).


# Overview & goals
Purpose. The verifier helps you mechanically check correctness of small imperative programs expressed with pre/postconditions and loop invariants. It’s intended for education, prototyping verification ideas, and testing invariants against an SMT backend.

### Key capabilities.
- Parse small imperative programs annotated with PRECONDITION: / POSTCONDITION: and loops annotated with INVARIANT(...) and VARIANT(...).
- Translate the program into an AST
   - Parser generation: bison
   - Lexer generation: flex   
- Compute verification conditions using weakest-precondition style rules.
- Translate generated VCs into Z3 formulas and check validity using the Z3 C API.
- Support basic arithmetic and boolean operators, min, max, and a fact function modeled in Z3.

### What it is not.
- Not a full programming language verifier (no pointers, arrays, concurrency).
- Not an automatic invariant generator — invariants must be provided by the user.
- An educational / prototyping tool — performance and completeness are limited by language support and the underlying SMT solver.


## Quick start

### Requirements
- `flex`, `bison`
- `gcc` or `clang`
- `libz3-dev`
- `make` (optional)

On Debian/Ubuntu:
```bash
sudo apt-get install flex bison build-essential libz3-dev
```

### Build
```bash
make
```

### Run
```bash
./hoare < tests/correct/sum.t
```
The verifier asserts the **negation** of the top-level VC into Z3. `unsat` ⇒ VC valid ⇒ program correct.

## Input format

A program: statements followed by `PRECONDITION:` and `POSTCONDITION:`. Example constructs:

- `x = expr;`
- `if (cond) { ... } else { ... }`
- `while (cond) INVARIANT (...) VARIANT (...) { ... }`
- Functions: `min(x,y)`, `max(x,y)`, `fact(x)`
- Ops: `+ - * / %`, comparisons, `and/or/not`
- Identifiers: `[A-Za-z_][A-Za-z0-9_]*`

Example (factorial test):
```c
x = fact(n)

PRECONDITION: n == 5
POSTCONDITION: x == 120
```

## How verification works (brief)

1. Parse input → AST.
2. `hoare_prover` walks program **backwards**, computing the precondition required so that `post` holds.
3. Build VC (Verification Condition): `pre -> hoare_prover(program, post)`.
4. Convert VC to Z3 ASTs and assert its **negation** to the solver.
   - `unsat` → correct; `sat` → counterexample; `unknown` → undecided.

### Example (developer)
Program:
```c
x = x + 1;
y = x;
PRECONDITION: x >= 0
POSTCONDITION: y >= 1
```
Backward proof:
1. Start with `wp = (y >= 1)`.
2. After `y = x;` → `wp = (x >= 1)`.
3. After `x = x + 1;` → `wp = (x + 1 >= 1)` → `x >= 0`.
VC: `PRE -> wp` i.e. `(x >= 0) -> (x >= 0)` (valid).

### Nested while (sum of triangles)

Program:
```c
total = 0;
i = 1;
while (i <= n) INVARIANT (
    total == (i - 1) * i * (i + 1) / 6
) VARIANT (n - i + 1) {
    j = 1;
    while (j <= i) INVARIANT (
        total == ((i - 1) * i * (i + 1) / 6) + (j - 1) * i
    ) VARIANT (i - j + 1) {
        total = total + i;
        j = j + 1;
    }
    i = i + 1;
}

PRECONDITION: n >= 0
POSTCONDITION: total == n * (n + 1) * (n + 2) / 6
```

Explanation:

- Outer loop invariant encodes that at iteration ```i```, the sum so far is ```(𝑖−1)⋅𝑖⋅(𝑖+1)/6```, the sum of first ```i-1``` triangular numbers.
- Inner loop invariant refines this to accumulate ```i``` exactly```j-1``` times.
- Variants ```(n - i + 1 and i - j + 1)``` prove termination.
- On loop exit ```(i = n+1)```, the invariant gives exactly the desired closed form: ```total == n*(n+1)*(n+2)/6```

Thus the Hoare triple is valid:
```c
{n >= 0}  program  {total == n*(n+1)*(n+2)/6}
```


## Mapping to Hoare rules (short)

- **Assignment axiom**  
  `{P[E/x]} x := E {P}`  
  Implemented by `hoare_AssignmentRule` using `substitute(post, id, expr)`.

- **Conditional**  
  Rule:
  ```
  {B∧P} S {Q}   {¬B∧P} T {Q}
  -------------------------
  {P} if B then S else T {Q}
  ```
  Implementation: compute `wp_if` and `wp_else` for branches and return `(B -> wp_if) ∧ (¬B -> wp_else)`.

- **While (partial correctness)**  
  Rule:
  ```
  {P ∧ B} S {P}
  -----------
  {P} while B do S done {¬B ∧ P}
  ```
  Implementation: with invariant `I`, verify `(I ∧ B) -> wp_body` (preservation) and `(I ∧ ¬B) -> post` (exit implies post).

- **While (termination check / total correctness)**  
  Use a `variant` `t` (integer). The tool:
  - Computes `variant_after` by substituting assignments from the body.
  - Requires `(I ∧ B) -> (variant_after < variant ∧ variant >= 0)`.

## Files of interest
- `Ast/` — AST, clone/substitute, printing, memory management.
- `Hoare/hoare.c` — `hoare_prover`, rules for assignment/if/while, evaluators.
- `Z3/z3_helpers.c` — `ast_to_z3`, `init_z3` (models `fact`), Z3 interactions.
- `Parser/` & `Lexer/` — grammar and lexer.
- `Hashmap/` — variable cache for Z3 translation.

## Tips & debugging
- Use Valgrind for memory issues.
- If Z3 returns `sat`, inspect the model to find counterexamples; usually invariants are too weak.
- For hard arithmetic, Z3 can return `unknown`; simplify VCs or strengthen invariants.
