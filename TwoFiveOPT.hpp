//
// Created by Michelangelo Valsecchi on 19/12/21.
//

#ifndef AI_CUP_2021_TWOFIVEOPT_HPP
#define AI_CUP_2021_TWOFIVEOPT_HPP

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>

#include "Problem.hpp"

using namespace std;

vector<int> gain_(int i, int j, vector<int> &tour, const Problem &P) {
    int old_two_opt_len = P.adjacency_matrix[tour[i]][tour[i-1]]
                          + P.adjacency_matrix[tour[j]][tour[j+1]];
    int new_two_opt_len = P.adjacency_matrix[tour[j]][tour[i-1]]
                          + P.adjacency_matrix[tour[i]][tour[j+1]];
    int two_opt_gain = - old_two_opt_len + new_two_opt_len;

    // Node shift 1 gain
    int old_node_shift_1_len = P.adjacency_matrix[tour[i]][tour[i-1]]
                               + P.adjacency_matrix[tour[i]][tour[i+1]]
                               + P.adjacency_matrix[tour[j]][tour[j+1]];
    int new_node_shift_1_len = P.adjacency_matrix[tour[i-1]][tour[i+1]]
                               + P.adjacency_matrix[tour[i]][tour[j]]
                               + P.adjacency_matrix[tour[i]][tour[j+1]];
    int node_shift_1_gain = - old_node_shift_1_len + new_node_shift_1_len;

    // Node shift 2 gain
    int old_node_shift_2_len = P.adjacency_matrix[tour[i]][tour[i-1]]
                               + P.adjacency_matrix[tour[j]][tour[j-1]]
                               + P.adjacency_matrix[tour[j]][tour[j+1]];
    int new_node_shift_2_len = P.adjacency_matrix[tour[j]][tour[i-1]]
                               + P.adjacency_matrix[tour[i]][tour[j]]
                               + P.adjacency_matrix[tour[j-1]][tour[j+1]];
    int node_shift_2_gain = - old_node_shift_2_len + new_node_shift_2_len;

    return {two_opt_gain, node_shift_1_gain, node_shift_2_gain};
}

int two_five_opt_loop(const Problem &P, vector<int> &tour, int &len) {
    int n = P.dimension - 1;
    vector<int> best_i = {-1, -1, -1}, best_j = {-1, -1, -1};
    vector<int> best_len = {0, 0, 0};

    for (int i = 1; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            vector<int> new_len = gain_(i, j, tour, P);
            for (int k = 0; k < 3; k++) {
                if (new_len[k] < best_len[k]) {
                    best_i[k] = i;
                    best_j[k] = j;
                    best_len[k] = new_len[k];
                }
            }
        }
    }
    if (best_len[0] >= 0 && best_len[1] >= 0 && best_len[2] >= 0) return 0;

    if (best_len[0] <= best_len[1] && best_len[0] <= best_len[2]) {
        len += best_len[0];
        reverse(tour.begin() + best_i[0], tour.begin() + best_j[0] + 1);
    } else if (best_len[1] <= best_len[0] && best_len[1] <= best_len[2]) {
        len += best_len[1];
//        int tmp = tour[best_i[1]];
//        for (int i = best_i[1]; i < best_j[1]; i++) tour[i] = tour[i+1];
//        tour[best_j[1]] = tmp;
        vector<int> array_one = {tour.begin(), tour.begin() + best_i[1]};
        vector<int> array_two = {tour.begin() + best_i[1] + 1, tour.begin() + best_j[1] + 1};
        vector<int> array_three = {tour.begin() + best_j[1] + 1, tour.end()};

        vector<int> sol = array_one;
        sol.insert(sol.end(), array_two.begin(), array_two.end());
        sol.push_back(tour[best_i[1]]);
        sol.insert(sol.end(), array_three.begin(), array_three.end());

        tour = sol;
    } else {
        len += best_len[2];
//        int tmp = tour[best_j[2]];
//        for (int i = best_j[2]; i > best_i[2]; i--) tour[i] = tour[i-1];
//        tour[best_i[2]] = tmp;
        vector<int> array_one = {tour.begin(), tour.begin() + best_i[2]};
        vector<int> array_two = {tour.begin() + best_i[2], tour.begin() + best_j[2]};
        vector<int> array_three = {tour.begin() + best_j[2] + 1, tour.end()};

        vector<int> sol = array_one;
        sol.push_back(tour[best_j[2]]);
        sol.insert(sol.end(), array_two.begin(), array_two.end());
        sol.insert(sol.end(), array_three.begin(), array_three.end());

        tour = sol;
    }
    return 1;
}

void two_five_opt(const Problem &P, vector<int> &tour, int &len) {
    int cross;
    do {
        cross = two_five_opt_loop(P, tour, len);
    } while (cross > 0);
}

#endif //AI_CUP_2021_TWOFIVEOPT_HPP
