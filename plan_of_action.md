# Phased Implementation Plan

A phased implementation plan is recommended to manage complexity and provide clear milestones.

---

## Phase 1: Core Simulation in Rust
This phase focuses on building the foundational simulation logic in Rust.

* **Data Modeling**: Define the core simulation data structures, such as `City`, `Tile`, and `ZoneData`, within a new Rust library crate.
* **Logic Porting**: Systematically port the existing C simulation logic from files like `s_sim.c`, `s_zone.c`, `s_scan.c`, and `s_traf.c` into idiomatic Rust functions or systems.
    *   **Migration Targets**:
        *   `micropolis-activity/src/sim/s_sim.c`: Contains the main simulation loop and coordination logic.
        *   `micropolis-activity/src/sim/s_zone.c`: Handles logic related to zone development (residential, commercial, industrial).
        *   `micropolis-activity/src/sim/s_scan.c`: Contains scanning functions that iterate over the city map.
        *   `micropolis-activity/src/sim/s_traf.c`: Manages the traffic simulation logic.
        *   `micropolis-activity/src/sim/s_eval.c`: Contains city evaluation logic.
    *   **Completed**:
        *   Ported `DoHospChur` and `RepairZone` from `s_zone.c` to `zone.rs`.
        *   Ported `GetAssValue` and `DoPopNum` from `s_eval.c` to `sim.rs`.
        *   Ported `DoProblems` from `s_eval.c` to `sim.rs` with stubs for helper functions.
* **Unit Testing**: Develop a comprehensive suite of unit and integration tests in Rust to validate the correctness of the ported logic. This may involve comparing outputs against the original C implementation. This phase is complete when a headless Rust library can successfully load a city state, run a simulation for N cycles, and produce a verifiably correct new city state.
    *   **Completed**:
        *   Added unit tests for `do_hosp_chur` and `repair_zone` in `zone.rs`.
        *   Added unit tests for `get_ass_value` and `do_pop_num` in `sim.rs`.
        *   Added an integration test for `step_simulation`.

---

## Phase 2: FFI API Definition and Scaffolding
This phase establishes the bridge between the Rust backend and the Python frontend.

* **API Design**: Design a "chunky" API layer using **PyO3**. Define a minimal set of high-level functions required by the Python front-end, such as `create_city(width, height)`, `step_simulation(inputs)`, `get_map_view(x, y, w, h) -> Vec<u16>`, and `get_city_stats() -> StatsStruct`.
* **Build and Package**: Use **Maturin** to configure the project to build a Python wheel.
* **Verification**: Create a minimal "scaffolding" Python script to import the compiled module, call each API function, and print the returned data to the console. This phase is complete when the FFI bridge is proven to be functional.

---

## Phase 3: Python UI Development with Arcade
This phase focuses on building the user interface using Python.

* **Application Shell**: Set up the main application window, the core game loop structure, and the asset loading pipeline in Python using the **Arcade** library.
* **Rendering Logic**: Implement the rendering logic by creating a `TileMap` or `SpriteList` in Arcade and updating it in the `on_draw` method based on the data retrieved from the `get_map_view` FFI call.
* **Input Handling**: Implement user input handling for mouse and keyboard. Translate user actions, such as clicking a tile to zone it, into the simple data structures defined by the FFI API and pass them to the `step_simulation` function. This phase is complete when a user can view the city and perform basic interactions.

---

## Phase 4: Integration and Polish
This final phase involves connecting all components and refining the application.

* **Full UI Implementation**: Connect all remaining UI elements, such as the budget window, graphs, and menus, to the Rust backend using new FFI functions as needed.
* **Performance Profiling**: Conduct thorough performance profiling of the fully integrated application, with a focus on time spent at the FFI boundary.
* **Optimization and Refinement**: Based on the profiling results, refine the FFI API to eliminate any identified bottlenecks, likely by consolidating calls or optimizing data transfer formats.
* **Final Touches**: Add audio, save/load functionality (which will involve serializing the Rust simulation state), and other final polish features.
