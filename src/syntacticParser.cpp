#include "global.h"

bool syntacticParse()
{
    logger.log("syntacticParse");
    logger.log(to_string(parsedQuery.queryType));
    
    string possibleQueryType = tokenizedQuery[0];

    if (tokenizedQuery.size() < 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    if (possibleQueryType == "CLEAR")
        return syntacticParseCLEAR();
    else if (possibleQueryType == "INDEX")
        return syntacticParseINDEX();
    else if (possibleQueryType == "LIST")
        return syntacticParseLIST();
    else if (possibleQueryType == "LOAD"){
        string second_argument = tokenizedQuery[1];
        if( second_argument == "MATRIX" )
            return syntacticParseLOAD_MATRIX();
        else
            return syntacticParseLOAD();
    }
    else if( possibleQueryType == "COMPUTE" )
        return syntacticParseCOMPUTE();
    else if( possibleQueryType == "CHECKSYMMETRY" )
        return syntacticParseCHECKSYMMETRY();
    else if (possibleQueryType == "TRANSPOSE")
        return syntacticParseTRANSPOSE_MATRIX();
    else if (possibleQueryType == "PRINT"){
        string second_argument = tokenizedQuery[1];
        if( second_argument == "MATRIX" )
            return syntacticParsePRINT_MATRIX();
        else
            return syntacticParsePRINT();
    }
    else if (possibleQueryType == "RENAME"){
        string second_argument = tokenizedQuery[1];
        if( second_argument == "MATRIX" )
            return syntacticParseRENAME_MATRIX();
        else
            return syntacticParseRENAME();
    }
    else if(possibleQueryType == "EXPORT"){
        string second_argument = tokenizedQuery[1];
        if( second_argument == "MATRIX" )
            return syntacticParseEXPORT_MATRIX();
        else
            return syntacticParseEXPORT();
    }
    else if(possibleQueryType == "SOURCE")
        return syntacticParseSOURCE();
    else if (possibleQueryType == "SORT")
        return syntacticParseSORT();
    else
    {
        string resultantRelationName = possibleQueryType;
        if (tokenizedQuery[1] != "<-" || tokenizedQuery.size() < 3)
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
        possibleQueryType = tokenizedQuery[2];
        if (possibleQueryType == "PROJECT")
            return syntacticParsePROJECTION();
        else if (possibleQueryType == "ORDER")
            return syntacticParseORDER_BY();
        else if (possibleQueryType == "GROUP")
            return syntacticParseGROUPBY();
        else if (possibleQueryType == "SELECT")
            return syntacticParseSELECTION();
        else if (possibleQueryType == "JOIN")
            return syntacticParseJOIN();
        else if (possibleQueryType == "CROSS")
            return syntacticParseCROSS();
        else if (possibleQueryType == "DISTINCT")
            return syntacticParseDISTINCT();
        else
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
    }
    return false;
}

ParsedQuery::ParsedQuery()
{
}

void ParsedQuery::clear()
{
    logger.log("ParseQuery::clear");
    this->queryType = UNDETERMINED;

    this->clearRelationName = "";

    this->crossResultRelationName = "";
    this->crossFirstRelationName = "";
    this->crossSecondRelationName = "";

    this->distinctResultRelationName = "";
    this->distinctRelationName = "";

    this->exportRelationName = "";

    this->indexingStrategy = NOTHING;
    this->indexColumnName = "";
    this->indexRelationName = "";

    this->joinBinaryOperator = NO_BINOP_CLAUSE;
    this->joinResultRelationName = "";
    this->joinFirstRelationName = "";
    this->joinSecondRelationName = "";
    this->joinFirstColumnName = "";
    this->joinSecondColumnName = "";

    this->loadRelationName = "";

    this->printRelationName = "";

    this->projectionResultRelationName = "";
    this->projectionColumnList.clear();
    this->projectionRelationName = "";

    this->renameFromColumnName = "";
    this->renameToColumnName = "";
    this->renameRelationName = "";

    this->selectType = NO_SELECT_CLAUSE;
    this->selectionBinaryOperator = NO_BINOP_CLAUSE;
    this->selectionResultRelationName = "";
    this->selectionRelationName = "";
    this->selectionFirstColumnName = "";
    this->selectionSecondColumnName = "";
    this->selectionIntLiteral = 0;

    this->sortColumnName.clear();
    this->sortingStrategy.clear();
    // this->sortingStrategy = NO_SORT_CLAUSE;
    this->sortResultRelationName = "";
    // this->sortColumnName = "";
    this->sortRelationName = "";

    this->sourceFileName = "";
}

/**
 * @brief Checks to see if source file exists. Called when LOAD command is
 * invoked.
 *
 * @param tableName 
 * @return true 
 * @return false 
 */
bool isFileExists(string tableName)
{
    string fileName = "../data/" + tableName + ".csv";
    struct stat buffer;
    return (stat(fileName.c_str(), &buffer) == 0);
}

/**
 * @brief Checks to see if source file exists. Called when SOURCE command is
 * invoked.
 *
 * @param tableName 
 * @return true 
 * @return false 
 */
bool isQueryFile(string fileName){
    fileName = "../data/" + fileName + ".ra";
    struct stat buffer;
    return (stat(fileName.c_str(), &buffer) == 0);
}
