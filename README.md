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
