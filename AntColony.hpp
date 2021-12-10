#ifndef AI_CUP_2021_ANTCOLONY_HPP
#define AI_CUP_2021_ANTCOLONY_HPP

#include <vector>
#include <random>
#include <iostream>
#include <chrono>

#include "Problem.hpp"
#include "TwoOPT.hpp"

#define TIME 1

using namespace std;
using namespace chrono;

struct Ant {
    vector<int> path;
    vector<char> visited;
    int current = 0;
    int dist = 0;

    Ant() = default;
    void init(int dim) {
        current = 0;
        dist = 0;
        path = vector<int>(dim);
        visited = vector<char>(dim, 0);
    }
};
    
vector<int> ant_colony(Problem &P, int seed, double alpha, double beta, double rho) {
    // Setup random generator
    mt19937 gen(seed);
    uniform_real_distribution<> dis(0.0, 1.0);

    // Variables to time the execution and don't exceed 180s
    long long iterTime = 0;


    // Setup iteration variables
    double q_0 = 1 - 13.0 / P.dimension;
    auto &adjacency = P.adjacency_matrix;
    vector<int> bestGlobalSolution = P.nn();
    int bestGlobalDist = P.get_cost(bestGlobalSolution);
    int total_ants = 10;
    vector<Ant> ants(total_ants);
    vector<double> candidates(P.dimension);

    // Setup pheromone matrix
    double initial_pheromone = 1.0 / double(bestGlobalDist * P.dimension);
    vector<vector<double>> pheromone(P.dimension, vector<double>(P.dimension, initial_pheromone));


    for (int iter = 0; true; iter++) {
#if TIME
        // Get initial time of iteration
        auto start = high_resolution_clock::now();
#endif

        // Initialize all ants to random cities
        for (Ant &a : ants) {
            a.init(P.dimension);
            a.path[a.current] = int(dis(gen) * P.dimension);
            a.visited[a.path[a.current]] = 1;
        }

        for (int step = 1; step < P.dimension; step++) {
            for (int a = 0; a < total_ants; a++) {
                Ant &ant = ants[a];

                int maxCity = -1;
                double sum = 0;
                for (int c = 0; c < P.dimension; c++) {
                    if (ant.visited[c] == 0) {
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
                    ant.visited[maxCity] = 1;
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
                            ant.visited[c] = 1;
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

        // Two opt random ants
        for (int i = 0; i < 2; i++) {
            Ant &randomAnt = ants[int(dis(gen) * total_ants)];
            two_opt(P, randomAnt.path, randomAnt.dist);
            if (randomAnt.dist < bestAntDist) {
                bestAntDist = randomAnt.dist;
                bestAntSolution = randomAnt.path;
            }
        }

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

#if TIME
        // Check if we can iterate once more
        auto end = high_resolution_clock::now();
        auto iterDur = duration_cast<milliseconds>(end - start).count();
        iterTime += iterDur;
        auto avgIterTime = iterTime / (iter + 1);
        if (iterTime + avgIterTime >= 175000) return bestGlobalSolution;
#endif
    }

    return bestGlobalSolution;
    
}

#endif //AI_CUP_2021_ANTCOLONY_HPP
