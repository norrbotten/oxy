#pragma once

#include "renderer/context.hpp"

#include "renderer/utils/camera.hpp"
#include "renderer/utils/color.hpp"

namespace Oxy::Renderer {

    class Scene {
    public:
        Scene(RenderContext& ctx)
            : m_ctx(ctx) {}

        ~Scene();

        Color get_sample(CameraRay ray);

    private:
        RenderContext& m_ctx;
    };

} // namespace Oxy::Renderer