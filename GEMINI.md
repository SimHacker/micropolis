# General Rules

* When the plan changes, update the project plan document
* When the context window or cache is getting full and tokens are running out, summarise the work done so far in a hand over document.

# Bash Coding Standards and Best Practices

## Core Principles
* Prioritize readability, maintainability, and error resilience
* Design scripts as self-documenting, self-contained utilities
* Anticipate and handle failures at every stage
* Provide clear logging for both success paths and error conditions

## Script Structure
* Begin with a descriptive header comment explaining purpose, inputs, and outputs
* Place all constants and configuration at the top of the script
* Implement a modular, function-based design with single-responsibility functions
* Create a `main()` function to manage overall execution flow
* Call `main "$@"` at the end of the script to enable testing of individual functions

```bash
#!/bin/bash
# Purpose: Process genome scaffolds and analyze sequence lengths
# Usage: ./script.sh <input_dir> <min_length>

# Configuration
readonly MIN_LENGTH="${2:-10000}"
readonly INPUT_DIR="${1:-/default/path}"

# Functions
process_file() {
    local file="$1"
    # Processing logic
}

main() {
    log_message "INFO" "Starting processing with min_length=${MIN_LENGTH}"
    # Main processing logic
}

# Execute main function
main "$@"
```

## Variable Naming and Usage
* Use UPPERCASE for global constants and environment variables
* Use lowercase for local variables and function names
* Always quote variable references: `"${variable}"` not `$variable`
* Use meaningful, descriptive names that indicate purpose or content
* Declare variables with appropriate scope (`local` within functions)
* Set `readonly` for constants to prevent accidental modification

```bash
# Global constants
readonly OUTPUT_DIR="/path/to/output"

process_file() {
    # Local variables
    local input_file="$1"
    local file_name="$(basename "${input_file}")"
}
```

## Error Handling and Validation
* Start scripts with `set -euo pipefail` to enable strict error handling
* Validate all inputs before processing (existence, permissions, format)
* Include custom error handling with descriptive messages
* Create error exit function with appropriate exit codes
* Use trap to handle cleanup on script exit
* Check command success with explicit conditionals

```bash
set -euo pipefail

error_exit() {
    local message="$1"
    local code="${2:-1}"
    log_message "ERROR" "${message}"
    exit "${code}"
}

# Validate input file
if [[ ! -f "${INPUT_FILE}" ]]; then
    error_exit "Input file does not exist: ${INPUT_FILE}" 2
fi

# Handle cleanup on exit
trap cleanup EXIT
```

## Logging and Debugging
* Implement hierarchical log levels: DEBUG, INFO, WARNING, ERROR, SUCCESS
* Include timestamps, log level, and context in all log messages
* Log both to files and to console with appropriate formatting
* Create separate log and error files for each run
* Use unique identifiers in log filenames (timestamp, job ID)
* Include resource usage data at regular intervals

```bash
log_message() {
    local level="$1"
    local message="$2"
    local timestamp="$(date '+%Y-%m-%d %H:%M:%S')"
    echo "[${timestamp}] ${level}: ${message}" | tee -a "${LOG_FILE}" >&2
}

report_resources() {
    log_message "RESOURCE" "Memory: $(free -h | grep 'Mem:' | awk '{print $3 " of " $2}')"
    log_message "RESOURCE" "Disk: $(df -h "${OUTPUT_DIR}" | tail -1 | awk '{print $5 " used"}')"
}
```

## File and Directory Management
* Verify directory existence before writing; create if necessary
* Use atomic file operations to prevent corrupted outputs
* Write to temporary files first, then move to final location
* Implement consistent naming conventions across all files
* Create hierarchical directory structures for complex data
* Use full paths for critical operations, relative paths for others
* Clean up temporary files and directories on completion

```bash
# Create output directory safely
mkdir -p "${OUTPUT_DIR}"

# Write output atomically
generate_data > "${TEMP_FILE}"
mv "${TEMP_FILE}" "${OUTPUT_FILE}"

# Clean up temporary files
cleanup() {
    rm -rf "${TEMP_DIR}"
    log_message "INFO" "Temporary files removed"
}
```

## Control Flow and Loops
* Use `if/then/else/fi` for conditionals (not curly braces)
* Use `for/do/done` for iteration (not curly braces)
* Prefer `for` loops with explicit sequences rather than C-style counting
* Use `while` loops for processing stream input
* Validate loop conditions before entering loops
* Include loop progress indicators for long-running operations

