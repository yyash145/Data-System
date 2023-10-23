#include "global.h"
/**
 * @brief 
 * SYNTAX: PRINT relation_name
 */
bool syntacticParseCOMPUTE()
{
    logger.log("syntacticParseCOMPUTE");
    if (tokenizedQuery.size() != 2)
    {
        // cout << tokenizedQuery.size() << endl; 
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = COMPUTE;
    parsedQuery.printRelationName = tokenizedQuery[1];
    return true;
}

bool semanticParseCOMPUTE()
{
    logger.log("semanticParseCOMPUTE");
    if (!tableCatalogue.isTable(parsedQuery.printRelationName))
    {
        cout << "SEMANTIC ERROR: MATRIX doesn't exist" << endl;
        return false;
    }
    return true;
}

vector<vector<int>> subtract(vector<vector<int>> &matrix, vector<vector<int>> &trans_matrix, int n, int m){
    for( int i = 0; i < n; i++ ){
        for( int j = 0; j < m; j++ ){
            trans_matrix[i][j] = matrix[i][j] - trans_matrix[i][j];
        }
    }
    return trans_matrix;
}

void executeCOMPUTE()
{
    logger.log("executeCOMPUTE");
    Table* table = tableCatalogue.getTable(parsedQuery.printRelationName);
    string table_name = table->tableName;
    string new_table_name = table_name + "_RESULT";

    int total_pages = table->blockCount;
    int blocks_in_row = sqrt(total_pages);
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
                for( int j = i; j < n; j++ ){
                    if( i == j )
                        matrix[i][j] = 0;
                    else{
                        int a = matrix[i][j];
                        int b = matrix[j][i];
                        matrix[i][j] = a-b;
                        matrix[j][i] = b-a;
                    }    
                }
            }
            bufferManager.writePage(new_table_name, page_num, matrix, n);
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
            auto matrix2 = read_matrix( table_name, page_num_2, read_cnt );
            int n2 = matrix2.size();
            int m2 = matrix2[0].size();

            auto trans_matrix1 = getTransMatrix( matrix1, n1, m1 );
            trans_matrix1 = subtract(matrix2, trans_matrix1, n2, m2);
            bufferManager.writePage(new_table_name, page_num_2, trans_matrix1, m1);
            write_cnt++;
            // writePage( table_name, page_num_2, matrix1, write_cnt );
            // logger.log(to_string(page_num_2) + "written2" + to_string(y) + " " + to_string(x));

            auto trans_matrix2 = getTransMatrix( matrix2, n2, m2 );
            trans_matrix2 = subtract(matrix1, trans_matrix2, n1, m1);
            bufferManager.writePage(new_table_name, page_num, trans_matrix2, m2);
            write_cnt++;
            // writePage( table_name, page_num_2, matrix1, write_cnt );
            // logger.log(to_string(page_num) + "written2" + to_string(y) + " " + to_string(x));
        }
    }
    // logger.log("done");
    printAccessBlocksInfo(read_cnt, write_cnt);

    Table* resultantTable = new Table(new_table_name);
    resultantTable->isMatrix = true;
    resultantTable->blockCount = table->blockCount;
    tableCatalogue.insertTable(resultantTable);

}