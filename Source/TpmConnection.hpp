#pragma once
#include <exception>
#include "TssWrappers.hpp"
#include "Tpm2ContextBundle.hpp"

//Represents the programs connection to the TPM2.
//This is assumed to be the only one in total 
class TpmConnection{
    public:
        TpmConnection();
        ~TpmConnection();
private:
    void StartupTpm2(bool performReset);
    void ShutdownTpm2(); 
    
    private:
    Tpm2ContextBundle contexts;
};

/*
 * Next functions:
 * -figure out logging (again...)
 * -Make getrandom nice
 * -Make getCap class
 *  -> get fixed-caps: general info (maybe)
 *  -> get variable-caps: check counts and auth
 * -Clear and provision (very basic)
 *
 */
