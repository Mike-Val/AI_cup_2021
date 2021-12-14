#include <iostream>
#include <ctime>
#include <vector>
#include <chrono>
#include <cmath>

#include "thread_pool.hpp"
#include "Problem.hpp"
#include "AntColony.hpp"
#include "SeedSearch.hpp"
#include "SeedTest.hpp"

using namespace std;

#define SINGLE_RUN 0
#define PARALLEL_RUN 0
#define SEARCH_SEED 0
#define TEST_SEED 1092841564

//eil76
//kroA100
//ch130
//d198
//lin318
//pr439
//pcb442
//rat783
//u1060
//fl1577

int main() {
    srand(time(NULL));
    Problem problem("fl1577");

#if SINGLE_RUN
    problem.print();
    int seed = rand();
    auto start = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
	auto sol = ant_colony(problem, seed);
    auto end = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
    cout << "Time: " << double(end - start) / 1000 << endl;
    cout << "Seed: " << seed << endl;
	cout << "Dist: " << problem.get_cost(sol) << endl;
    cout << "Error: " << 100 * double(problem.get_cost(sol) - problem.best_known_solution) / problem.best_known_solution << endl;
	// print sol
	for (auto i : sol) {
		cout << i << ", ";
	}
#endif

#if PARALLEL_RUN
    int n = 10;
    vector<int> seeds(n);
    vector<int> costs(n);

    problem.print();
    thread_pool pool;
    cout << "Threads: " << pool.get_thread_count() << endl;

    for (int i = 0; i < n; i++) {
        seeds[i] = rand();
        pool.push_task([&costs, &problem](int i, int s) {
            vector<int> solution = ant_colony(problem, s);
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
    variance_err = sqrt(variance_err);

    cout << "Mean Error: " << 100 * avg_err << endl << "Variance Error: " << 100 * variance_err << endl;
#endif

#if SEARCH_SEED
    cout << "Searching best seed" << endl;
    int tests = 10;
    int bestSeed = search_seed(tests);
    cout << "Best seed found: " << bestSeed << endl;
#endif

#if TEST_SEED
    cout << "Testing seed: " << TEST_SEED << endl;
    testSeed(TEST_SEED);
#endif
}