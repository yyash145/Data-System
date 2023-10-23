#include "global.h"

/**
 * @brief 
 * SYNTAX: EXPORT MATRIX <relation_name> 
 */

bool syntacticParseEXPORT_MATRIX()
{
    logger.log("syntacticParseEXPORT_MATRIX");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = EXPORT_MATRIX;
    parsedQuery.exportRelationName = tokenizedQuery[2];
    return true;
}

bool semanticParseEXPORT_MATRIX()
{
    logger.log("semanticParseEXPORT_MATRIX");
    //Table should exist
    if (tableCatalogue.isTable(parsedQuery.exportRelationName))
        return true;
    cout << "SEMANTIC ERROR: No such Matrix exists" << endl;
    return false;
}

void appendToCSVFile(const std::vector<int>& nums, const std::string& fileName) {
    // Open the CSV file in append mode (ios::app), which creates the file if it doesn't exist
    std::ofstream file("../data/"+fileName+".csv", std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Error opening the file: " << fileName << std::endl;
        return;
    }
    std::stringstream ss;
    for (size_t i = 0; i < nums.size(); ++i) {
        ss << nums[i];
        if (i < nums.size() - 1) {
            ss << ",";
        }
    }
    ss << "\n";
    file << ss.str();
    file.close();
    // std::cout << "Data appended to " << fileName << std::endl;
}


void executeEXPORT_MATRIX()
{
    logger.log("executeEXPORT_MATRIX");
    Table* table = tableCatalogue.getTable(parsedQuery.exportRelationName);
    // table->makePermanent();

    // if(!table->isPermanent())
    //     bufferManager.deleteFile(table->sourceFileName);

    int numOfBlocks = table->blockCount;
    string tableName = table->tableName;

    std::ofstream newFile("../data/"+tableName+".csv");
    if (!newFile.is_open()) {
        cout << "Error creating the file: " << tableName << std::endl;
        return;
    }
    newFile.close();
    
    int k = BLOCK_DIM;
    int k2 = k;
    int B = sqrt(numOfBlocks);
    // int limit = ceil(PRINT_COUNT*1.0 / k);
    int read_cnt = 0, write_cnt = 0;
    // cout << B << " " << limit << endl;
    for (int i = 0; i < B; i++)
    {
        for (int k1 = 0; k1 < k2; k1++)
        {
            vector<int> nums;
            int i1 = k1;
            for (int j = 0; j < B; j++)
            {
                int pageNumber = i * B + j;
                vector<vector<int>> m1 = read_matrix(tableName, pageNumber, read_cnt);
                // logger.log("read: "+to_string(read_cnt));
                k2 = m1.size();
                for (int j1 = 0; j1 < m1[0].size(); j1++)
                {                                                       
                    // cout << m1[i1][j1] << " ";
                    nums.push_back(m1[i1][j1]);
                }
            }
            // cout << endl;  
            appendToCSVFile(nums, tableName);
        }
    }
    printAccessBlocksInfo(read_cnt, write_cnt);
    // table->unload();
    // logger.log("Matrix::~delte all pages");
    for (int pageCounter = 0; pageCounter < table->blockCount; pageCounter++)
        bufferManager.deleteFile(table->tableName, pageCounter);
}