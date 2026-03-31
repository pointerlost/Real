//
// Created by pointerlost on 12/6/25.
//
#pragma once

namespace Real::core {
    struct CoreSystems;
}

namespace Real::assets {

    class ResourceManager {
    public:
        ResourceManager();

        void Load(core::CoreSystems* core);

    private:
        void LoadAssets(core::CoreSystems* core) const;
        void LoadShaders();
    };

}
