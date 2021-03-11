#include "Tpm2Capabilites.hpp"

tss2_typedefs::Capability_PTR Tpm2Capabilites::InternalGetCapNoAuth(
    Tpm2ContextBundle& contexts, TPM2_CAP capabilityConst, UINT32 propertyConst, UINT32 propertyCount){
    if(contexts.GetEsysContext() == nullptr){
        throw std::invalid_argument("InternalGetCapNoAuth: esysContext not initalized");
    }
    if(propertyCount > TPM2_MAX_TPM_PROPERTIES){
        throw std::invalid_argument("InternalGetCapNoAuth: propertyCount larger than spec allowed");
    }
    
    tss2_typedefs::Capability_PTR result;
    TPMS_CAPABILITY_DATA* capabilities = nullptr;
    TPMI_YES_NO* moreData = nullptr;
    TPM2_RC rc  = Esys_GetCapability(contexts.GetEsysContext(), ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, 
                                     capabilityConst, propertyConst, propertyCount, moreData, &capabilities);
    if(rc != TPM2_RC_SUCCESS) {
        spdlog::error("InternalGetCapNoAuth: Could not get capabilities! Reason: {0}", tss2_logging::decodeTss2Rc(rc));
    }
    
    if(moreData){
        spdlog::warn("note: select larger propertyCount for InternalGetCapNoAuth, more data was avaliable");
        Esys_Free(moreData);
    }
    result.reset(capabilities);
    return result;
}

UINT32 Tpm2Capabilites::GetRawCapabilities(Tpm2ContextBundle& contexts, TPM2_CAP capabilityConst, UINT32 propertyConst){
    if(contexts.GetEsysContext() == nullptr){
        throw std::invalid_argument("GetRawCapabilities: context not initalized");
    }
    auto capabilities = InternalGetCapNoAuth(contexts, capabilityConst, propertyConst);
    
    size_t recievedPropertyCount = capabilities->data.tpmProperties.count;
    TPMS_TAGGED_PROPERTY* propertiesInternal = capabilities->data.tpmProperties.tpmProperty;
    for(size_t i=0; i < recievedPropertyCount; i++){
        if(propertiesInternal[i].property == propertyConst){
           return propertiesInternal[i].value;
        }
    }
    //value not found, programming error
    throw std::invalid_argument("GetRawCapabilities:propertyConst " + std::to_string(propertyConst) + "not found");
}

bool Tpm2Capabilites::IsInLockout(Tpm2ContextBundle& contexts){
    auto holder = GetRawCapabilities(contexts, TPM2_CAP_TPM_PROPERTIES, TPM2_PT_PERMANENT);
    return tss2_valueConverters::EvalBitfieldElement(holder, 0x00000f00);
}

bool Tpm2Capabilites::IsOwnerAuthSet(Tpm2ContextBundle& contexts){
    auto holder = GetRawCapabilities(contexts, TPM2_CAP_TPM_PROPERTIES, TPM2_PT_PERMANENT);
    return tss2_valueConverters::EvalBitfieldElement(holder, 0xf0000000);
}
bool Tpm2Capabilites::IsEndorsementAuthSet(Tpm2ContextBundle& contexts){
    auto holder = GetRawCapabilities(contexts, TPM2_CAP_TPM_PROPERTIES, TPM2_PT_PERMANENT);
    return tss2_valueConverters::EvalBitfieldElement(holder, 0x0f000000);
}
bool Tpm2Capabilites::IsLockoutAuthSet(Tpm2ContextBundle& contexts){
    auto holder = GetRawCapabilities(contexts, TPM2_CAP_TPM_PROPERTIES, TPM2_PT_PERMANENT);
    return tss2_valueConverters::EvalBitfieldElement(holder, 0x00f00000);
}

//querys the tpm for special mapped capabilities. Only for special Properties and logging, prefer GetRawCapabilities for flexiblity
Tpm2Capabilites::ParsedCapabilities Tpm2Capabilites::GetMappedCapabilities(Tpm2ContextBundle& contexts, CapacityCategory category){
    if(contexts.GetEsysContext() == nullptr){
        throw std::invalid_argument("GetCapabilities: context not initalized");
    }

    //select Auth-type depending on category
    TPM2_CAP capabilityConst;
    UINT32 propertyConst;
    CapabilityMap parseMap;
    switch(category){
        case AllFixed:
            capabilityConst = TPM2_CAP_TPM_PROPERTIES;
            propertyConst = TPM2_PT_FIXED;
            parseMap = {
                {TPM2_PT_DAY_OF_YEAR, UINT},
                {TPM2_PT_YEAR, UINT}, 
                {TPM2_PT_VENDOR_STRING_1, STRING}, 
                {TPM2_PT_VENDOR_STRING_2, STRING}, 
                {TPM2_PT_VENDOR_STRING_3, STRING}, 
                {TPM2_PT_VENDOR_STRING_4, STRING}, 
                {TPM2_PT_MANUFACTURER, STRING}, 
            };
            break;
        case AllVariable:
            capabilityConst = TPM2_CAP_TPM_PROPERTIES;
            propertyConst = TPM2_PT_VAR;
            parseMap = {
                {TPM2_PT_PERMANENT, UINT},
                {TPM2_PT_STARTUP_CLEAR, UINT},  
            };
            break;
    }
    
    auto ret = InternalGetCapNoAuth(contexts, capabilityConst, propertyConst);
    //select parse-type dependung on enum
    return ParseCapabilities(std::move(ret), parseMap);    
}

Tpm2Capabilites::ParsedCapabilities Tpm2Capabilites::ParseCapabilities(
    tss2_typedefs::Capability_PTR capabilities, const CapabilityMap& capsToParse){
    if(capabilities == nullptr){
        throw std::invalid_argument("ParseCapabilities: capabilities not initalized");
    }
    if(capsToParse.empty()){
        spdlog::warn("Note: ParsedCapabilities got empty parse instructions");
        return ParsedCapabilities();
    }
    
    ParsedCapabilities result;
    size_t parsedPropertyCount = 0;
    size_t recievedPropertyCount = capabilities->data.tpmProperties.count;
    TPMS_TAGGED_PROPERTY* propertiesInternal = capabilities->data.tpmProperties.tpmProperty;
    
    for(size_t i=0; i < recievedPropertyCount; i++){
        TPM2_PT property = propertiesInternal[i].property;
        UINT32 value     = propertiesInternal[i].value;
        
        //subcategory not requested by caller, skip it
        if(capsToParse.count(property) == 0){
            continue;
        }
        
        //requested subcategory found
        parsedPropertyCount++;
        switch(capsToParse.at(property)){
            case UINT:
                result[propertiesInternal[i].property] = tss2_valueConverters::ConvertUint32ToString(value);
                break;
            case HEX:
                result[propertiesInternal[i].property] = tss2_valueConverters::ConvertUint32ToString(value);
                break;
            case STRING:
                result[propertiesInternal[i].property] = tss2_valueConverters::InterpretUint32AsAsciiString(value);
                break;
            default:
                result[propertiesInternal[i].property] = tss2_valueConverters::InterpretUint32AsAsciiString(value);
                break;
        }
    }
    //sanity-check: were as many caps returned as expected?
    if(capsToParse.size() != parsedPropertyCount){
        throw std::invalid_argument(
        "ParseCapabilities: CapabilityMap required " + std::to_string(capsToParse.size()) +
        " caps, but recieved " + std::to_string(parsedPropertyCount)
        );
    }
    return result;
}
