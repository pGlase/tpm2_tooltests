#pragma once
#include <tss2/tss2_esys.h>
#include <tss2/tss2_rc.h>
#include <memory>
#include <iostream>

namespace tss2_logging{
    //returns the result of Tss2_RC_Decode as c++ string
    static std::string decodeTss2Rc(TSS2_RC rc){
        return Tss2_RC_Decode(rc);
    }

} //end of namespace tss2_logging

namespace tss2_valueConverters{
    static bool invalidChar (UINT8 c) 
    {  
        return !(c>=0 && c <128); 
    }
    //converts a UInt32 value to a string
    static std::string ConvertUint32ToString(UINT32 value){
        return std::to_string(value);
    }

    //interprets a UInt32 value as an ASCII-encoded string
    static std::string InterpretUint32AsAsciiString(UINT32 value){
        //UINT32 ==> 32bit <=> 4*char(8*1)
        std::string result;
        UINT8 intBuffer[4];
        //first char = highest pair
        //ToDo: Endianess on ARM vs x64
        intBuffer[3] = (value & 0x000000ff);
        intBuffer[2] = (value & 0x0000ff00) >> 8;
        intBuffer[1] = (value & 0x00ff0000) >> 16;
        intBuffer[0] = (value & 0xff000000) >> 24;
        for(const auto& x : intBuffer){
            if(std::isprint(x) && !(std::isspace(x)) ){
                result += static_cast<char>(x);
            }
        }
        return result;
    }
    
    static std::string WriteBitfieldElement(UINT32 value, UINT32 bitmask){
        if(value & bitmask){return "Set";}
        else{return "Unset";}
    }
    
    static bool EvalBitfieldElement(UINT32 value, UINT32 bitmask){
        return (value & bitmask);
    }
    
} //end of namespace tss2_valueConverters


namespace tssWrappers{
struct ESYS_CONTEXT_Deleter{
    void operator()(ESYS_CONTEXT* esysContext) {
        if(esysContext != nullptr){
            Esys_Free(esysContext);
        }      
    }
};

struct TSS2_TCTI_CONTEXT_Deleter{
    void operator()(TSS2_TCTI_CONTEXT* tctiContext) {
        if(tctiContext != nullptr){
            Esys_Free(tctiContext);
        }      
    }
};

struct TSS2_SYS_CONTEXT_Deleter{
    void operator()(TSS2_SYS_CONTEXT* sysContext) {
        if(sysContext != nullptr){
            Esys_Free(sysContext);
        }      
    }
};

struct TPM_CAPABILITY_DATA_Deleter{
    void operator()(TPMS_CAPABILITY_DATA* capData) {
        if(capData != nullptr){
            Esys_Free(capData);
        }      
    }
};

struct TPM2B_DIGEST_Deleter{
    void operator()(TPM2B_DIGEST* digest) {
        if(digest != nullptr){
            Esys_Free(digest);
        }      
    }
};

} //end of namespace tssWrappers

namespace tss2_typedefs{
    using namespace tssWrappers;
    using ESYS_CONTEXT_PTR = std::unique_ptr<ESYS_CONTEXT, ESYS_CONTEXT_Deleter>;
    using TCTI_CONTEXT_PTR = std::unique_ptr<TSS2_TCTI_CONTEXT, TSS2_TCTI_CONTEXT_Deleter>;
    using SYS_CONTEXT_PTR = std::unique_ptr<TSS2_SYS_CONTEXT, TSS2_SYS_CONTEXT_Deleter>;
    using Capability_PTR = std::unique_ptr<TPMS_CAPABILITY_DATA, TPM_CAPABILITY_DATA_Deleter>; 
    using Digest_PTR = std::unique_ptr<TPM2B_DIGEST, TPM2B_DIGEST_Deleter>;    

} //end of namespace tss2_typedefs


