#include "global.h"
/**
 * @brief 
 * SYNTAX: LOAD MATRIX relation_name
 */
bool syntacticParseLOAD_MATRIX()
{
    logger.log("syntacticParseLOAD_MATRIX");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.loadRelationName = tokenizedQuery[2];
    parsedQuery.queryType = LOAD_MATRIX;
    return true;
}

bool semanticParseLOAD_MATRIX()
{   
    logger.log("semanticParseLOAD_MATRIX");
    if (tableCatalogue.isTable(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Matrix already exists" << endl;
        return false;
    }
    if (!isFileExists(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeLOAD_MATRIX()
{
    logger.log("executeLOAD_MATRIX");
    Table *table = new Table(parsedQuery.loadRelationName);
    table->isMatrix = true;
    if ( table->load() )
    {
        tableCatalogue.insertTable(table);
        cout << "Loaded Matrix. Column Count: " << table->columnCount << " Row Count: " << table->rowCount << endl;
    }
    return;
}