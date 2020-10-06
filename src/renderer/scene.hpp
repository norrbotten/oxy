#pragma once

#include <type_traits>
#include <vector>

#include "renderer/context.hpp"

#include "renderer/geometry/object.hpp"

#include "renderer/utils/camera.hpp"
#include "renderer/utils/color.hpp"

namespace Oxy::Renderer {

    class Scene {
    public:
        Scene(RenderContext& ctx)
            : m_ctx(ctx) {}

        ~Scene();

        template <typename T, std::enable_if<std::is_base_of<Object, T>::value>>
        void add_object(T* obj) {
            m_objects.push_back((Object*)obj);
        }

        auto num_objects() const { return m_objects.size(); }

        Color get_sample(CameraRay ray);

    private:
        RenderContext& m_ctx;

        std::vector<Object*> m_objects;
    };

} // namespace Oxy::Renderer