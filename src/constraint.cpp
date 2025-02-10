#include "constraint.h"
#include "body.h"
#include "matrix_mn.h"
#include "vec2.h"
#include "vec_n.h"
#include <algorithm>

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

JointConstraint::JointConstraint()
    : Constraint(), jacobian(1, 6), cached_lambda(1), bias(0.0f) {
    cached_lambda.zero();
}
JointConstraint::JointConstraint(Body *a, Body *b, const Vec2 &anchor_point)
    : Constraint(), jacobian(1, 6), cached_lambda(1), bias(0.0f) {
    this->a = a;
    this->b = b;
    this->a_point = a->worldspace_to_localspace(anchor_point);
    this->b_point = b->worldspace_to_localspace(anchor_point);

    cached_lambda.zero();
}

void JointConstraint::pre_solve(const float dt) {
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

    // Warm starting
    // Before anything else, apply the cached_lambda from the previous solve()
    // call
    // compute final impulses with direction and magnitude
    const MatrixMN jt = jacobian.transpose();
    VecN impulses = jt * cached_lambda;

    // apply lambda impulse to both A and B
    a->apply_impulse_linear(Vec2(impulses[0], impulses[1]));
    a->apply_impulse_angular(impulses[2]);
    b->apply_impulse_linear(Vec2(impulses[3], impulses[4]));
    b->apply_impulse_angular(impulses[5]);

    // compute bias (baumgarte stabilization)
    const float beta = 0.1f;
    // compute the positional error
    float C = (pb - pa).dot(pb - pa);
    C = std::max(0.0f, C - 0.001f);
    bias = (beta / dt) * C;
}

void JointConstraint::post_solve() {
    // TODO
}

void JointConstraint::solve() {
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
    rhs[0] -= bias;
    // Solve lambda using the **Gauss-Seidel method**
    // Ax = b
    VecN lambda = MatrixMN::solve_gauss_seidel(lhs, rhs);

    // save the lambda
    cached_lambda += lambda;

    // compute final impulses with direction and magnitude
    VecN impulses = jt * lambda;

    // apply lambda impulse to both A and B
    a->apply_impulse_linear(Vec2(impulses[0], impulses[1]));
    a->apply_impulse_angular(impulses[2]);
    b->apply_impulse_linear(Vec2(impulses[3], impulses[4]));
    b->apply_impulse_angular(impulses[5]);
}

PenetrationConstraint::PenetrationConstraint()
    : Constraint(), jacobian(2, 6), cached_lambda(2), bias(0.0f) {
    cached_lambda.zero();
    friction = 0.0f;
}
PenetrationConstraint::PenetrationConstraint(Body *a, Body *b,
                                             const Vec2 &a_collision_point,
                                             const Vec2 &b_collision_point,
                                             const Vec2 &normal)
    : Constraint(), jacobian(2, 6), cached_lambda(2), bias(0.0f) {
    this->a = a;
    this->b = b;
    this->a_point = a->worldspace_to_localspace(a_collision_point);
    this->b_point = b->worldspace_to_localspace(b_collision_point);
    this->normal = a->worldspace_to_localspace(normal);

    cached_lambda.zero();
    friction = 0.0f;
}

/**
 * C = [[-n, -ra X n, n, rb X n], [-t, -ra X t, t, rb X t]] * [[va], [wa], [vb],
 * [wb]]
 */
void PenetrationConstraint::pre_solve(const float dt) {
    // get collision points in world space
    const Vec2 pa = a->localspace_to_worldspace(a_point);
    const Vec2 pb = b->localspace_to_worldspace(b_point);
    Vec2 n = a->localspace_to_worldspace(normal);

    // Load the jacobian
    jacobian.zero();
    const Vec2 ra = pa - a->position;
    const Vec2 rb = pb - b->position;

    Vec2 j1 = -n;
    jacobian.rows[0][0] = j1.x;
    jacobian.rows[0][1] = j1.y;

    float j2 = -ra.cross(n);
    jacobian.rows[0][2] = j2;

    Vec2 j3 = n;
    jacobian.rows[0][3] = j3.x;
    jacobian.rows[0][4] = j3.y;

    float j4 = rb.cross(n);
    jacobian.rows[0][5] = j4;

    // populate second row of Jacobian
    // tangent vector - friction
    friction = std::max(a->friction, b->friction);
    if (friction > 0.0) {
        // tangent is perpendicular to the normal
        Vec2 t = n.normal();
        jacobian.rows[1][0] = -t.x;
        jacobian.rows[1][1] = -t.y;
        jacobian.rows[1][2] = -ra.cross(t);

        jacobian.rows[1][3] = t.x;
        jacobian.rows[1][4] = t.y;
        jacobian.rows[1][5] = rb.cross(t);
    }

    // Warm starting
    // Before anything else, apply the cached_lambda from the previous solve()
    // call
    // compute final impulses with direction and magnitude
    const MatrixMN jt = jacobian.transpose();
    VecN impulses = jt * cached_lambda;

    // apply lambda impulse to both A and B
    a->apply_impulse_linear(Vec2(impulses[0], impulses[1]));
    a->apply_impulse_angular(impulses[2]);
    b->apply_impulse_linear(Vec2(impulses[3], impulses[4]));
    b->apply_impulse_angular(impulses[5]);

    // compute bias (baumgarte stabilization)
    const float beta = 0.2f;
    // compute the positional error
    float C = (pb - pa).dot(-n);
    C = std::min(0.0f, C + 0.001f);

    // calculate relative velocity pre-impulse normal to compute elasticity
    Vec2 va = a->velocity + Vec2(-a->angular_vel * ra.y, a->angular_vel * ra.x);
    Vec2 vb = b->velocity + Vec2(-b->angular_vel * rb.y, b->angular_vel * rb.x);
    float v_rel_dot_normal = (va - vb).dot(n);

    // restitution between two bodies
    float e = std::min(a->restitution, b->restitution);

    // considering elasticity
    bias = (beta / dt) * C + (e * v_rel_dot_normal);
}

void PenetrationConstraint::solve() {
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
    rhs[0] -= bias;
    // Solve lambda using the **Gauss-Seidel method**
    // Ax = b
    VecN lambda = MatrixMN::solve_gauss_seidel(lhs, rhs);
    // think about the sign of the lambda, because we want to _accumulate_
    // save a temp lambda
    VecN old_lambda = cached_lambda;
    // save the lambda
    cached_lambda += lambda;
    cached_lambda[0] = (cached_lambda[0] < 0.0f) ? 0.0f : cached_lambda[0];

    // keep friction values between -(λn*μ) and (λn*μ)
    if (friction > 0.0) {
        const float max_friction = cached_lambda[0] * friction;
        cached_lambda[1] =
            std::clamp(cached_lambda[1], -max_friction, max_friction);
    }

    lambda = cached_lambda - old_lambda;

    // compute final impulses with direction and magnitude
    VecN impulses = jt * lambda;

    // apply lambda impulse to both A and B
    a->apply_impulse_linear(Vec2(impulses[0], impulses[1]));
    a->apply_impulse_angular(impulses[2]);
    b->apply_impulse_linear(Vec2(impulses[3], impulses[4]));
    b->apply_impulse_angular(impulses[5]);
}

void PenetrationConstraint::post_solve() {
    // TODO
}
