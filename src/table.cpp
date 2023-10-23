// #include "global.h"

// /**
//  * @brief Construct a new Table:: Table object
//  *
//  */
// Table::Table()
// {
//     logger.log("Table::Table");
// }

// /**
//  * @brief Construct a new Table:: Table object used in the case where the data
//  * file is available and LOAD command has been called. This command should be
//  * followed by calling the load function;
//  *
//  * @param tableName 
//  */
// Table::Table(string tableName)
// {
//     logger.log("Table::Table");
//     this->sourceFileName = "../data/" + tableName + ".csv";
//     this->tableName = tableName;
// }

// /**
//  * @brief Construct a new Table:: Table object used when an assignment command
//  * is encountered. To create the table object both the table name and the
//  * columns the table holds should be specified.
//  *
//  * @param tableName 
//  * @param columns 
//  */
// Table::Table(string tableName, vector<string> columns)
// {
//     logger.log("Table::Table");
//     this->sourceFileName = "../data/temp/" + tableName + ".csv";
//     this->tableName = tableName;
//     this->columns = columns;
//     this->columnCount = columns.size();
//     this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * columnCount));
//     this->writeRow<string>(columns);
// }

// /**
//  * @brief The load function is used when the LOAD command is encountered. It
//  * reads data from the source file, splits it into blocks and updates table
//  * statistics.
//  *
//  * @return true if the table has been successfully loaded 
//  * @return false if an error occurred 
//  */
// bool Table::load()
// {
//     if( this->isMatrix )
//         logger.log("Matrix::load");
//     else
//         logger.log("Table::load");
//     fstream fin(this->sourceFileName, ios::in);
//     string line;
//     if (getline(fin, line))
//     {
//         fin.close();
//         if ( this->extractColumnNames(line) )
//             if( !this->isMatrix ){
//                 if ( this->blockify() )
//                     return true;
//             }
//             else{
//                 if ( this->blockify_matrix() )
//                     return true;
//             }
//     }
//     fin.close();
//     return false;
// }

// /**
//  * @brief Function extracts column names from the header line of the .csv data
//  * file. 
//  *
//  * @param line 
//  * @return true if column names successfully extracted (i.e. no column name
//  * repeats)
//  * @return false otherwise
//  */
// bool Table::extractColumnNames(string firstLine)
// {
//     logger.log("Table::extractColumnNames");
//     unordered_set<string> columnNames;
//     string word;
//     stringstream s(firstLine);
//     while (getline(s, word, ','))
//     {
//         word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
//         if ( !this->isMatrix and columnNames.count(word) )
//             return false;
//         columnNames.insert(word);
//         this->columns.emplace_back(word);
//     }
//     this->columnCount = this->columns.size();
//     this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * this->columnCount));
//     return true;
// }

// /**
//  * @brief This function splits all the rows and stores them in multiple files of
//  * one block size. 
//  *
//  * @return true if successfully blockified
//  * @return false otherwise
//  */

// std::vector<std::vector<int>> read_matrix(string path, string filename) {
//     std::vector<std::vector<int>> data;
//     std::ifstream file(path + filename);
//     if (!file.is_open()) {
//         std::cerr << "Error: Unable to open the file." << std::endl;
//         return data;  
//     }
//     std::string line;
//     while (std::getline(file, line)) {
//         std::vector<int> row;
//         std::stringstream lineStream(line);
//         std::string cell;
//         while (std::getline(lineStream, cell, ',')) {
//             row.push_back(std::stoi(cell));
//         }
//         data.push_back(row);
//     }
//     file.close();
//     return data;
// }

