#ifndef PROBLEM_HPP
#define PROBLEM_HPP

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <random>
#include <sstream>

using namespace std;


class Problem {
public:
	string name;
	int dimension;
	string edge_weight_type;
	int best_known_solution;
	vector<vector<int>> adjacency_matrix;
    int colony_size = 12;
    double alpha = 0.1;
    double beta = 2;
    double rho = 0.1;
    double q_0;


    Problem() = default;

	explicit Problem(const filesystem::path path) {
		ifstream file(path);
		if (!file.is_open()) {
			cout << "Error: file " << path << " not found" << endl;
			exit(1);
		}
		struct Tuple {
			double x,y;
			Tuple(double x, double y) : x(x), y(y) {}
		};
		vector<Tuple> nodes;
		for (int i = 1; !file.eof(); i++) {
			string line;
			getline(file, line);
			stringstream ss(line);
			string token;
			vector<string> row;
			while (getline(ss, token, ' ')) {
				row.push_back(token);
			}
			if (line == "EOF") break;
			if (i == 1) {name = row.back(); continue;}
			if (i == 4) {
                dimension = stoi(row.back());
//                q_0 = 1 - 20.0 / dimension;
                q_0 = 0.9;
                continue;
            }
			if (i == 5) {edge_weight_type = row.back(); continue;}
			if (i == 6) {best_known_solution = stoi(row.back()); continue;}
			if (i >= 8) {
				nodes.push_back(Tuple(stod(row[1], 0), stod(row[2], 0)));
//                cout << nodes.back().x << " " << nodes.back().y << endl;
			}
		}
		file.close();
		adjacency_matrix = vector<vector<int>>(dimension, vector<int>(dimension, 0));
		for (int i = 0; i < nodes.size(); i++) {
			for (int j = 0; j < nodes.size(); j++) {
				double distance = sqrt(pow(nodes[i].x - nodes[j].x, 2) + pow(nodes[i].y - nodes[j].y, 2));
				adjacency_matrix[i][j] = round(distance);
			}
		}
        file.close();

        if (dimension < 700) colony_size = 20;
        else colony_size = 10;

//        if (name == "eil76") {
//            colony_size = 20;
//        } else if (name == "kroA100") {
//            colony_size = 20;
//        } else if (name == "ch130") {
//            colony_size = 20;
//        } else if (name == "d198") {
//            colony_size = 20;
//        } else if (name == "lin318") {
//            colony_size = 20;
//        } else if (name == "pr439") {
//            colony_size = 20;
//        } else if (name == "pcb442") {
//            colony_size = 20;
//        } else if (name == "rat783") {
//            colony_size = 10;
//        } else if (name == "u1060") {
//            colony_size = 10;
//        } else if (name == "fl1577") {
//            colony_size = 10;
//        }
	}

	void print() const {
		cout << "Name: " << name << endl;
		cout << "Dimension: " << dimension << endl;
		cout << "Best known solution: " << best_known_solution << endl;
	}

	[[nodiscard]] int get_cost(const vector<int> &solution) const {
		int score = 0;
		for (int i = 0; i < solution.size(); i++) {
			score += adjacency_matrix[solution[i]][solution[(i+1) % solution.size()]];
		}
		return score;
	}

    [[nodiscard]] double get_error(const vector<int> &sol) const {
        return double(get_cost(sol) - best_known_solution) / best_known_solution;
    }
    [[nodiscard]] double get_error(const double dist) const {
        return (dist - best_known_solution) / best_known_solution;
    }

	vector<int> nn() const {
		vector<int> path(dimension);
		vector<char> visited(dimension, 0);
		
		path[0] = 0;
		visited[0] = 1;

		for (int c = 1; c < dimension; c++) {
			int current = path[c-1];
			int next_city;
			int dist = INT_MAX;
			for (int i = 0; i < dimension; i++) {
				if (visited[i] == 0 && adjacency_matrix[current][i] < dist) {
					next_city = i;
					dist = adjacency_matrix[current][i];
				}
			}
			path[c] = next_city;
			visited[next_city] = 1;
		}

		return path;
	}

    [[nodiscard]] bool save_solution(const vector<int> &solution, const string &path) const {
        ofstream file(path);
        if (!file.is_open()) {
            cout << "Error: file " << path << " not found" << endl;
            return false;
        }
        file << "NAME: " << name << endl;
        file << "TYPE: TSP" << endl;
        file << "DIMENSION: " << dimension << endl;
        file << "BEST_KNOWN: " << best_known_solution << endl;
        file << "FOUND_SOLUTION_COST: " << get_cost(solution) << endl;
        file << "TOUR_SECTION" << endl;
        for (int i : solution) {
            file << i + 1 << endl;
        }
        file << "EOF" << endl;
        file.close();
        return true;
    }

};

#endif