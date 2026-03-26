//
// Created by pointerlost on 3/23/26.
//
#pragma once
#include "Types.h"

namespace Real { struct UUID; }

namespace Real::util {
    bool IsSubString(const String& sub, const String& str);
    bool TryParseUUID(const String& strUUID, UUID& uuid);
}
