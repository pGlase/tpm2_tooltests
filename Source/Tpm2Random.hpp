#pragma once
#include <string>
#include <vector>

#include "TssWrappers.hpp"
#include "Tpm2ContextBundle.hpp"

#include "spdlog/spdlog.h"

class Tpm2Random{
public:
    Tpm2Random() = delete;
public:
    static std::vector<BYTE> GetRandomBytes(Tpm2ContextBundle& contexts, size_t count);

private:
    
};

