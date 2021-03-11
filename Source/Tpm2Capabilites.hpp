#pragma once
#include <string>
#include <map>

#include "TssWrappers.hpp"
#include "Tpm2ContextBundle.hpp"

#include "spdlog/spdlog.h"

class Tpm2Capabilites{
public:
    Tpm2Capabilites() = delete;
public:
    enum CapacityCategory{AllFixed, AllVariable};
    enum CapDataFormat{UINT, HEX, STRING};
    using CapabilityMap = std::map<TPM2_PT, CapDataFormat>;
    using ParsedCapabilities = std::map<TPM2_PT, std::string>;
    
    static Tpm2Capabilites::ParsedCapabilities GetMappedCapabilities(Tpm2ContextBundle& contexts, CapacityCategory category);
    static UINT32 GetRawCapabilities(Tpm2ContextBundle& contexts, TPM2_CAP capabilityConst, UINT32 propertyConst);
    
    static bool IsInLockout(Tpm2ContextBundle& contexts);
    static bool IsOwnerAuthSet(Tpm2ContextBundle& contexts);
    static bool IsEndorsementAuthSet(Tpm2ContextBundle& contexts);
    static bool IsLockoutAuthSet(Tpm2ContextBundle& contexts);

private:
    static tss2_typedefs::Capability_PTR InternalGetCapNoAuth(Tpm2ContextBundle& contexts, TPM2_CAP capabilityConst, UINT32 propertyConst, UINT32 propertyCount=TPM2_MAX_TPM_PROPERTIES);
    static ParsedCapabilities ParseCapabilities(tss2_typedefs::Capability_PTR capabilities, const CapabilityMap& capsToParse);
};
