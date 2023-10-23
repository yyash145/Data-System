# DSM23 Project Phase 2 Report

## IMPLEMENTATION OF EXTERNAL SORTING

```
Command:SORT <table_name> BY <column_name1, column_name2,..., column_namek> IN <ASC|DESC, ASC|DESC,..., ASC|DESC>
```

1- First all pages for a table(T) are sorted individually and its done in inplace fashion. For this we are using custom comparator as sorting can be based on any number of columns and there sorting strategy is also designed which can be on ASC/DESC basis.

2- And now we will use a priority queue like its used in K-way merge sort. priority queue elements will be of type
pair<vector<int>, pair<int,int> > where first vector<int> represents the values of the columns on basis of you want to sort in order of sorting.

3- Sorting startegy is pushed in a separate vector<int> (1 for ASC and 0 for DESC).
The first int refers to the page number of page and second int is the row number in that page you are interested in.
priority queue also has its comparator which is written in a class.

4- Since its given that maximum 10 blocks can be taken in buffer. We will pick the first 9 pages in buffer and add there first element in priority queue. Then will run a while loop till pq is empty and pop elements from it

5- For each element we will call a writeRow function of Table class which will write that row in a table created initially and when all the elements are added in the table then that table will be in sorted order.

6- And after we call table->blockify all pages will be created for that table which will be temporary and after next stage of merging is done this previos table of 9 pages will be deleted.

7-Now since all pages were coverted into tables, we need to merge these tables where each table is sorted individually
and create a cursor for each table

8- Will iterate thorought the whole table as we keep on adding elements from that table in priority queue in order to sort for second stage of merging. And eventually we will delete the previos tables once we get the new merged tables.

9- We will keep on doing this until we are left with only one sorted table in the end.
This way we will get our final sorted table then just to buffer.write_page to create pages for that table.

## JOIN

```
Command: <new_relation_name> <- JOIN <tablename1>, <tablename2> ON <column1> <bin_op> <column2>
```

### Logic

-   We implement JOIN by taking sorted tables(SORT-MERGE JOIN).

-   We implemented different codes for all the `binary operators`, that is being asked from us, i.e., `"==", ">", ">=", "<", "<="`.

-   In each of these, we traverse through the Infinity loop, _while(true)_ and returns when all the rows in _Table1_ completes.

-   We move the Moving Cursor of both the tables and point them to their respective position expained below.

-   Let _joinFirstColumnValue, joinSecondColumnValue_ be the values that we compare in both the Tables.

#### `Equals to (==)`

-   We use 3 cursors for _Table1_, namely cursor1a, cursor1b, cursor1c and 2 cursors for _Table2_, namely cursor2a, cursor2b.

-   _Cursor1a_ will be our static cursor for Table1, and will move when _Table1_ Comparision Value < _Table2_ Comparision Value.

-   _Cursor2a_ will be our static cursor for Table2, and will move when _Table1_ Comparision Value > _Table2_ Comparision Value.

-   Since, we don't have _cursor.getPrevious_, so we used _cursor1b, cursor2b_ (Moving Cursors) for _getPrevious_ functionality.

    -   Perform cursor1.getPrevious() on table1 at every iteration.
    -   When _Table1_ Comparision Value == _Table2_ Comparision Value and we are at the last row of Table2, points _Moving Cursor_ to the _Static_.
    -   When Table2 completes, set _Moving Cursor_ of Table2 to the _Static Cursor_.
    -   When _Table1_ Comparision Value != _Table2_ Comparision Value, points _Moving Cursor_ of Table 2 to the _Static Cursor_.

-   Since _cursor.getNext()_ returns the data in that particular row and if we need data from the next row, we use another cursor for it, i.e., _cursor1c_.

    -   At the start, we point _cursor1c_ to its next row
    -   Move it at the end of Table2.
    -   Move it, when _Table1_ Comparision Value < _Table2_ Comparision Value

-   _if(joinFirstColumnValue < joinSecondColumnValue):_

    -   Increment all Cursors of Table1
    -   Point _Moving Cursor_ of Table2 to its _Static Cursor._

