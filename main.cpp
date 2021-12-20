#include <iostream>
#include <chrono>

#include "Problem.hpp"
#include "AntColony.hpp"

using namespace std;
using namespace chrono;

int main(int argc, char *argv[]) {
    int seed = strtol(argv[1], NULL, 10);
    Problem problem(argv[2]);
    problem.print();
    cout << "Starting solver" << endl;
    cout << "------------------" << endl;
    auto start = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    auto sol = ant_colony(problem, seed);
    auto end = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    assert(problem.valid(sol));
    cout << "Time: " << double(end - start) / 1000 << " s" << endl;
    cout << "Dist: " << problem.get_cost(sol) << endl;
    cout << "Error: " << 100 * problem.get_error(sol) << " %" << endl;
    cout << "------------------" << endl;
    if (argc == 4 && problem.save_solution(sol, argv[3])) cout << "Solution saved to " << argv[3] << endl;

}