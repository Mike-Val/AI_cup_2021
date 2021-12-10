//
// Created by Michelangelo Valsecchi on 09/12/21.
//

#ifndef AI_CUP_2021_MATRIX_HPP
#define AI_CUP_2021_MATRIX_HPP

#include <vector>

using namespace std;

template <typename T>
struct Matrix {
    vector<vector<T>> data;

    Matrix() = default;
    explicit Matrix(const vector<vector<T>> &data) : data(data) {}
    Matrix(const Matrix &M) : Matrix(M.data) {};
    explicit Matrix(int size) : data(size, vector<T>(size)) {};
    Matrix(int size, T initial_value) : data(size, vector<T>(size, initial_value)) {};

    T& operator()(int row, int col) {
        if (row > col) return data[row][col];
        else return data[col][row];
    }
    const T& operator()(int row, int col) const {
        if (row > col) return data[row][col];
        else return data[col][row];
    }
};

#endif //AI_CUP_2021_MATRIX_HPP
