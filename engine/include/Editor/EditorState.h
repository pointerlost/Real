//
// Created by pointerlost on 10/8/25.
//
#pragma once

namespace Real {
    class Entity;
}

namespace Real {

    struct ViewportDebugSettings {
        bool showPhysics   = false;
        bool showColliders = false;
        bool showGrid   = false;
        bool showBounds    = false;
    };

    struct EditorState {
        // TODO: There is a raw pointer danger here, so i should change it to an Entity UUID maybe?
        Entity* selectedEntity{};
        // TODO: I need to replace editorCamera with viewportCamera, so i should move the camera stuff to the ViewportPanel!!
        Entity* editorCamera{};
        bool Running = true;
        bool FpsMode = true;

        ViewportDebugSettings debug;
    };
}
