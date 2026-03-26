//
// Created by pointerlost on 10/4/25.
//
#pragma once
#include <unordered_map>
#include "Common/Utils.h"
#include "Core/UUID.h"

namespace Real::graphics { struct Model; }

namespace Real::assets {

    class AssetManager {
    public:
        // Models
                      void                 RegisterModel(const Ref<graphics::Model>& model);
        [[nodiscard]] Ref<graphics::Model> GetModel     (const String& name) const;
        [[nodiscard]] bool                 IsModelExist (const String& name) const;

    private:
        std::unordered_map<UUID, Ref<graphics::Model>> m_Models;
        std::unordered_map<String, UUID>               m_ModelNameToUUID;
    };
}
