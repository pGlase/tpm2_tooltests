#pragma once

#include <memory>
#include <utility>
#include <exception>

#include <tss2/tss2_esys.h>
#include <tss2/tss2_rc.h>

#include "TssWrappers.hpp"

class Tpm2ContextBundle{
    
public:
    Tpm2ContextBundle static InitDefaultUnauthContexts();
    ~Tpm2ContextBundle();
    ESYS_CONTEXT*       GetEsysContext();
    TSS2_TCTI_CONTEXT*  GetTctiContext();
    TSS2_SYS_CONTEXT*   GetSysContext();
    
    bool SetEsysContext(ESYS_CONTEXT* esysContext);
    bool SetTctiContext(TSS2_TCTI_CONTEXT* tctiContext);
    bool SetSysContext(TSS2_SYS_CONTEXT* sysContext);
    Tpm2ContextBundle(const Tpm2ContextBundle&) = delete;
    Tpm2ContextBundle(Tpm2ContextBundle&&) = default;
    Tpm2ContextBundle& operator=(Tpm2ContextBundle other) = delete;
    Tpm2ContextBundle& operator=(Tpm2ContextBundle&& other) = default;
    
private:
    Tpm2ContextBundle() = default;
 
    
    struct ContextBundle{
        tss2_typedefs::ESYS_CONTEXT_PTR esysContext;
        tss2_typedefs::TCTI_CONTEXT_PTR tctiContext;
        tss2_typedefs::SYS_CONTEXT_PTR sysContext;
    };
    
    void static InitDefaultEsyssContexts(ContextBundle& context);
    void static InitDefaultSysContext(ContextBundle& context);

    bool InitEsysContext(ESYS_CONTEXT* esysContext);
    bool InitTctiContext(TSS2_TCTI_CONTEXT* tctiContext);
    bool InitSysContext(TSS2_SYS_CONTEXT* sysContext);
    
    void StartupTpm2(bool performReset);
    void ShutdownTpm2();
    
private:
    ContextBundle tpm2Contexts;   
};

