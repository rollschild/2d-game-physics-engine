#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include "body.h"
#include "matrix_mn.h"
#include "vec2.h"
#include "vec_n.h"

class Constraint {
  public:
    Body *a;
    Body *b;

    // anchor point in A's local space
    Vec2 a_point;
    // anchor point in B's local space
    Vec2 b_point;

    virtual ~Constraint() = default;

    MatrixMN get_inv_matrix() const;
    VecN get_velocities() const;

    // the `{}` is needed here
    // otherwise you will get the `undefined reference to vtable` error!
    // https://gcc.gnu.org/faq.html#vtables
    virtual void solve() {};
    virtual void pre_solve([[maybe_unused]] const float dt) {};
    virtual void post_solve() {};
};

class JointConstraint : public Constraint {
  private:
    MatrixMN jacobian;
    VecN cached_lambda;
    float bias;

  public:
    JointConstraint();
    JointConstraint(Body *a, Body *b, const Vec2 &anchor_point);
    void solve() override;
    void pre_solve(const float dt) override;
    void post_solve() override;
};

class PenetrationConstraint : public Constraint {
  private:
    MatrixMN jacobian;
    VecN cached_lambda;
    float bias;

    // normal direction of the penetration in A's local space
    Vec2 normal;
    // friction coefficient between the two penetrating bodies
    float friction;

  public:
    PenetrationConstraint();
    PenetrationConstraint(Body *a, Body *b, const Vec2 &a_collision_point,
                          const Vec2 &b_collision_point, const Vec2 &normal);
    void solve() override;
    void pre_solve(const float dt) override;
    void post_solve() override;
};

#endif
