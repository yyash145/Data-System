#include "global.h"
/**
 * @brief 
 * SYNTAX: CHECKSYMMETRY relation_name
 */
bool syntacticParseCHECKSYMMETRY()
{
    logger.log("syntacticCHECKSYMMETRY");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = CHECKSYMMETRY;
    parsedQuery.printRelationName = tokenizedQuery[1];
    return true;
}

bool semanticParseCHECKSYMMETRY()
{
    logger.log("semanticParseCHECKSYMMETRY");
    if (!tableCatalogue.isTable(parsedQuery.printRelationName))
    {
        cout << "SEMANTIC ERROR: MATRIX doesn't exist" << endl;
        return false;
    }
    return true;
}

vector<vector<int>> checkTransMatrix( vector<vector<int>> &matrix1, int n1, int m1 ){
    vector<vector<int>> trans_matrix1(m1, vector<int>(n1));
    for( int i = 0; i < n1; i++ ){
        for( int j = 0; j < m1; j++ ){
            trans_matrix1[j][i] = matrix1[i][j];
        }
    }
    return trans_matrix1;
}

void executeCHECKSYMMETRY()
{
    logger.log("executeCHECKSYMMETRY");
    Table* table = tableCatalogue.getTable(parsedQuery.printRelationName);
    string table_name = table->tableName;
    int total_pages = table->blockCount;
    int blocks_in_row = sqrt(total_pages);
    int read_cnt = 0;
    int write_cnt = 0;
    map<int,int> mpp;
    logger.log(to_string(total_pages));
    bool flag = true;

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
                    if( matrix[i][j] != matrix[j][i] ){
                        flag = false;
                        break;
                    }
                }
            }
        }
        else{
            int page_num_2 = y*blocks_in_row + x;
            mpp[page_num_2]++;
            auto matrix1 = read_matrix( table_name, page_num, read_cnt );
            auto matrix2 = read_matrix( table_name, page_num_2, read_cnt );
            int n1 = matrix1.size();
            int m1 = matrix1[0].size();
            for( int i = 0; i < n1; i++ ){
                for( int j = 0; j < m1; j++ ){
                    if( matrix2[j][i] != matrix1[i][j] ){
                        flag = false;
                        break;
                    }
                }
            }
        }
    }
    if( flag )
        cout << "TRUE" << endl;
    else
        cout << "FALSE" << endl; 
    printAccessBlocksInfo(read_cnt, write_cnt);
}