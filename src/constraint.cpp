#include "constraint.h"
#include "body.h"
#include "matrix_mn.h"
#include "vec2.h"
#include "vec_n.h"

VecN Constraint::get_velocities() const {
    VecN V(6);
    V.zero();
    V[0] = a->velocity.x;
    V[1] = a->velocity.y;
    V[2] = a->angular_vel;
    V[3] = b->velocity.x;
    V[4] = b->velocity.y;
    V[5] = b->angular_vel;

    return V;
}

/**
 * Mat6x6 with the all inverse mass and inverse I of bodies a and b
 *  [ 1/ma  0     0     0     0     0     ]
 *  [ 0     1/ma  0     0     0     0     ]
 *  [ 0     0     1/Ia  0     0     0     ]
 *  [ 0     0     0     1/mb  0     0     ]
 *  [ 0     0     0     0     1/mb  0     ]
 *  [ 0     0     0     0     0     1/Ib  ]
 */
MatrixMN Constraint::get_inv_matrix() const {
    MatrixMN inv_m(6, 6);
    inv_m.zero();

    inv_m.rows[0][0] = a->inv_mass;
    inv_m.rows[1][1] = a->inv_mass;
    inv_m.rows[2][2] = a->inv_I;
    inv_m.rows[3][3] = b->inv_mass;
    inv_m.rows[4][4] = b->inv_mass;
    inv_m.rows[5][5] = b->inv_I;

    return inv_m;
}

JointConstraint::JointConstraint() : Constraint(), jacobian(1, 6) {}
JointConstraint::JointConstraint(Body *a, Body *b, const Vec2 &anchor_point)
    : Constraint(), jacobian(1, 6) {
    this->a = a;
    this->b = b;
    this->a_point = a->worldspace_to_localspace(anchor_point);
    this->b_point = b->worldspace_to_localspace(anchor_point);
}

void JointConstraint::solve() {
    // get anchor point position in world space
    const Vec2 pa = a->localspace_to_worldspace(a_point);
    const Vec2 pb = b->localspace_to_worldspace(b_point);
    // Load the jacobian
    jacobian.zero();
    const Vec2 ra = pa - a->position;
    const Vec2 rb = pb - b->position;
    Vec2 j1 = (pa - pb) * 2.0;
    jacobian.rows[0][0] = j1.x;
    jacobian.rows[0][1] = j1.y;
    float j2 = 2.0 * ra.cross(pa - pb);
    jacobian.rows[0][2] = j2;

    Vec2 j3 = (pb - pa) * 2.0;
    jacobian.rows[0][3] = j3.x;
    jacobian.rows[0][4] = j3.y;

    float j4 = rb.cross(pb - pa) * 2.0;
    jacobian.rows[0][5] = j4;

    // v = get_velocities()
    const VecN v = get_velocities();

    // inv_M = get_invM()
    const MatrixMN inv_M = get_inv_matrix();

    const MatrixMN jt = jacobian.transpose();

    // compute lambda -> impulse to apply to objects A and B
    // lambda = -(JV + b) / (JM^(-1)Jt)
    // (JM^(-1)Jt)lambda = -(JV + b)
    VecN rhs = jacobian * v * -1.0f;
    MatrixMN lhs = jacobian * inv_M * jt;
    // Solve lambda using the **Gauss-Seidel method**
    // Ax = b
    VecN lambda = MatrixMN::solve_gauss_seidel(lhs, rhs);

    // compute final impulses with direction and magnitude
    VecN impulses = jt * lambda;

    // apply lambda impulse to both A and B
    a->apply_impulse_linear(Vec2(impulses[0], impulses[1]));
    a->apply_impulse_angular(impulses[2]);
    b->apply_impulse_linear(Vec2(impulses[3], impulses[4]));
    b->apply_impulse_angular(impulses[5]);
}
