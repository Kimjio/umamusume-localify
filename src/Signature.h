#pragma once

#include <winnt.h>

inline uint32_t SigScanOffset(const char* signature, const char* mask, MEMORY_BASIC_INFORMATION memInfo, int offset = 0)
{
    const size_t length = strlen(mask);

    for (size_t i = offset; i < memInfo.RegionSize; i++)
    {
        char* memory = reinterpret_cast<char*>(memInfo.BaseAddress) + i;

        size_t j;
        for (j = 0; j < length; j++)
        {
            if (mask[j] != '?' && signature[j] != memory[j])
                break;
        }

        if (j == length)
        {
            return i + length;
        }
    }

    return 0;
}
