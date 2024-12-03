/*
* Name: Vandan Amin
* Student ID: 44006979
* Date: Nov 19 2024
*
* CS 465 - Scientific Computing
* Department of Computer Science, Central Washington University
* Dr. Donald Davendra
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <limits.h>

#define MAX_CITIES 1000
#define MAX_ITERATIONS 10000
#define POPULATION_SIZE 50
#define MUTATION_RATE 0.1

// Data structure for storing city coordinates
typedef struct {
    int id;
    double x, y;
} City;

int calculateDistance(City a, City b);

City cities[MAX_CITIES];
int cityCount;
int distance[MAX_CITIES][MAX_CITIES];
char edgeWeightType[20];
char edgeWeightFormat[20];

// Function to read STSP or ATSP data from a file
void readData(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        fclose(file);
        return;
    }

    char line[256];
    int is_atsp = 0;
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "DIMENSION", 9) == 0) {
            sscanf(line, "%*s %d", &cityCount);
            if (cityCount > MAX_CITIES) {
                printf("City count exceeds MAX_CITIES limit (%d)\n", MAX_CITIES);
                fclose(file);
                return;
            }
        } else if (strncmp(line, "EDGE_WEIGHT_TYPE", 16) == 0) {
            sscanf(line, "%*s %s", edgeWeightType);
            if (strcmp(edgeWeightType, "EXPLICIT") == 0) {
                is_atsp = 1;
            }
        } else if (strncmp(line, "EDGE_WEIGHT_FORMAT", 18) == 0) {
            sscanf(line, "%*s %s", edgeWeightFormat);
        } else if (strncmp(line, "NODE_COORD_SECTION", 18) == 0) {
            for (int i = 0; i < cityCount; i++) {
                if (fscanf(file, "%d %lf %lf", &cities[i].id, &cities[i].x, &cities[i].y) != 3) {
                    printf("Error reading city data\n");
                    fclose(file);
                    return;
                }
            }
            break;
        } else if (strncmp(line, "EDGE_WEIGHT_SECTION", 19) == 0) {
            if (strcmp(edgeWeightFormat, "FULL_MATRIX") == 0) {
                for (int i = 0; i < cityCount; i++) {
                    for (int j = 0; j < cityCount; j++) {
                        if (fscanf(file, "%d", &distance[i][j]) != 1) {
                            printf("Error reading distance data\n");
                            fclose(file);
                            return;
                        }
                    }
                }
            } else if (strcmp(edgeWeightFormat, "UPPER_ROW") == 0) {
                for (int i = 0; i < cityCount; i++) {
                    for (int j = i + 1; j < cityCount; j++) {
                        if (fscanf(file, "%d", &distance[i][j]) != 1) {
                            printf("Error reading distance data\n");
                            fclose(file);
                            return;
                        }
                        distance[j][i] = distance[i][j];
                    }
                }
            } else {
                printf("Unsupported EDGE_WEIGHT_FORMAT: %s\n", edgeWeightFormat);
                fclose(file);
                return;
            }
            break;
        }
    }

    if (!is_atsp) {
        // Initialize distance matrix for STSP based on EDGE_WEIGHT_TYPE
        for (int i = 0; i < cityCount; i++) {
            for (int j = 0; j < cityCount; j++) {
                if (i != j) {
                    if (strcmp(edgeWeightType, "EUC_2D") == 0) {
                        distance[i][j] = calculateDistance(cities[i], cities[j]);
                    } else {
                        printf("Unsupported EDGE_WEIGHT_TYPE: %s\n", edgeWeightType);
                        fclose(file);
                        return;
                    }
                } else {
                    distance[i][j] = 0;
                }
            }
        }
    }

    fclose(file);
}

// Function to calculate the distance between two cities
int calculateDistance(City a, City b) {
    return (int)(sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)) + 0.5);
}

// Approach: Simulated Annealing
void simulatedAnnealing() {
    clock_t startTime = clock();
    int currentSolution[MAX_CITIES];
    int bestSolution[MAX_CITIES];
    int currentCost, bestCost;
    double temperature = 10000.0;
    double coolingRate = 0.995;

    // Initialize the solution (0, 1, 2, ..., cityCount - 1)
    for (int i = 0; i < cityCount; i++) {
        currentSolution[i] = i;
        bestSolution[i] = i;
    }

    // Calculate the initial cost
    currentCost = 0;
    for (int i = 0; i < cityCount - 1; i++) {
        currentCost += distance[currentSolution[i]][currentSolution[i + 1]];
    }
    currentCost += distance[currentSolution[cityCount - 1]][currentSolution[0]];
    bestCost = currentCost;

    // Simulated Annealing loop
    for (int iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
        // Generate a new solution by swapping two cities
        int city1 = rand() % cityCount;
        int city2 = (city1 + 1 + rand() % (cityCount - 1)) % cityCount;
        int temp = currentSolution[city1];
        currentSolution[city1] = currentSolution[city2];
        currentSolution[city2] = temp;

        // Calculate the new cost
        int newCost = 0;
        for (int i = 0; i < cityCount - 1; i++) {
            newCost += distance[currentSolution[i]][currentSolution[i + 1]];
        }
        newCost += distance[currentSolution[cityCount - 1]][currentSolution[0]];

        // Decide whether to accept the new solution
        if (newCost < currentCost || (exp((currentCost - newCost) / temperature) > ((double)rand() / RAND_MAX))) {
            currentCost = newCost;
            if (currentCost < bestCost) {
                bestCost = currentCost;
                memcpy(bestSolution, currentSolution, sizeof(int) * cityCount);
            }
        } else {
            // undo the swap
            temp = currentSolution[city1];
            currentSolution[city1] = currentSolution[city2];
            currentSolution[city2] = temp;
        }

        // Cool down
        temperature *= coolingRate;
    }

    // Print results and time
    printf("Total distance found by Simulated Annealing: %d\n", bestCost);
    clock_t endTime = clock();
    double timeSpent = (double)(endTime - startTime) / CLOCKS_PER_SEC;
    printf("Time taken by Simulated Annealing: %.3f seconds\n", timeSpent);
}

// Approach: Genetic Algorithm
void geneticAlgorithm() {
    clock_t startTime = clock();
    int population[POPULATION_SIZE][MAX_CITIES];
    int fitness[POPULATION_SIZE];
    int bestSolution[MAX_CITIES];
    int bestCost = INT_MAX;

    // Initialize population
    for (int i = 0; i < POPULATION_SIZE; i++) {
        for (int j = 0; j < cityCount; j++) {
            population[i][j] = j;
        }
        // Shuffle cities to create random individuals
        for (int j = 0; j < cityCount; j++) {
            int swapIndex = rand() % cityCount;
            int temp = population[i][j];
            population[i][j] = population[i][swapIndex];
            population[i][swapIndex] = temp;
        }
    }

    // Genetic Algorithm loop
    for (int generation = 0; generation < MAX_ITERATIONS; generation++) {
        // Calculate fitness
        for (int i = 0; i < POPULATION_SIZE; i++) {
            fitness[i] = 0;
            for (int j = 0; j < cityCount - 1; j++) {
                fitness[i] += distance[population[i][j]][population[i][j + 1]];
            }
            fitness[i] += distance[population[i][cityCount - 1]][population[i][0]];

            // Update best solution
            if (fitness[i] < bestCost) {
                bestCost = fitness[i];
                memcpy(bestSolution, population[i], sizeof(int) * cityCount);
            }
        }

        // Selection and Crossover
        int newPopulation[POPULATION_SIZE][MAX_CITIES];
        for (int i = 0; i < POPULATION_SIZE; i += 2) {
            // Select two parents (tournament selection)
            int parent1 = rand() % POPULATION_SIZE;
            int parent2 = rand() % POPULATION_SIZE;
            int crossoverPoint = rand() % cityCount;

            // Crossover
            for (int j = 0; j < crossoverPoint; j++) {
                newPopulation[i][j] = population[parent1][j];
                newPopulation[i + 1][j] = population[parent2][j];
            }
            for (int j = crossoverPoint; j < cityCount; j++) {
                newPopulation[i][j] = population[parent2][j];
                newPopulation[i + 1][j] = population[parent1][j];
            }
        }

        // Mutation
        for (int i = 0; i < POPULATION_SIZE; i++) {
            if (((double)rand() / RAND_MAX) < MUTATION_RATE) {
                int city1 = rand() % cityCount;
                int city2 = rand() % cityCount;
                int temp = newPopulation[i][city1];
                newPopulation[i][city1] = newPopulation[i][city2];
                newPopulation[i][city2] = temp;
            }
        }

        // Replace old population with new population
        memcpy(population, newPopulation, sizeof(int) * POPULATION_SIZE * cityCount);
    }

    // Print the results and time
    printf("Total distance found by Genetic Algorithm: %d\n", bestCost);
    clock_t endTime = clock();
    double timeSpent = (double)(endTime - startTime) / CLOCKS_PER_SEC;
    printf("Time taken by Genetic Algorithm: %.3f seconds\n", timeSpent);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];

    readData(filename);
    simulatedAnnealing();
    geneticAlgorithm();

    return 0;
}
