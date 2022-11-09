#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "..\lib\flog.h"

// DEFINE NDEBUG TO DISABLE LOGS
//Constants to specify NEW_STRUCT element type and element output mode

#define ELEM_TYPE int ///< Type of element a NEW_STRUCT operates with
#define OUT_FORM "%d" ///< Output format for printf functions

//end of block

//inside defines

#define CANL 0xDEADBEEF ///< Left cannary of a structure
#define CANR 0xD34DB33F ///< Right cannary of a structure

#define POISON1 0xBD ///< 1 byte Poison
#define POISON2 0xBDCF ///< 2 byte Poison
#define POISON4 0xBADC0FEE ///< 4 byte Poison
#define POISON8 0xBADC0FEEF04DED32 ///< 8 byte Poison

#define MULT 37u ///< Multiplier for hash

#define setPoison(var) setPoisonInside (&var, sizeof (var)) ///< Macros for auto-poison

#define isPoison(var) isPoisonInside (&var, sizeof (var)) ///< Macros for checking for poison

#ifndef NDEBUG
#define NEW_STRUCTDump(strct) NEW_STRUCTDumpInside (&strct, #strct, __FILE__, __FUNCTION__, __LINE__) ///< NEW_STRUCT dump macros
#else
#define NEW_STRUCTDump(strct) ;
#endif
//endof inside defines

/// @brief Describes errors within NEW_STRUCT
enum NEW_STRUCTErrorCodes {
    ok                   = 0,    ///< All ok
    POISON_ACCESS        = 1<<0, ///< One or more struct elements is poison
    BAD_CAN_L            = 1<<1, ///< Dead left cannary of structure
    BAD_CAN_R            = 1<<2, ///< Dead right cannary of structure
    BAD_DATA_CAN_L       = 1<<3, ///< Dead left cannary of  DATA
    BAD_DATA_CAN_R       = 1<<4, ///< Dead right cannary of DATA
    NULL_DATA_PTR        = 1<<5, ///< NULL ptr for DATA
    NULL_DATA_CAN_L_PTR  = 1<<6, ///< NULL ptr for left  DATA cannary
    NULL_DATA_CAN_R_PTR  = 1<<7, ///< NULL ptr for right DATA cannary
    POISONED_ERRCOD      = 1<<9, ///< Errcod variable is poisoned; Ususally means that struct has been destructed
    WRONG_HASH           = 1<<10 ///< Hash was changed without any changes from specified function
};

/// @brief Stores a NEW_STRUCT with several support variables
struct NEW_STRUCT {

    unsigned int  canL      = 0xDEADBEEF; ///< left cannary of struct
    unsigned int  hash      = 0;          ///< hash value
    size_t        errCode   = ok;         ///< error code
    void*         DATA      = NULL;       ///< Ptr to DATA
    unsigned int* DATACanL  = NULL;       ///< left cannary of DATA
    unsigned int* DATACanR  = NULL;       ///< right cannary of DATA
    unsigned int  canR      = 0xD34DB33F; ///< right cannary of struct
};

/// @brief Constructs a NEW_STRUCT var
/// @param strct ptr to a var to construct
void NEW_STRUCTCtor (NEW_STRUCT* strct);

/// @brief Destroys a NEW_STRUCT var.
/// @param strct ptr to a destroyable variable
void NEW_STRUCTDtor (NEW_STRUCT* strct);

/// @brief Dumps NEW_STRUCT to logs_out
/// @param strct ptr to NEW_STRUCT
/// @param NEW_STRUCTName name of NEW_STRUCT
/// @param fileName name of file function was called at
/// @param funcName name of func function was called at
/// @param line line function was called at
void NEW_STRUCTDumpInside (NEW_STRUCT* strct, const char* NEW_STRUCTName, const char* fileName, const char* funcName, size_t line);

/// @brief Prints errors drom a NEW_STRUCT variable
/// @param strct ptr to NEW_STRUCT
/// @return error code strct->errCode
unsigned long long NEW_STRUCTErrCheck (NEW_STRUCT* strct);

/// @brief Prints errors for a NEW_STRUCTDump func
/// @param strct ptr to NEW_STRUCT
void NEW_STRUCTLogPrintErrors (NEW_STRUCT* strct);

/// @brief Counts hash for NEW_STRUCT
/// @param strct ptr to NEW_STRUCT
void NEW_STRUCTCountHash (NEW_STRUCT* strct);

/// @brief Verifies hash for NEW_STRUCT (tells if it was changed without any NEW_STRUCT specified functions)
/// @param strct ptr to NEW_STRUCT
void NEW_STRUCTVerifyHash (NEW_STRUCT* strct);

/// @brief Sets var to a poisoned state
/// @param var ptr to var
/// @param sizeofVar sizeof var
void setPoisonInside (void* var, size_t sizeofVar);

/// @brief Checks for a variable to be poisoned
/// @param var ptr to var
/// @param sizeofVar size of var
/// @return 1 if poisoned, 0 if not
bool isPoisonInside (void* var, size_t sizeofVar);