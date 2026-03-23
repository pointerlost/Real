//
// Created by pointerlost on 3/21/26.
//
#pragma once
#include "Core/IApplication.h"
#include "Core/IApplicationContext.h"

namespace realgame::app {

    struct GameContext final : public Real::core::IApplicationContext {
    };

    class GameApplication final : public Real::core::IApplication {
    public:
        explicit GameApplication(GameContext ctx) noexcept;

        void Init() override;
        void Update(float dt) override;
        void Render() override;
        void Shutdown() override;
        Real::core::IApplicationContext &GetContext() override { return m_Ctx; }

    private:
        GameContext m_Ctx;
    };
}
