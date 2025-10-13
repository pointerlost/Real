#include <memory>
#include "Core/Engine.h"

int main() {
    const auto engine = Real::CreateScope<Real::Engine>();
    engine->InitResources();
    engine->Running();
    return 0;
}