```bash
# Preferred loop format
for file in "${FILES[@]}"; do
    process_file "${file}"
done

# Process file content line by line
while IFS= read -r line; do
    process_line "${line}"
done < "${INPUT_FILE}"
```

## SLURM Job Management
* Generate dynamic job names that include relevant parameters
* Include job name in all output file paths for traceability
* Use job dependencies to create processing pipelines
* Prioritize resource efficiency (CPU, memory, runtime)
* Implement job array submissions for parallel batch processing
* Verify output file existence before submitting dependent jobs
* Use consistent partition assignment for related jobs

```bash
# Dynamic job naming
JOB_NAME="process_${SAMPLE_ID}_${TIMESTAMP}"

# SLURM directives
#SBATCH --job-name="${JOB_NAME}"
#SBATCH --output="logs/${JOB_NAME}_%j.out"
#SBATCH --error="logs/${JOB_NAME}_%j.err"
#SBATCH --time=1:00:00
#SBATCH --mem=4G
#SBATCH --cpus-per-task=2

# Job dependency example
if [[ -f "${PREVIOUS_OUTPUT}" ]]; then
    sbatch --dependency=afterok:${PREVIOUS_JOB_ID} "${NEXT_SCRIPT}" "${PARAMS}"
fi
```

## Performance Considerations
* Pre-allocate storage for large operations
* Process data in appropriate batch sizes
* Monitor and log resource usage at regular intervals
* Use pipes instead of temporary files for sequential operations
* Validate processed data after each critical step
* Implement checkpointing for long-running operations

```bash
# Process in batches with progress tracking
local total_items="${#ITEMS[@]}"
local batch_size=100
local processed=0

for ((i=0; i<total_items; i+=batch_size)); do
    process_batch "${i}" "$((i+batch_size < total_items ? i+batch_size : total_items))"
    processed=$((i+batch_size < total_items ? i+batch_size : total_items))
    log_message "INFO" "Progress: ${processed}/${total_items} items ($(( processed * 100 / total_items ))%)"
done
```

## Command Execution and Tools
* Use absolute paths for critical system commands
* Validate command existence before execution
* Capture and check command exit codes explicitly
* Use built-in bash commands when possible for performance
* Prefer modern utilities that handle Unicode and large files correctly
* Document command options with comments for complex operations

```bash
# Verify command exists
if ! command -v seqkit &>/dev/null; then
    error_exit "Required command 'seqkit' not found"
fi

# Execute with error checking
if ! seqkit stats "${INPUT_FILE}" > "${OUTPUT_FILE}"; then
    error_exit "seqkit command failed on ${INPUT_FILE}"
fi
```

## Documentation and Maintainability
* Include usage examples in script header comments
* Document function purpose, parameters, and return values
* Add comments explaining complex logic or non-obvious decisions
* Include version information and change history
* Document dependencies and required environment
* Add inline citations to relevant documentation or references

```bash
#==============================================================================
# analyze_scaffolds.sh - Analyze genome scaffold lengths
# 
# Usage: ./analyze_scaffolds.sh INPUT_DIR MIN_LENGTH
#
# Examples:
#   ./analyze_scaffolds.sh /data/scaffolds 10000
#   ./analyze_scaffolds.sh /data/scaffolds 5000 > results.txt
#
# Dependencies:
#   - seqkit (v2.0+)
#   - GNU coreutils
#
# Version: 1.2 (2025-03-28)
#==============================================================================
```


# Handover document header - Handover Document

Use this document as a general tempalte for writing a handover document 

## Current State

### Key Architectural Components

1. **Data Model**: 
   - Uses NetworkX's MultiDiGraph to represent relationships between entities
   - Core relationships:
     - Proteins → Peptides → Modifications
     - Peptides → Grid Cells (with intensities)
     - Samples → Conditions/Replicates
   - Complete graph persistence to disk for faster reloading

2. **Grid Assignment Logic**:
   - Multiple strategies for coordinate determination:
     - Explicit grid coordinates (Grid_Row/Col columns)
     - Source file name pattern extraction (e.g., "_A1_", "B5.wiff")
     - Area Sample number mapping to grid positions
     - Retention time (RT) and mass-based coordinate calculation
   - Fallback hash-based assignment for peptides without coordinates
   - Comprehensive validation and consistency checking

