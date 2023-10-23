#include "global.h"
/**
 * @brief Construct a new Page object. Never used as part of the code
 *
 */
Page::Page()
{
    this->pageName = "";
    this->tableName = "";
    this->pageIndex = -1;
    this->rowCount = 0;
    this->columnCount = 0;
    this->rows.clear();
}

/**
 * @brief Construct a new Page:: Page object given the table name and page
 * index. When tables are loaded they are broken up into blocks of BLOCK_SIZE
 * and each block is stored in a different file named
 * "<tablename>_Page<pageindex>". For example, If the Page being loaded is of
 * table "R" and the pageIndex is 2 then the file name is "R_Page2". The page
 * loads the rows (or tuples) into a vector of rows (where each row is a vector
 * of integers).
 *
 * @param tableName 
 * @param pageIndex 
 */
Page::Page(string tableName, int pageIndex)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->pageName = "../data/temp/" + this->tableName + "_Page" + to_string(pageIndex);
    Table table = *tableCatalogue.getTable(tableName);
    logger.log("Page::Successsss");
    this->columnCount = table.columnCount;
    uint maxRowCount = table.maxRowsPerBlock;
    vector<int> row(columnCount, 0);
    this->rows.assign(maxRowCount, row);

    ifstream fin(pageName, ios::in);
    this->rowCount = table.rowsPerBlockCount[pageIndex];
    int number;

    for (uint rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < columnCount; columnCounter++)
        {
            fin >> number;
            this->rows[rowCounter][columnCounter] = number;
        }
    }

    fin.close();
}

/**
 * @brief Get row from page indexed by rowIndex
 * 
 * @param rowIndex 
 * @return vector<int> 
 */
vector<int> Page::getRow(int rowIndex)
{
    logger.log("Page::getRow");
    vector<int> result;
    result.clear();
    if (rowIndex >= this->rowCount)
        return result;
    return this->rows[rowIndex];
}

vector<vector<int>> Page::getPageRows()
{
    logger.log("Page::getPageRows");
    vector<vector<int>> result;
    result.clear();
    for (int i = 0; i < this->rowCount; i++)
    {
        result.push_back(this->rows[i]);
    }
    return result;
}

Page::Page(string tableName, int pageIndex, vector<vector<int>> rows, int rowCount)
{   
    // rows[0];
    logger.log("Page::Page");
    // type()
    logger.log( to_string(rows.size())  );
    logger.log(to_string(rows[0].size()));
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->rows = rows;
    this->rowCount = rowCount;
    this->columnCount = rows[0].size();
    logger.log("Page::Page");
    this->pageName = "../data/temp/"+this->tableName + "_Page" + to_string(pageIndex);
    logger.log("Page::Page SUCCESS");
}

/**
 * @brief writes current page contents to file.
 * 
 */
void Page::writePage()
{
    logger.log("Page::writePage " + this->pageName);
    ofstream fout(this->pageName, ios::trunc);
    // cout << this->rowCount << " " << this->columnCount << endl;
    // cout << this->rows.size() << " " << this->rows[0].size() << endl;

    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        // for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        for (int columnCounter = 0; columnCounter < this->rows[rowCounter].size(); columnCounter++)
        {
            // cout << this->rows[rowCounter][columnCounter] << " ";
            if (columnCounter != 0)
                fout << " ";
            fout << this->rows[rowCounter][columnCounter];
        }
        
        // if( (rowCounter != this->rowCount-1) )
            fout << endl;
        // cout << rowCounter << " " << this->rowCount << "HEY" << endl;
    }

    // cout << "SSS" << endl;
    fout.close();
}
