#include "scene.hpp"

namespace Oxy::Renderer {

    Scene::~Scene() {}

    Color Scene::get_sample(CameraRay ray) {
        (void)ray;
        return Color(0, 0, 0);
    }

} // namespace Oxy::Renderer