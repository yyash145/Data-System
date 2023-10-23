#include "global.h"
/**
 * @brief File contains method to process SORT commands.
 *
 * syntax:
 * SORT relation_name BY column_name IN sorting_order
 *
 * SORT EMPLOYEE BY GENDER, SALARY IN ASC, DESC
 * sorting_order = ASC | DESC
 */

vector<int> sortedColumnsIndexList;
vector<int> sortStratColumnsList;

bool syntacticParseSORT()
{
    logger.log("syntacticParseSORT");
    int query_size = tokenizedQuery.size();

    if (tokenizedQuery[2] != "BY")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    if ((query_size - 4) % 2 == 1)
    {
        cout << "SYNTAX ERROR: Columns not properly mapped with type of sorting" << endl;
        return false;
    }

    int total_sort_columns = (query_size - 4) / 2;

    if (tokenizedQuery[3 + total_sort_columns] != "IN")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    parsedQuery.queryType = SORT;
    parsedQuery.sortRelationName = tokenizedQuery[1];

    int sort_col_start_index = 3;
    int sort_strat_start_index = 3 + total_sort_columns + 1;

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

bool semanticParseSORT()
{
    logger.log("semanticParseSORT");

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

bool sortCol(const vector<int> &a, const vector<int> &b)
{

    int n = sortedColumnsIndexList.size();
    for (int i = 0; i < n; i++)
    {
        int index = sortedColumnsIndexList[i];
        if (sortStratColumnsList[i])
        {
            if (a[index] < b[index])
                return true;
            if (a[index] > b[index])
                return false;
        }
        else
        {
            if (a[index] > b[index])
                return true;
            if (a[index] < b[index])
                return false;
        }
    }
    return true;
}

class Compare
{
public:
    bool operator()(const pair<vector<int>, pair<int, int>> &p1, const pair<vector<int>, pair<int, int>> &p2)
    {
        vector<int> a = p1.first;
        vector<int> b = p2.first;
        int n = sortedColumnsIndexList.size();
        for (int i = 0; i < n; i++)
        {
            if (sortStratColumnsList[i])
            {
                if (a[i] < b[i])
                    return false;
                if (a[i] > b[i])
                    return true;
            }
            else
            {
                if (a[i] > b[i])
                    return false;
                if (a[i] < b[i])
                    return true;
            }
        }
        return false;
    }
};

class CompareTables
{
public:
    bool operator()(const pair<vector<int>, pair<int, vector<int>>> &p1, const pair<vector<int>, pair<int, vector<int>>> &p2)
    {
        vector<int> a = p1.first;
        vector<int> b = p2.first;
        int n = sortedColumnsIndexList.size();
        for (int i = 0; i < n; i++)
        {
            if (sortStratColumnsList[i])
            {
                if (a[i] < b[i])
                    return false;
                if (a[i] > b[i])
                    return true;
            }
            else
            {
                if (a[i] > b[i])
                    return false;
                if (a[i] < b[i])
                    return true;
            }
        }
        return false;
    }
};

void MergePages(Table *table, vector<vector<vector<int>>> &pages)
{

    for (int i = 0; i < pages.size(); i++)
    {

        sort(pages[i].begin(), pages[i].end(), sortCol);
    }

    priority_queue<pair<vector<int>, pair<int, int>>, vector<pair<vector<int>, pair<int, int>>>, Compare> pq;

    for (int i = 0; i < pages.size(); i++)
    {
        vector<int> temp;
        for (int j = 0; j < sortedColumnsIndexList.size(); j++)
        {
            temp.push_back(pages[i][0][sortedColumnsIndexList[j]]);
            // cout << pages[i][0][sortedColumnsIndexList[j]] << " ";
        }
        // cout << " | " << i << " " << 0 << endl;
        pq.push({temp, {i, 0}});
    }

    int first = 0;

    while (pq.size() != 0)
    {
        pair<vector<int>, pair<int, int>> top = pq.top();

        pq.pop();
        int rowNumber = top.second.second;
        int pageNumber = top.second.first;

        table->writeRow<int>(pages[pageNumber][rowNumber]);
        if (rowNumber + 1 < pages[pageNumber].size())
        {

            vector<int> temp;
            for (int j = 0; j < sortedColumnsIndexList.size(); j++)
            {
                temp.push_back(pages[pageNumber][rowNumber + 1][sortedColumnsIndexList[j]]);
            }

            pq.push({temp, {pageNumber, rowNumber + 1}});
        }
    }

    table->blockify();
}

void MergeTables(Table *restable, vector<Table *> tables)
{

    vector<Cursor> cursors;
    for (int i = 0; i < tables.size(); i++)
    {
        cursors.push_back(tables[i]->getCursor());
    }

    priority_queue<pair<vector<int>, pair<int, vector<int>>>, vector<pair<vector<int>, pair<int, vector<int>>>>, CompareTables> pq;

    for (int i = 0; i < cursors.size(); i++)
    {
        vector<int> row = cursors[i].getNext();
        vector<int> temp;
        for (int j = 0; j < sortedColumnsIndexList.size(); j++)
        {
            temp.push_back(row[sortedColumnsIndexList[j]]);
            // cout << row[sortedColumnsIndexList[j]] << " ";
        }
        if (!row.empty())
        {
            pq.push({temp, {i, row}});
            // cout << " | " << i << endl;
        }
    }
    // cout << "S2" << endl;

    while (pq.size() != 0)
    {
        pair<vector<int>, pair<int, vector<int>>> top = pq.top();
        pq.pop();
        int tableNumber = top.second.first;
        vector<int> row = top.second.second;
        restable->writeRow<int>(row);
        vector<int> nextRow = cursors[tableNumber].getNext();

        if (!nextRow.empty())
        {
            vector<int> temp;
            for (int j = 0; j < sortedColumnsIndexList.size(); j++)
            {
                temp.push_back(nextRow[sortedColumnsIndexList[j]]);
            }
            pq.push({temp, {tableNumber, nextRow}});
        }
    }
    // cout << "S3" << endl;
    restable->blockify();
    // cout<<"Table Name :"<<restable->tableName<<endl;
    // restable->print();
}

void executeSORT()
{

    logger.log("executeSORT");

    sortedColumnsIndexList.clear();

    sortStratColumnsList.clear();
    // cout << sortedColumnsIndexList.size() << endl;

    Table *table = tableCatalogue.getTable(parsedQuery.sortRelationName);
    string originalTableName = table->tableName;
    int maxRowsPerBlock = table->maxRowsPerBlock;
    vector<string> sortedColumns = parsedQuery.sortColumnName;

    for (int i = 0; i < sortedColumns.size(); i++)
    {
        int index = table->getColumnIndex(sortedColumns[i]);
        // cout << sortedColumns[i] << " ** " << index << endl;
        sortedColumnsIndexList.push_back(index);
        int sort_strat = parsedQuery.sortingStrategy[i] == "ASC" ? 1 : 0;
        sortStratColumnsList.push_back(sort_strat);
    }
    // cout << "lets sort2" << endl;
    // cout << sortedColumnsIndexList.size() << endl;

    bool hasKeyCol = false;
    for (int i = 0; i < sortedColumnsIndexList.size(); i++)
    {
        // cout << sortedColumnsIndexList[i] << " -- " << endl;
        if (sortedColumnsIndexList[i] == 0)
        {
            hasKeyCol = true;
            break;
        }
    }
    // cout << "lets sort3" << endl;
    // cout << hasKeyCol << endl;

    if (!hasKeyCol)
    {
        sortedColumnsIndexList.push_back(0);
        sortStratColumnsList.push_back(1); // 1 -> Ascending
    }

    // uint M = parsedQuery.allottedBuffer-1;
    int m = BLOCK_COUNT - 1;
    int blockCount = table->blockCount;
    int blockSet = 0;
    int phase_num = 0;
    int i = 0;
    vector<Table *> tables;

    while (i < blockCount)
    {

        vector<vector<vector<int>>> pages;
        int j = 0; // push into pages
        for (j = 0; j < m && i + j < blockCount; j++)
        {
            // cout << "see1" << table->tableName << " " << i+j << endl;
            Page temp = bufferManager.getPage(table->tableName, i + j);
            // cout << "see2" << endl;
            Page *page = &temp;
            vector<vector<int>> rows = temp.getPageRows();
            pages.push_back(rows);
        }
        i += j;

        Table *restable = new Table("temp_sort_set_" + to_string(blockSet) + "_phase_" + to_string(phase_num), table->columns);
        tableCatalogue.insertTable(restable);

        MergePages(restable, pages);
        // cout<<"Table Name :"<<restable->tableName<<endl;
        // restable->print();
        tables.push_back(restable);
        // for(int k=0;k<pages.size();k++){
        //     delete pages[k];
        // }
        blockSet++;
    }

    // cout << tables.size() << endl;

    blockSet = 0;
    phase_num++;

    while (tables.size() > 1)
    {
        // cout<<"Tables Size: "<<tables.size()<<endl;
        vector<Table *> restables;
        int i = 0;
        while (i < tables.size())
        {
            vector<Table *> tempTables;
            int j = 0;
            for (j = 0; j < m && i + j < tables.size(); j++)
            {
                tempTables.push_back(tables[i + j]);
            }
            i += j;
            Table *restable = new Table("temp_sort_set_" + to_string(blockSet) + "_phase_" + to_string(phase_num), table->columns);
            tableCatalogue.insertTable(restable);
            // cout << i << " " << j << " " << phase_num  << " " << tempTables.size() << endl;
            MergeTables(restable, tempTables);
            // cout << "S" << endl;
            restables.push_back(restable);
            for (int k = 0; k < tempTables.size(); k++)
            {
                tableCatalogue.deleteTable(tempTables[k]->tableName);
            }
            blockSet++;
        }

        phase_num++;
        blockSet = 0;
        // for(int k=0;k<tables.size();k++){
        //     delete tables[k];
        // }
        tables = restables;
    }

    if (tables.size() == 1)
    {
        Table *sortedTable = tables[0];
        // sortedTable->print();
        int row_num = 1;
        vector<vector<int>> page;
        Cursor cursor = sortedTable->getCursor();
        vector<int> row = cursor.getNext();
        page.push_back(row);
        int page_num = 0;

        while (!row.empty())
        {
            // resultTable->writeRow<int>(row);
            if (row_num % maxRowsPerBlock == 0)
            {
                // cout << page_num << " " << row_num;
                bufferManager.writePage(originalTableName, page_num, page, page.size());
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
        bufferManager.writePage(originalTableName, page_num, page, page.size());
        // cout << "Success3" << endl;
        page.clear();
        // cout << "clearing table" << endl;
        tableCatalogue.deleteTable(sortedTable->tableName);
        // resultTable->blockify();
    }

    parsedQuery.sortColumnName.clear();
    parsedQuery.sortingStrategy.clear();
    // sortedColumnsIndexList.clear();
    // sortStratColumnsList.clear();

    for (int page_num = 0; page_num < table->blockCount; page_num++)
    {
        bufferManager.deleteFromPool(originalTableName, page_num);
    }

    // cout<<"Table "<< resultTable->tableName <<" created"<<endl;
    bufferManager.deleteAll();
    return;
}