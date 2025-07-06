# ar-CAD (almost-ready-CAD)
An extremely lightweight (feature-wise) program for 3D modeling. It *almost* works well! It is a second iteration of a now deprecated, unnamed project for the Geometric Modeling 1 course at Warsaw University of Technology.

## Dependencies
The project uses the following libraries:
- **GLFW** for window management
- **GLAD** for loading OpenGL function pointers
- **entt** for an implementation of an Entity Component System architecture
- **spdlog** for logging
- **nativefiledialog-extended**
- **nlohmann-json**
- **Dear ImGui** for the UI layer

Everything is installed through **vcpkg** package manager in manifest mode.

## Current development
- [ ] Create a UI layer
- [ ] Create an entt wrapper
- [ ] Create a rendering layer