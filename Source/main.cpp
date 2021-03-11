#include <iostream>
#include "spdlog/spdlog.h"
#include "Tpm2ContextBundle.hpp"
#include "Tpm2Capabilites.hpp"
#include "Tpm2Random.hpp"

using namespace tssWrappers;
using namespace tss2_typedefs;
using namespace tss2_valueConverters;

int main(){
    //testing various functions from the different classes - no testing lib yet chosen
    Tpm2ContextBundle contexts = Tpm2ContextBundle::InitDefaultUnauthContexts();
    
    auto fixedCaps = Tpm2Capabilites::GetMappedCapabilities(contexts, Tpm2Capabilites::CapacityCategory::AllFixed);
    
    spdlog::info("AllFixed capabilities");
    spdlog::info("Format:\n\t\t\t   TPM2_PT : Value");
    
    for(auto const& [key, val] : fixedCaps){
        spdlog::info("{0} : {1}", key, val);
    }
    
    spdlog::info("Testing AllVariable output:");
    UINT32 buffer = Tpm2Capabilites::GetRawCapabilities(contexts, TPM2_CAP_TPM_PROPERTIES, TPM2_PT_PERMANENT);
    spdlog::info("ownerAuthSet: \t{0}", WriteBitfieldElement(buffer, 0xf0000000));
    spdlog::info("endorsementAuthSet: \t{0}", WriteBitfieldElement(buffer, 0x0f000000));
    spdlog::info("lockoutAuthSet: \t{0}", WriteBitfieldElement(buffer, 0x00f00000));
    spdlog::info("reserved1: \t\t{0}", WriteBitfieldElement(buffer, 0x000f0000));
    spdlog::info("disableClear: \t{0}", WriteBitfieldElement(buffer, 0x0000f000));
    spdlog::info("inLockout: \t\t{0}", WriteBitfieldElement(buffer, 0x00000f00));
    spdlog::info("tpmGeneratedEPS: \t{0}", WriteBitfieldElement(buffer, 0x00000f0));
    spdlog::info("reserved2: \t\t{0}", WriteBitfieldElement(buffer, 0x0000000f));
    
    if(Tpm2Capabilites::IsOwnerAuthSet(contexts) || Tpm2Capabilites::IsOwnerAuthSet(contexts) || Tpm2Capabilites::IsOwnerAuthSet(contexts)){
        spdlog::info("An authentication was set, no automated take_ownership should be executed");
    }

    if(Tpm2Capabilites::IsInLockout(contexts)){
        spdlog::info("Lockout is set");
    }
    //using spdlog timestamps as broad indication here
    spdlog::info("random start");
    auto aa = Tpm2Random::GetRandomBytes(contexts, 64);
    spdlog::info("random aa finished, bytes:{0}", aa.size());
    auto ab = Tpm2Random::GetRandomBytes(contexts, std::numeric_limits<UINT16>::max());
    spdlog::info("random ab finished, bytes:{0}", ab.size());
    try{
        auto ac = Tpm2Random::GetRandomBytes(contexts, 100000);
        spdlog::info("random end, bytes:{0}", ac.size());
    }catch(const std::invalid_argument& ex){
        (void)ex;
        spdlog::info("GetRandomBytes died as expected");
    }
    
    return 0;
}
