## Gerstner Wave Model

Classical sinusoidal displacement moves vertices only vertically, producing unrealistic symmetric profiles. The Gerstner (trochoidal) wave models water particles moving in **circular orbits**, compressing geometry near crests and stretching it in troughs -- matching asymmetric wave profiles observed in real oceans.

For a vertex at position **P** with normalized wave direction **D** projected onto the XZ plane:

```
k       = 2 * pi / lambda                    wave number
omega   = sqrt(g * k)                        angular frequency (dispersion relation)
f       = k * (D . P) - omega * t            phase

P'.x    = P.x + (Q / k) * D.x * cos(f)      horizontal displacement along wave direction
P'.y    = P.y + (Q / k) * sin(f)             vertical displacement
P'.z    = P.z + (Q / k) * D.z * cos(f)       horizontal displacement perpendicular component
```

Where:
- **lambda** is the wavelength (distance between consecutive crests)
- **Q** is the steepness factor (Q = 0 yields a flat plane; Q = 1 produces the sharpest crest before self-intersection)
- **g = 9.8 m/s^2** is gravitational acceleration
- The **dispersion relation** `sqrt(g * k)` ties wave speed to wavelength: longer waves travel faster, matching real ocean physics

The wave direction is projected onto the XZ plane and normalized before computation, ensuring displacement occurs only in the horizontal ground plane and vertically. The fragment shader is a pure pass-through -- all visual output is a constant ocean-blue vertex color `(0.25, 0.52, 0.96)`, deliberately omitting lighting to isolate the geometric wave behavior.

| Parameter | Value |
|-----------|-------|
| Steepness (Q) | 0.4 |
| Wavelength | 0.25 |
| Direction | (0.5, 0.5, 0.5) projected to XZ |
| Time step | 0.005 s |

## Building

### Prerequisites

| Dependency | Install (Debian/Ubuntu) |
|------------|------------------------|
| CMake 3.25+ | `sudo apt install cmake` |
| C++14 compiler | `sudo apt install g++` |
| OpenGL 4.0 | GPU drivers |
| GLFW 3 | `sudo apt install libglfw3-dev` |
| GLM | `sudo apt install libglm-dev` |

### Compile and Run

```bash
cd src
mkdir build && cd build
cmake ..
make
./phviewer_vs
```

> **Note:** Shader and model paths in `PHViewer.cpp` (lines 211-217) are hardcoded. Update them to match your local directory, or use the `SOURCE_DIR` compile definition already provided by the CMakeLists.
