#pragma once

namespace Oxy::Renderer {

    class Object {
    public:
        Object();
        virtual ~Object();

        virtual void setup() {}

    private:
    };

} // namespace Oxy::Renderer
