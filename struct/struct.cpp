#include "struct.h"


void setPoisonInside (void* var, size_t sizeofVar) {

    if (var == NULL) return;

    switch(sizeofVar) {
        case 1 : *((unsigned char*     ) var)      = POISON1; break;
        case 2 : *((unsigned short*    ) var)      = POISON2; break;
        case 4 : *((unsigned int*      ) var)      = POISON4; break;
        case 8 : *((unsigned long long*) var)      = POISON8; break;

        default:
            memset (var, POISON1, sizeofVar);
        break;
    }
}

bool isPoisonInside (void* var, size_t sizeofVar) {

    if (var == NULL) return 1;

    switch(sizeofVar) {
        case 1:
            if ( *( (unsigned char*     ) var) == POISON1) return 1;
            else                                           return 0;
            break;
        case 2:
            if ( *( (unsigned short*    ) var) == POISON2) return 1;
            else                                           return 0;
            break;
        case 4:
            if ( *( (unsigned int*      ) var) == POISON4) return 1;
            else                                           return 0;
            break;
        case 8:
            if ( *( (unsigned long long*) var) == POISON8) return 1;
            else                                           return 0;
            break;
        default:
            for (int i = 0; i < sizeofVar; i++) {

                if (*( ( (unsigned char*) var) + i) != POISON1) return 0;
            }
            return 1;
            break;
    }
}

void NEW_STRUCTCtor (NEW_STRUCT* strct) {

    strct->canL      = CANL;
    strct->canR      = CANR;
    strct->hash      = 0;
    strct->errCode   = ok;

    //Allocate DATA and set DATA cannaries here

   *strct->DATACanL = CANL;
   *strct->DATACanR = CANR;

    NEW_STRUCTCountHash (strct);

}

void NEW_STRUCTDtor (NEW_STRUCT* strct) {

    if (strct == NULL) return;

    setPoison ( strct->canL      );
    setPoison ( strct->canR      );
    setPoison (*strct->DATACanL );
    setPoison (*strct->DATACanR );
    setPoison ( strct->DATACanL );
    setPoison ( strct->DATACanR );
    setPoison ( strct->errCode   );
    setPoison ( strct->hash      );

    //Poison DATA here

    free      ( strct->DATACanL );
    setPoison ( strct->DATA     );
}

void NEW_STRUCTDumpInside (NEW_STRUCT* strct, const char* NEW_STRUCTName, const char* fileName, const char* funcName, size_t line) {

    if (strct == NULL) return;


    NEW_STRUCTVerifyHash (strct);

    if (strct == NULL){
        flogprintf ( "In file %s, function %s, line %u, NEW_STRUCT named %s is a NULL ptr \n", fileName, funcName, line, NEW_STRUCTName);
        return;
    }

    flogprintf ( "In file %s, function %s, line %u, NEW_STRUCT named %s was dumped : \n", fileName, funcName, line, NEW_STRUCTName);

    flogprintf ( "\t" "Errors : \n");
    NEW_STRUCTLogPrintErrors (strct);

                                           flogprintf ( "\t" "hash = %u (", strct->hash);
    if      ( strct->hash      == POISON4) flogprintf ( "POISONED)\n")
    else                                   flogprintf ( "ok)\n")

                                           flogprintf ( "\t" "canL = 0x%X (", strct->canL);
    if      ( strct->canL      == POISON4) flogprintf ( "POISONED)\n")
    else if ( strct->canL      == CANL   ) flogprintf ( "ok)\n")
    else                                   flogprintf ( "NOT_OK)\n")

                                           flogprintf ( "\t" "canR = 0x%X (", strct->canR);
    if      ( strct->canR      == POISON4) flogprintf ( "POISONED)\n")
    else if ( strct->canR      == CANR   ) flogprintf ( "ok)\n")
    else                                   flogprintf ( "NOT_OK)\n")

                                          flogprintf ( "\t" "DATACanL = 0x%X (", *strct->DATACanL);
    if      (*strct->DATACanL == POISON4) flogprintf ( "POISONED)\n")
    else if (*strct->DATACanL == CANL   ) flogprintf ( "ok)\n")
    else                                  flogprintf ( "NOT_OK)\n")

                                          flogprintf ( "\t" "DATACanR = 0x%X (", *strct->DATACanR);
    if      (*strct->DATACanR == POISON4) flogprintf ( "POISONED)\n")
    else if (*strct->DATACanR == CANR   ) flogprintf ( "ok)\n")
    else                                  flogprintf ( "NOT_OK)\n")

    if ((size_t) strct->DATA  == POISON4) goto A;

    //Dump Data Here

A:
    flogprintf ( "\t" "End of DATA" "\n" "End of dump \n");

    NEW_STRUCTCountHash (strct);
}

