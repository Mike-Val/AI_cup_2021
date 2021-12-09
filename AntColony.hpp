#ifndef AI_CUP_2021_ANTCOLONY_HPP
#define AI_CUP_2021_ANTCOLONY_HPP

#include <vector>
#include <random>
#include <iostream>

#include "Problem.hpp"
#include "TwoOPT.hpp"

using namespace std;

struct Ant {
    vector<int> path;
    vector<bool> visited;
    int current;
    int dist;

    Ant() = default;
    void init(int dim) {
        current = 0;
        dist = 0;
        path = vector<int>(dim);
        visited = vector<bool>(dim, false);
    }
};
    
vector<int> ant_colony(Problem &P, int seed, double alpha, double beta, double rho) {
//    cout << "Seed: " << seed << endl;
    mt19937 gen(seed);
    uniform_real_distribution<> dis(0.0, 1.0);

    double q_0 = 1 - 13.0 / P.dimension;

    int total_ants = 10;
    
    auto &adjacency = P.adjacency_matrix;

    vector<int> bestGlobalSolution = P.nn();
    int bestGlobalDist = P.get_cost(bestGlobalSolution);

    // Setup pheromone matrix
    double initial_pheromone = 1.0 / double(bestGlobalDist * P.dimension);
    vector<vector<double>> pheromone(P.dimension, vector<double>(P.dimension, initial_pheromone));

    vector<Ant> ants(total_ants);
    vector<double> candidates(P.dimension);

    for (int iter = 0; iter < 1000; iter++) {
        // Initialize all ants to random cities
        for (Ant &a : ants) {
            a.init(P.dimension);
            a.path[a.current] = int(dis(gen) * P.dimension);
            a.visited[a.path[a.current]] = true;
        }

        for (int step = 1; step < P.dimension; step++) {
            for (int a = 0; a < total_ants; a++) {
                Ant &ant = ants[a];

                int maxCity = -1;
                double sum = 0;
                for (int c = 0; c < P.dimension; c++) {
                    if (!ant.visited[c]) {
                        double prob = pheromone[ant.path[ant.current]][c] * pow(adjacency[ant.path[ant.current]][c], -beta);
                        candidates[c] = prob;
                        sum += prob;
                        if (maxCity == -1 || prob > candidates[maxCity]) {
                            maxCity = c;
                        }
                    } else candidates[c] = 0;
                }

                double q = dis(gen);
                if (q < q_0 || sum == candidates[maxCity]) {// Exploitation
                    ant.dist += adjacency[ant.path[ant.current]][maxCity];
                    ant.path[++ant.current] = maxCity;
                    ant.visited[maxCity] = true;
                } else {
                    // Exploration
//                    sum -= candidates[maxCity];
//                    candidates[maxCity] = 0;
                    double p = dis(gen) * sum;
                    double running_sum = 0;
                    for (int c = 0; c < P.dimension; c++) {
                        running_sum += candidates[c];
                        if (p < running_sum) {
                            ant.dist += adjacency[ant.path[ant.current]][c];
                            ant.path[++ant.current] = c;
                            ant.visited[c] = true;
                            break;
                        }
                    }
                }

                // Local update
                int prev = ant.path[ant.current-1];
                int curr = ant.path[ant.current];
                pheromone[prev][curr] = pheromone[prev][curr] * (1.0 - rho) + rho * initial_pheromone;
                pheromone[curr][prev] = pheromone[prev][curr];
            }
        }
        // Get best ant
        vector<int> &bestAntSolution = ants[0].path;
        int bestAntDist = INT_MAX;
        for (Ant &a : ants) {
            a.dist += adjacency[a.path[a.current]][a.path[0]];
            if (a.dist < bestAntDist) {
                bestAntDist = a.dist;
                bestAntSolution = a.path;
            }
        }

        // Two opt best ant
        two_opt(P, bestAntSolution, bestAntDist);

        // Get best path
        if (bestAntDist < bestGlobalDist) {
            bestGlobalDist = bestAntDist;
            bestGlobalSolution = bestAntSolution;
        }

        // Global Update
        for (int c = 0; c < P.dimension; c++) {
            int curr = bestGlobalSolution[c];
            int next = bestGlobalSolution[(c+1) % P.dimension];
            pheromone[curr][next] = pheromone[curr][next] * (1.0 - alpha) + alpha / bestAntDist;
            pheromone[next][curr] = pheromone[curr][next];
        }
    }

    return bestGlobalSolution;
    
}

#endif //AI_CUP_2021_ANTCOLONY_HPP
