
#include "global.h"
/**
 * @brief File contains method to process SORT commands.
 *
 * syntax:
 * <new_table> <- ORDER BY <attribute> ASC|DESC ON <table_name>
 *
 * Result <- ORDER BY Salary ASC ON EMPLOYEE_BIG
 * sorting_order = ASC | DESC
 */

bool syntacticParseORDER_BY()
{
    logger.log("syntacticParseSORT");
    int query_size = tokenizedQuery.size();

    if (tokenizedQuery[2] != "ORDER" and tokenizedQuery[3] != "BY")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    if ((query_size - 6) % 2 == 1)
    {
        cout << "SYNTAX ERROR: Columns not properly mapped with type of sorting" << endl;
        return false;
    }

    int total_sort_columns = (query_size - 6) / 2;

    if (tokenizedQuery[4 + 2 * total_sort_columns] != "ON")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    parsedQuery.queryType = ORDER_BY;
    parsedQuery.sortResultRelationName = tokenizedQuery[0];
    parsedQuery.sortRelationName = tokenizedQuery[query_size - 1];

    int sort_col_start_index = 4;
    int sort_strat_start_index = 4 + total_sort_columns;

    for (int i = 0; i < total_sort_columns; i++)
    {

        string col_name = tokenizedQuery[sort_col_start_index + i];
        string sort_strat = tokenizedQuery[sort_strat_start_index + i];

        // cout << col_name << " " << sort_strat << endl;

        if (sort_strat != "ASC" and sort_strat != "DESC")
        {
            cout << "SYNTAX ERROR: ASC OR DESC NOT WRITTEN CORRECTLY" << endl;
            return false;
        }

        parsedQuery.sortColumnName.push_back(col_name);
        parsedQuery.sortingStrategy.push_back(sort_strat);
    }
    return true;
}

bool semanticParseORDER_BY()
{
    logger.log("semanticParseSORT");

    if (tableCatalogue.isTable(parsedQuery.sortResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.sortRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    vector<string> sortedColumns = parsedQuery.sortColumnName;
    for (int i = 0; i < sortedColumns.size(); i++)
    {
        if (!tableCatalogue.isColumnFromTable(sortedColumns[i], parsedQuery.sortRelationName))
        {
            cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
            return false;
        }
    }

    return true;
}

void executeORDER_BY()
{

    logger.log("executeORDER_BY");
    // cout << sortedColumnsIndexList.size() << endl;

    Table *table = tableCatalogue.getTable(parsedQuery.sortRelationName);
    string originalTableName = table->tableName;
    int maxRowsPerBlock = table->maxRowsPerBlock;

    int row_num = 1;
    vector<vector<int>> page;
    Cursor cursor = table->getCursor();
    vector<int> row = cursor.getNext();
    page.push_back(row);
    int page_num = 0;

    while (!row.empty())
    {
        // resultTable->writeRow<int>(row);
        if (row_num % maxRowsPerBlock == 0)
        {
            // cout << page_num << " " << row_num;
            bufferManager.writePage(parsedQuery.sortResultRelationName, page_num, page, page.size());
            // cout << " Success" << endl;
            page.clear();
            page_num++;
        }
        // cout << row_num << endl;
        row = cursor.getNext();
        page.push_back(row);
        row_num++;
    }
    // cout << "Success2 " << page_num << " " << page.size()  << endl;
    bufferManager.writePage(parsedQuery.sortResultRelationName, page_num, page, page.size());
    // cout << "Success3" << endl;
    page.clear();

    auto resultTable = new Table(parsedQuery.sortResultRelationName);
    *resultTable = *table;
    resultTable->sourceFileName = "../data/" + parsedQuery.sortResultRelationName + ".csv";
    resultTable->tableName = parsedQuery.sortResultRelationName;

    tableCatalogue.insertTable(resultTable);
    parsedQuery.sortRelationName = parsedQuery.sortResultRelationName;
    executeSORT();
    // bufferManager.deleteAll(); // commented
}