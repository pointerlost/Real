//
// Created by pointerlost on 3/23/26.
//
#pragma once

namespace Real::asset {

    // AssetSystem owns everything and wires them together internally
    class AssetSystem {
    public:
        AssetSystem();

        TextureManager&  Textures()   { return *m_Textures; }
        MaterialFactory& Materials()  { return *m_Materials; }
        ShaderManager&   Shaders()    { return *m_Shaders; }
        AssetImporter&   Importer()   { return *m_Importer; }

    private:
        Scope<TextureManager>  m_Textures;
        Scope<MaterialFactory> m_Materials;
        Scope<ShaderManager>   m_Shaders;
        Scope<AssetImporter>   m_Importer;
    };
}