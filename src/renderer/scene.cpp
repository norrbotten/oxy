#include "renderer/scene.hpp"

namespace Oxy::Renderer {

    Scene::~Scene() {
        for (auto obj : m_objects)
            delete obj;
    }

    Color Scene::get_sample(CameraRay ray) {
        IntersectionResult res;

        for (auto obj : m_objects) {
            IntersectionResult obj_res;

            auto tr_origin = obj->world_to_local(ray.origin);
            auto tr_dir    = obj->world_to_local_dir(ray.dir);

            if (obj->intersects_ray(tr_origin, tr_dir, obj_res)) {
                if (obj_res.t < res.t)
                    res = obj_res;
            }
        }

        if (res.hit) {
            res.hitpos    = res.hitobj->local_to_world(res.hitpos);
            res.hitnormal = res.hitobj->local_to_world_dir(res.hitnormal);

            return Color(-glm::dot(res.hitnormal, ray.dir));
        }

        return Color();
    }

    void Scene::setup() {
        for (auto obj : m_objects) {
            obj->setup();
        }
    }

} // namespace Oxy::Renderer