3. **Performance Optimizations**:
   - Area sample grid cache prevents redundant calculations (major performance gain)
   - Parallel processing with ThreadPoolExecutor for batch operations
   - Reduced logging verbosity with environment variable `PROTEOMICS_LOG_LEVEL`
   - Strategic caching of grid data for visualization
   - Batch processing of graph updates

4. **Visualization System**:
   - Interactive 8x10 grid visualization replicating actual 2D gel appearance
   - Heatmap view for comparative analysis across conditions
   - Color-coded intensity representation
   - PTM-specific filtering with Ascore cutoff support

## Recent Refactoring

The codebase has undergone significant optimization work focused on grid assignment performance:

1. **Grid Assignment Performance**:
   - Added caching system for Area Sample → Grid position mappings
   - Eliminated redundant calculations of the same mapping
   - Reduced verbose logging that was slowing down processing
   - Implemented environment variable control for logging verbosity
   - Added summary statistics instead of detailed peptide lists

2. **Data Processing Optimization**:
   - Better thread management in parallel processing
   - Batch processing of rows for more efficient NetworkX operations
   - Fixed error handling and added comprehensive logging
   - Added area sample grid cache with efficient lookup

3. **Logging Improvements**:
   - Fixed compatibility with loguru logging system
   - Added conditional logging based on verbosity level
   - Reduced log file size while maintaining critical information
   - Added top-5 grid summary at INFO level

4. **Grid Consistency Analysis**:
   - Added automatic analysis of grid assignments after loading
   - Detection of peptides mapped to multiple grid cells
   - Summary statistics for grid assignment patterns
   - Support for investigating grid assignment inconsistencies

## Known Issues

1. **Neo4j References**: 
   The UI code may still contain Neo4j database references, despite the backend using NetworkX exclusively. The error "Could not connect to Neo4j database" indicates that some UI components are still trying to use a Neo4j connection that doesn't exist.

2. **Memory Usage**: 
   While performance is improved, processing very large datasets may still require optimization of the in-memory graph structure. Consider implementing more aggressive caching or database offloading for extremely large datasets.

3. **Grid Inconsistencies**: 
   Some peptides and proteins are assigned to multiple grid cells, which may need further investigation. The current implementation detects these cases but doesn't yet resolve them automatically.

4. **UI/Backend Synchronization**: 
   The UI may not be fully aware of all the optimizations made in the backend. Some components might be using older patterns or assumptions that need to be updated.

## Next Steps

### Immediate Priorities

1. **UI Refactoring**: 
   - Update UI components to fully use the NetworkX implementation
   - Remove any remaining Neo4j dependencies
   - Ensure heatmap view properly connects to the NetworkX backend
   - Update any direct database queries to use the graph API instead

2. **Testing and Validation**:
   - Create comprehensive test cases for grid assignment logic
   - Validate correct grid assignments with known datasets
   - Test performance with larger datasets
   - Verify threading behavior with stress tests

### Medium-Term Improvements

1. **Additional Caching Strategies**:
   - Consider implementing more aggressive caching for large datasets
   - Add optional persistent caching to disk for frequently accessed data
   - Implement LRU cache for peptide-grid relationships

2. **Visualization Enhancements**:
   - Improve the heatmap view to better highlight inconsistent grid assignments
   - Add visual indicators for peptides with multiple grid assignments
   - Enhance the UI for grid browsing and exploration
   - Add export options for grid assignment analysis

3. **Data Processing Refinements**:
   - Fine-tune the grid assignment algorithm for edge cases
   - Add ML-based grid prediction for ambiguous cases
   - Implement smarter batch processing for very large files

### Long-Term Vision

1. **Machine Learning Integration**:
   - Develop predictive models for grid assignment
   - Implement clustering for improved PTM analysis
   - Add anomaly detection for unusual peptide patterns

2. **Advanced Analytics**:
   - Enhance statistical analysis of PTM patterns
   - Add time-series analysis for longitudinal studies
   - Implement comparison tools across multiple experiments

3. **Architecture Evolution**:
   - Consider hybrid database approach for very large datasets
   - Evaluate microservices architecture for processing pipeline
   - Explore cloud deployment options for collaborative research

## Configuration and Environment

### Environment Variables

- `PROTEOMICS_LOG_LEVEL`: Controls logging verbosity (INFO or DEBUG)

### Important Files

- `app/modules/data_loader.py`: Core implementation of the NetworkX data model and grid assignment logic
- `app/ui/dashboard.py`: Main UI controller that might need Neo4j reference cleanup
- `app/ui/heatmap_view.py`: Likely contains Neo4j references that need updating
- `app/ui/gel_grid.py`: 2D gel grid visualization component

