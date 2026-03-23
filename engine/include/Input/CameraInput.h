//
// Created by pointerlost on 10/8/25.
//
#pragma once

namespace Real {
    class Entity;

    class CameraInput {
    public:
        explicit CameraInput(Entity* camera);

        void Update();

    private:
        Entity* m_Camera;
    };
}
