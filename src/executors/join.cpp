#include "global.h"
/**
 * @brief
 * SYNTAX: R <- JOIN relation_name1, relation_name2 ON column_name1 bin_op column_name2
 */
bool syntacticParseJOIN()
{
    logger.log("syntacticParseJOIN");
    if (tokenizedQuery.size() != 9 || tokenizedQuery[5] != "ON")
    {
        cout << "SYNTAC ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = JOIN;
    parsedQuery.joinResultRelationName = tokenizedQuery[0];
    parsedQuery.joinFirstRelationName = tokenizedQuery[3];
    parsedQuery.joinSecondRelationName = tokenizedQuery[4];
    parsedQuery.joinFirstColumnName = tokenizedQuery[6];
    parsedQuery.joinSecondColumnName = tokenizedQuery[8];

    string binaryOperator = tokenizedQuery[7];
    if (binaryOperator == "<")
        parsedQuery.joinBinaryOperator = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.joinBinaryOperator = GREATER_THAN;
    else if (binaryOperator == ">=" || binaryOperator == "=>")
        parsedQuery.joinBinaryOperator = GEQ;
    else if (binaryOperator == "<=" || binaryOperator == "=<")
        parsedQuery.joinBinaryOperator = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.joinBinaryOperator = EQUAL;
    // else if (binaryOperator == "!=")
    //     parsedQuery.joinBinaryOperator = NOT_EQUAL;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    return true;
}

bool semanticParseJOIN()
{
    logger.log("semanticParseJOIN");

    if (tableCatalogue.isTable(parsedQuery.joinResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.joinFirstRelationName) || !tableCatalogue.isTable(parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.joinFirstColumnName, parsedQuery.joinFirstRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.joinSecondColumnName, parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}

Table *createJoinTable(Table *table1, Table *table2)
{
    // logger.log("Create New Join Relation Table");
    vector<string> columnList = table1->columns;
    columnList.insert(columnList.end(), table2->columns.begin(), table2->columns.end());
    // logger.log("New Table is about to create with colulmn names as : ");
    // int i = 0;
    // for (i; i < columnList.size() - 1; i++)
    //     logger.log(columnList[i] + ", ");
    // logger.log(columnList[i]);
    return new Table(parsedQuery.joinResultRelationName, columnList);
}

void equalsTo(Table table1, long long int rowsInTable1, Table table2, long long int rowsInTable2, Table *resultantTable, long long int &readCount, long long int &writeCount)
{
    logger.log("Join : Equals To");
    int i = 0, j = 0, l = 0, count = 0;
    bool incrementCursor3 = true;

    Cursor cursor1a = table1.getCursor();
    Cursor cursor1b = table1.getCursor();
    Cursor cursor1c = table1.getCursor();
    Cursor cursor2a = table2.getCursor();
    Cursor cursor2b = table2.getCursor();
    cursor1c.getNext();

    int joinFirstColumnValue = -1e9;
    int joinSecondColumnValue = -1e9;
    int prevFirstColumnValue = -1e9;
    int prevSecondColumnValue = -1e9;

    int joinFirstColumnIndex = table1.getColumnIndex(parsedQuery.joinFirstColumnName);
    int joinSecondColumnIndex = table2.getColumnIndex(parsedQuery.joinSecondColumnName);
    // logger.log("joinFirstColumnIndex -> " + to_string(joinFirstColumnIndex));
    // logger.log("joinSecondColumnIndex -> " + to_string(joinSecondColumnIndex));

    int row_num = 1;
    int page_num = 0;
    vector<vector<int>> page;
    int maxRowsPerBlock = resultantTable->maxRowsPerBlock;

    while (true)
    {
        bool isLast = false;
        vector<int> dataInFirstTable = cursor1b.getNext();
        vector<int> dataInSecondTable = cursor2b.getNext();

        if (j == rowsInTable2 - 1)
        {
            isLast = true;
        }

        if (dataInSecondTable.size() == 0)
        {
            cursor2b = cursor2a;
            dataInSecondTable = cursor2b.getNext();
            dataInFirstTable = cursor1c.getNext();
            j = l;
            i++;
            incrementCursor3 = false;
        }

        if (dataInFirstTable.size() == 0)
        {
            bufferManager.writePage(parsedQuery.joinResultRelationName, page_num, page, page.size());
            page.clear();
            page_num++;
            break;
        }

        if (i < rowsInTable1)
            joinFirstColumnValue = dataInFirstTable[joinFirstColumnIndex];
        else
            joinFirstColumnValue = -1e9;

        if (j < rowsInTable2)
            joinSecondColumnValue = dataInSecondTable[joinSecondColumnIndex];
        else
        {
            if (dataInSecondTable.size() != 0)
                joinSecondColumnValue = dataInSecondTable[joinSecondColumnIndex];
            else
                joinSecondColumnValue = -1e9;
        }

        if (joinFirstColumnValue > joinSecondColumnValue && joinSecondColumnValue != -1e9)
        {
            readCount++;
            cursor2a.getNext();
            cursor2b = cursor2a;
            cursor1b = cursor1a;
            incrementCursor3 = true;
            i--;
            l = count;
            j = l;
        }
        else if (joinFirstColumnValue < joinSecondColumnValue && joinFirstColumnValue != -1e9)
        {
            readCount++;
            if (joinFirstColumnValue == prevSecondColumnValue)
            {
                cursor1a.getNext();
                cursor1b = cursor1a;
                cursor1c.getNext();
                cursor2b = cursor2a;
                incrementCursor3 = false;
                j = l;
            }
            else
            {
                dataInFirstTable = cursor1a.getNext();
                cursor1c.getNext();
                cursor2b = cursor2a;
                cursor1b = cursor1a;
            }
        }
        else if (joinFirstColumnValue == joinSecondColumnValue && joinSecondColumnValue != -1e9)
        {
            readCount++;
            vector<int> row = dataInFirstTable;
            row.insert(row.end(), dataInSecondTable.begin(), dataInSecondTable.end());
            if (row_num % maxRowsPerBlock == 0)
            {
                bufferManager.writePage(parsedQuery.joinResultRelationName, page_num, page, page.size());
                page.clear();
                page_num++;
            }
            page.push_back(row);
            row_num++;
            resultantTable->writeRow<int>(row);
            writeCount++;

            // cout << "i = " << i << ", j = " << j << ", l = " << l << "  -->  ";
            // for(int q=0; q<row.size(); q++)
            //     cout << row[q] << " ";
            // cout << endl;

            if (isLast == true)
            {
                cursor1a.getNext();
                cursor1b = cursor1a;
            }
            cursor1b = cursor1a;
            prevFirstColumnValue = joinFirstColumnValue;
            prevSecondColumnValue = joinSecondColumnValue;
            if (incrementCursor3 == true)
                count++;
            i--;
            j++;
        }
        i++;
    }
    resultantTable->blockify();
}

void lessThan(Table table1, long long int rowsInTable1, Table table2, long long int rowsInTable2, Table *resultantTable, long long int &readCount, long long int &writeCount)
{
    logger.log("Join : Less Than");
    int i = 0, j = 0, l = 0;
    int totalrows = rowsInTable1 + rowsInTable2;

    Cursor cursor1a = table1.getCursor();
    Cursor cursor1b = table1.getCursor();
    Cursor cursor1c = table1.getCursor();
    Cursor cursor2a = table2.getCursor();
    Cursor cursor2b = table2.getCursor();
    cursor1c.getNext();

    int joinFirstColumnValue = -1e9;
    int joinSecondColumnValue = -1e9;
    int prevFirstColumnValue = -1e9;
    int prevSecondColumnValue = -1e9;

    int joinFirstColumnIndex = table1.getColumnIndex(parsedQuery.joinFirstColumnName);
    int joinSecondColumnIndex = table2.getColumnIndex(parsedQuery.joinSecondColumnName);
    // logger.log("joinFirstColumnIndex -> " + to_string(joinFirstColumnIndex));
    // logger.log("joinSecondColumnIndex -> " + to_string(joinSecondColumnIndex));

    int row_num = 1;
    int page_num = 0;
    vector<vector<int>> page;
    int maxRowsPerBlock = resultantTable->maxRowsPerBlock;

    while (true)
    {
        vector<int> dataInFirstTable = cursor1b.getNext();
        vector<int> dataInSecondTable = cursor2b.getNext();

        if (dataInSecondTable.size() == 0)
        {
            cursor2b = cursor2a;
            dataInSecondTable = cursor2b.getNext();
            dataInFirstTable = cursor1c.getNext();
            cursor1a = cursor1b;
            i++;
            j = l;
        }

        if (dataInFirstTable.size() == 0)
        {
            bufferManager.writePage(parsedQuery.joinResultRelationName, page_num, page, page.size());
            page.clear();
            page_num++;

            break;
        }

        if (i < rowsInTable1)
            joinFirstColumnValue = dataInFirstTable[joinFirstColumnIndex];
        else
        {
            if (dataInFirstTable.size() != 0)
                joinFirstColumnValue = dataInFirstTable[joinFirstColumnIndex];
            else
                joinFirstColumnValue = -1e9;
        }

        if (j < rowsInTable2)
            joinSecondColumnValue = dataInSecondTable[joinSecondColumnIndex];
        else
        {
            if (dataInSecondTable.size() != 0)
                joinSecondColumnValue = dataInSecondTable[joinSecondColumnIndex];
            else
                joinSecondColumnValue = -1e9;
        }

        if (joinFirstColumnValue < joinSecondColumnValue)
        {
            vector<int> row = dataInFirstTable;
            row.insert(row.end(), dataInSecondTable.begin(), dataInSecondTable.end());
            readCount++;

            if (row_num % maxRowsPerBlock == 0)
            {
                bufferManager.writePage(parsedQuery.joinResultRelationName, page_num, page, page.size());
                page.clear();
                page_num++;
            }
            page.push_back(row);
            row_num++;
            resultantTable->writeRow<int>(row);
            writeCount++;

            // cout << "i = " << i << ", j = " << j << ", l = " << l << "  -->  ";
            // for(int q=0; q<row.size(); q++)
            //     cout << row[q] << " ";
            // cout << endl;

            cursor1b = cursor1a;
            i--;
            j++;
        }
        else if (joinFirstColumnValue >= joinSecondColumnValue)
        {
            readCount++;
            cursor2a.getNext();
            cursor2b = cursor2a;
            cursor1b = cursor1a;
            j++;
            i--;
            l++;
        }
        i++;
    }
    resultantTable->blockify();
}

void lessThanEqual(Table table1, long long int rowsInTable1, Table table2, long long int rowsInTable2, Table *resultantTable, long long int &readCount, long long int &writeCount)
{
    logger.log("Join : Less Than and Equals to");
    int i = 0, j = 0, l = 0;

    Cursor cursor1a = table1.getCursor();
    Cursor cursor1b = table1.getCursor();
    Cursor cursor1c = table1.getCursor();
    Cursor cursor2a = table2.getCursor();
    Cursor cursor2b = table2.getCursor();
    cursor1c.getNext();

    int joinFirstColumnValue = -1e9;
    int joinSecondColumnValue = -1e9;
    int prevFirstColumnValue = -1e9;
    int prevSecondColumnValue = -1e9;

    int joinFirstColumnIndex = table1.getColumnIndex(parsedQuery.joinFirstColumnName);
    int joinSecondColumnIndex = table2.getColumnIndex(parsedQuery.joinSecondColumnName);
    // logger.log("joinFirstColumnIndex -> " + to_string(joinFirstColumnIndex));
    // logger.log("joinSecondColumnIndex -> " + to_string(joinSecondColumnIndex));

    int row_num = 1;
    int page_num = 0;
    vector<vector<int>> page;
    int maxRowsPerBlock = resultantTable->maxRowsPerBlock;

    while (true)
    {
        vector<int> dataInFirstTable = cursor1b.getNext();
        vector<int> dataInSecondTable = cursor2b.getNext();

        if (dataInSecondTable.size() == 0)
        {
            cursor2b = cursor2a;
            dataInSecondTable = cursor2b.getNext();
            dataInFirstTable = cursor1c.getNext();
            cursor1a = cursor1b;
            i++;
            j = l;
        }

        if (dataInFirstTable.size() == 0)
        {
            bufferManager.writePage(parsedQuery.joinResultRelationName, page_num, page, page.size());
            page.clear();
            page_num++;
            break;
        }

        if (i < rowsInTable1)
            joinFirstColumnValue = dataInFirstTable[joinFirstColumnIndex];
        else
        {
            if (dataInFirstTable.size() != 0)
                joinFirstColumnValue = dataInFirstTable[joinFirstColumnIndex];
            else
                joinFirstColumnValue = -1e9;
        }

        if (j < rowsInTable2)
            joinSecondColumnValue = dataInSecondTable[joinSecondColumnIndex];
        else
        {
            if (dataInSecondTable.size() != 0)
                joinSecondColumnValue = dataInSecondTable[joinSecondColumnIndex];
            else
                joinSecondColumnValue = -1e9;
        }

        if (joinFirstColumnValue <= joinSecondColumnValue)
        {
            readCount++;
            vector<int> row = dataInFirstTable;
            row.insert(row.end(), dataInSecondTable.begin(), dataInSecondTable.end());

            if (row_num % maxRowsPerBlock == 0)
            {
                bufferManager.writePage(parsedQuery.joinResultRelationName, page_num, page, page.size());
                page.clear();
                page_num++;
            }
            page.push_back(row);
            row_num++;
            resultantTable->writeRow<int>(row);
            writeCount++;

            // cout << "i = " << i << ", j = " << j << ", l = " << l << "  -->  ";
            // for(int q=0; q<row.size(); q++)
            //     cout << row[q] << " ";
            // cout << endl;

            cursor1b = cursor1a;
            i--;
            j++;
        }
        else if (joinFirstColumnValue > joinSecondColumnValue)
        {
            readCount++;
            cursor2a.getNext();
            cursor2b = cursor2a;
            cursor1b = cursor1a;
            j++;
            i--;
            l++;
        }
        i++;
    }
    resultantTable->blockify();
}

void greaterThan(Table table1, long long int rowsInTable1, Table table2, long long int rowsInTable2, Table *resultantTable, long long int &readCount, long long int &writeCount)
{
    logger.log("Join : Greater Than");
    int i = 0, j = 0;

    Cursor cursor1a = table1.getCursor();
    Cursor cursor1b = table1.getCursor();
    Cursor cursor1c = table1.getCursor();
    Cursor cursor2a = table2.getCursor(); // fixed at top
    Cursor cursor2b = table2.getCursor();
    cursor1c.getNext();

    int joinFirstColumnValue = -1e9;
    int joinSecondColumnValue = -1e9;
    int prevFirstColumnValue = -1e9;
    int prevSecondColumnValue = -1e9;

    int joinFirstColumnIndex = table1.getColumnIndex(parsedQuery.joinFirstColumnName);
    int joinSecondColumnIndex = table2.getColumnIndex(parsedQuery.joinSecondColumnName);
    // logger.log("joinFirstColumnIndex -> " + to_string(joinFirstColumnIndex));
    // logger.log("joinSecondColumnIndex -> " + to_string(joinSecondColumnIndex));

    int row_num = 1;
    int page_num = 0;
    vector<vector<int>> page;
    int maxRowsPerBlock = resultantTable->maxRowsPerBlock;

    while (true)
    {
        vector<int> dataInFirstTable = cursor1b.getNext();
        vector<int> dataInSecondTable = cursor2b.getNext();

        if (dataInSecondTable.size() == 0)
        {
            cursor2b = cursor2a;
            dataInSecondTable = cursor2b.getNext();
            dataInFirstTable = cursor1c.getNext();
            cursor1a = cursor1b;
            i++;
            j = 0;
        }

        if (dataInFirstTable.size() == 0)
        {
            bufferManager.writePage(parsedQuery.joinResultRelationName, page_num, page, page.size());
            page.clear();
            page_num++;
            break;
        }

        if (i < rowsInTable1)
            joinFirstColumnValue = dataInFirstTable[joinFirstColumnIndex];
        else
        {
            if (dataInFirstTable.size() != 0)
                joinFirstColumnValue = dataInFirstTable[joinFirstColumnIndex];
            else
                joinFirstColumnValue = -1e9;
        }

        if (j < rowsInTable2)
            joinSecondColumnValue = dataInSecondTable[joinSecondColumnIndex];
        else
        {
            if (dataInSecondTable.size() != 0)
                joinSecondColumnValue = dataInSecondTable[joinSecondColumnIndex];
            else
                joinSecondColumnValue = -1e9;
        }

        if (joinFirstColumnValue <= joinSecondColumnValue)
        {
            readCount++;
            cursor1a = cursor1b;
            cursor2b = cursor2a;
            cursor1c.getNext();
            j = 0;
        }
        else if (joinFirstColumnValue > joinSecondColumnValue)
        {
            readCount++;
            vector<int> row = dataInFirstTable;
            row.insert(row.end(), dataInSecondTable.begin(), dataInSecondTable.end());
            if (row_num % maxRowsPerBlock == 0)
            {
                bufferManager.writePage(parsedQuery.joinResultRelationName, page_num, page, page.size());
                page.clear();
                page_num++;
            }
            page.push_back(row);
            row_num++;
            resultantTable->writeRow<int>(row);
            writeCount++;

            // cout << "i = " << i << ", j = " << j << "  -->  ";
            // for(int q=0; q<row.size(); q++)
            //     cout << row[q] << " ";
            // cout << endl;

            cursor1b = cursor1a;
            i--;
            j++;
        }
        i++;
    }
    resultantTable->blockify();
}

void greaterThanEqual(Table table1, long long int rowsInTable1, Table table2, long long int rowsInTable2, Table *resultantTable, long long int &readCount, long long int &writeCount)
{
    logger.log("Join : Greater Than and Equals to");
    int i = 0, j = 0, l = 0;

    Cursor cursor1a = table1.getCursor();
    Cursor cursor1b = table1.getCursor();
    Cursor cursor1c = table1.getCursor();
    Cursor cursor2a = table2.getCursor();
    Cursor cursor2b = table2.getCursor();
    cursor1c.getNext();

    int joinFirstColumnValue = -1e9;
    int joinSecondColumnValue = -1e9;
    int prevFirstColumnValue = -1e9;
    int prevSecondColumnValue = -1e9;

    int joinFirstColumnIndex = table1.getColumnIndex(parsedQuery.joinFirstColumnName);
    int joinSecondColumnIndex = table2.getColumnIndex(parsedQuery.joinSecondColumnName);
    // logger.log("joinFirstColumnIndex -> " + to_string(joinFirstColumnIndex));
    // logger.log("joinSecondColumnIndex -> " + to_string(joinSecondColumnIndex));

    int row_num = 1;
    int page_num = 0;
    vector<vector<int>> page;
    int maxRowsPerBlock = resultantTable->maxRowsPerBlock;

    while (true)
    {
        vector<int> dataInFirstTable = cursor1b.getNext();
        vector<int> dataInSecondTable = cursor2b.getNext();

        if (dataInSecondTable.size() == 0)
        {
            cursor2b = cursor2a;
            dataInSecondTable = cursor2b.getNext();
            dataInFirstTable = cursor1c.getNext();
            cursor1a = cursor1b;
            i++;
            j = 0;
        }

        if (dataInFirstTable.size() == 0)
        {
            bufferManager.writePage(parsedQuery.joinResultRelationName, page_num, page, page.size());
            page.clear();
            page_num++;
            break;
        }

        if (i < rowsInTable1)
            joinFirstColumnValue = dataInFirstTable[joinFirstColumnIndex];
        else
        {
            if (dataInFirstTable.size() != 0)
                joinFirstColumnValue = dataInFirstTable[joinFirstColumnIndex];
            else
                joinFirstColumnValue = -1e9;
        }

        if (j < rowsInTable2)
            joinSecondColumnValue = dataInSecondTable[joinSecondColumnIndex];
        else
        {
            if (dataInSecondTable.size() != 0)
                joinSecondColumnValue = dataInSecondTable[joinSecondColumnIndex];
            else
                joinSecondColumnValue = -1e9;
        }

        if (joinFirstColumnValue < joinSecondColumnValue)
        {
            readCount++;
            cursor1a = cursor1b;
            cursor2b = cursor2a;
            cursor1c.getNext();
            j = 0;
        }

        else if (joinFirstColumnValue >= joinSecondColumnValue)
        {
            readCount++;
            vector<int> row = dataInFirstTable;
            row.insert(row.end(), dataInSecondTable.begin(), dataInSecondTable.end());
            if (row_num % maxRowsPerBlock == 0)
            {
                bufferManager.writePage(parsedQuery.joinResultRelationName, page_num, page, page.size());
                page.clear();
                page_num++;
            }
            page.push_back(row);
            row_num++;
            resultantTable->writeRow<int>(row);
            writeCount++;

            // cout << "i = " << i << ", j = " << j << "  -->  ";
            // for(int q=0; q<row.size(); q++)
            //     cout << row[q] << " ";
            // cout << endl;

            cursor1b = cursor1a;
            i--;
            j++;
        }
        i++;
    }
    resultantTable->blockify();
}

void executeJOIN()
{
    logger.log("executeJOIN");

    // sort(table1);
    // sort(table2);

    // logger.log("createNewJoinRelationTable");
    Table t1 = *tableCatalogue.getTable(parsedQuery.joinFirstRelationName);
    Table t2 = *tableCatalogue.getTable(parsedQuery.joinSecondRelationName);

    string temp1_name = "temp1";
    string temp2_name = "temp2";

    parsedQuery.sortColumnName.push_back(parsedQuery.joinFirstColumnName);
    parsedQuery.sortingStrategy.push_back("ASC");

    Table *a1 = create_sorted_copy_for_table(temp1_name, &t1);

    parsedQuery.sortColumnName.clear();
    parsedQuery.sortColumnName.push_back(parsedQuery.joinSecondColumnName);

    Table *a2 = create_sorted_copy_for_table(temp2_name, &t2);

    Table table1 = *a1;
    Table table2 = *a2;

    // table1.print();
    // table2.print();

    Table *resultantTable = createJoinTable(&table1, &table2);
    tableCatalogue.insertTable(resultantTable);
    // logger.log("Resultant Table Created");

    long long int readCount = 0;
    long long int writeCount = 0;
    long long int rowsInTable1 = table1.rowCount;
    long long int rowsInTable2 = table2.rowCount;
    // logger.log("Table 1 Row Count = " + to_string(rowsInTable1));
    // logger.log("Table 2 Row Count = " + to_string(rowsInTable2));

    if (parsedQuery.joinBinaryOperator == 4)
        equalsTo(table1, rowsInTable1, table2, rowsInTable2, resultantTable, readCount, writeCount);
    else if (parsedQuery.joinBinaryOperator == 0)
        lessThan(table1, rowsInTable1, table2, rowsInTable2, resultantTable, readCount, writeCount);
    else if (parsedQuery.joinBinaryOperator == 2)
        lessThanEqual(table1, rowsInTable1, table2, rowsInTable2, resultantTable, readCount, writeCount);
    else if (parsedQuery.joinBinaryOperator == 1)
        greaterThan(table1, rowsInTable1, table2, rowsInTable2, resultantTable, readCount, writeCount);
    else if (parsedQuery.joinBinaryOperator == 3)
        greaterThanEqual(table1, rowsInTable1, table2, rowsInTable2, resultantTable, readCount, writeCount);
    else
        cout << "Error" << endl;

    // tableCatalogue.insertTable(resultantTable);

    // cout << endl;
    // printAccessBlocksInfo(readCount, writeCount);
    tableCatalogue.deleteTable(temp1_name);
    tableCatalogue.deleteTable(temp2_name);
    bufferManager.deleteAll();
    string fileName = "../data/temp/" + parsedQuery.joinResultRelationName + ".csv";
    bufferManager.deleteFile(fileName);
    return;
}
