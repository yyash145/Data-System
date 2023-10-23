#include "global.h"

/**
 * @brief 
 * SYNTAX: RENAME column_name TO column_name FROM relation_name
 */
bool syntacticParseRENAME_MATRIX()
{
    logger.log("syntacticParseRENAME_MATRIX");
    if (tokenizedQuery.size() != 4)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = RENAME_MATRIX;
    parsedQuery.renameFromMatrixName = tokenizedQuery[2];
    parsedQuery.renameToMatrixName = tokenizedQuery[3];
    return true;
}

bool semanticParseRENAME_MATRIX()
{
    logger.log("semanticParseRENAME_MATRIX");

    if (!tableCatalogue.isTable(parsedQuery.renameFromMatrixName))
    {
        cout << "Matrix Name - " << parsedQuery.renameFromMatrixName << endl;
        cout << "SEMANTIC ERROR: MATRIX doesn't exist" << endl;
        return false;
    }

    if (tableCatalogue.isTable(parsedQuery.renameToMatrixName))
    {
        cout << "SEMANTIC ERROR: MATRIX with name already exists" << endl;
        return false;
    }

    return true;
}

void executeRENAME_MATRIX()
{
    // tableCatalogue.printTableKey();
    logger.log("executeRENAME_MATRIX");
    Table* table = tableCatalogue.getTable(parsedQuery.renameFromMatrixName);
    table->tableName = parsedQuery.renameToMatrixName;
    table->sourceFileName = "../data/" + parsedQuery.renameToMatrixName + ".csv";
    tableCatalogue.changeMatrixName(parsedQuery.renameFromMatrixName, parsedQuery.renameToMatrixName);
    bufferManager.renameFile(parsedQuery.renameFromMatrixName, parsedQuery.renameToMatrixName);
    // tableCatalogue.printTableKey();
    return;
}