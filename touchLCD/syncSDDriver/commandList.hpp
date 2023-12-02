#pragma once

#include <inttypes.h>
class CCommandList
{
    static constexpr uint8_t s_commandOffset = 64;
public:
    static constexpr uint8_t s_cmd0 = s_commandOffset + 0;
    static constexpr uint8_t s_cmd1 = s_commandOffset + 1;
    static constexpr uint8_t s_cmd8 = s_commandOffset + 8;
    static constexpr uint8_t s_cmd9 = s_commandOffset + 9;
    static constexpr uint8_t s_cmd10 = s_commandOffset + 10;
    static constexpr uint8_t s_cmd16 = s_commandOffset + 16;
    static constexpr uint8_t s_cmd17 = s_commandOffset + 17;
    static constexpr uint8_t s_cmd24 = s_commandOffset + 24;
    static constexpr uint8_t s_cmd41 = s_commandOffset + 41;
    static constexpr uint8_t s_cmd55 = s_commandOffset + 55;
    static constexpr uint8_t s_cmd58 = s_commandOffset + 58;
};