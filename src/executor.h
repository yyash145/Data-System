#include"semanticParser.h"

void executeCommand();
void doCommand();

void executeCLEAR();
void executeCROSS();
void executeDISTINCT();
void executeEXPORT();
void executeINDEX();
void executeJOIN();
void executeLIST();
void executeLOAD();
void executeLOAD_MATRIX();
void executeTRANSPOSE_MATRIX();
void executeCHECKSYMMETRY();
void executeEXPORT_MATRIX();
void executePRINT_MATRIX();
void executePRINT();
void executeCOMPUTE();
void executePROJECTION();
void executeRENAME();
void executeSELECTION();
void executeSORT();
void executeORDER_BY();
void executeRENAME_MATRIX();
void executeGROUPBY();
void executeSOURCE();
void printAccessBlocksInfo(int blocks_read, int blocks_written);
vector<vector<int>> read_matrix(string table_name, int page_num, int &read_cnt);
vector<vector<int>> getTransMatrix( vector<vector<int>> &matrix1, int n1, int m1 );
Table* create_sorted_copy_for_table(string sorted_table_name, Table *table_old);

bool evaluateBinOp(int value1, int value2, BinaryOperator binaryOperator);
void printRowCount(int rowCount);