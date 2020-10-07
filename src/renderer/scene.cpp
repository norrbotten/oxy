#include "renderer/scene.hpp"

#define USE_SCENE_BVH 1

namespace Oxy::Renderer {

    Scene::~Scene() {
        for (auto obj : m_objects)
            delete obj;

        if (m_bvh != nullptr)
            delete m_bvh;
    }

    Color Scene::get_sample(CameraRay ray) {
        IntersectionResult res;

#if USE_SCENE_BVH == 0
        for (auto obj : m_objects) {
            IntersectionResult obj_res;

            auto tr_origin = obj->world_to_local(ray.origin);
            auto tr_dir    = obj->world_to_local_dir(ray.dir);

            if (obj->intersect_ray(tr_origin, tr_dir, obj_res)) {
                if (obj_res.t < res.t)
                    res = obj_res;
            }
        }

        if (res.hit) {
            return Color(-glm::dot(res.hitnormal, ray.dir));
        }

#else
        if (dumb_bvh_traverse_objectptr(m_bvh, m_objects, ray.origin, ray.dir, res)) {
            return Color(-glm::dot(res.hitnormal, ray.dir));
        }
#endif

        return Color();
    }

    void Scene::setup() {
        for (auto obj : m_objects) {
            obj->setup();
        }

#if USE_SCENE_BVH == 1
        m_bvh = build_bvh_generic<Object*>(m_objects, 0, m_objects.size());
#endif
    }

} // namespace Oxy::Renderer
