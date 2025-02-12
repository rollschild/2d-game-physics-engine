# 2D Game Physics Engine

[![built with nix](https://builtwithnix.org/badge.svg)](https://builtwithnix.org)

## Build & Run

- `nix develop` to enable Nix
- `cmake . -B build`
- `cmake --build build`
- `./build/src/main`

## Dependencies

- `SDL`
  - for multi-platform capability

## Physics

- **Newtonian physics**
  - **inertia**
  - **Force, mass, acceleration**
    - $F = ma$
  - **Action and reaction**
- **Variable Delta-Time**
  - delta time _changes_ between frames
  - _BUT_ will result in **non-deterministic** simulation
  - _could_ use a constant delta time ensure determinism
- $p = p_0 + v_0t + at^2/2$
  - $p$ is position
- **Newton's dot notation**
  - ẋ - $x$ is function of time $t$, the derivative of $x$ with respect to $t$
  - ẍ - second derivative of $x$ with respect to time $t$
- **Euler integration**
  - the most basic numerical integration technique
  - only 100% when rate of change is constant
- **Implicit Euler**
  - good enough for _most_ game applications
- **Verlet Integration**
  - better accuracy than **implicit Euler**
  - less memory usage when simulating a large number of particles
  - best suited for simulating particles and constraints between particles
- **Runge-Kutta Integration**
  - a family of integrators
  - **RK4** is the most popular
    - very accurate
- **Drag Force**
  - resistive force
  - depends on velocity
- **Friction**
  - **kinetic friction**
  - **static friction**
- **jerk** - rate of change of acceleration
- **Spring Force**
- **Angular Velocity**
  - **theta** - angle
  - **omega** - angular velocity
  - **alpha** - angular acceleration
- Initiate angular movement by applying **torque**
- **Moment of Intertia**
  - $I_{solidcircle} = (r^2 / 2) * m$
  - $I_{solidrectangle} = ((w^2 + h^2) / 12) * m$
- **local space** vs. **world space**
- **Collision**
  - Steps:
    1. Collision detection - shape-specific
    2. Collision resolution - agnostic to shape
- Techniques of collision detection
  - **broad phase**
    - use bounding circles/boxes
  - **narrow phase**
- **projection method** vs. **impulse method** vs. **penalty method**
  - **projection method**: adjust position of the colliding objects
  - **impulse method**: adjust velocity of colliding objects by apply impulses
  - **penalty method**: adjust acceleration of colliding objects
- **projection method**
  - $d_{a} = depth * (m_b / (m_a + m_b))$
  - $d_{b} = depth * (m_a / (m_a + m_b))$
- **Momentum**
  - **conservation of momentum**
- **Impulse** - change in momentum by performing a direct change in velocity
  - $J = F * Δt = ΔP = mv' - mv = mΔv$
  - $Jn$ - impulse vector
- **AABB**
- **Separating Axis Theorem (SAT)**
  - _ONLY_ works for **convex polygons**
    - compared to **concave polygon**
  - one of the most popular collision detection algos for rigid bodies
  - with **GJK** algorithm
    - works for both convex & concave
- **Oriented Bounding Box (OBB)**
  - if there is _at least one axis_ that separates both shapes, then they are _not_ colliding
- **Support Point**
  - vertex in the second polygon that is furthest away _behind_ a certain edge of the first polygon
- **linear point velocity** with rotation
- **impulse along the tangent**
  - similar to **friction**
- Polygon-Circle Collision
  1. compute the nearest edge (closet to center of circle)

## Game Engine Design

- the **world** object

### Constraints

- the most popular technique to implement motion of rigid bodies is to apply **constraints**
- define restrictions on how bodies move
- Types:
  - **joint** constraints
  - **non-penetration**
- remove **degrees of freedom**
- the **constraint function C**
  - takes the **state** of a pair of rigid bodies as parameter - **state vector** with:
    - positions
    - masses
    - orientations
    - velocities
  - outputs a scalar number
  - when value of the function is in the acceptable range, the constraint is _satisfied_
  - in each step of the simulation, we must apply forces or impulses on the rigid bodies to attempt to keep the value of `C` in the allowed range
- modern physics engines solve **systems of constraints**
- **equality constraints**
- **inequality constraints**
  - solve collisions
- **Baumgarte stabilization factor (bias term)**
- Generalized velocity-based constraints
  - $JV + b = 0$
  - $J$ is the **Jacobian Matrix**
- **Warm starting**
- **Reference Edge**
  - the original axis where the contact of best penetration was found
- **Incident Edge**
  - a face of the "other" rigid body that is opposing the reference edge
  - the least aligned
- **linear interpolation**
  - $lerp(a, b, t) = a + t(b - a)$

## Optimization Techniques

### Contact Caching

- **manifold caching**
- **warm starting**

### Continuous Collision Detection

- **tunneling**
- **ray casting**
  - cast a ray between two soon-to-be-colliding objects
  - then get the **time of impact**