### Logging

- Primary log file: `proteomics_data.log` with 10MB rotation
- Debug level logging includes detailed grid assignment information
- INFO level provides summary statistics only

## Performance Metrics

- **File Loading**: Substantial speed improvement over previous implementation
- **Grid Assignment**: No longer redundantly calculates the same grid positions
- **Memory Usage**: More efficient with large datasets due to better caching
- **Concurrency**: Thread-safe implementation with proper error handling

## Contact Information

For questions about the recent refactoring work or implementation details:

- Developer: [Your Name/Contact Info]
- Last Updated: [Current Date]

## Acknowledgements

This handover document summarizes the work completed during performance optimization and refactoring of the 2D Gel Dashboard for Proteomics application. The improvements focus on enhancing the grid assignment process, optimizing memory usage, and implementing a more efficient data processing pipeline.
handover.md
16 KB


# R Programming Standards and Best Practices

This guide outlines the standards and best practices for writing efficient, maintainable, and reproducible R code. Following these guidelines will ensure consistency and readability across projects.

## Core Principles

* Write code that is **readable**, **maintainable**, and **reproducible**
* Prioritize **functional programming** approaches over imperative programming
* Design code to be **modular** with clearly defined responsibilities
* Follow **consistent naming conventions** and code organization
* Ensure **comprehensive documentation** of all functions and complex operations
* Prefer **tidyverse** approaches for data manipulation and visualization

## Code Design and Workflow

### General Programming Style

* Break complex problems into smaller, focused functions
* Prioritize composable functions that each do one thing well
* Place commas at the start of each line rather than the end in multi-line statements
* Limit line length to 80-100 characters for improved readability
* Use the native pipe operator (`|>`) rather than the magrittr pipe (`%>%`)
* Align assignments within code blocks for improved readability
* Use meaningful spacing and indentation to visually organize your code

### Commenting and Documentation

* Include comprehensive header comments for scripts and functions
* Document **why** code was written, not just what it does
* Use roxygen2 style documentation for all functions:
  - `@param` for all parameters with types and expected formats
  - `@return` describing the output shape and format
  - `@examples` showing usage with sample data
  - `@description` for high-level function purpose
* Include details on input/output data frames, including expected columns
* Comment complex or non-obvious code sections with explanations

```r
#' Calculate weighted mean of values by group
#' 
#' @description
#' Calculates the weighted mean of a numeric column, with weights applied
#' and results grouped by a categorical variable.
#' 
#' @param data A data frame containing the values and weights
#' @param value_col Name of the numeric column containing values
#' @param weight_col Name of the numeric column containing weights
#' @param group_col Name of the categorical column for grouping
#' 
#' @return A data frame with columns:
#' - group: The grouping variable values
#' - weighted_mean: The calculated weighted means
#' 
#' @examples
#' df <- data.frame(
#'   category = c("A", "A", "B", "B", "C"),
#'   value = c(10, 20, 15, 25, 30),
#'   weight = c(1, 2, 1, 3, 2)
#' )
#' calculateWeightedMeanByGroup(df, value_col = value, weight_col = weight, group_col = category)
calculateWeightedMeanByGroup <- function(data, value_col, weight_col, group_col) {
  data |>
    group_by({{ group_col }}) |>
    summarize(
      weighted_mean = weighted.mean({{ value_col }}, {{ weight_col }})
    )
}
```

### Variable and Function Naming

* Use **descriptive**, **precise** names that clearly indicate purpose
* Follow consistent naming conventions:
  - `camelCase` for function names (e.g., `calculateMean`, `filterData`)
  - `snake_case` for variable names (e.g., `total_count`, `mean_value`)
  - `SCREAMING_SNAKE_CASE` for constants (e.g., `MAX_ITERATIONS`, `DEFAULT_PATH`)
* Never reuse variable names within the same script or function
* Avoid abbreviations unless very common in the domain
* Include relevant units in names where appropriate (e.g., `time_seconds`, `distance_km`)

### Error Handling

* Validate inputs at the beginning of functions
* Use informative error messages that explain what went wrong and how to fix it
* Avoid using `tryCatch()` during interactive development as it masks useful errors
* Only add error handling once code is working correctly and well-tested

## Functional Programming Practices

### Functional Programming Fundamentals

