//
// Created by pointerlost on 2/20/26.
//
#pragma once

namespace Real::core { class IApplicationContext; }

namespace Real::core {

    class IApplication {
    public:
        virtual ~IApplication() = default;

        virtual void Init() = 0;
        virtual void Update(float /* dt */) = 0;
        virtual void Render() = 0;
        virtual void Shutdown() = 0;
        virtual IApplicationContext& GetContext() = 0;
    };
}
