#include "global.h"
/**
 * @brief 
 * SYNTAX: TRANSPOSE MATRIX relation_name
 */
bool syntacticParseTRANSPOSE_MATRIX()
{
    logger.log("syntacticParseTRANSPOSE_MATRIX");
    if (tokenizedQuery[1] != "MATRIX")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = TRANSPOSE_MATRIX;
    parsedQuery.printRelationName = tokenizedQuery[2];
    return true;
}

bool semanticParseTRANSPOSE_MATRIX()
{
    logger.log("semanticParseTRANSPOSE_MATRIX");
    if (!tableCatalogue.isTable(parsedQuery.printRelationName))
    {
        cout << "SEMANTIC ERROR: MATRIX doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeTRANSPOSE_MATRIX()
{
    logger.log("executeTRANSPOSE_MATRIX2");
    Table* table = tableCatalogue.getTable(parsedQuery.printRelationName);
    string table_name = table->tableName;
    int total_pages = table->blockCount;
    int blocks_in_row = sqrt(total_pages);
    // int k = 3;
    int read_cnt = 0;
    int write_cnt = 0;
    map<int,int> mpp;
    // logger.log(to_string(total_pages));

    for( int page_num = 0; page_num < total_pages; page_num++ ){
        if( mpp.count(page_num) > 0 )
            continue;
        int x = page_num / blocks_in_row;
        int y = page_num % blocks_in_row;
        if( x == y ){
            auto matrix = read_matrix( table_name, page_num, read_cnt );
            int n = matrix.size();
            for( int i = 0; i < n; i++ ){
                for( int j = i+1; j < n; j++ ){
                    swap(matrix[i][j], matrix[j][i]);
                }
            }
            bufferManager.writePage(table_name, page_num, matrix, n);
            write_cnt++;
            // writePage( table_name, page_num, matrix, write_cnt );
            // logger.log(to_string(page_num) + "written" + to_string(x) + " " + to_string(y));
        }
        else{

            int page_num_2 = y*blocks_in_row + x;
            mpp[page_num_2]++;

            auto matrix1 = read_matrix( table_name, page_num, read_cnt );
            int n1 = matrix1.size();
            int m1 = matrix1[0].size();
            matrix1 = getTransMatrix( matrix1, n1, m1 );
            auto matrix2 = read_matrix( table_name, page_num_2, read_cnt );
            bufferManager.writePage(table_name, page_num_2, matrix1, m1);
            write_cnt++;
            // writePage( table_name, page_num_2, matrix1, write_cnt );
            // logger.log(to_string(page_num_2) + "written2" + to_string(y) + " " + to_string(x));

            
            int n2 = matrix2.size();
            int m2 = matrix2[0].size();
            matrix2 = getTransMatrix( matrix2, n2, m2 );
            bufferManager.writePage(table_name, page_num, matrix2, m2);
            write_cnt++;
            // writePage( table_name, page_num, matrix2, write_cnt );
            // logger.log(to_string(page_num) + "written2" + to_string(x) + " " + to_string(y));
        }
    }
    // logger.log("done");
    printAccessBlocksInfo(read_cnt, write_cnt);
}

