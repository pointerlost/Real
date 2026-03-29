//
// Created by pointerlost on 3/23/26.
//
#pragma once
#include "Common/Types.h"
#include "compressonator/include/cmp_compressonatorlib/compressonator.h"

namespace Real::graphics {
    enum class ImageFormatState;
}

namespace Real::util::compression {
    using namespace graphics;

    CMP_FORMAT       GetCMPDestinationFormat(int channelCount);
    String           DebugCMPStatus(CMP_ERROR error);
    String           ImageFormatState_EnumToString(ImageFormatState state);
    String           FormatToString(int format);
    String           InternalFormatToString(int format);
    ImageFormatState ImageFormatState_StringToEnum(const String& state);
}
