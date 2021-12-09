#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>

#include <Eigen/Dense>



vector<int> random_2opt(std::mt19937 &gen, Problem &p, vector<int> &current, int &delta) {
	std::uniform_real_distribution<> dis(0.0, 1.0);
	int n = current.size();

	int a = int(dis(gen) * n) % n;
	int b = (a+1) % n;
	int d = int(dis(gen) * n) % n;
	int c = (d+1) % n;

	int gain = p.adjacency_matrix(current[a], current[d]) + p.adjacency_matrix(current[b], current[c]);
	int base = p.adjacency_matrix(current[a], current[b]) + p.adjacency_matrix(current[d], current[c]);
	delta = gain - base;

	vector<int> next = current;
	reverse(next.begin() + a, next.begin() + d + 1);
	
	return next;
}


vector<int> annealing(Problem &problem, int seed, double T_max = 10000, double T_min = 1, double cooling = 0.9) {
	std::mt19937 gen(seed);
	std::uniform_real_distribution<> dis(0.0, 1.0);
	
	double T = T_max;

	vector<int> current = problem.nn();
	int current_cost = problem.get_cost(current);
	
	vector<int> best = current;
	int best_cost = current_cost;
	
	while (T > T_min) {
		int stuck = 0;
		do {
			int delta;
			vector<int> next = random_2opt(gen, problem, current, delta);
			if (delta >= 0) stuck++;
			
			double random_num = dis(gen);
			if (delta < 0 || random_num < exp(-double(delta)/T)) {
				current = next;
				current_cost = problem.get_cost(current);
				if (current_cost < best_cost) {
					best = current;
					best_cost = current_cost;
				}
				stuck = 0;
			}
		} while (stuck < 3);
		T *= cooling;
	}
	return best;
}