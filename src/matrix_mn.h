#ifndef MATRIX_MN_H
#define MATRIX_MN_H

#include "vec_n.h"

struct MatrixMN {
    int M; // rows
    int N; // cols

    VecN *rows; // rows number of VecN

    MatrixMN();
    MatrixMN(int M, int N);
    MatrixMN(const MatrixMN &m);

    ~MatrixMN();

    void zero(); // zero out the entire matrix
    MatrixMN transpose() const;

    const MatrixMN &operator=(const MatrixMN &m);
    VecN operator*(const VecN &v) const;         // m1 * v
    MatrixMN operator*(const MatrixMN &m) const; // m1 * m2

    static VecN solve_gauss_seidel(const MatrixMN A, const VecN &b);
};

#endif
