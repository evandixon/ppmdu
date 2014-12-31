#ifndef UTILITY_HPP
#define UTILITY_HPP
/*
utility.hpp
2014/12/21
psycommando@gmail.com
Description: A header with a bunch of useful includes for the PPMD utilities. 
*/
#include <chrono>
#include <string>
#include "gfileutils.hpp"
#include "gfileio.hpp"
#include "gstringutils.hpp"
#include "gbyteutils.hpp"
#include "poco_wrapper.hpp"
#include <iosfwd>

namespace utils
{
//===============================================================================================
// Struct
//===============================================================================================

    /************************************************************************
        data_array_struct
            A base structure for structures to be read from files, such as 
            headers, and any constant size blocks of data !
    ************************************************************************/
    struct data_array_struct
    {
        virtual ~data_array_struct(){}
        virtual unsigned int size()const=0;

        //The method expects that the interators can be incremented at least as many times as "size()" returns !
        virtual std::vector<uint8_t>::iterator       WriteToContainer(  std::vector<uint8_t>::iterator       itwriteto )const = 0;

        //The method expects that the interators can be incremented at least as many times as "size()" returns !
        virtual std::vector<uint8_t>::const_iterator ReadFromContainer( std::vector<uint8_t>::const_iterator itReadfrom )     = 0;
    };

    /************************************************************************
        Resolution
            A struct with human readable naming to make handling resolution
            values easier.
    ************************************************************************/
    struct Resolution
    {
        uint32_t width, height;

        inline bool operator==( const Resolution & right )const
        {
            return (this->height == right.height) && (this->width == right.width);
        }
    };

    /************************************************************************
        MrChronometer
            A small utility RAII class that that tells the time elapsed 
            between its construction and destruction.
    ************************************************************************/
    struct MrChronometer
    {

        MrChronometer( const std::string name = "*", std::ostream * messageoutput = nullptr );
        ~MrChronometer();

        std::chrono::steady_clock::time_point  _start;
        std::string                            _name;
        std::ostream                         * _output;
    };

//===============================================================================================
// Classes
//===============================================================================================


//===============================================================================================
// Function
//===============================================================================================

    /************************************************************************
        SimpleHandleException
            A function to avoid having to rewrite a thousand time the same 2 
            lines of code in case of exception.

            Simply write the exception's "what()" function output to "cerr", 
            and triggers an assert in debug.
    ************************************************************************/
    void SimpleHandleException( std::exception & e );

};

#endif 