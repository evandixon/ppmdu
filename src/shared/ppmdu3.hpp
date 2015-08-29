#ifndef PPMDU3_HPP
#define PPMDU3_HPP
/*
ppmdu3.hpp
2015/08/29
psycommando@gmail.com
Description: Interface for DLL/SO made with the library.
*/


//Use extern C to ensure our function names aren't mangled
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

//Macro to expose our interface
#if defined(WIN32) || defined(WIN64)

    //Windows systems
    #if defined(PPMDU3_LIB_SHARED)
        #define PPMDU_API __declspec(dllexport) //Exporting DLL
    #elif !defined(PPMDU3_LIB_STATIC)
        #define PPMDU_API __declspec(dllimport) //Importing DLL
    #else
        #define PPMDU_API                       //Static library
    #endif

#else

    //Non-windows systems
    #if __GNUC__ >= 4
        #define PPMDU_API __attribute__((visibility("default")))
    #else
        #define PPMDU_API
    #endif

#endif

namespace ppmdu3
{

//============================================================================================
//  IPPMDU3 Interface
//============================================================================================

    namespace gametext
    {
        typedef unsigned int ppmdu3_gstrplhndl; //Game Strings Pool Handle
        typedef uint32_t     ppmdu3_strhash;    //unique Hash for strings in the PMD3 games.
        typedef size_t       ppmdu3_index;      //Represent an index in a container within the DLL's context

        //------------------ Alloc ------------------
        /*
            ppmdu3_CreateGameStringsPool
                Instantiate a GameStringsPool object.

                GameStringsPool objects are automatically destroyed when the library is unloaded, 
                but you can control its lifetime more accurately by usingg the 
                ppmdu3_ReleaseGameStringsPool function to delete the object.
        */
        PPMDU_API int ppmdu3_CreateGameStringsPool( ppmdu3_gstrplhndl & out_hndl );

        /*
            ppmdu3_ReleaseGameStringsPool
                Delete a GameStringsPool object.
        */
        PPMDU_API int ppmdu3_ReleaseGameStringsPool( ppmdu3_gstrplhndl hndl );

        //------------------ IO ------------------
        /*
            ppmdu3_LoadGameStrings
                If returns 0, success, otherwise fail..

                - hndl : The GameStringsPool object to use.
                - path : Path to the "Message" directory where all game strings are stored.
        */
        PPMDU_API int ppmdu3_LoadGameStrings( ppmdu3_gstrplhndl hndl, const char * path, size_t strl );

        /*
            ppmdu3_WriteGameStrings
                If returns 0, success, otherwise fail..

                - hndl : The GameStringsPool object to use.
                - path : Path to the "Message" directory where all game strings are stored.
        */
        PPMDU_API int ppmdu3_WriteGameStrings( ppmdu3_gstrplhndl hndl, const char * path, size_t strl );


        //------------------ Access ------------------
        /*
            ppmdu3_GetStringByHash
                Returns a loaded string from the specified GameStringsPool object.

                - hndl : The GamesStringsPool object to load from.
                - strh : The unique Hash of the string.
        */
        PPMDU_API int ppmdu3_GetStringByHash( ppmdu3_gstrplhndl hndl, ppmdu3_strhash strh, wchar_t * out_str, size_t strmaxlen );

        /*
            ppmdu3_GetStringByCategoryAndIndex
                Returns a loaded string from the specified GameStringsPool object, 
                from the specified category(name of the file it was loaded from), and at the given index within said category!

                - hndl     : The handle to the GameStringsPool.
                - catindex : The index of the category to look into.
                - strindex : The index of the string within the category.
        */
        PPMDU_API int ppmdu3_GetStringByCategoryAndIndex( ppmdu3_gstrplhndl hndl, ppmdu3_index catindex, ppmdu3_index strindex, wchar_t * out_str, size_t strmaxlen );

        /*
        */
        PPMDU_API int GetCategoryName( ppmdu3_gstrplhndl hndl, ppmdu3_index catindex, char * out_str, size_t strmaxlen );

        /*
        */
        PPMDU_API int FindCategoryByName( ppmdu3_gstrplhndl hndl, const char * in_str, size_t strl, ppmdu3_index & out_catindex );

        /*
        */
        PPMDU_API size_t GetNbCategoriesLoaded( ppmdu3_gstrplhndl hndl );

        /*
        */
        PPMDU_API size_t GetNbStringsLoaded( ppmdu3_gstrplhndl hndl, ppmdu3_index catindex );


        //------------------ Modification ------------------


        PPMDU_API int ppmdu3_SetStringByHash( ppmdu3_gstrplhndl hndl, ppmdu3_strhash strh, const wchar_t * in_str, size_t strl );

        PPMDU_API int ppmdu3_SetStringByIndex( ppmdu3_gstrplhndl hndl,  ppmdu3_index catindex,  ppmdu3_index strindex, const wchar_t * in_str, size_t strl );

        /*
            ppmdu3_AddString
                Returns hash of the new string, or 0 if it fails.
        */
        PPMDU_API ppmdu3_strhash ppmdu3_AddString( ppmdu3_gstrplhndl hndl, ppmdu3_index catindex,  const wchar_t * in_str, size_t strl );

        /*
            ppmdu3_RemString
                Returns 0 if it fails.
        */
        PPMDU_API int ppmdu3_RemString( ppmdu3_gstrplhndl hndl, ppmdu3_strhash strh );

        /*
            ppmdu3_AddString
                Returns index of the new category + 1, or 0 if it fails.
        */
        PPMDU_API size_t ppmdu3_AddCategory( ppmdu3_gstrplhndl hndl,  const wchar_t * in_str, size_t strl );

        /*
            ppmdu3_RemString
                Returns hash of the new string, or 0 if it fails.
        */
        PPMDU_API int ppmdu3_RemCategory( ppmdu3_gstrplhndl hndl, ppmdu3_index catindex );
    };


    /*******************************************************************
        IGameStringsUtil
    *******************************************************************/
    //class PPMDU_API IGameStringsUtil
    //{
    //public:
    //};

    //
    ///*******************************************************************
    //    IPPMDU3
    //        Interface to access general functionalities of the 
    //        PPMDU3 library.
    //*******************************************************************/
    //class PPMDU_API IPPMDU3
    //{
    //public:
    //    //Call this to deinit
    //    virtual void release() = 0;

    //    //Obtain a pointer to the game string utility interface
    //    virtual IGameStringsUtil * GameStringUtil() = 0; 

    //};

//============================================================================================
//  IPPMDU3 Access
//============================================================================================

};
#ifdef __cplusplus
};
#endif

#endif