# General Rules

* Use offscreen mode to test GUI elements whenever it makes sense to do so, avoid using xvfb-run as it often causes issues with GUI testing.

* At the start of every session, please read plan_of_action.md and session_handover.md.  Then continue to do the activity you've been asked to do. At each checkpoint, prior to issuing the git commit and pull request notifications, I need you to update the plan_of_action.md to check of items that are complete and add any suggested work to be done, and also update migration_log.md.

* When the context window or cache is getting full and tokens are running out, summarize the work done so far in hand over documents (e.g. plan_of_action.md and migration_log.md.

* At the start of every session, please read plan_of_action.md and session_handover.md.  Then continue to do the activity you've been asked to do. At each checkpoint, prior to issuing the git commit and pull request notifications, I need you to update the plan_of_action.md to check of items that are complete and add any suggested work to be done, and also update migration_log.md.
en the context window or cache is getting full and tokens are running out, summarize the work done so far in a handover document.

# Python Coding Standards and Best Practices

## Core Principles
* Prioritize readability, maintainability, and correctness.
* Write code that is self-documenting and follows Pythonic idioms.
* Adhere to the Zen of Python (import this).
* Ensure code is robust with comprehensive error handling and testing.
* Use map and list comprehensions instead of for loops where possible.

## Code Style and Formatting (PEP 8)
* **Indentation**: Use 4 spaces per indentation level.
* **Line Length**: Limit lines to 88 characters for code (like the Black code formatter).
* **Imports**:
    *   Import modules at the top of the file.
    *   Group imports in the order: standard library, third-party, local application.
    *   Use absolute imports.
    *   Use a tool like `isort` to automatically manage import ordering.
* **Whitespace**: Use whitespace to improve readability, but avoid extraneous whitespace.
* **Autoformatters**: Use tools like `black` and `ruff` to enforce consistent style.

## Naming Conventions (PEP 8)
* **Variables, Functions, and Modules**: `snake_case`.
* **Classes**: `PascalCase`.
* **Constants**: `UPPER_SNAKE_CASE`.
* **Private Attributes**: Use a leading underscore `_private_attribute`.
* **Name Mangling**: Use two leading underscores `__name_mangled` for attributes you don't want subclasses to inherit.

## Documentation (Docstrings & Type Hinting)
* **Docstrings (PEP 257)**:
    *   Write docstrings for all public modules, classes, functions, and methods.
    *   Use the Google Python Style for docstrings.
    *   Include a one-line summary, followed by a more detailed description, `Args`, `Returns`, and `Raises` sections.
* **Type Hinting (PEP 484)**:
    *   Use type hints for all function signatures and variables where the type may not be obvious.
    *   This improves static analysis and code completion.

```python
from typing import List

def calculate_average(numbers: List[float]) -> float:
    """Calculates the average of a list of numbers.

    Args:
        numbers: A list of floating-point numbers.

    Returns:
        The average of the numbers in the list.

    Raises:
        ValueError: If the list of numbers is empty.
    """
    if not numbers:
        raise ValueError("The list of numbers cannot be empty.")
    return sum(numbers) / len(numbers)
```

## Error Handling
* **Use specific exceptions**: Catch specific exceptions rather than using a bare `except:` clause.
* **Create custom exceptions**: Define custom exception classes for your application to represent specific error conditions.
* **Use `try...except...else...finally`**:
    *   The `try` block contains code that might raise an exception.
    *   The `except` block handles the exception.
    *   The `else` block executes if no exceptions are raised.
    *   The `finally` block executes no matter what, and is used for cleanup.

## Logging
* Use the `logging` module instead of `print()` for debugging and tracking events.
* Configure different logging levels (`DEBUG`, `INFO`, `WARNING`, `ERROR`, `CRITICAL`).
* Use a centralized logging configuration for your application.

```python
import logging

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

def my_function():
    logging.info("Starting my_function.")
    try:
        # ... function logic ...
        logging.debug("Intermediate step successful.")
    except Exception as e:
        logging.error(f"An error occurred: {e}", exc_info=True)
    logging.info("Finished my_function.")
```

## Virtual Environments and Dependency Management
* **`venv`**: Always use a virtual environment for each project to isolate dependencies.
* **`pip` and `requirements.txt`**: Use `pip` to install packages and `pip freeze > requirements.txt` to record the dependencies.
* **Poetry or Pipenv**: For more complex projects, consider using tools like Poetry or Pipenv for dependency management and packaging.

## Testing
* **`unittest` or `pytest`**: Write unit tests for your code. `pytest` is generally recommended for its simplicity and powerful features.
* **Test Coverage**: Aim for high test coverage to ensure your code is reliable.
* **Fixtures**: Use fixtures (especially in `pytest`) to set up and tear down test state.

## Script Structure
* Use the `if __name__ == "__main__":` block to make your scripts reusable as modules.

```python
def main():
    # Main logic of the script
    pass

if __name__ == "__main__":
    main()
```

# LLM Rule: Debug Rule 66 - Verbose Step-Trace Debugging for Python

**Description:**
This rule defines a process for instrumenting specified Python code (functions, methods) with detailed logging statements to trace execution flow and data state at runtime. The goal is to produce verbose console output that helps pinpoint exactly where a process is failing or where data becomes problematic.

**Invocation:**
Triggered when the user explicitly requests `<execute debug 66>` for specific functions/methods, or asks for a highly detailed, step-by-step debugging trace on specified code sections.

**Procedure:**

1.  **Identify Targets:** Determine the primary function(s) or method(s) suspected of causing the issue, including any key helper functions they call internally.

2.  **Instrument Code:** Using the `edit_file` tool, modify the target Python code by adding logging statements at critical points:
    *   **Function Entry/Exit:**
        *   Add `logging.debug(f"--- Entering {__name__}.{function.__name__} ---")` at the start.
        *   Add `logging.debug(f"--- Exiting {__name__}.{function.__name__} ---")` just before the return statement.
    *   **Argument Inspection:**
        *   Immediately after entry, log key input arguments: `logging.debug(f"    Arg: {arg_name} = {repr(arg_value)}")`.
    *   **Major Logic Steps:**
        *   Add `logging.debug("    Step: [Description of step about to happen]...")` *before* the step.
        *   Add `logging.debug("    Step: [Description of step completed].")` *after* the step.
    *   **Data State Inspection:**
        *   Before a crucial operation on a variable (e.g., a pandas DataFrame), log its state:
            *   `logging.debug(f"    Data State ({variable_name}): Shape={df.shape}, Columns={df.columns.tolist()}")`
            *   `logging.debug(f"    Data Head:\n{df.head().to_string()}")`
    *   **Loops:**
        *   Inside the loop, log the current iteration: `logging.debug(f"    Looping: Processing item {item}")`
    *   **Conditional Logic (`if/else`):**
        *   Add `logging.debug("    Condition TRUE: [Brief reason]")` inside the `if` block.
        *   Add `logging.debug("    Condition FALSE: [Brief reason]")` inside the `else` block.
    *   **Function Call Results:**
        *   Immediately after calling an important internal or helper function, log the result: `logging.debug(f"    Result from {called_function.__name__}: {repr(result_variable)}")`

3.  **Output Formatting:**
    *   Use the `logging` module with a `DEBUG` level.
    *   Use f-strings for easy formatting.
    *   Use indentation to indicate nesting levels.

4.  **Result:** The execution of the instrumented code will produce a detailed trace, allowing the user and the AI to follow the execution path and inspect data at intermediate steps to identify the source of errors or unexpected behavior.
