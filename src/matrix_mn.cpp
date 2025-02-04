#include "matrix_mn.h"
#include "vec_n.h"
#include <stddef.h>

MatrixMN::MatrixMN() : M(0), N(0), rows(nullptr) {}

MatrixMN::MatrixMN(int M, int N) : M(M), N(N) {
    rows = new VecN[M];
    for (int i = 0; i < M; i++) {
        rows[i] = VecN(N);
    }
}

MatrixMN::~MatrixMN() { delete[] rows; }

MatrixMN::MatrixMN(const MatrixMN &m) { *this = m; }

void MatrixMN::zero() {
    for (int i = 0; i < M; i++) {
        rows[i].zero();
    }
}

/**
 * Return a new matrix, where
 *   rows are the columns
 *   columns are the rows
 */
MatrixMN MatrixMN::transpose() const {
    MatrixMN result(N, M);
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < M; c++) {
            result.rows[r][c] = rows[c][r];
        }
    }

    return result;
}

const MatrixMN &MatrixMN::operator=(const MatrixMN &m) {

    M = m.M;
    N = m.N;

    rows = new VecN[M];
    for (int i = 0; i < M; i++) {
        rows[i] = m.rows[i];
    }

    return *this;
}

VecN MatrixMN::operator*(const VecN &v) const {
    if (v.N != N) {
        return v;
    }

    VecN result(M);
    for (int r = 0; r < M; r++) {
        result[r] = v.dot(rows[r]);
    }

    return result;
}

/**
 * ONLY POSSIBLE when N == m.M
 * result's rows is M and columns is m.N
 * _NOT_ commutative
 */
MatrixMN MatrixMN::operator*(const MatrixMN &m) const {
    if (N != m.M && M != m.N) {
        return m;
    }

    MatrixMN transposed = m.transpose();
    MatrixMN result(M, m.N);

    for (int r = 0; r < M; r++) {
        for (int c = 0; c < m.N; c++) {
            result.rows[r][c] = rows[r].dot(transposed.rows[c]);
        }
    }

    return result;
}

VecN MatrixMN::solve_gauss_seidel(const MatrixMN A, const VecN &b) {
    const int N = b.N;
    VecN X(N);
    X.zero();

    // iterate N times
    for (int iter = 0; iter < N; iter++) {
        for (int n = 0; n < N; n++) {
            float dx =
                (b[n] / A.rows[n][n]) - (A.rows[n].dot(X) / A.rows[n][n]);

            if (dx == dx) {
                // ensure it's not NaN
                X[n] += dx;
            }
        }
    }

    return X;
}
