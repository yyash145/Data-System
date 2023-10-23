#include"global.h"

void executeCommand(){

    switch(parsedQuery.queryType){
        case CLEAR: executeCLEAR(); break;
        case CROSS: executeCROSS(); break;
        case DISTINCT: executeDISTINCT(); break;
        case EXPORT: executeEXPORT(); break;
        case INDEX: executeINDEX(); break;
        case JOIN: executeJOIN(); break;
        case LIST: executeLIST(); break;
        case LOAD: executeLOAD(); break;
        case LOAD_MATRIX: executeLOAD_MATRIX(); break;
        case TRANSPOSE_MATRIX: executeTRANSPOSE_MATRIX(); break;
        case EXPORT_MATRIX: return executeEXPORT_MATRIX();
        case CHECKSYMMETRY: executeCHECKSYMMETRY(); break;
        case PRINT: executePRINT(); break;
        case PRINT_MATRIX: executePRINT_MATRIX(); break;
        case COMPUTE: executeCOMPUTE(); break;
        case PROJECTION: executePROJECTION(); break;
        case RENAME: executeRENAME(); break;
        case RENAME_MATRIX: executeRENAME_MATRIX(); break;
        case SELECTION: executeSELECTION(); break;
        case SORT: executeSORT(); break;
        case ORDER_BY: executeORDER_BY(); break;
        case GROUPBY: executeGROUPBY(); break;
        case SOURCE: executeSOURCE(); break;
        default: cout<<"PARSING ERROR"<<endl;
    }

    return;
}


void doCommand()
{   
    if (syntacticParse() && semanticParse())
        executeCommand();
    return;
}

void printRowCount(int rowCount){
    cout<<"\n\nRow Count: "<<rowCount<<endl;
    return;
}

void printAccessBlocksInfo(int blocks_read, int blocks_written){
    cout<<"Number of blocks read: "<< blocks_read << endl;
    cout<<"Number of blocks written: "<< blocks_written << endl;
    cout<<"Number of blocks accessed: "<< blocks_read + blocks_written <<endl;
}

vector<vector<int>> read_matrix(string table_name, int page_num, int &read_cnt) {
    std::ifstream inputFile("../data/temp/"+ table_name +"_Page" + to_string(page_num));
    std::vector<std::vector<int>> matrix;
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open the input file." << std::endl;
        return matrix;
    }
    std::string line;
    while (std::getline(inputFile, line)) {
        std::vector<int> row;
        std::istringstream iss(line);
        int num;
        while (iss >> num) {
            row.push_back(num);
        }
        matrix.push_back(row);
    } 
    inputFile.close();
    read_cnt++;
    return matrix;
}

vector<vector<int>> getTransMatrix( vector<vector<int>> &matrix1, int n1, int m1 ){
    vector<vector<int>> trans_matrix1(m1, vector<int>(n1));
    for( int i = 0; i < n1; i++ ){
        for( int j = 0; j < m1; j++ ){
            trans_matrix1[j][i] = matrix1[i][j];
        }
    }
    return trans_matrix1;
}

Table* create_sorted_copy_for_table(string sorted_table_name, Table *table_old){
    int maxRowsPerBlock = table_old->maxRowsPerBlock;
    int row_num = 1;
    vector<vector<int>> page;
    Cursor cursor1 = table_old->getCursor();
    vector<int> row = cursor1.getNext();
    page.push_back(row);
    int page_num = 0;    

    while(!row.empty()){
        if( row_num % maxRowsPerBlock == 0 ){
            bufferManager.writePage( sorted_table_name, page_num, page, page.size() );
            page.clear();
            page_num++;
        }
        row = cursor1.getNext();
        page.push_back(row);
        row_num++;
    }

    bufferManager.writePage( sorted_table_name, page_num, page, page.size() );
    page.clear();

    auto table = new Table(sorted_table_name);
    *table = *table_old;
    table->sourceFileName = "../data/" + sorted_table_name + ".csv";
    table->tableName = sorted_table_name;
    tableCatalogue.insertTable(table);
    parsedQuery.sortRelationName = sorted_table_name;
    executeSORT();
    bufferManager.deleteAll();
    return table;
}