// bool Table::blockify_matrix(){
//     string file_name = this->tableName + ".csv"; 
//     int k = BLOCK_DIM;
//     string path = "../data/";
//     vector<vector<int>> matrix = read_matrix( path, file_name);
//     int n = matrix.size();
//     // for( int i = 0; i < n; i++ ){
//     //     for( int j = 0; j < n; j++ ){
//     //         cout << matrix[i][j] << " ";
//     //     }
//     //     cout << endl;
//     // }
//     int page_num = 0;
//     for( int x = 0; x < n; x += k ){
//         for( int y = 0; y < n; y += k ){
//             int x_size = min(k, n-x);
//             int y_size = min(k, n-y);
//             vector<vector<int>> rows(x_size, vector<int>(y_size));
//             for( int i = x; i < min(x+k,n); i++ ){
//                 for( int j = y; j < min(y+k,n); j++ ){
//                     rows[i-x][j-y] = matrix[i][j];
//                 }
//             }
//             bufferManager.writePage(this->tableName, page_num, rows, x_size);
//             page_num++;
//         }
//     }
//     this->blockCount = page_num;
//     this->rowCount = n;
//     logger.log("Matrix::blockify SUCCESS");
//     return true;
// }

// bool Table::blockify()
// {
//     logger.log("Table::blockify");
//     ifstream fin(this->sourceFileName, ios::in);
//     string line, word;
//     vector<int> row(this->columnCount, 0);
//     vector<vector<int>> rowsInPage(this->maxRowsPerBlock, row);
//     int pageCounter = 0;
//     unordered_set<int> dummy;
//     dummy.clear();
//     this->distinctValuesInColumns.assign(this->columnCount, dummy);
//     this->distinctValuesPerColumnCount.assign(this->columnCount, 0);

//     if( !this->isMatrix )
//         getline(fin, line);

//     while (getline(fin, line))
//     {
//         stringstream s(line);
//         logger.log(line);
//         for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
//         {
//             if (!getline(s, word, ','))
//                 return false;
//             row[columnCounter] = stoi(word);
//             rowsInPage[pageCounter][columnCounter] = row[columnCounter];
//         }
//         pageCounter++;
//         this->updateStatistics(row);
//         if (pageCounter == this->maxRowsPerBlock)
//         {
//             bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
//             this->blockCount++;
//             this->rowsPerBlockCount.emplace_back(pageCounter);
//             pageCounter = 0;
//         }
//     }
//     if (pageCounter)
//     {
//         bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
//         this->blockCount++;
//         this->rowsPerBlockCount.emplace_back(pageCounter);
//         pageCounter = 0;
//     }

//     if (this->rowCount == 0)
//         return false;
//     this->distinctValuesInColumns.clear();
//     logger.log("Table::blockify SUCCESS");
//     return true;
// }

// /**
//  * @brief Given a row of values, this function will update the statistics it
//  * stores i.e. it updates the number of rows that are present in the column and
//  * the number of distinct values present in each column. These statistics are to
//  * be used during optimisation.
//  *
//  * @param row 
//  */
// void Table::updateStatistics(vector<int> row)
// {
//     this->rowCount++;
//     for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
//     {
//         if (!this->distinctValuesInColumns[columnCounter].count(row[columnCounter]))
//         {
//             this->distinctValuesInColumns[columnCounter].insert(row[columnCounter]);
//             this->distinctValuesPerColumnCount[columnCounter]++;
//         }
//     }
// }

// /**
//  * @brief Checks if the given column is present in this table.
//  *
//  * @param columnName 
//  * @return true 
//  * @return false 
//  */
// bool Table::isColumn(string columnName)
// {
//     logger.log("Table::isColumn");
//     for (auto col : this->columns)
//     {
//         if (col == columnName)
//         {
//             return true;
//         }
//     }
//     return false;
// }

// /**
//  * @brief Renames the column indicated by fromColumnName to toColumnName. It is
//  * assumed that checks such as the existence of fromColumnName and the non prior
//  * existence of toColumnName are done.
//  *
//  * @param fromColumnName 
//  * @param toColumnName 
//  */
// void Table::renameColumn(string fromColumnName, string toColumnName)
// {
//     logger.log("Table::renameColumn");
//     for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
//     {
//         if (columns[columnCounter] == fromColumnName)
//         {
//             columns[columnCounter] = toColumnName;
//             break;
//         }
//     }
//     return;
// }

