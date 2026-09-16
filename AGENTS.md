# Repository Guidelines

## Project Structure & Module Organization

This repository is a PlatformIO firmware project for the WCH CH32V203 MCU using the `noneos-sdk` framework. Application code lives in `src/`; the current entry point is `src/main.c`. Put shared project headers in `include/` and reusable, project-specific libraries in `lib/<library_name>/`. Add PlatformIO unit tests under `test/`, preferably one subdirectory per feature. Build artifacts are generated in `.pio/` and must remain untracked. Board, upload, debug, and serial settings are defined in `platformio.ini`.

## Build, Test, and Development Commands

Run commands from the repository root with PlatformIO Core installed:

- `pio run` builds the default `generic` environment (`genericCH32V203C8T6`).
- `pio run -e evt` builds for the CH32V203C8T6 EVT R0 board.
- `pio run -t upload` builds and flashes with WCH-Link.
- `pio run -e generic_isp -t upload` flashes the generic board through ISP; use `evt_isp` for the EVT board.
- `pio device monitor -b 115200` opens the configured serial monitor.
- `pio test -e generic` runs tests placed under `test/`.
- `pio run -t clean` removes generated build output.

## Coding Style & Naming Conventions

Write C consistent with `src/main.c`: four-space indentation, braces on the same line for functions and control statements, and one declaration or operation per line. Use `UPPER_SNAKE_CASE` for macros and hardware constants, `PascalCase` only where required by SDK types, and descriptive `camelCase` for local variables. Keep interrupt handlers small and preserve the WCH interrupt attributes. Include vendor headers with angle brackets and project headers with quotes. No formatter or linter is configured, so review formatting as part of each change.

## Testing Guidelines

Use PlatformIO Test Runner for new tests. Name test directories after the behavior or module, for example `test/test_gpio_blinky/`, and keep hardware-dependent tests clearly documented. There is currently no coverage requirement; every change should at minimum build successfully for each affected environment. For GPIO, clock, upload, or timing changes, report the board and programmer used for hardware verification.

## Commit & Pull Request Guidelines

Recent commits use short, lowercase, imperative summaries such as `add project` and `update gitignore`. Follow that style, keep commits focused, and avoid committing `.pio/` output. Pull requests should explain the behavior changed, list tested environments and commands, link related issues, and include serial logs or wiring notes when hardware behavior is relevant. Call out changes to pin assignments, clock configuration, or upload protocol explicitly.
