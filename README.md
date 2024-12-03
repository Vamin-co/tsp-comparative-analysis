### TSP Project: Comparative Analysis of TSP Algorithms

#### Overview
This project compares Simulated Annealing (SA) and Genetic Algorithm (GA) approaches for solving the Traveling Salesman Problem (TSP). Implemented in C, the study evaluates their performance in terms of solution quality and computational efficiency across multiple datasets.

#### Features
- Simulated Annealing with configurable temperature and cooling rate.
- Genetic Algorithm with population-based optimization.
- Performance comparison using distance and runtime metrics.

#### Requirements
- C Compiler (e.g., GCC)
- Make utility (optional)

#### Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/Vamin-co/tsp-comparative-analysis
   cd tsp-comparative-analysis
   ```
2. Compile the code:
   ```bash
   make
   ```

#### Usage
Run the executable with a TSP dataset file:
```bash
./tsp_solver <dataset-file>
```
Example:
```bash
./tsp_solver a280.tsp
```

#### Results
Performance metrics, including the total distance and runtime for both algorithms, are printed to the console. 

#### Datasets
The project includes tests on various datasets such as `a280.tsp`, `bayg29.tsp`, and `brazil58.tsp`. The datasets can be customized by the user.

