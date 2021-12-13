#ifndef OPT_HPP
#define OPT_HPP

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>

#include "Problem.hpp"

using namespace std;

void swap2opt(vector<int> &tour, int i, int j) {
    reverse(tour.begin() + i, tour.begin() + j + 1);
}

int gain(int i, int j, vector<int> &tour, Problem &P) {
    int old_link_len = P.adjacency_matrix[tour[i]][tour[i - 1]] + P.adjacency_matrix[tour[j]][tour[j + 1]];
    int changed_links_len = P.adjacency_matrix[tour[j]][tour[i - 1]] + P.adjacency_matrix[tour[i]][tour[j + 1]];
    return changed_links_len - old_link_len;
}

int two_opt_loop(Problem &P, vector<int> &tour, int &len) {
    int cross = 0;
    int n = P.dimension - 1;
    init:
    for (int i = 1; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            int new_len = len + gain(i, j, tour, P);
            if (new_len < len) {
                swap2opt(tour, i, j);
                len = new_len;
                cross += 1;
                goto init;
            }
        }
    }
    return cross;
}

void two_opt(Problem &P, vector<int> &tour, int &len) {
    int cross;
    do {
        cross = two_opt_loop(P, tour, len);
    } while (cross > 0);
}


#endif