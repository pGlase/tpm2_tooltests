#include "TpmConnection.hpp"


TpmConnection::TpmConnection():
    contexts(Tpm2ContextBundle::InitDefaultUnauthContexts()){
    StartupTpm2(true);
};

TpmConnection::~TpmConnection(){
    std::cout << "trying to shutdown tpm\n";
    ShutdownTpm2();
}

//send startup command with connection handles
//returns if action was successful
bool TpmConnection::StartupTpm2(bool performReset){
    //default: perform TPM Reset or TPM Restart
    TPM2_SU arg = TPM2_SU_CLEAR;
    
    if(!performReset){
        //restore Session
        arg = TPM2_SU_STATE;
    }
    
    TSS2_RC rc = Esys_Startup(contexts.GetEsysContext(), arg);
    if (rc != TPM2_RC_SUCCESS) {
        std::cout << "could not perform startup! " << tss2_logging::decodeTss2Rc(rc) <<std::endl;
        return false;
    }else{
        return true;
    }
}

//send shutdown command with unauthenicated handles
//returns if action was successful
bool TpmConnection::ShutdownTpm2(){
    TSS2_RC rc = Esys_Shutdown(contexts.GetEsysContext(), ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, TPM2_SU_CLEAR);
    if (rc != TPM2_RC_SUCCESS) {
        std::cout << "could not perform shutdown! " << tss2_logging::decodeTss2Rc(rc) <<std::endl;
        return false;
    }else{
        return true;
    }
}
