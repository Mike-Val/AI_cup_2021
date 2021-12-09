//
// Created by Michelangelo Valsecchi on 09/12/21.
//

#ifndef AI_CUP_2021_LTMATRIX_HPP
#define AI_CUP_2021_LTMATRIX_HPP

#include <vector>

using namespace std;

template <typename T>
struct LTMatrix {
    vector<vector<T>> data;

    LTMatrix() = default;
    LTMatrix(const vector<vector<T>> &data) : data(data) {}
    LTMatrix(const LTMatrix &M) : LTMatrix(M.data) {};
    LTMatrix(int size) : data(size, vector<T>(size)) {};
    LTMatrix(int size, T initial_value) : data(size, vector<T>(size, initial_value)) {};

    T& operator()(int row, int col) {
        if (row > col) return data[row][col];
        else return data[col][row];
    }
    const T& operator()(int row, int col) const {
        if (row > col) return data[row][col];
        else return data[col][row];
    }
};

#endif //AI_CUP_2021_LTMATRIX_HPP
