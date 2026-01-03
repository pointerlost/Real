//
// Created by pointerlost on 10/8/25.
//
#pragma once
#include "Scene/Entity.h"

namespace Real {

    class CameraInput {
    public:
        explicit CameraInput(Entity* camera);

        void Update();

    private:
        Entity* m_Camera;
    };
}
