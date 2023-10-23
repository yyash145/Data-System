#include"global.h"

bool semanticParse(){
    logger.log("semanticParse");

    logger.log(to_string(parsedQuery.queryType));

    switch(parsedQuery.queryType){
        case CLEAR: return semanticParseCLEAR();
        case CROSS: return semanticParseCROSS();
        case DISTINCT: return semanticParseDISTINCT();
        case EXPORT: return semanticParseEXPORT();
        case INDEX: return semanticParseINDEX();
        case JOIN: return semanticParseJOIN();
        case LIST: return semanticParseLIST();
        case LOAD: return semanticParseLOAD();
        case LOAD_MATRIX: return semanticParseLOAD_MATRIX();
        case COMPUTE: return semanticParseCOMPUTE();
        case EXPORT_MATRIX: return semanticParseEXPORT_MATRIX();
        case TRANSPOSE_MATRIX: return semanticParseTRANSPOSE_MATRIX();
        case CHECKSYMMETRY: return semanticParseCHECKSYMMETRY();
        case PRINT: return semanticParsePRINT();
        case PRINT_MATRIX: return semanticParsePRINT_MATRIX();
        case PROJECTION: return semanticParsePROJECTION();
        case RENAME: return semanticParseRENAME();
        case RENAME_MATRIX: return semanticParseRENAME_MATRIX();
        case SELECTION: return semanticParseSELECTION();
        case SORT: return semanticParseSORT();
        case ORDER_BY: return semanticParseORDER_BY();
        case GROUPBY: return semanticParseGROUPBY();
        case SOURCE: return semanticParseSOURCE();
        default: cout<<"SEMANTIC ERROR"<<endl;
    }

    return false;
}