//
// Created by pointerlost on 2/14/26.
//
#pragma once

namespace Real::event {
    class SceneEvents;
}

namespace Real {

    class ISceneListener {
    public:
        virtual ~ISceneListener() = default;
        virtual void OnSceneAttach(entt::registry& /*registry*/, event::SceneEvents& /*events*/) = 0;
        virtual void OnSceneDetach(entt::registry& /*registry*/, event::SceneEvents& /*events*/) = 0;
    };
}