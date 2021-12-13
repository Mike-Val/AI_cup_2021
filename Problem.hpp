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

    Problem() = default;

	explicit Problem(const filesystem::path& path) {
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
			if (i == 4) {dimension = stoi(row.back()); continue;}
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
	}

	void print() const {
		cout << "Name: " << name << endl;
		cout << "Dimension: " << dimension << endl;
		cout << "Edge weight type: " << edge_weight_type << endl;
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

	vector<int> nn() {
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
};

#endif