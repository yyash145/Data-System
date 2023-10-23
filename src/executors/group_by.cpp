#include "global.h"
/**
 * @brief
 * SYNTAX: <new_table> <- GROUP BY <grouping_attribute> FROM <table_name> RETURN MAX|MIN|SUM|AVG(<attribute>)
 */

// <new_table> < -GROUP BY<grouping_attribute> FROM<table_name> HAVING<aggregate(attribute)><bin_op><attribute_value> RETURN<aggregate_func(attribute)>

// T2 < -GROUP BY Department_ID FROM EMPLOYEE HAVING
//      SUM(Salary) > 70,
//     000 RETURN SUM(Salary)

// T2 < -GROUP BY Department_ID FROM EMPLOYEE HAVING
//      COUNT(Salary) > 3,
//     000 RETURN Count(Salary)

bool syntacticParseGROUPBY()
{
    logger.log("syntacticParseGROUPBY");

    if (tokenizedQuery.size() != 13 || tokenizedQuery[3] != "BY" || tokenizedQuery[5] != "FROM" || tokenizedQuery[7] != "HAVING" || tokenizedQuery[8][tokenizedQuery[8].length() - 1] != ')' || (tokenizedQuery[8][3] != '(' && tokenizedQuery[8][5] != '(') || tokenizedQuery[11] != "RETURN" || tokenizedQuery[12][tokenizedQuery[12].length() - 1] != ')' ||
        (tokenizedQuery[12][3] != '(' && tokenizedQuery[12][5] != '('))
    {
        // cout << "main SYNTAX error" << endl;
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    string binaryOperator = tokenizedQuery[9];
    if (binaryOperator == "<")
        parsedQuery.groupByBinaryOperator = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.groupByBinaryOperator = GREATER_THAN;
    else if (binaryOperator == ">=" || binaryOperator == "=>")
        parsedQuery.groupByBinaryOperator = GEQ;
    else if (binaryOperator == "<=" || binaryOperator == "=<")
        parsedQuery.groupByBinaryOperator = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.groupByBinaryOperator = EQUAL;
    else if (binaryOperator == "!=")
        parsedQuery.groupByBinaryOperator = NOT_EQUAL;
    else
    {
        // cout << "syntax eror in groupByBinaryOperator" << endl;
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    string aggrgatefunc = tokenizedQuery[8].substr(0, 3);

    if (aggrgatefunc == "COU")
    {
        aggrgatefunc = tokenizedQuery[8].substr(0, 5);
    }

    string resultfunc = tokenizedQuery[12].substr(0, 3);

    if (resultfunc == "COU")
    {
        resultfunc = tokenizedQuery[12].substr(0, 5);
    }

    if (aggrgatefunc != "MAX" && aggrgatefunc != "MIN" && aggrgatefunc != "SUM" && aggrgatefunc != "AVG" && aggrgatefunc != "COUNT" && resultfunc != "MAX" && resultfunc != "MIN" && resultfunc != "SUM" && resultfunc != "AVG" && resultfunc != "COUNT")
    {
        // cout << "SYNATAX ERROR IN AGGREGATE FUNCTION" << endl;
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    parsedQuery.queryType = GROUPBY;
    parsedQuery.groupByResultRelationName = tokenizedQuery[0];
    parsedQuery.groupByGroupingAttributeName = tokenizedQuery[4];
    parsedQuery.groupByRelationName = tokenizedQuery[6];
    parsedQuery.havingAggregateOperatorName = aggrgatefunc;

    parsedQuery.havingAggregateAttributeName = tokenizedQuery[8].substr(4, tokenizedQuery[8].length() - 5);

    if (aggrgatefunc == "COUNT")
    {
        parsedQuery.havingAggregateAttributeName = tokenizedQuery[8].substr(6, tokenizedQuery[8].length() - 7);
    }

    // parsedQuery.havingOperatorName = tokenizedQuery[9];

    parsedQuery.havingIntegerValue = tokenizedQuery[10];

    parsedQuery.groupByResultAggregateOperatorName = resultfunc;
    parsedQuery.groupByResultAggregateAttributeName = tokenizedQuery[12].substr(4, tokenizedQuery[12].length() - 5);

    if (resultfunc == "COUNT")
    {
        parsedQuery.groupByResultAggregateAttributeName = tokenizedQuery[12].substr(6, tokenizedQuery[12].length() - 7);
    }

    parsedQuery.sortColumnName.push_back(parsedQuery.groupByGroupingAttributeName);
    parsedQuery.sortingStrategy.push_back("ASC");

    return true;
}

bool semanticParseGROUPBY()
{
    logger.log("semanticParseGROUPBY");

    if (tableCatalogue.isTable(parsedQuery.groupByResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }
    if (!tableCatalogue.isTable(parsedQuery.groupByRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    Table *table = tableCatalogue.getTable(parsedQuery.groupByRelationName);
    if (!tableCatalogue.isColumnFromTable(parsedQuery.groupByGroupingAttributeName, parsedQuery.groupByRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.havingAggregateAttributeName, parsedQuery.groupByRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.groupByResultAggregateAttributeName, parsedQuery.groupByRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }

    return true;
}

int aggregateFunction(vector<vector<int>> combinedrows, int columnIndex, string func)
{

    int mini = INT_MAX, maxi = INT_MIN, sum = 0, count = 0, avg = 0;

    count = combinedrows.size();
    for (int p = 0; p < combinedrows.size(); p++)
    {
        mini = min(mini, combinedrows[p][columnIndex]);
        maxi = max(maxi, combinedrows[p][columnIndex]);
        sum += combinedrows[p][columnIndex];
    }
    avg = sum / count;

    if (func == "MAX")
    {
        return maxi;
    }
    else if (func == "MIN")
    {
        return mini;
    }
    else if (func == "SUM")
    {
        return sum;
    }
    else if (func == "AVG")
    {
        return avg;
    }
    else if (func == "COUNT")
    {
        return count;
    }

    return 0;
}

bool isTaken(int havingValue, int checkingValue, int operation)
{

    if (operation == 0 and havingValue < checkingValue)
    {
        // cout << havingValue << " " << checkingValue << " "
        //      << "<" << endl;
        return true;
    }
    else if (operation == 1 and havingValue > checkingValue)
    {
        // cout << havingValue << " " << checkingValue << " "
        //      << ">" << endl;
        return true;
    }
    else if (operation == 2 and havingValue <= checkingValue)
    {
        // cout << havingValue << " " << checkingValue << " "
        //      << "<=" << endl;
        return true;
    }
    else if (operation == 3 and havingValue >= checkingValue)
    {
        // cout << havingValue << " " << checkingValue << " "
        //      << ">=" << endl;
        return true;
    }
    else if (operation == 4 and havingValue == checkingValue)
    {
        // cout << havingValue << " " << checkingValue << " "
        //      << "==" << endl;
        return true;
    }
    else if (operation == 5 and havingValue != checkingValue)
    {
        // cout << havingValue << " " << checkingValue << " "
        //      << "!=" << endl;
        return true;
    }

    return false;
}

void makeTable(Table *resultantTable, vector<vector<int>> &res)
{

    logger.log("makeTable Called " + to_string(res.size()));

    //     Table *resultTable = new Table(originalTableName + "_copy", table->columns);
    tableCatalogue.insertTable(resultantTable);

    for (int i = 0; i < res.size(); i++)
    {
        // logger.log("res[i] " + to_string(res[i].size()));

        resultantTable->writeRow<int>(res[i]);
    }

    resultantTable->blockify();

    return;
}

void executeGROUPBY()
{

    logger.log("executeGROUPBY");
    parsedQuery.sortRelationName = parsedQuery.groupByRelationName;
    parsedQuery.sortResultRelationName = parsedQuery.groupByResultRelationName;

    Table *table_old = tableCatalogue.getTable(parsedQuery.sortRelationName);
    string temp_sort = "temp";
    Table *table = create_sorted_copy_for_table(temp_sort, table_old);

    // auto table = new Table(parsedQuery.sortResultRelationName);
    // *table = *table_old;
    // table->sourceFileName = "../data/" + parsedQuery.sortResultRelationName + ".csv";
    // table->tableName = parsedQuery.sortResultRelationName;
    // tableCatalogue.insertTable(table);
    // parsedQuery.sortRelationName = parsedQuery.sortResultRelationName;

    // Table *table = tableCatalogue.getTable(parsedQuery.groupByRelationName);
    // table->print();

    vector<string> columns = {parsedQuery.groupByGroupingAttributeName, parsedQuery.groupByResultAggregateOperatorName + parsedQuery.groupByResultAggregateAttributeName};
    // Cursor cursor(parsedQuery.sortResultRelationName, 0);
    Cursor cursor(temp_sort, 0);

    int groupingIndex = table->getColumnIndex(parsedQuery.groupByGroupingAttributeName);
    int havingIndex = table->getColumnIndex(parsedQuery.havingAggregateAttributeName);
    int resultIndex = table->getColumnIndex(parsedQuery.groupByResultAggregateAttributeName);

    vector<int> temp;
    vector<int> firstrow;
    vector<vector<int>> combinedrows;
    vector<vector<int>> res;
    int row_cnt = 0;

    for (int i = 0; i < table->rowCount; i++)
    {

        if (i == 0)
        {
            firstrow = cursor.getNext();
        }
        else
        {
            firstrow = temp;
            // temp.clear();
        }

        combinedrows.push_back(firstrow);
        while (1)
        {
            temp = cursor.getNext();

            if (i + 1 >= table->rowCount or firstrow[groupingIndex] != temp[groupingIndex])
            {

                break;
            }

            i++;
            combinedrows.push_back(temp);
        }

        int havingValue = aggregateFunction(combinedrows, havingIndex, parsedQuery.havingAggregateOperatorName);

        int resultValue = aggregateFunction(combinedrows, resultIndex, parsedQuery.groupByResultAggregateOperatorName);

        if (isTaken(havingValue, stoi(parsedQuery.havingIntegerValue), parsedQuery.groupByBinaryOperator))
        {

            // cout << "resulted rows " << firstrow[groupingIndex] << " " << resultValue << endl;
            res.push_back({firstrow[groupingIndex], resultValue});
            row_cnt++;
        }
        // cout << "fursiei " << firstrow[groupingIndex] << " " << resultValue << " " << res.size() << endl;

        combinedrows.clear();
    }

    Table *resultantTable = new Table(parsedQuery.sortResultRelationName, columns, true);
    // cout << "hello2" << endl;
    resultantTable->rowCount = row_cnt;
    resultantTable->blockCount = ceil((resultantTable->rowCount * 1.0) / resultantTable->maxRowsPerBlock);
    // cout << resultantTable->rowCount << " " << resultantTable->blockCount << " " << resultantTable->maxRowsPerBlock <<endl;
    for (int i = 0; i < resultantTable->blockCount - 1; i++)
    {
        resultantTable->rowsPerBlockCount.push_back(resultantTable->maxRowsPerBlock);
    }
    resultantTable->rowsPerBlockCount.push_back(resultantTable->rowCount % resultantTable->maxRowsPerBlock);
    tableCatalogue.insertTable(resultantTable);

    int counter = 0;
    int row_num = 1;
    vector<vector<int>> page;
    vector<int> row = res[counter++];
    page.push_back(row);
    int page_num = 0;
    int maxRowsPerBlock2 = resultantTable->maxRowsPerBlock;

    while (counter < res.size())
    {
        if (row_num % maxRowsPerBlock2 == 0)
        {
            bufferManager.writePage(parsedQuery.sortResultRelationName, page_num, page, page.size());
            page.clear();
            page_num++;
        }
        row = res[counter++];
        page.push_back(row);
        row_num++;
    }

    bufferManager.writePage(parsedQuery.sortResultRelationName, page_num, page, page.size());

    page.clear();

    tableCatalogue.deleteTable(temp_sort);

    return;
}