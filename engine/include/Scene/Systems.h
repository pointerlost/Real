//
// Created by pointerlost on 10/24/25.
//
#pragma once
#include <vector>
#include "Core/Utils.h"

namespace Real { class Scene; }

namespace Real {

    struct ISystem {
        virtual ~ISystem() = default;
        virtual void Update(Scene* scene, float deltaTime) = 0;
    };

    class Systems : public ISystem {
    public:
        void Init();
        void UpdateAll(Scene* scene, float deltaTime) const;

    protected:
        void Update(Scene* scene, float deltaTime) override {}

    private:
        std::vector<Scope<Systems>> m_Updatables;
    };

}
