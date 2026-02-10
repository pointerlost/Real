//
// Created by pointerlost on 10/24/25.
//
#pragma once
#include <vector>
#include "Core/Utils.h"
#include "entt/entt.hpp"

namespace Real { class Scene; }

namespace Real {

    struct ISystem {
        virtual ~ISystem() = default;
        virtual void Init() = 0;
        virtual void Update(Scene* scene, float deltaTime) = 0;
        virtual void Shutdown() = 0;
        virtual void SetRegistry(entt::registry& registry) = 0;
    };

    class Systems : public ISystem {
    public:
        void Init() override;
        void Update(Scene* scene, float deltaTime) override;
        void Shutdown() override;
        void SetRegistry(entt::registry& registry) override;

        virtual void OnSceneAttach(Scene* scene) {}
        virtual void OnSceneDetach(Scene* scene) {}

        const std::vector<Scope<Systems>>& GetSubSystems() const { return m_SubSystems; }

    private:
        std::vector<Scope<Systems>> m_SubSystems;
    };

}
