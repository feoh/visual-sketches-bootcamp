# Independent oracle

These values use `x=10`, `vx=10`, zero acceleration, drag rate `1`, and two
semi-implicit 0.25-second steps: `v *= exp(-dt)` then `x += v * dt`. They were
calculated independently from the application and are strictly parsed by tests.
