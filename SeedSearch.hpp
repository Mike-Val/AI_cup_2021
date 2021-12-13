//
// Created by Michelangelo Valsecchi on 10/12/21.
//

#ifndef AI_CUP_2021_SEEDSEARCH_HPP
#define AI_CUP_2021_SEEDSEARCH_HPP

#include <vector>
#include <random>
#include <string>
#include <iomanip>
#include <iostream>
#include <chrono>

#include "Problem.hpp"
#include "thread_pool.hpp"
#include "AntColony.hpp"

using namespace std;

vector<Problem> load_problems(vector<string> &names) {
    vector<Problem> problems(names.size());
    for (int i = 0; i < names.size(); i++) {
        problems[i] = Problem("/Users/mike/Desktop/AI_cup_2021/problems/" + names[i] + ".tsp");
    }
    return problems;
}

struct ProblemResult {
    string name;
    int dist;
    double error;

    ProblemResult() = default;
};

struct Result {
    int seed;
    vector<ProblemResult> results;
    double meanError = 0;

    Result() = default;
    explicit Result(int seed, size_t dim) : seed(seed), results(dim) {}
};

void test_seed(Result &result, vector<Problem> &P, int seed) {
    double meanError = 0;

    auto start = high_resolution_clock::now();
    for (int i = 0; i < P.size(); i++) {
        auto &pr = result.results[i];
        auto &p = P[i];
        pr.name = p.name;
        pr.dist = p.get_cost(ant_colony(p, seed, 0.1, 2, 0.1));
        pr.error = p.get_error(pr.dist) * 100;
        meanError += pr.error;
    }
    auto end = high_resolution_clock::now();

    result.meanError = meanError / P.size();

    ios state(nullptr);
    state.copyfmt(cout);
    cout << "Seed: " << setfill(' ') << setw(11) <<  seed;
    cout << "      Mean Error: " << fixed << setprecision(8) << result.meanError << " %";
    cout << "      Time: " << setprecision(3) << duration_cast<milliseconds>(end - start).count() / 1000.0 << " s";
    cout << "     (";
    for (ProblemResult &pr : result.results) {
        cout << " " << pr.name << "->" << pr.error << " ";
    }
    cout << ")" << endl;
    cout.copyfmt(state);
}


int search_seed(int tests = 1) {
    vector<string> problem_names = {
            "eil76",
            "kroA100",
            "ch130",
            "d198",
            "lin318",
            "pr439",
            "pcb442",
            "rat783",
            "u1060",
            "fl1577",
    };

    auto P = load_problems(problem_names);
    thread_pool pool;
    srand(time(NULL));

    vector<Result> results(tests);
    for (Result &r : results) {
        int seed = rand();
        r = Result(seed, P.size());
        pool.push_task([&r, &P, seed](){
            test_seed(r, P, seed);
        });
    }

    pool.wait_for_tasks();

    Result &bestResult = results[0];
    for (Result &r : results) {
        if (r.meanError < bestResult.meanError) bestResult = r;
    }

    return bestResult.seed;
}

#endif //AI_CUP_2021_SEEDSEARCH_HPP
