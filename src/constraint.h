#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include "body.h"

class Constraint {
  public:
    Body *a;
    Body *b;

    MatrixMN get_inv_matrix();
    VecN vec;

    void solve();
};

#endif
