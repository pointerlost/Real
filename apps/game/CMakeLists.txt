project(game)

add_executable(game)

target_sources(game PRIVATE
    src/GameApplication.cpp
    main.cpp
)

target_include_directories(game PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/vendor
    ${CMAKE_SOURCE_DIR}/vendor/physx/include
)

target_link_libraries(game
    PRIVATE engine
)
