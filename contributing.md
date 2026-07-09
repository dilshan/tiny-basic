# 3. Contributing Guidelines

We love contributions! Whether you are fixing an instruction decoding bug, optimizing a hardware abstraction layer, writing sample BASIC programs, or improving documentation, your help is incredibly valuable.

## How to Propose Changes

1.  **Search First:** Before diving into code, check the issue tracker to see if your bug or feature request has already been discussed.
2.  **Open an Issue:** For significant code changes, refactors, or new syntax features, please open an issue first to discuss the design layout. This keeps our core architectural goals aligned.
3.  **Fork and Branch:** Fork the repository and create your feature branch from the development branch (e.g., `git checkout -b feature/optimized-integer-math`).
4.  **Keep it Focused:** A single Pull Request should solve exactly one issue or add one cohesive capability.

## Code Quality and Style Rules

Because this project targets strict readability and constraint-driven environments, please adhere to these implementation rules:
* **Language Standards:** For C/C++ core engines, stick to strict ANSI C (C89) or minimal modern subsets without heavy runtime overhead. For assembly ports, comment every logical block.
* **Formatting:** Follow the repository's formatting style guide (e.g., spaces instead of tabs, explicit brace placement, and camelCase or snake_case conventions as established in the current code).
* **Low Footprint:** Ensure that your changes do not drastically inflate the binary footprint or introduce dynamic memory allocations. Every byte matters.
* **Backward Compatibility**: If you modify the behavior of an existing function or a core parser component, ensure that it remains compatible with the syntax supported by previous versions.
* **Add or Update Documentation**: If you modify the behavior of an existing function or introduce a new function, update the existing documentation or add new documentation in the `/docs/manual` directory.

## Testing and Validation

* All additions to the core parser must include a corresponding test files (written in `.bas`).
* Verify that your modifications run correctly across all core hardware targets.

## Commit Sign-Off

Please write descriptive, imperative commit messages (e.g., `Fix: correct token bounds checking in LET interpreter`).