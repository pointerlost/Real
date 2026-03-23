project(editor)

add_executable(editor
        include/UIResources.h
        src/UIResources.cpp)

target_sources(editor PRIVATE
    src/ApplicationBuilder.cpp
    src/ViewportPanel.cpp
    src/EditorState.cpp
    src/EditorApplication.cpp
    src/Editor.cpp
    src/InspectorPanel.cpp
    src/HierarchyPanel.cpp
    src/scenes/SandboxScene.cpp
    main.cpp
)

target_include_directories(glad_local PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glad/include)

target_include_directories(editor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/vendor
    ${CMAKE_SOURCE_DIR}/vendor/physx/include
)

target_link_libraries(editor PRIVATE
    engine
)
