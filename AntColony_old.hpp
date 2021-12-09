#ifndef ANT_HPP
#define ANT_HPP

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>

#include "Problem.hpp"
#include "TwoOPT.hpp"

using namespace std;

struct Ant {
	vector<int> path;
	vector<bool> to_visit;
	int dist;
	int current_index;
	int current_city;

	Ant() {}

	Ant(int initial_city, int dimension) {
		to_visit = vector<bool>(dimension, 1);
		path = vector<int>(dimension);
		current_index = 0;
		path[current_index] = initial_city;
		to_visit[initial_city] = 0;
		dist = 0;
		current_city = initial_city;
	}

	void move(vector<vector<int>> &adjacency_matrix, int city) {
		dist += adjacency_matrix[city][path[current_index]];
		path[current_index++] = city;
		to_visit[city] = false;
		current_city = city;
	}
};


vector<int> ant_colony(Problem &problem, int seed, double alpha, double beta, double rho, double q_0, int total_ants = 10) {
	mt19937 gen(seed);
	uniform_real_distribution<> dis(0.0, 1.0);

	q_0 = 1 - 13.0/problem.dimension;
	q_0 = 0.98;

	vector<vector<int>> &adjacency_matrix = problem.adjacency_matrix;

	double initial_pheromone = double(1) / double(problem.get_cost(problem.nn()) * problem.dimension);
	vector<vector<double>> pheromone_matrix(problem.dimension, vector<double>(problem.dimension, initial_pheromone));

	vector<int> best_solution;
	int best_length = INT_MAX;

	vector<Ant> ants(total_ants);
	for (int itr = 0; itr < 2000; itr++) {
		for (int i = 0; i < total_ants; i++) {
			int city = int(dis(gen) * problem.dimension);
			ants[i] = Ant(city, problem.dimension);
		}

		for (int i = 0; i < problem.dimension-1; i++) {
			for (int a = 0; a < total_ants; a++) {
				int current_city = ants[a].current_city;

				vector<double> candidate_cities(problem.dimension);
				double sum_candidates = 0;
				int max_candidate = 0;
				for (int c = 0; c < problem.dimension; c++) {
					if (ants[a].to_visit[c]) {
						double weight = pheromone_matrix[current_city][c] * pow(1.0/adjacency_matrix[current_city][c], beta);
						sum_candidates += weight;
						candidate_cities[c] = weight;
						if (weight > candidate_cities[max_candidate]) max_candidate = c;
					} else candidate_cities[c] = 0;
				}

				if (dis(gen) <= q_0 || sum_candidates == candidate_cities[max_candidate]) {
					ants[a].move(adjacency_matrix, max_candidate);
				} else {
					double rnd = dis(gen) * (sum_candidates - candidate_cities[max_candidate]);
					candidate_cities[max_candidate] = 0;
					double prob = 0;
					for (int j = 0; j < problem.dimension; j++) {
						prob += candidate_cities[j];
						if (rnd <= prob) {
							ants[a].move(adjacency_matrix, j);
							break;
						}
					}
				}
				int next_city = ants[a].current_city;
				pheromone_matrix[current_city][next_city] = pheromone_matrix[current_city][next_city] * (1 - rho) + rho * initial_pheromone;
				pheromone_matrix[next_city][current_city] = pheromone_matrix[current_city][next_city];
			}
		}

		Ant &best_ant = ants[0];
		int best_current_length = INT_MAX;
		for (int i = 0; i < total_ants; i++) {
			ants[i].dist += adjacency_matrix[ants[i].path[0]][ants[i].path[problem.dimension - 1]];
			if (ants[i].dist < best_current_length) {
				best_current_length = ants[i].dist;
				best_ant = ants[i];
			}
		}

		for (int i = 0; i < problem.dimension; i++) {
			for (int j = 0; j < problem.dimension; j++) {
				pheromone_matrix[i][j] = pheromone_matrix[i][j] * (1 - alpha);
			}
		}

		vector<int> best_solution_2opt = two_opt(problem, best_ant.path);
		int best_length_2opt = problem.get_cost(best_solution_2opt);
		if (best_length_2opt < best_length) {
			best_length = best_length_2opt;
			best_solution = best_solution_2opt;
		}

		for (int i = 0; i < problem.dimension; i++) {
			int a = best_solution_2opt[i];
			int b = best_solution_2opt[(i + 1)%problem.dimension];
			pheromone_matrix[a][b] = pheromone_matrix[a][b] + alpha / best_length_2opt;
			pheromone_matrix[b][a] = pheromone_matrix[a][b];
		}
	}

	return best_solution;
}

#endif