// /**
//  * @brief Function prints the first few rows of the table. If the table contains
//  * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
//  * the rows are printed.
//  *
//  */
// void Table::print()
// {
//     logger.log("Table::print");
//     uint count = min((long long)PRINT_COUNT, this->rowCount);

//     //print headings
//     if( !this->isMatrix )
//         this->writeRow(this->columns, cout);

//     Cursor cursor(this->tableName, 0);
//     vector<int> row;
//     for (int rowCounter = 0; rowCounter < count; rowCounter++)
//     {
//         row = cursor.getNext();
//         this->writeRow(row, cout);
//     }
//     printRowCount(this->rowCount);
// }



// /**
//  * @brief This function returns one row of the table using the cursor object. It
//  * returns an empty row is all rows have been read.
//  *
//  * @param cursor 
//  * @return vector<int> 
//  */
// void Table::getNextPage(Cursor *cursor)
// {
//     logger.log("Table::getNext");

//         if (cursor->pageIndex < this->blockCount - 1)
//         {
//             cursor->nextPage(cursor->pageIndex+1);
//         }
// }



// /**
//  * @brief called when EXPORT command is invoked to move source file to "data"
//  * folder.
//  *
//  */
// void Table::makePermanent()
// {
//     logger.log("Table::makePermanent");
//     if(!this->isPermanent())
//         bufferManager.deleteFile(this->sourceFileName);
//     string newSourceFile = "../data/" + this->tableName + ".csv";
//     ofstream fout(newSourceFile, ios::out);

//     //print headings
//     if( !this->isMatrix )
//         this->writeRow(this->columns, fout);

//     Cursor cursor(this->tableName, 0);
//     vector<int> row;
//     for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
//     {
//         row = cursor.getNext();
//         this->writeRow(row, fout);
//     }
//     fout.close();
// }

// /**
//  * @brief Function to check if table is already exported
//  *
//  * @return true if exported
//  * @return false otherwise
//  */
// bool Table::isPermanent()
// {
//     string name =  "../data/" + this->tableName + ".csv";
//     logger.log("Table::isPermanent | " + this->sourceFileName + " | " + name  );
//     if (this->sourceFileName == name )
//     return true;
//     return false;
// }

// /**
//  * @brief The unload function removes the table from the database by deleting
//  * all temporary files created as part of this table
//  *
//  */
// void Table::unload(){
//     logger.log("Table::~unload");
//     for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
//         bufferManager.deleteFile(this->tableName, pageCounter);
//     if (!isPermanent())
//         bufferManager.deleteFile(this->sourceFileName);
// }

// /**
//  * @brief Function that returns a cursor that reads rows from this table
//  * 
//  * @return Cursor 
//  */
// Cursor Table::getCursor()
// {
//     logger.log("Table::getCursor");
//     Cursor cursor(this->tableName, 0);
//     return cursor;
// }
// /**
//  * @brief Function that returns the index of column indicated by columnName
//  * 
//  * @param columnName 
//  * @return int 
//  */
// int Table::getColumnIndex(string columnName)
// {
//     logger.log("Table::getColumnIndex");
//     for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
//     {
//         if (this->columns[columnCounter] == columnName)
//             return columnCounter;
//     }
// }

#include "global.h"

/**
 * @brief Construct a new Table:: Table object
 *
 */
Table::Table()
{
    logger.log("Table::Table");
}

/**
 * @brief Construct a new Table:: Table object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param tableName 
 */
Table::Table(string tableName)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/" + tableName + ".csv";
    this->tableName = tableName;
}

/**
 * @brief Construct a new Table:: Table object used when an assignment command
 * is encountered. To create the table object both the table name and the
 * columns the table holds should be specified.
 *
 * @param tableName 
 * @param columns 
 */
