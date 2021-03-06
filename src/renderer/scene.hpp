#pragma once

#include <type_traits>
#include <vector>

#include "renderer/context.hpp"

#include "renderer/utils/camera.hpp"
#include "renderer/utils/color.hpp"

#include "renderer/geometry/object.hpp"

namespace Oxy::Renderer {

    class Scene {
    public:
        Scene(RenderContext& ctx)
            : m_ctx(ctx)
            , m_bvh(nullptr) {}

        ~Scene();

        template <typename T>
        void add_object(T* obj) {
            m_objects.push_back((Object*)obj);
        }

        auto num_objects() const { return m_objects.size(); }

        void setup();

        Color get_sample(CameraRay ray);

    private:
        RenderContext& m_ctx;

        UnoptimizedBVHNode<Object*>* m_bvh;
        std::vector<Object*>         m_objects;
    };

} // namespace Oxy::Renderer