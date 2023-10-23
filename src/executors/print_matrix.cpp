#include "global.h"
/**
 * @brief
 * SYNTAX: PRINT MATRIX relation_name
 */
bool syntacticParsePRINT_MATRIX()
{
    logger.log("syntacticParsePRINT_MATRIX");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = PRINT_MATRIX;
    parsedQuery.printRelationName = tokenizedQuery[2];
    // logger.log("Yes: " + to_string(parsedQuery.queryType));
    return true;
}

bool semanticParsePRINT_MATRIX()
{
    logger.log("semanticParsePRINT_MATRIX");
    if (!tableCatalogue.isTable(parsedQuery.printRelationName))
    {
        cout << "SEMANTIC ERROR: MATRIX doesn't exist" << endl;
        return false;
    }
    return true;
}

void executePRINT_MATRIX()
{
    logger.log("executePRINT_MATRIX");
    Table* table = tableCatalogue.getTable(parsedQuery.printRelationName);
    int numOfBlocks = table->blockCount;
    string tableName = table->tableName;
    int k = BLOCK_DIM;
    int k2 = k;
    int B = sqrt(numOfBlocks);
    int limit = ceil(PRINT_COUNT*1.0 / k);
    int read_cnt = 0, write_cnt = 0;
    // cout << B << " " << limit << endl;
    int row_cnt = 0;
    for (int i = 0; i < min(B, limit); i++)
    {
        for (int k1 = 0; k1 < k2 and row_cnt < PRINT_COUNT; k1++)
        {
            int i1 = k1;
            int col_cnt = 0;
            for (int j = 0; j < min(B, limit); j++)
            {
                int pageNumber = i * B + j;
                vector<vector<int>> m1 = read_matrix(tableName, pageNumber, read_cnt);
                k2 = m1.size();
                for (int j1 = 0; j1 < m1[0].size() and col_cnt < PRINT_COUNT; j1++)
                {                                                       
                    cout << m1[i1][j1] << " ";
                    col_cnt++;
                }
            }
            cout << endl;
            row_cnt++;
        }
    }
    printAccessBlocksInfo(read_cnt, write_cnt);
}
