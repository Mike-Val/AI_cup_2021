#include <iostream>
#include <ctime>
#include <vector>
#include <chrono>

#include "thread_pool.hpp"
#include "Problem.hpp"
#include "AntColony.hpp"

using namespace std;

int main() {
    Problem problem("/Users/mike/Desktop/AI_cup_2021/problems/ch130.tsp");
    problem.print();

    srand(time(NULL));

    int seed = rand();
    auto start = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
	auto sol = ant_colony(problem, seed, 0.1, 2, 0.1);
    auto end = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
    cout << "Time: " << double(end - start) / 1000 << endl;
    cout << "Seed: " << seed << endl;
	cout << "Dist: " << problem.get_cost(sol) << endl;
    cout << "Error: " << 100 * double(problem.get_cost(sol) - problem.best_known_solution) / problem.best_known_solution << endl;
	// print sol
	for (auto i : sol) {
		cout << i << ", ";
	}
    return 0;

    int n = 100;
    vector<int> seeds(n);
    vector<int> costs(n);

    thread_pool pool;

    for (int i = 0; i < n; i++) {
        seeds[i] = rand();
        pool.push_task([&costs, &problem](int i, int s) {
            vector<int> solution = ant_colony(problem, s, 0.1, 2, 0.1);
            costs[i] = problem.get_cost(solution);
        }, i, seeds[i]);
    }

    pool.wait_for_tasks();

    int best_seed;
    int best_cost = INT_MAX;
    for (int i = 0; i < n; i++) {
        if (costs[i] < best_cost) {
            best_cost = costs[i];
            best_seed = seeds[i];
        }
    }

    cout << "Best Seed: " << best_seed << endl << "Best Cost: " << best_cost << endl << "Best Error: "
         << 100 * (double(best_cost) / problem.best_known_solution - 1) << endl;

    double avg_err = 0;
    for (int c: costs) {
        avg_err += double(c - problem.best_known_solution) / problem.best_known_solution;
    }
    avg_err /= n;
    double variance_err = 0;
    for (int c: costs) {
        double err = double(c - problem.best_known_solution) / problem.best_known_solution;
        variance_err += pow(err - avg_err, 2);
    }
    variance_err /= n;

    cout << "Mean Error: " << 100 * avg_err << endl << "Variance Error: " << 100 * variance_err << endl;
}