-   _if(joinFirstColumnValue == joinSecondColumnValue):_

    -   We append the data of both the tables in the row and push in in the page.
    -   Move the _Moving Cursor_ to the _Static Cursor_.
    -   We check if it is the last row, increment Static Cursor of First Table.
    -   Have a note of Previous Checked value of both the Tables.

-   _if(joinFirstColumnValue > joinSecondColumnValue):_
    -   Move the _Static Cursor_ to the next value of Table2.
    -   Move the _Moving Cursor_ to the _Static Cursor_ of Table2.
    -   Move the _Moving Cursor_ to the _Static Cursor_ of Table1.

#### `Less Than (<) & Less Than and Equals to (<=)`

-   We use three cursors for _Table1_, namely cursor1a, cursor1b, cursor1c and two cursors for _Table2_, namely cursor2a, cursor2b.

-   _Cursor1a_ will be our static cursor for Table1, and will move when Table2 got completed.

-   _Cursor2a_ will be our static cursor for Table2, and will move when condition gets false, i.e., _Table1_ Comparision Value >= (or >) _Table2_ Comparision Value.

-   Since, we don't have _cursor.getPrevious_, so we used _cursor1b, cursor2b_ (Moving Cursors) for _getPrevious_ functionality.

    -   In _while(true)_ loop, we do _cursor1b.getNext()_ for every iteration and after checking that table 2 doesn't finishes, we point this cursor to the static cursor, i.e., _cursor1a_.
    -   In _while(true)_ loop, we do _cursor2b.getNext()_ when second Table ends, we point to our static Cursor.
    -   In the same loop, when condition doesn't satisfies, i.e., _Table1_ Comparision Value >= (or >) _Table2_ Comparision Value, we point static cursor to our moving cursor as a part of the optimization.

-   Since _cursor.getNext()_ returns the data in that particular row and if we need data from the next row, we use another cursor for it, i.e., _cursor1c_.
    -   At the start, we point _cursor1c_ to its next row and move it, whenever we move our static cursor, i.e., at the end of Table2.

#### `Less Than (<)`

-   _if(joinFirstColumnValue < joinSecondColumnValue):_

    -   We append the data of both the tables in the row and push in in the page.
    -   Move the _Moving Cursor_ to the _Static Cursor_.

-   _if(joinFirstColumnValue >= joinSecondColumnValue):_
    -   Move the _Static Cursor_ to the next value of Table2.
    -   Move the _Moving Cursor_ to the _Static Cursor_ of Table 2.
    -   Move the Static Cursor\_ to the next row (Moving Cursor) in Table1.

#### `Less Than and Equals to (<=)`

-   _if(joinFirstColumnValue <= joinSecondColumnValue):_

    -   We append the data of both the tables in the row and push in in the page.
    -   Move the _Moving Cursor_ to the _Static Cursor_.

-   _if(joinFirstColumnValue > joinSecondColumnValue):_
    -   Move the _Static Cursor_ to the next value of Table2.
    -   Move the _Moving Cursor_ to the _Static Cursor_ of Table 2.
    -   Move the Static Cursor\_ to the next row (Moving Cursor) in Table1.

#### `Greater Than (>) and Greater Than and Equals to (>=)`

-   We use 3 cursors for _Table1_, namely cursor1a, cursor1b, cursor1c and 2 cursors for _Table2_, namely cursor2a, cursor2b.

-   _Cursor1a_ will be our static cursor for _Table1_, and will move:

    -   When _Table2_ got completed.
    -   When the condition gets false, i.e., the value in _Table1_ is smaller than (or equals to) that of _Table2_, then we keep our static cursor to that particular Value, to eliminate extra Search(Read) operations, as an optimization step.

-   _Cursor2a_ will be our static cursor for Table2, and will always be at the first row of Table2, because the condition demands that.

