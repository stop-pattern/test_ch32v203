# Repository Guidelines

## Start and Resume

Read [progress](docs/progress.md), [development workflow](docs/development-workflow.md), and the relevant specifications before acting. Check Git status, delegated work, and actual process/device state; a stale checkpoint is not proof that a process is running. Firmware implementation remains gated by the user's implementation instruction and resolved prerequisites in [implementation plan](docs/implementation-plan.md).

## Autonomous Development

For an authorized goal, define scope and measurable acceptance criteria, then autonomously repeat planning, implementation, tests/builds, error analysis, fixes, rebuilds, authorized upload, log capture, and debugging until acceptance is evidenced. Do not ask again for already granted authority. Unresolved product decisions or new scope require user input; continue independent in-scope work meanwhile. Create a tool-managed goal only when explicitly requested, and follow the goal tool's lifecycle rules.

Persist every meaningful finding, failed attempt, command/result, pending operation, and next action promptly in [work log](docs/work-log.md) and the current checkpoint. Checkpoint before long operations, delegation, context handoff, and turn completion. Do not rely on conversation memory or claim required unperformed HIL as complete.

## Parallel Work

Actively delegate bounded read-only investigation, error analysis, and review. Also parallelize implementation, tests, and documentation when responsibilities are independent. Assign file ownership, inputs, deliverables, and acceptance criteria. The coordinator owns shared progress files, integration, and Git index/commits. Serialize shared build directories, source snapshots, boards, probes, and debug/serial sessions; review agent results before integration.

## Project Layout and Commands

This is a CH32V203 PlatformIO `noneos-sdk` project. Application code is in `src/` (currently `main.c`), shared headers in `include/`, libraries in `lib/<name>/`, tests in `test/test_<behavior>/`, and settings in `platformio.ini`. Keep `.pio/` untracked.

Use [ch32v203-platformio](skills/ch32v203-platformio/SKILL.md) for CLI discovery and build/upload procedures. Examples: `pio run -e evt`, or all four environments with `pio run -e generic -e evt -e generic_isp -e evt_isp`. Authorized EVT upload uses `pio run -e evt -t upload`; USB ISP uses `evt_isp`. Always select the actual board explicitly. Embedded `pio test` may flash hardware; no host test environment is currently defined.

## Code and Verification

Follow [coding style](docs/coding-style.md): Google C++ Style Guide with C++20, four-space indentation, and `.cpp`/`.hpp` extensions. Every project function, class, and struct declaration requires Doxygen comments. Project firmware uses no exceptions, RTTI, or heap allocation; vendor SDK/stack code keeps its upstream C conventions. Preserve required ISR attributes, ABI names, and register operations at hardware boundaries. OS/thread-specific and Google-library adoption guidance is outside the firmware scope. Existing code cleanup is deferred; do not reformat or migrate it as part of this documentation work. C++20 toolchain compatibility is a future P-02/T-20 check; no formatter/linter is currently configured.

Use PlatformIO Unity and [test plan](docs/test-plan.md). Target 90% branch coverage for protocol, transformation, configuration, and state-machine logic. Verify affected build environments; record hardware, firmware identity, logs, and measurements for HIL. Documentation-only work needs document/skill checks, not an unrelated firmware rebuild.

## Documentation and Commits

Maintain Japanese human-facing specifications as a coherent current description. On additions or removals, search related requirements, interfaces, tests, decisions, plans, and skills; rewrite affected sections as if the resulting design had always been present. Remove obsolete assertions and references. Keep historical evidence in the work log, decision rationale, and Git, not contradictory correction paragraphs in current specifications.

Commit small, coherent, verified units frequently using short lowercase imperative messages such as `update development workflow`. Preserve unrelated user changes. PRs explain the outcome, requirements/issues, verification and remaining gaps, with logs or wiring notes where relevant. Installed skill copies must match their versioned originals.
