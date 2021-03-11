#include "Tpm2ContextBundle.hpp"

Tpm2ContextBundle Tpm2ContextBundle::InitDefaultUnauthContexts(){
    ContextBundle contextBundle;
    Tpm2ContextBundle newObject;
    //init single contexts
    InitDefaultEsyssContexts(contextBundle);
    InitDefaultSysContext(contextBundle);
    //init result
    newObject.InitEsysContext(contextBundle.esysContext.release());
    newObject.InitTctiContext(contextBundle.tctiContext.release());
    newObject.InitSysContext(contextBundle.sysContext.release());
    //startup
    newObject.StartupTpm2(true);
    return newObject;
}

Tpm2ContextBundle::~Tpm2ContextBundle(){
    ShutdownTpm2();
}

ESYS_CONTEXT* Tpm2ContextBundle::GetEsysContext(){
    return tpm2Contexts.esysContext.get();
}
TSS2_TCTI_CONTEXT* Tpm2ContextBundle::GetTctiContext(){
    return tpm2Contexts.tctiContext.get();
}
TSS2_SYS_CONTEXT* Tpm2ContextBundle::GetSysContext(){
    return tpm2Contexts.sysContext.get();
}
    
bool Tpm2ContextBundle::InitEsysContext(ESYS_CONTEXT* esysContext){
    if(esysContext == nullptr || GetEsysContext() != nullptr){
        return false;
    }
    tpm2Contexts.esysContext.reset(esysContext);
    return true;
}


bool Tpm2ContextBundle::InitTctiContext(TSS2_TCTI_CONTEXT* tctiContext){
    if(tctiContext == nullptr || GetTctiContext() != nullptr){
        return false;
    }
    tpm2Contexts.tctiContext.reset(tctiContext); 
    return true;
}

bool Tpm2ContextBundle::InitSysContext(TSS2_SYS_CONTEXT* sysContext){
    if(sysContext == nullptr || GetSysContext() != nullptr){
        return false;
    }
    tpm2Contexts.sysContext.reset(sysContext);
    return true;    
}

//inits a bundle with default values. Will try all possible TCTIs as defined in the TSS spec
void Tpm2ContextBundle::InitDefaultEsyssContexts(ContextBundle& context){
    ESYS_CONTEXT* newEsysContext = nullptr;
    TSS2_TCTI_CONTEXT* newTctiContext = nullptr;

    TSS2_RC rc = Esys_Initialize(&newEsysContext, newTctiContext, NULL);
    if (rc != TPM2_RC_SUCCESS) {
        	if(newEsysContext){
                Esys_Free(newEsysContext);
            }
            if(newTctiContext){
                Esys_Free(newTctiContext);
            }
        throw std::runtime_error("could not initalize context because of " + tss2_logging::decodeTss2Rc(rc) +
                                 "\n Hint: Is the TPM-driver loaded or the swtpm started?");
    }
    context.esysContext.reset(newEsysContext);
    context.tctiContext.reset(newTctiContext);
}

void Tpm2ContextBundle::InitDefaultSysContext(ContextBundle& context){
    TSS2_SYS_CONTEXT* newSysContext = nullptr;
    
    TSS2_RC rc = Esys_GetSysContext(context.esysContext.get(), &newSysContext);
    if (rc != TPM2_RC_SUCCESS) {
        if(newSysContext){
            Esys_Free(newSysContext);
        }
        throw std::runtime_error("could not get sysContext because " + tss2_logging::decodeTss2Rc(rc));
    }
    context.sysContext.reset(newSysContext);
};


//send startup command with connection handles
//returns if action was successful
void Tpm2ContextBundle::StartupTpm2(bool performReset){
    //default: perform TPM Reset or TPM Restart
    TPM2_SU arg = TPM2_SU_CLEAR;
    
    if(!performReset){
        //restore Session
        arg = TPM2_SU_STATE;
    }
    
    TSS2_RC rc = Esys_Startup(GetEsysContext(), arg);
    if (rc != TPM2_RC_SUCCESS) {
        throw std::runtime_error("could not perform startup! " + tss2_logging::decodeTss2Rc(rc));
    }
}

//send shutdown command with unauthenicated handles
//returns if action was successful
void Tpm2ContextBundle::ShutdownTpm2(){
    TSS2_RC rc = Esys_Shutdown(GetEsysContext(), ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, TPM2_SU_CLEAR);
    if (rc != TPM2_RC_SUCCESS) {
        throw std::runtime_error("could not perform shutdown! " + tss2_logging::decodeTss2Rc(rc));
    }
}
