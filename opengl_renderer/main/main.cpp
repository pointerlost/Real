#include <memory>
#include "Core/Engine.h"

int main() {
    const auto engine = Real::CreateScope<Real::Engine>();
    engine->InitResources();
    engine->InitGameResources(); // This is not permanent, I'll remove after adding game state
    engine->Running();
    return 0;
}
