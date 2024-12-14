# 2D Game Physics Engine

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
- **Runge-Kutta Integration**
  - a family of integrators
  - **RK4** is the most popular
    - very accurate
-
