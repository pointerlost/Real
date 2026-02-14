//
// Created by pointerlost on 2/14/26.
//
#pragma once

namespace Real {
    class Scene;
}

namespace Real {

    class ISceneListener {
    public:
        virtual ~ISceneListener() = default;
        virtual void OnSceneAttach(Scene* scene) = 0;
        virtual void OnSceneDetach(Scene* scene) = 0;
    };
}