-   Since, we don't have _cursor.getPrevious_, so we used _cursor1b, cursor2b_ (Moving Cursors) for _getPrevious_ functionality.

    -   In _while(true)_ loop, we do _cursor1b.getNext()_ and when the condition satifies, we point this cursor to the static cursor (Previous Cursor), i.e., _cursor1a_.
    -   In _while(true)_ loop, we do _cursor2b.getNext()_ when the second Table ends, we point to our Static Cursor (at the top).
    -   In _while(true)_ loop, when condition doesn't satisfies, i.e., _Table1_ Comparision Value < (or <= ) _Table2_ Comparision Value, we point cursor2b to the Static Cursor.

-   Since _cursor.getNext()_ returns the data in that particular row and if we need data from the next row, we use another cursor for it, i.e., _cursor1c_.
    -   At the start, we point _cursor1c_ to its next row and move it, whenever we move our static cursor, i.e., at the end of Table2.
    -   When Table2 completes, we increment the _cursor1c_ pointer to its next row.
    -   we need not to do comparision when Table1 value <= (or <) Table2 Value, when the data is in sorted order. So we increment this cursor, _cursor1c_ and points to the next row.

#### `Greater Than (>)`

-   _if(joinFirstColumnValue <= joinSecondColumnValue):_

    -   Point the _Moving Cursor_ to the _Static Cursor_ of Table 1.
    -   Move the Static Cursor\_ to the next row in Table2.
    -   Move _Cursor1c_ to its next value, to update the data for next iteration.

-   _if(joinFirstColumnValue > joinSecondColumnValue):_
    -   We append the data of both the tables in the row and push in in the page.
    -   Move the _Moving Cursor_ to the _Static Cursor_, i.e., at the top.

#### `Greater Than and Equals to (>=)`

-   _if(joinFirstColumnValue < joinSecondColumnValue):_

    -   Point the _Static Cursor_ to the _Moving Cursor_ of Table 1, i.e., increment Table1 Pointer.
    -   Point the _Moving Cursor_ to the _Static one_.
    -   Move _Cursor1c_ to its next value, to update the data for next iteration.

-   _if(joinFirstColumnValue >= joinSecondColumnValue):_
    -   We append the data of both the tables in the row and push in in the page.
    -   Move the _Moving Cursor_ to the _Static Cursor_, i.e., at the top.

## ORDER BY

```
Command:<new_table> <- ORDER BY <attribute> ASC|DESC ON <table_name>

```

1- Order By is using sort function only but before calling the sort function we will check syntax and semantics.\
2- And then create copy for the original table and creating copy pages using bufferManager.writePage with the name same as resultant table where you want to save the input.\
3- After that just call the sort function with table as resultant table and then it sorts the resultant table inplace and get the desired output.

## GROUP BY with HAVING clause

```
Command:<new_table> <- GROUP BY <grouping_attribute> FROM <table_name> HAVING <aggregate(attribute)> <bin_op> <attribute_value> RETURN <aggregate_func(attribute)>
```

1- Firstly, use an external sorting algorithm to arrange the relation in ascending order based on the specified GROUP_BY attribute.\
2- Next, gather all the tuples associated with each unique group value, and subsequently, apply the specified aggregate function as outlined in the HAVING clause.\
3- For every row within each group that fulfilled the conditions specified in the HAVING clause, pass those rows into the aggregate function mentioned in the RETURN clause.\
4- Finally, store the resulting output in the given result relation.

## Assumptions:

-   If it given to Sort on basis of some columns but still there are some rows with same values for that columns, then in the it will sort on basis of ASC order of Primary Key

-   Output result table should not be empty means condition needs to be true for some rows

## Learning

-   Thoroughly understood Table and Buffer Manager class working and purpose and its need to delete files from buffer.

-   Groupby and Join Algorithm Logic and code, which earlier was unknown.

-   Learned to perform debugging by using a deeper understanding of the repository.

## Contribution

1 - **Nikhil Chawla(2022201045)**\
 SORT, ORDER BY, Integeration \
 `Contribution percentage - 33.33%`\

2 - **Rishabh Gupta(2022202011)**\
 GROUP BY with HAVING clause, Integeration\
 `Contribution percentage - 33.33%`

3 - **Yash Maheshwari(2022201074)**\
 JOIN, Integeration\
 `Contribution percentage - 33.33%`\
