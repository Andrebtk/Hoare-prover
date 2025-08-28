# Hoare Verifier — README

Tiny Hoare-logic verifier for small imperative programs annotated with `PRECONDITION:`, `POSTCONDITION:` and `while` loops with `INVARIANT(...)` and `VARIANT(...)`.  
Parses a toy imperative language, builds an AST, generates verification conditions (VCs) with a backward Hoare-style prover (`hoare_prover`) and discharges them with Z3 (via the Z3 C API).

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