unsigned long long NEW_STRUCTErrCheck (NEW_STRUCT* strct) {

    if (strct == NULL) return 0;

    //checking for poison
    if (isPoison ( strct->errCode  )) {
        strct->errCode = POISONED_ERRCOD;
        return strct->errCode;
    }
    if (isPoison ( strct->canL     ) or
        isPoison ( strct->canR     ) or
        isPoison ( strct->DATA    ) or
        isPoison ( strct->DATACanL) or
        isPoison (*strct->DATACanL) or
        isPoison ( strct->DATACanR) or
        isPoison (*strct->DATACanR) or
        isPoison ( strct->hash     )   ) strct->errCode |= POISON_ACCESS;

    //end of check

    if ( strct->canL      != CANL         ) strct->errCode |= BAD_CAN_L;
    if ( strct->canR      != CANR         ) strct->errCode |= BAD_CAN_R;
    if ( strct->DATACanL == NULL         ) strct->errCode |= NULL_DATA_CAN_L_PTR;
    if ( strct->DATACanR == NULL         ) strct->errCode |= NULL_DATA_CAN_R_PTR;
    if (*strct->DATACanL != CANL         ) strct->errCode |= BAD_DATA_CAN_L;
    if (*strct->DATACanR != CANR         ) strct->errCode |= BAD_DATA_CAN_R;
    if ( strct->DATA     == NULL         ) strct->errCode |= NULL_DATA_PTR;

    NEW_STRUCTCountHash (strct);

    return strct->errCode;
}

void NEW_STRUCTLogPrintErrors (NEW_STRUCT* strct) {

    if (strct == NULL) return;

    //NEW_STRUCTCountHash (strct);

    char names [11][40] = {};
    int iter = 0;
    if (strct->errCode & POISON_ACCESS        ) strcpy (names[iter++], "\t\t[POISON_ACCESS       ]\n");
    if (strct->errCode & BAD_CAN_L            ) strcpy (names[iter++], "\t\t[BAD_CAN_L           ]\n");
    if (strct->errCode & BAD_CAN_R            ) strcpy (names[iter++], "\t\t[BAD_CAN_R           ]\n");
    if (strct->errCode & BAD_DATA_CAN_L      ) strcpy (names[iter++],  "\t\t[BAD_DATA_CAN_L      ]\n");
    if (strct->errCode & BAD_DATA_CAN_R      ) strcpy (names[iter++],  "\t\t[BAD_DATA_CAN_R      ]\n");
    if (strct->errCode & NULL_DATA_PTR       ) strcpy (names[iter++],  "\t\t[NULL_DATA_PTR       ]\n");
    if (strct->errCode & NULL_DATA_CAN_L_PTR ) strcpy (names[iter++],  "\t\t[NULL_DATA_CAN_L_PTR ]\n");
    if (strct->errCode & NULL_DATA_CAN_R_PTR ) strcpy (names[iter++],  "\t\t[NULL_DATA_CAN_R_PTR ]\n");
    if (strct->errCode & POISONED_ERRCOD      ) strcpy (names[iter++], "\t\t[POISONED_ERRCOD     ]\n");
    if (strct->errCode & WRONG_HASH           ) strcpy (names[iter++], "\t\t[WRONG_HASH          ]\n");

    if (iter == 0) flogprintf ( "\t\t[ok]\n")
    else
        for (int i = 0; i < iter; i++) flogprintf ( "%s", names[i])

}

void NEW_STRUCTCountHash (NEW_STRUCT* strct) {

    if (strct == NULL) return;

    unsigned int newHash = 0, multiplier = 1;
    char* ptr = (char*) &strct->canL;

    for (int i = 0; i < sizeof (unsigned int); i++) {

        newHash += ((unsigned int) *ptr++) * multiplier;
        multiplier *= MULT;
    }

    ptr = (char*) &strct->DATA;

    while (ptr != ((char*) &strct->canR) + sizeof (unsigned int)) {

        newHash += ((unsigned int) *ptr++) * multiplier;
        multiplier *= MULT;
    }

    ptr = (char*) strct->DATACanL;

    if (strct->DATACanL != NULL) {
        while (ptr != ((char*) strct->DATACanR) + sizeof (unsigned int)) {

            newHash += ((unsigned int) *ptr++) * multiplier;
            multiplier *= MULT;
        }
    }

    strct->hash = newHash;

}

void NEW_STRUCTVerifyHash (NEW_STRUCT* strct) {

    if (strct == NULL) return;

    unsigned int oldHash = strct->hash;
    NEW_STRUCTCountHash (strct);

    if (strct->hash != oldHash) {

        strct->errCode |= WRONG_HASH;
    }

    //flog (strct->hash);
}