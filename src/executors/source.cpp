#include "global.h"
using namespace std;

/**
 * @brief 
 * SYNTAX: SOURCE filename
 */
bool syntacticParseSOURCE()
{
    logger.log("syntacticParseSOURCE");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = SOURCE;
    parsedQuery.sourceFileName = tokenizedQuery[1];
    return true;
}

bool semanticParseSOURCE()
{
    logger.log("semanticParseSOURCE");
    if (!isQueryFile(parsedQuery.sourceFileName))
    {
        cout << "SEMANTIC ERROR: File doesn't exist" << endl;
        return false;
    }
    return true;
}


void executeSOURCE() {

    logger.log("executeSOURCE");
    regex delim("[^\\s,]+");
    string query;
    tokenizedQuery.clear();
    // parsedQuery.clear();

    string filePath = "../data/" + parsedQuery.sourceFileName + ".ra";
    ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        logger.log("File not found: " + filePath);
        return;
    }


    while (getline(inputFile, query)) {
        
        auto words_begin = sregex_iterator(query.begin(), query.end(), delim);
        auto words_end = sregex_iterator();
        for (sregex_iterator i = words_begin; i != words_end; ++i)
            tokenizedQuery.emplace_back((*i).str());

        if (tokenizedQuery.size() == 1 && tokenizedQuery.front() == "QUIT")
        {
            break;
        }

        if (tokenizedQuery.empty())
        {
            continue;
        }

        if (tokenizedQuery.size() == 1)
        {
            cout << "SYNTAX ERROR" << endl;
            continue;
        }
        logger.log("Executing query: " + query);
        // cout << query << endl;
        doCommand();

        tokenizedQuery.clear();
        // executeQuery(query);
    }

    inputFile.close();
}
