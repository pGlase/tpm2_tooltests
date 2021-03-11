#include "Tpm2Random.hpp"

std::vector<BYTE> Tpm2Random::GetRandomBytes(Tpm2ContextBundle& contexts, size_t byteCount){
    if(contexts.GetEsysContext() == nullptr){
        throw std::invalid_argument("GetRandomBytes: context not initalized");
    }
    
    if(byteCount == 0){
        return std::vector<BYTE>();
    }
    
    if(byteCount > std::numeric_limits<UINT16>::max()){
        throw std::invalid_argument("GetRandomBytes: Cannot query more than " + std::to_string(std::numeric_limits<UINT16>::max()) + " byte at once");
    }
    
    TPM2B_DIGEST* randomBytes = nullptr;
    TPM2_RC rc = Esys_GetRandom(contexts.GetEsysContext(), ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, byteCount, &randomBytes);
    if(rc != TPM2_RC_SUCCESS)
    {
        throw std::runtime_error("GetRandomBytes: Error while generating bytes: " + tss2_logging::decodeTss2Rc(rc));
    }
    
    if(randomBytes == nullptr){
        throw std::runtime_error("GetRandomBytes: Error while generating bytes: No bytes were returned");
    }
    
    //wrap into smartpointer to prevent leaks on further errors
    tss2_typedefs::Digest_PTR digest(randomBytes);
    if(static_cast<UINT16>(byteCount) < (digest->size) ){
        throw std::runtime_error("GetRandomBytes: Requested " + std::to_string(byteCount) + " but TPM returned only " + std::to_string(randomBytes->size));
    }
    
    std::vector<BYTE> ret;
    ret.reserve(byteCount);
    ret.insert(ret.end(), &digest->buffer[0], &digest->buffer[byteCount]);
    return ret;
}
