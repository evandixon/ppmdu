#ifndef BASETYPES_H
#define BASETYPES_H
/*
basetypes.h
2014/09/14
psycommando@gmail.com
Description: Handle the definitions for basic types, and attempt to ensure portability.


##TODO: Is this really useful ?

*/
#include <vector>
#include <array>
#include <string>
#include <exception>
#include <cstdint>  //<- If this include fails, your compiler doesn't support C++ 11. If some typedefs are wrong, 
                    //   your platform or compiler doesn't support those types.
                    //   This code was made to support C++ 11, on MSVC and GNU GCC compilers, used on x86-x64 PCs.

//=========================================================
//Some commonly re-used types
//=========================================================

namespace pmd2 { namespace types 
{
//=========================================================
// Typedefs
//=========================================================
    //Iterators
    typedef std::vector<uint8_t>::iterator       itbyte_t;
    typedef std::vector<uint8_t>::const_iterator constitbyte_t;

    //Vectors
    typedef std::vector<uint8_t> bytevec_t;      //Alias for vector<uint8_t>
    typedef bytevec_t::size_type bytevec_szty_t; //size_type for vector<uint8_t>

//=========================================================
// Exceptions
//=========================================================
    /*
        Ex_StringException
            Base exception type for this library!
    */
    class Ex_StringException : public std::exception
    { 
    public: 
        Ex_StringException( const char * text = "Unknown Exception" )
            :m_what(text)
        {
        }


        virtual const char * what()const throw()
        {
            return m_what.c_str();
        }

    protected:
        std::string m_what;
    };

};};

#endif