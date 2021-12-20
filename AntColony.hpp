#ifndef AI_CUP_2021_ANTCOLONY_HPP
#define AI_CUP_2021_ANTCOLONY_HPP

#include <vector>
#include <random>
#include <iostream>
#include <chrono>
#include <climits>

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
    int currentCity = -1;
    int prevCity = -1;

    Ant() = default;
    void reset(int dim) {
        current = -1;
        dist = 0;
        path = vector<int>(dim);
        visited = vector<char>(dim, 0);
    }

    void move(int next) {
        prevCity = currentCity;
        currentCity = next;
        current += 1;
        path[current] = next;
        visited[next] = 1;
    }

};

vector<int> ant_colony(const Problem &P, const int seed) {
    auto start = high_resolution_clock::now();

    // Setup random generator
    default_random_engine gen(seed);
    uniform_real_distribution<double> dis(0.0, 1.0);

    // Variables to time the execution and don't exceed 180s
    long long iterTime = 0;

    // Setup iteration variables
    double alpha = P.alpha;
    double beta = P.beta;
    double rho = P.rho;
    double q_0 = P.q_0;
    auto &adjacency = P.adjacency_matrix;
    vector<int> bestGlobalSolution = P.nn();
    int bestGlobalDist = P.get_cost(bestGlobalSolution);
    vector<double> candidates(P.dimension);

    // Setup Ants
    int total_ants = P.colony_size;
    vector<Ant> ants(total_ants);

    // Setup pheromone matrix
    double initial_pheromone = 1.0 / double(bestGlobalDist * P.dimension);
    vector<vector<double>> pheromone(P.dimension, vector<double>(P.dimension, initial_pheromone));

    auto end = high_resolution_clock::now();
    long long setupTime = duration_cast<milliseconds>(end - start).count();

    int iter;
    for (iter = 0; bestGlobalDist > P.best_known_solution; iter++) {
#if TIME
        // Get initial time of iteration
        start = high_resolution_clock::now();
#endif

        // Initialize all ants to random cities
        for (Ant &a : ants) {
            a.reset(P.dimension);
            a.move(int(dis(gen) * P.dimension));
        }

        for (int step = 1; step < P.dimension; step++) {
            for (int a = 0; a < total_ants; a++) {
                Ant &ant = ants[a];

                int maxCity = -1;
                double sum = 0;
                for (int c = 0; c < P.dimension; c++) {
                    if (ant.visited[c] == 0) {
                        double prob = pheromone[ant.currentCity][c] * pow(adjacency[ant.currentCity][c], -beta);
                        candidates[c] = prob;
                        sum += prob;
                        if (maxCity == -1 || prob > candidates[maxCity]) {
                            maxCity = c;
                        }
                    } else candidates[c] = 0;
                }

                double q = dis(gen);
                if (q > q_0 && sum != candidates[maxCity]) {
//                    sum -= candidates[maxCity];
//                    candidates[maxCity] = 0;
                    double p = dis(gen) * sum;
                    double running_sum = 0;
                    for (int c = 0; c < P.dimension; c++) {
                        running_sum += candidates[c];
                        if (p <= running_sum) {
                            maxCity = c;
                            break;
                        }
                    }
                }
                ant.dist += adjacency[ant.currentCity][maxCity];
                ant.move(maxCity);

                // Local update
                int prev = ant.currentCity;
                int curr = ant.prevCity;
                pheromone[prev][curr] = pheromone[prev][curr] * (1.0 - rho) + rho * initial_pheromone;
                pheromone[curr][prev] = pheromone[prev][curr];
            }
        }
        // Get best ant
        Ant &bestAnt = ants[0];
        for (Ant &a : ants) {
            int prev = a.path[P.dimension-1];
            int curr = a.path[0];
            pheromone[prev][curr] = pheromone[prev][curr] * (1.0 - rho) + rho * initial_pheromone;
            pheromone[curr][prev] = pheromone[prev][curr];
            a.dist += adjacency[prev][curr];
            if (a.dist < bestAnt.dist) bestAnt = a;
        }


        // Two opt best ant
        two_opt(P, bestAnt.path, bestAnt.dist);

        // Two opt random ants
        for (int i = 0; i < 1; i++) {
            Ant &randomAnt = ants[int(dis(gen) * total_ants)];
            two_opt(P, randomAnt.path, randomAnt.dist);
            if (randomAnt.dist < bestAnt.dist) bestAnt = randomAnt;
        }

        // Get best path
        if (bestAnt.dist < bestGlobalDist) {
            bestGlobalDist = bestAnt.dist;
            bestGlobalSolution = bestAnt.path;
        }

        // Global Update
        for (int c = 0; c < P.dimension; c++) {
            int curr = bestGlobalSolution[c];
            int next = bestGlobalSolution[(c+1) % P.dimension];
            pheromone[curr][next] = pheromone[curr][next] * (1.0 - alpha) + alpha / bestGlobalDist;
            pheromone[next][curr] = pheromone[curr][next];
        }

#if TIME
        // Check if we can iterate once more
        end = high_resolution_clock::now();
        long long iterDur = duration_cast<milliseconds>(end - start).count();
        iterTime += iterDur;
        long long avgIterTime = iterTime / (iter + 1);
        if (setupTime + iterTime + avgIterTime >= 176000) {
            return bestGlobalSolution;
        }
#endif
    }
    return bestGlobalSolution;
}

#endif //AI_CUP_2021_ANTCOLONY_HPP
