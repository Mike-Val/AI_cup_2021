//
// Created by Michelangelo Valsecchi on 14/12/21.
//

#ifndef AI_CUP_2021_SEEDTEST_HPP
#define AI_CUP_2021_SEEDTEST_HPP

#include <vector>
#include <string>
#include <iostream>
#include <chrono>

#include "SeedSearch.hpp"

using namespace std;
using namespace chrono;


void testSeed(int seed) {
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

    Result result(seed, P.size());

    for (int i = 0; i < P.size(); i++) {
        auto &pr = result.results[i];
        pool.push_task([&pr, &seed](const Problem &p){
            pr.name = p.name;
            pr.q_0 = p.q_0;
            pr.alpha = p.alpha;
            pr.beta = p.beta;
            pr.rho = p.rho;
            pr.ants = p.colony_size;
            auto start = high_resolution_clock::now();
            vector<int> sol = ant_colony(p, seed);
            auto end = high_resolution_clock::now();
            pr.dist = p.get_cost(sol);
            pr.time = duration_cast<milliseconds>(end - start).count();
            pr.error = p.get_error(pr.dist) * 100;
        }, P[i]);
    }
    pool.wait_for_tasks();

    for (auto &pr : result.results) {
        result.meanError += pr.error;
    }
    result.meanError /= P.size();

    ios state(nullptr);
    state.copyfmt(cout);
    cout << "Mean Error: " << fixed << setprecision(8) << result.meanError << " %" << setprecision(3) << endl;
    for (ProblemResult &pr : result.results) {
        cout << setfill(' ') << setw(10) << pr.name << " -> " << pr.error << " %   ";
        cout << "[" << pr.time / 1000.0 << " ms]   ";
        cout << "Q_0: " << pr.q_0 << "   ";
        cout << "Alpha: " << pr.alpha << "   ";
        cout << "Beta: " << pr.beta << "   ";
        cout << "Rho: " << pr.rho << "   ";
        cout << "Ants: " << pr.ants << "   ";
        cout << "Dist: " << pr.dist << endl;
    }
    cout.copyfmt(state);
}


#endif //AI_CUP_2021_SEEDTEST_HPP