Table::Table(string tableName, vector<string> columns)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/temp/" + tableName + ".csv";
    this->tableName = tableName;
    this->columns = columns;
    this->columnCount = columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * columnCount));
    this->writeRow<string>(columns);
}

Table::Table(string tableName, vector<string> columns, bool flag)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/temp/" + tableName + ".csv";
    this->tableName = tableName;
    this->columns = columns;
    this->columnCount = columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * columnCount));
}


/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates table
 * statistics.
 *
 * @return true if the table has been successfully loaded 
 * @return false if an error occurred 
 */
bool Table::load()
{
    if( this->isMatrix )
        logger.log("Matrix::load");
    else
        logger.log("Table::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))
    {
        fin.close();
        if ( this->extractColumnNames(line) )
            if( !this->isMatrix ){
                if ( this->blockify() )
                    return true;
            }
            else{
                if ( this->blockify_matrix() )
                    return true;
            }
    }
    fin.close();
    return false;
}

/**
 * @brief Function extracts column names from the header line of the .csv data
 * file. 
 *
 * @param line 
 * @return true if column names successfully extracted (i.e. no column name
 * repeats)
 * @return false otherwise
 */
bool Table::extractColumnNames(string firstLine)
{
    logger.log("Table::extractColumnNames");
    unordered_set<string> columnNames;
    string word;
    stringstream s(firstLine);
    while (getline(s, word, ','))
    {
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
        if ( !this->isMatrix and columnNames.count(word) )
            return false;
        columnNames.insert(word);
        this->columns.emplace_back(word);
    }
    this->columnCount = this->columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * this->columnCount));
    return true;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */

std::vector<std::vector<int>> read_matrix(string path, string filename) {
    std::vector<std::vector<int>> data;
    std::ifstream file(path + filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open the file." << std::endl;
        return data;  
    }
    std::string line;
    while (std::getline(file, line)) {
        std::vector<int> row;
        std::stringstream lineStream(line);
        std::string cell;
        while (std::getline(lineStream, cell, ',')) {
            row.push_back(std::stoi(cell));
        }
        data.push_back(row);
    }
    file.close();
    return data;
}

bool Table::blockify_matrix(){
    string file_name = this->tableName + ".csv"; 
    int k = BLOCK_DIM;
    string path = "../data/";
    vector<vector<int>> matrix = read_matrix( path, file_name);
    int n = matrix.size();
    // for( int i = 0; i < n; i++ ){
    //     for( int j = 0; j < n; j++ ){
    //         cout << matrix[i][j] << " ";
    //     }
    //     cout << endl;
    // }
    int page_num = 0;
    for( int x = 0; x < n; x += k ){
        for( int y = 0; y < n; y += k ){
            int x_size = min(k, n-x);
            int y_size = min(k, n-y);
            vector<vector<int>> rows(x_size, vector<int>(y_size));
            for( int i = x; i < min(x+k,n); i++ ){
                for( int j = y; j < min(y+k,n); j++ ){
                    rows[i-x][j-y] = matrix[i][j];
                }
            }
            bufferManager.writePage(this->tableName, page_num, rows, x_size);
            page_num++;
        }
    }
    this->blockCount = page_num;
    this->rowCount = n;
    logger.log("Matrix::blockify SUCCESS");
    return true;
}

