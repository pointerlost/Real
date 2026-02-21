project(editor)

add_library(editor STATIC)

target_sources(editor PRIVATE
    include/IEditor.h
    src/ViewportPanel.cpp
    include/ViewportPanel.h
    include/EditorState.h
    src/EditorState.cpp
    include/EditorApplication.h
    src/EditorApplication.cpp
    src/Editor.cpp
    include/Editor.h
    include/InspectorPanel.h
    include/HierarchyPanel.h
    include/IPanel.h
    src/InspectorPanel.cpp
    src/HierarchyPanel.cpp
    main.cpp
    include/scenes/SandboxScene.h
    src/scenes/SandboxScene.cpp
)

target_include_directories(editor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ../vendor
    ../vendor/physx/include
)

target_link_libraries(editor
    PRIVATE engine
)