* Design **pure functions** whenever possible (same inputs always produce same outputs)
* Isolate side effects (I/O, plotting, randomization) in dedicated functions
* Minimize state changes and mutation of data
* Use functional composition to build complex operations from simple ones

### Avoiding Loops

* Prefer vectorized operations and functional approaches over explicit loops
* Use the **purrr** package functions as your first choice for iteration:
  - `map()`, `map_dbl()`, `map_chr()`, etc. for single-input iteration
  - `map2()`, `pmap()` for multi-input iteration
  - `walk()` family for side effects
* If purrr isn't available, use base R's `lapply()`, `sapply()`, or `vapply()`
* Only use `for` loops when vectorized solutions are impractical or inefficient

```r
# AVOID THIS:
result <- c()
for (i in 1:length(numbers)) {
  result[i] <- numbers[i]^2
}

# DO THIS INSTEAD:
# Vectorized solution
result <- numbers^2

# Or with purrr
result <- map_dbl(numbers, ~ .x^2)
```

### Efficient Data Manipulation Patterns

* Pre-allocate output containers for large operations
* Use `reduce()` and `accumulate()` for sequential operations on list elements
* Create function factories to generate specialized functions

```r
# Using reduce to sequentially combine elements
accumulate(letters[1:5], paste, sep = ".")
# [1] "a" "a.b" "a.b.c" "a.b.c.d" "a.b.c.d.e"

# Function factory example
createScalingFunction <- function(scaling_factor) {
  function(x) {
    x * scaling_factor
  }
}
double <- createScalingFunction(2)
triple <- createScalingFunction(3)
```

### Using Expand Grid for Combinations

* Instead of nested loops or map calls, use `expand_grid()` with `pmap()`
* This provides explicit tabular structure for combinations and is easier to parallelize

```r
# Process all combinations of two lists
params <- expand_grid(
  x = 1:5,
  y = c("a", "b", "c")
) 

results <- params |>
  mutate(result = pmap_chr(list(x, y), ~ paste0(..1, ..2)))
```

## Tidyverse and Tidy Evaluation

### Data Wrangling Standards

* Use **tidyverse** functions for data manipulation (dplyr, tidyr, purrr)
* Chain operations with the pipe operator
* Use **janitor** for data cleaning and standardizing column names
* Keep transformations in logical groupings with clear intermediate object names

### Using Tidy Evaluation

* Use curly-curly (`{{}}`) for passing column names to tidyverse functions
* Use `.data[[var_name]]` pronoun for dynamic column selection with strings
* Use the `:=` operator when creating dynamic column names

```r
# Using curly-curly for column name passing
summarizeByGroup <- function(data, group_var, summarize_var) {
  data |>
    group_by({{ group_var }}) |>
    summarize(mean = mean({{ summarize_var }}, na.rm = TRUE))
}

# Using .data pronoun for string column names
summarizeByString <- function(data, group_name, value_name) {
  data |>
    group_by(.data[[group_name]]) |>
    summarize(mean = mean(.data[[value_name]], na.rm = TRUE))
}

# Using := for dynamic column naming
createSummary <- function(data, var, stat_name = "mean") {
  data |>
    summarize("{{ var }}_{{ stat_name }}" := mean({{ var }}, na.rm = TRUE))
}
```

## Package and Dependency Management

### Loading Libraries

* Use **pacman** for reproducible library management
* Load all packages at the beginning of scripts
* Provide fallback installation code for missing dependencies

```r
# Standard package loading preamble
if (!require("pacman")) { 
  install.packages("pacman")
  library(pacman) 
}

pacman::p_load(
  tidyverse,  # Data manipulation and visualization
  lubridate,  # Date handling
  glue,       # String interpolation
  janitor,    # Data cleaning
  fs          # File system operations
)

# For Bioconductor packages
if (!requireNamespace("BiocManager", quietly = TRUE)) {
    install.packages("BiocManager")
}
BiocManager::install("GenomicRanges")
```

### Managing Function Name Conflicts

* Use the **conflicted** package to explicitly resolve function name conflicts
* Explicitly qualify ambiguous function calls with package names
* Set conflict preferences at the start of scripts

```r
library(conflicted)

# Set default preferences for common conflicts
conflicted::conflict_prefer("filter", "dplyr")
conflicted::conflict_prefer("select", "dplyr")
conflicted::conflict_prefer("rename", "dplyr")
```

## Project Organization and Workflow

### Structure and Organization

* Organize projects with consistent directory structure
* Create dedicated directories for data, scripts, outputs, and documentation
* Use relative paths with the **here** package for file references
* Maintain a clear separation between data acquisition, processing, and analysis

