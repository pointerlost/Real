//
// Created by pointerlost on 3/23/26.
//
#include "Common/StringUtils.h"
#include "Core/UUID.h"

namespace Real::util {

    bool IsSubString(const String& subStr, const String& string) {
        return string.find(subStr) != String::npos;
    }

    bool TryParseUUID(const String& strUUID, UUID& uuid) {
        try {
            uuid = UUID(std::stoul(strUUID));
            return true;
        } catch (...) {
            return false;
        }
    }

}
