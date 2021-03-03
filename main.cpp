#include <tss2/tss2_fapi.h>
#include <tss2/tss2_esys.h>
#include <string>
#include <sstream>
#include <iostream>
#include <optional>
#include <cstring>
#include <algorithm>
//you need tss2-tpm-dev >=3.0.3 
//TPM2_GetCapability


/*

----------------------------------------------------------------------
Libraries have been installed in:
   /usr/local/lib

If you ever happen to want to link against installed libraries
in a given directory, LIBDIR, you must either use libtool, and
specify the full pathname of the library, or use the '-LLIBDIR'
flag during linking and do at least one of the following:
   - add LIBDIR to the 'LD_LIBRARY_PATH' environment variable
     during execution
   - add LIBDIR to the 'LD_RUN_PATH' environment variable
     during linking
   - use the '-Wl,-rpath -Wl,LIBDIR' linker flag
   - have your system administrator add LIBDIR to '/etc/ld.so.conf'

See any operating system documentation about shared libraries for
more information, such as the ld(1) and ld.so(8) manual pages.
----------------------------------------------------------------------
g++ main.cpp -ltss2-fapi -ltss2-esys -lcrypto -ldl -lpthread -ljson-c -o tpm2_pascal
g++ main.cpp -ltss2-fapi -lcrypto -ldl -lpthread -ljson-c --static


# Multiarch support
/usr/local/lib/x86_64-linux-gnu
/lib/x86_64-linux-gnu
/usr/lib/x86_64-linux-gnu

*/


static char *
get_uint32_as_chars(UINT32 value) {
    static char buf[5];

    value = std::stoi(std::to_string(value),0,10);
    UINT8 *bytes = (UINT8 *) &value;

    /*
     * move the start of the string to the beginning
     * first non space character
     * Record the number of skips in i.
     */
    unsigned i;
    for (i = 0; i < sizeof(value); i++) {
        UINT8 b = *bytes;
        if (!isspace(b)) {
            break;
        }
        bytes++;
    }

    /* record the number of trailing spaces in j */
    unsigned j;
    for (j = sizeof(value) - i; j > i; j--) {
        UINT8 b = bytes[j - 1];
        /* NULL bytes count as space */
        if (b && !isspace(b)) {
            break;
        }
    }

    memcpy(buf, bytes, j);
    buf[j] = '\0';
    return buf;
}



static std::string convertHexToString(const std::string& hex){
	int len = hex.length();
	std::string newString;
	for(int i=0; i< len; i+=2)
	{
		//std::string byte = hex.substr(i,2);
		//std::cout << byte << "\n" << std::endl;
		//char chr = static_cast<char>(std::stol(byte, 0, 10));
		//newString.push_back(chr);
		std::istringstream iss(hex.substr(i, 2));
    	int temp;
    	iss >> std::hex >> temp;
    	newString += static_cast<char>(temp);
		//newString += std::to_string(std::stol(byte, 0, 10));
	}
	return newString;
}

static std::string toHexString(BYTE* bytes, size_t count) {  
    std::istringstream iss;
	std::cout << count;
    for (size_t i=0; i< count; i++) {
		std::cout << std::hex << std::to_string(bytes[i]) << "\n";
        //std::istringstream iss() << std::hex << bytes[i];
    }
    return iss.str();
}