### Reproducibility Best Practices

* Use version control (Git) for tracking code changes
* Document dependencies with `sessionInfo()` or **renv**
* Include seed setting for random processes: `set.seed()`
* Create self-contained analysis with all data processing steps
* Write automated tests for critical functions

## Editing and Refactoring Code

* Always review code changes before submitting or finalizing
* Check that code deletions don't remove unrelated functionality
* When refactoring:
  - Ensure all tests still pass
  - Verify outputs remain unchanged
  - Document significant design changes
* Maintain a change log for major modifications
* Focus changes on addressing specific requirements rather than wholesale rewrites

---

Remember: Prioritize readability and maintainability over cleverness or terseness. Code is read far more often than it is written.

# LLM Rule: Debug Rule 66 - Verbose Step-Trace Debugging

**Description:**
This rule defines a process for instrumenting specified R code (functions, S4 methods) with detailed logging statements to trace execution flow and data state at runtime. The goal is to produce verbose console output that helps pinpoint exactly where a process is failing or where data becomes problematic.

**Invocation:**
Triggered when the user explicitly requests `<execute debug 66>` for specific functions/methods, or asks for highly detailed, step-by-step debugging trace on specified code sections.

**Procedure:**

1.  **Identify Targets:** Determine the primary function(s) or S4 method(s) suspected of causing the issue, including any key helper functions they call internally.

2.  **Instrument Code:** Using the `edit_file` tool, modify the target R code by adding logging statements at critical points:
    *   **Function Entry/Exit:**
        *   Add `message(sprintf("--- Entering [FunctionName] ---"))` at the start.
        *   Add `message(sprintf("--- Exiting [FunctionName] ---"))` just before the return statement. If possible, include the value being returned: `message(sprintf("--- Exiting [FunctionName]. Returning: %s ---", capture.output(str(returnValue))))`.
    *   **Argument Inspection:**
        *   Immediately after entry, log key input arguments using `message(sprintf("   [FunctionName] Arg: [ArgumentName] = %s", capture.output(str(argumentValue))))` or similar using `print()` or specific formatting depending on the argument type.
    *   **Major Logic Steps:**
        *   Add `message(sprintf("   [FunctionName] Step: [Description of step about to happen]..."))` *before* the step.
        *   Add `message(sprintf("   [FunctionName] Step: [Description of step completed]."))` *after* the step. (e.g., "Filtering data", "Pivoting wider", "Calling helper X").
    *   **Data State Inspection:**
        *   *Before* a data frame/tibble/matrix is used in a crucial operation (passed to another function, used in a join, pivoted, filtered, passed to `stats::cor`, etc.), log its state:
            *   `message(sprintf("      Data State ([VariableName]): Dims = %d rows, %d cols", nrow(variableName), ncol(variableName)))`
            *   `message("      Data State ([VariableName]) Structure:")`
            *   `utils::str(variableName)`
            *   `message("      Data State ([VariableName]) Head:")`
            *   `print(head(variableName))`
    *   **Loops/Mapping Functions (e.g., `purrr::map`, `lapply`, `for`):**
        *   Inside the loop/map function, log the current iteration identifier(s): `message(sprintf("   [map/loop] Processing item: %s", itemIdentifier))`
        *   Log the result obtained for that iteration *before* it's returned or stored.
    *   **Conditional Logic (`if/else`):**
        *   Add `message(sprintf("   [FunctionName] Condition TRUE: [Brief reason]"))` inside the `if` block.
        *   Add `message(sprintf("   [FunctionName] Condition FALSE: [Brief reason]"))` inside the `else` block.
    *   **Function Call Results:**
        *   Immediately after calling an important internal or helper function, log the structure/value of the result: `message(sprintf("   [FunctionName] Result from [CalledFunction]: %s", capture.output(str(resultVariable))))`

3.  **Output Formatting:**
    *   Use `message()` for general flow and information.
    *   Use `print()` and `utils::str()` for detailed object inspection.
    *   Use `sprintf()` within `message()` for consistent formatting.
    *   Use indentation or prefixes (e.g., `>>`, `  `, `    `) to indicate nesting levels (e.g., inside helpers called by main functions, inside loops).

4.  **Result:** The execution of the instrumented code will produce a detailed trace in the R console, allowing the user and the AI to follow the execution path and inspect data at intermediate steps to identify the source of errors or unexpected behavior.

