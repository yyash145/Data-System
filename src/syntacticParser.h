#include "tableCatalogue.h"

using namespace std;

enum QueryType
{
    CLEAR,
    CROSS,
    DISTINCT,
    EXPORT,
    INDEX,
    JOIN,
    LIST,
    LOAD_MATRIX,
    COMPUTE,
    LOAD,
    PRINT_MATRIX,
    TRANSPOSE_MATRIX,
    EXPORT_MATRIX,
    CHECKSYMMETRY,
    PRINT,
    PROJECTION,
    RENAME,
    RENAME_MATRIX,
    SELECTION,
    SORT,
    ORDER_BY,
    GROUPBY,
    SOURCE,
    UNDETERMINED
};

enum BinaryOperator
{
    LESS_THAN,
    GREATER_THAN,
    LEQ,
    GEQ,
    EQUAL,
    NOT_EQUAL,
    NO_BINOP_CLAUSE
};

// enum SortingStrategy
// {
//     ASC,
//     DESC,
//     NO_SORT_CLAUSE
// };

enum SelectType
{
    COLUMN,
    INT_LITERAL,
    NO_SELECT_CLAUSE
};

class ParsedQuery
{

public:
    QueryType queryType = UNDETERMINED;

    string clearRelationName = "";

    string crossResultRelationName = "";
    string crossFirstRelationName = "";
    string crossSecondRelationName = "";

    string distinctResultRelationName = "";
    string distinctRelationName = "";

    string exportRelationName = "";

    IndexingStrategy indexingStrategy = NOTHING;
    string indexColumnName = "";
    string indexRelationName = "";

    BinaryOperator joinBinaryOperator = NO_BINOP_CLAUSE;
    string joinResultRelationName = "";
    string joinFirstRelationName = "";
    string joinSecondRelationName = "";
    string joinFirstColumnName = "";
    string joinSecondColumnName = "";

    string loadRelationName = "";

    string printRelationName = "";

    string projectionResultRelationName = "";
    vector<string> projectionColumnList;
    string projectionRelationName = "";

    string renameFromColumnName = "";
    string renameToColumnName = "";
    string renameRelationName = "";

    string renameFromMatrixName = "";
    string renameToMatrixName = "";

    SelectType selectType = NO_SELECT_CLAUSE;
    BinaryOperator selectionBinaryOperator = NO_BINOP_CLAUSE;
    string selectionResultRelationName = "";
    string selectionRelationName = "";
    string selectionFirstColumnName = "";
    string selectionSecondColumnName = "";
    int selectionIntLiteral = 0;

    // SortingStrategy sortingStrategy = NO_SORT_CLAUSE;
    string sortResultRelationName = "";
    // string sortColumnName = "";

    vector<string> sortColumnName;
    vector<string> sortingStrategy;

    string sortRelationName = "";

    string sourceFileName = "";

    BinaryOperator groupByBinaryOperator = NO_BINOP_CLAUSE;
    string groupByResultRelationName = "";
    string groupByGroupingAttributeName = "";
    string groupByRelationName = "";
    string havingAggregateOperatorName = "";
    string havingAggregateAttributeName = "";
    string havingIntegerValue = "";
    string groupByResultAggregateOperatorName = "";
    string groupByResultAggregateAttributeName = "";

    ParsedQuery();
    void clear();
};

bool syntacticParse();
bool syntacticParseCLEAR();
bool syntacticParseCROSS();
bool syntacticParseDISTINCT();
bool syntacticParseEXPORT();
bool syntacticParseINDEX();
bool syntacticParseJOIN();
bool syntacticParseLIST();
bool syntacticParseLOAD();
bool syntacticParseCOMPUTE();
bool syntacticParsePRINT_MATRIX();
bool syntacticParseLOAD_MATRIX();
bool syntacticParseTRANSPOSE_MATRIX();
bool syntacticParseCHECKSYMMETRY();
bool syntacticParseEXPORT_MATRIX();
bool syntacticParseGROUPBY();
bool syntacticParsePRINT();
bool syntacticParsePROJECTION();
bool syntacticParseRENAME();
bool syntacticParseRENAME_MATRIX();
bool syntacticParseSELECTION();
bool syntacticParseSORT();
bool syntacticParseORDER_BY();
bool syntacticParseSOURCE();

bool isFileExists(string tableName);
bool isQueryFile(string fileName);