int main(){
/*
class TpmConnection{

std::optional<ESYS_CONTEXT*> initEsysContext(){

}

private:
ESYS_CONTEXT* esysContext = nullptr;
TSS2_TCTI_CONTEXT* tctiContext = nullptr;
TSS2_SYS_CONTEXT* sysContext = nullptr;

};
*/

TSS2_RC rc = 0;

//FAPI_CONTEXT* fapi_context;
size_t numBytes = 5;
//uint8_t* randomBytes = nullptr;

ESYS_CONTEXT* esysContext = nullptr;
TSS2_TCTI_CONTEXT* tctiContext = nullptr;
TSS2_SYS_CONTEXT* sysContext = nullptr;

/*Note: 
 If not specified, load a TCTI in this order: 
 Library libtss2-tcti-default.so (link to the preferred TCTI)
 Library libtss2-tcti-tabrmd.so (tabrmd)
 Device /dev/tpmrm0 (kernel resident resource manager) 
 Device /dev/tpm0 (hardware TPM) 
 TCP socket localhost:2321 (TPM simulator) 
*/

rc = Esys_Initialize(&esysContext, tctiContext, NULL);
if (rc != TPM2_RC_SUCCESS) {
	std::cout << "could not initalize! " << rc << std::endl;
	if(esysContext){
		Esys_Free(esysContext);
	}
	if(tctiContext){
		Esys_Free(tctiContext);
	}
	if(sysContext){
		Esys_Free(sysContext);
	}
		return -2;
}

rc = Esys_GetSysContext(esysContext, &sysContext);
if (rc != TPM2_RC_SUCCESS) {
	std::cout << "could not get context! " << rc <<std::endl;
	if(esysContext){
		Esys_Free(esysContext);
	}
	if(tctiContext){
		Esys_Free(tctiContext);
	}
	if(sysContext){
		Esys_Free(sysContext);
	}
		return -1;
}

rc = Esys_Startup(esysContext, TPM2_SU_CLEAR);
if (rc != TPM2_RC_SUCCESS) {
	std::cout << "could not startup! " << rc <<std::endl;

	if(esysContext){
		Esys_Free(esysContext);
	}
	if(tctiContext){
		Esys_Free(tctiContext);
	}
	if(sysContext){
		Esys_Free(sysContext);
	}
	return -1;
}

TPMS_CAPABILITY_DATA* capabilities = nullptr;
TPMI_YES_NO moreData;
rc = Esys_GetCapability(esysContext, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, TPM2_CAP_TPM_PROPERTIES, TPM2_PT_FIXED, TPM2_MAX_TPM_PROPERTIES, &moreData, &capabilities);
if (rc != TPM2_RC_SUCCESS && rc != TPM2_RC_RETRY) {
	std::cout << "could not GetCapability! " << rc <<std::endl;

	if(esysContext){
		Esys_Free(esysContext);
	}
	if(tctiContext){
		Esys_Free(tctiContext);
	}
	if(sysContext){
		Esys_Free(sysContext);
	}
	if(capabilities){
		Esys_Free(capabilities);
	}
	return -3;
}
if(moreData){
	std::cout << "the fuck ist moreData???" << std::endl;
	if(rc == TPM2_RC_RETRY){
		std::cout << "offenbar waren bei getCapability noch daten" << std::endl;		
	}
}
/*
TPM2_PT_MANUFACTURER:
  raw: 0x49424D20
  value: "IBM "
TPM2_PT_VENDOR_STRING_1:
  raw: 0x53572020
  value: "SW"
TPM2_PT_VENDOR_STRING_2:
  raw: 0x2054504D
  value: "TPM"
TPM2_PT_VENDOR_STRING_3:
  raw: 0x0
  value: ""
TPM2_PT_VENDOR_STRING_4:
  raw: 0x0
  value: ""
*/

size_t propertyCount= capabilities->data.tpmProperties.count;
TPMS_TAGGED_PROPERTY* propertiesInternal = capabilities->data.tpmProperties.tpmProperty;

std::string tpmDataString;
//size_t propertyCount = capabilities->data.tpmProperties.count;
for(int i=0; i < propertyCount; i++){
	TPM2_PT property = propertiesInternal[i].property;
    UINT32 value = propertiesInternal[i].value;
	std::string temp;
	switch(property){
		case TPM2_PT_DAY_OF_YEAR:
			tpmDataString += "Day of year for tpm: " + std::to_string(std::stoi(std::to_string(value),0,10)) + "\n";
		break;
		case TPM2_PT_YEAR:
			tpmDataString += "Year for tpm: " + std::to_string(std::stoi(std::to_string(value),0,10)) + "\n";
		break;
		case TPM2_PT_VENDOR_STRING_1:
			temp = std::string(get_uint32_as_chars(value));
			std::reverse(temp.begin(),temp.end());
			tpmDataString += "VendorString1: " + temp + "\n" + std::to_string(value) + "\n";
		break;
		case TPM2_PT_VENDOR_STRING_2:
			temp = std::string(get_uint32_as_chars(value));
			std::reverse(temp.begin(),temp.end());
			tpmDataString += "VendorString2: " + temp + "\n" + std::to_string(value) + "\n";
		break;
		case TPM2_PT_VENDOR_STRING_3:
			temp = std::string(get_uint32_as_chars(value));
			std::reverse(temp.begin(),temp.end());
			tpmDataString += "VendorString3: " + temp + "\n" + std::to_string(value) + "\n";
		break;
		case TPM2_PT_VENDOR_STRING_4:
			temp = std::string(get_uint32_as_chars(value));
			std::reverse(temp.begin(),temp.end());
			tpmDataString += "VendorString4: " + temp + "\n" + std::to_string(value) + "\n";
		break;
		case TPM2_PT_MANUFACTURER:
			temp = std::string(get_uint32_as_chars(value));
			std::reverse(temp.begin(),temp.end());
			tpmDataString += "TPM_Manifactorer: " + temp + "\n" + std::to_string(value) + "\n";
		break;
		default:
		//ignore
		break;
	}
}

std::cout << "Results of TPM-Check:" << std::endl;
std::cout << tpmDataString << std::endl;

TPM2B_DIGEST* randomBytes = nullptr;
size_t randomByteCount = 1;
std::string randomString;
rc = Esys_GetRandom(esysContext, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, randomByteCount, &randomBytes);
if(rc != TPM2_RC_SUCCESS)
{
	std::cout << "error while getRandom: " << rc << std::endl;
}
else if(randomBytes != nullptr){
	if(randomByteCount =! randomBytes->size){
		std::cout << "NOTE: wanted " << randomByteCount << " got " << randomBytes->size << " randombytes\n";
	}
	std::cout << "Randombytes:" << toHexString(randomBytes->buffer, randomBytes->size) << std::endl;
}

//done here
if(esysContext){
	Esys_Free(esysContext);
}
if(tctiContext){
	Esys_Free(tctiContext);
}
if(sysContext){
	Esys_Free(sysContext);
}
if(capabilities){
	Esys_Free(capabilities);
}
if(randomBytes){
	Esys_Free(randomBytes);
}

return 0;
}


//rc = Tss2_Sys_Startup(sysContext, TPM2_SU_CLEAR);
//if (rc != TPM2_RC_SUCCESS) {
//	std::cout << "could not startup! " << rc <<std::endl;
//	return -1;
//}

//rc = Fapi_Initialize(&fapi_context, nullptr);
//if(rc != TSS2_RC_SUCCESS){
//	return -1;
//}

//provisions the fapi-stack. only needed the very first time.
//rc = Fapi_Provision(fapi_context, nullptr, nullptr, nullptr);
//if(rc != TSS2_RC_SUCCESS){
//	return -1;
//}

/*
rc = Fapi_GetRandom(fapi_context, numBytes, &randomBytes);
if(rc != TSS2_RC_SUCCESS){
	Fapi_Free(randomBytes);
	Fapi_Free(fapi_context);
	return -2;
}else{
	unsigned int result = static_cast<unsigned int>(*randomBytes);
	std::cout << "Random result: " << result << std::endl;
	Fapi_Free(randomBytes);
	Fapi_Free(fapi_context);
	return 0;
	}
}
*/