bool Table::blockify()
{
    logger.log("Table::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    vector<int> row(this->columnCount, 0);
    vector<vector<int>> rowsInPage(this->maxRowsPerBlock, row);
    int pageCounter = 0;
    unordered_set<int> dummy;
    dummy.clear();
    this->distinctValuesInColumns.assign(this->columnCount, dummy);
    this->distinctValuesPerColumnCount.assign(this->columnCount, 0);

    if( !this->isMatrix )
        getline(fin, line);

    while (getline(fin, line))
    {
        stringstream s(line);
        logger.log(line);
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (!getline(s, word, ','))
                return false;
            row[columnCounter] = stoi(word);
            rowsInPage[pageCounter][columnCounter] = row[columnCounter];
        }
        pageCounter++;
        this->updateStatistics(row);
        if (pageCounter == this->maxRowsPerBlock)
        {
            bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
            this->blockCount++;
            this->rowsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
    }
    if (pageCounter)
    {
        bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
        this->blockCount++;
        this->rowsPerBlockCount.emplace_back(pageCounter);
        pageCounter = 0;
    }

    if (this->rowCount == 0)
        return false;
    this->distinctValuesInColumns.clear();
    logger.log("Table::blockify SUCCESS");
    return true;
}

/**
 * @brief Given a row of values, this function will update the statistics it
 * stores i.e. it updates the number of rows that are present in the column and
 * the number of distinct values present in each column. These statistics are to
 * be used during optimisation.
 *
 * @param row 
 */
void Table::updateStatistics(vector<int> row)
{
    this->rowCount++;
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (!this->distinctValuesInColumns[columnCounter].count(row[columnCounter]))
        {
            this->distinctValuesInColumns[columnCounter].insert(row[columnCounter]);
            this->distinctValuesPerColumnCount[columnCounter]++;
        }
    }
}

/**
 * @brief Checks if the given column is present in this table.
 *
 * @param columnName 
 * @return true 
 * @return false 
 */
bool Table::isColumn(string columnName)
{
    logger.log("Table::isColumn");
    for (auto col : this->columns)
    {
        if (col == columnName)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Renames the column indicated by fromColumnName to toColumnName. It is
 * assumed that checks such as the existence of fromColumnName and the non prior
 * existence of toColumnName are done.
 *
 * @param fromColumnName 
 * @param toColumnName 
 */
void Table::renameColumn(string fromColumnName, string toColumnName)
{
    logger.log("Table::renameColumn");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (columns[columnCounter] == fromColumnName)
        {
            columns[columnCounter] = toColumnName;
            break;
        }
    }
    return;
}

/**
 * @brief Function prints the first few rows of the table. If the table contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Table::print()
{
    logger.log("Table::print");
    uint count = min((long long)PRINT_COUNT, this->rowCount);
    
    //print headings
    if( !this->isMatrix )
        this->writeRow(this->columns, cout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < count; rowCounter++)
    {
        row = cursor.getNext();
        this->writeRow(row, cout);
    }
    printRowCount(this->rowCount);
}



/**
 * @brief This function returns one row of the table using the cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param cursor 
 * @return vector<int> 
 */
void Table::getNextPage(Cursor *cursor)
{
    logger.log("Table::getNext");

        if (cursor->pageIndex < this->blockCount - 1)
        {
            cursor->nextPage(cursor->pageIndex+1);
        }
}



/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Table::makePermanent()
{
    logger.log("Table::makePermanent");
    if(!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->tableName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    //print headings
    if( !this->isMatrix )
        this->writeRow(this->columns, fout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        row = cursor.getNext();
        this->writeRow(row, fout);
    }
    fout.close();
}

/**
 * @brief Function to check if table is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Table::isPermanent()
{
    string name =  "../data/" + this->tableName + ".csv";
    logger.log("Table::isPermanent | " + this->sourceFileName + " | " + name  );
    if (this->sourceFileName == name )
    return true;
    return false;
}

/**
 * @brief The unload function removes the table from the database by deleting
 * all temporary files created as part of this table
 *
 */
void Table::unload(){
    logger.log("Table::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->tableName, pageCounter);
    if (!isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
}

/**
 * @brief Function that returns a cursor that reads rows from this table
 * 
 * @return Cursor 
 */
Cursor Table::getCursor()
{
    logger.log("Table::getCursor");
    Cursor cursor(this->tableName, 0);
    return cursor;
}
/**
 * @brief Function that returns the index of column indicated by columnName
 * 
 * @param columnName 
 * @return int 
 */
int Table::getColumnIndex(string columnName)
{
    logger.log("Table::getColumnIndex");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (this->columns[columnCounter] == columnName)
            return columnCounter;
    }
}
