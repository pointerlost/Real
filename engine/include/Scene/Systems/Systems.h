//
// Created by pointerlost on 10/24/25.
//
#pragma once
#include "ISystem.h"
#include <vector>
#include "Core/Utils.h"
#include "Common/RealTypes.h"

namespace Real { class Scene; }

namespace Real {

    class Systems final : public ISystem {
    public:
        Systems() = default;

        Systems(const Systems&) = delete;
        Systems& operator=(const Systems&) = delete;

    public:
        void AddSystem(Scope<ISystem> system);

        void Init() override;
        void Update(Scene*, f32) override;
        void Shutdown() override;

        void OnSceneAttach(Scene* scene);
        void OnSceneDetach(Scene* scene);

    private:
        Vector<Scope<ISystem>> m_SubSystems;
    };

}
