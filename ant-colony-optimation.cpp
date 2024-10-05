#include <iostream>
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm> 
#include <cstdlib>
#include <ctime>

using namespace std;

const int NUM_ANTS = 10;        // Number of ants
const int NUM_CITIES = 5;       // Number of cities
const int MAX_ITERATIONS = 100; // Max number of iterations
const double ALPHA = 1.0;       // Pheromone importance
const double BETA = 5.0;        // Heuristic importance
const double EVAPORATION = 0.5; // Pheromone evaporation rate
const double Q = 100.0;         // Pheromone deposit factor

// Distance between cities
double distanceMatrix[NUM_CITIES][NUM_CITIES] = {
    {0, 2, 9, 10, 7},
    {1, 0, 6, 4, 3},
    {15, 7, 0, 8, 12},
    {6, 3, 12, 0, 5},
    {10, 4, 8, 2, 0}
};

// Pheromone levels between cities
double pheromoneMatrix[NUM_CITIES][NUM_CITIES];

// Ant class definition
class Ant {
public:
    vector<int> visitedCities;
    double totalDistance = 0.0;

    Ant() {
        visitedCities.reserve(NUM_CITIES); 
    }

    void visitCity(int city) {
        visitedCities.push_back(city);
    }

    bool isVisited(int city) {
        return (find(visitedCities.begin(), visitedCities.end(), city) != visitedCities.end());
    }

    void reset() {
        visitedCities.clear();
        totalDistance = 0.0;
    }
};

// Function to calculate the probability of an ant choosing a city
int chooseNextCity(Ant& ant, int currentCity) {
    vector<double> probabilities(NUM_CITIES, 0.0);
    double denominator = 0.0;

    // Calculate the denominator of the probability formula
    for (int i = 0; i < NUM_CITIES; i++) {
        if (!ant.isVisited(i)) {
            denominator += pow(pheromoneMatrix[currentCity][i], ALPHA) * pow(1.0 / distanceMatrix[currentCity][i], BETA);
        }
    }

    // Calculate the probabilities for each city
    for (int i = 0; i < NUM_CITIES; i++) {
        if (!ant.isVisited(i)) {
            probabilities[i] = (pow(pheromoneMatrix[currentCity][i], ALPHA) * pow(1.0 / distanceMatrix[currentCity][i], BETA)) / denominator;
        }
    }

    // Choose next city based on probability
    double randValue = (double)rand() / RAND_MAX;
    double cumulativeProbability = 0.0;

    for (int i = 0; i < NUM_CITIES; i++) {
        if (!ant.isVisited(i)) {
            cumulativeProbability += probabilities[i];
            if (randValue <= cumulativeProbability) {
                return i;
            }
        }
    }

    return 0;
}

// Function to update pheromone trails
void updatePheromones(vector<Ant>& ants) {
    // Evaporate existing pheromones
    for (int i = 0; i < NUM_CITIES; i++) {
        for (int j = 0; j < NUM_CITIES; j++) {
            pheromoneMatrix[i][j] *= (1.0 - EVAPORATION);
        }
    }

    // Add new pheromones based on ants' solutions
    for (int k = 0; k < NUM_ANTS; k++) {
        Ant& ant = ants[k];
        double contribution = Q / ant.totalDistance;
        for (int i = 0; i < NUM_CITIES - 1; i++) {
            int from = ant.visitedCities[i];
            int to = ant.visitedCities[i + 1];
            pheromoneMatrix[from][to] += contribution;
            pheromoneMatrix[to][from] += contribution;
        }
    }
}

// Main ACO algorithm
int main() {
    srand(time(0));

    // Initialize pheromone levels
    for (int i = 0; i < NUM_CITIES; i++) {
        for (int j = 0; j < NUM_CITIES; j++) {
            pheromoneMatrix[i][j] = 1.0; // Initial pheromone level
        }
    }

    vector<Ant> ants(NUM_ANTS);

    double bestDistance = numeric_limits<double>::max();
    vector<int> bestTour;

    // ACO loop
    for (int iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
        // Reset ants and send them to random starting cities
        for (int k = 0; k < NUM_ANTS; k++) {
            ants[k].reset();
            ants[k].visitCity(rand() % NUM_CITIES);
        }

        // Move ants through cities
        for (int k = 0; k < NUM_ANTS; k++) {
            Ant& ant = ants[k];

            while (ant.visitedCities.size() < NUM_CITIES) {
                int currentCity = ant.visitedCities.back();
                int nextCity = chooseNextCity(ant, currentCity);
                ant.visitCity(nextCity);
                ant.totalDistance += distanceMatrix[currentCity][nextCity];
            }

            // Complete the tour (return to the starting city)
            ant.totalDistance += distanceMatrix[ant.visitedCities.back()][ant.visitedCities[0]];

            // Update the best tour
            if (ant.totalDistance < bestDistance) {
                bestDistance = ant.totalDistance;
                bestTour = ant.visitedCities;
            }
        }

        // Update pheromones
        updatePheromones(ants);

        // Output the best distance in the current iteration
        cout << "Iteration " << iteration + 1 << " - Best Distance: " << bestDistance << endl;
    }

    // Output the final best tour and distance
    cout << "Best tour: ";
    for (int city : bestTour) {
        cout << city << " ";
    }
    cout << "\nBest distance: " << bestDistance << endl;

    return 0;
}
