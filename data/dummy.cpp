#include <iostream>
#include <vector>
#include <sstream>
using namespace std;
#include <fstream>


// int main(){
//     string line;
//     const char* sourceFileName = "AA.csv";
//     ifstream fin(sourceFileName, ios::in);
//     // getline(fin, line);
//     // logger.log(line);
    
//     while (getline(fin, line))
//     {
//         // logger.log(line);
//         cout << line << endl;
//     }



// vector<vector<int>> read_matrix(string table_name, int page_num, int &read_cnt) {
//     std::ifstream inputFile("temp/"+ table_name +"_Page" + to_string(page_num));
//     std::vector<std::vector<int>> matrix;
//     if (!inputFile.is_open()) {
//         std::cerr << "Failed to open the input file." << std::endl;
//         return matrix;
//     }
//     std::string line;
//     while (std::getline(inputFile, line)) {
//         std::vector<int> row;
//         std::istringstream iss(line);
//         int num;
//         while (iss >> num) {
//             row.push_back(num);
//         }
//         matrix.push_back(row);
//     }
//     inputFile.close();
//     read_cnt++;
//     return matrix;
// }

// void writePage(string table_name, int page_num, vector<vector<int>> &rows, int &write_cnt)
// {
//     string page_name = "temp/"+ table_name +"_Page" + to_string(page_num);
//     ofstream fout(page_name, ios::trunc);
//     int rowCount = rows.size();
//     int columnCount = rows[0].size();
//     for (int rowCounter = 0; rowCounter < rowCount; rowCounter++)
//     {
//         for (int columnCounter = 0; columnCounter < columnCount; columnCounter++)
//         {
//             if (columnCounter != 0)
//                 fout << " ";
//             fout << rows[rowCounter][columnCounter];
//         }
//         fout << endl;
//     }
//     fout.close();
//     write_cnt++;
// }

// void printAccessBlocksInfo(int blocks_read, int blocks_written){
//     cout<<"\n\nBlocks Read: "<< blocks_read << endl;
//     cout<<"\n\nBlocks Written: "<< blocks_written << endl;
//     cout<<"\n\nBlocks Accessed: "<< blocks_read + blocks_written <<endl;
// }


// void executeTRANSPOSE_MATRIX()
// {
//     string table_name = "random_matrix";
//     int num_of_pages = 50;
//     // logger.log(table_name);
//     // logger.log(to_string(num_of_pages));
//     // string table_name = "A";
//     // int num_of_pages = 3;
//     int page_num2 = 0;
//     int row = 0;
//     int rows_in_page = 0;
//     int read_cnt = 0;
//     int write_cnt = 0;
    
//     for( int page_num = 0; page_num < num_of_pages; page_num++ ){

//         vector<vector<int>> matrix1 = read_matrix( table_name, page_num, read_cnt);
//         vector<vector<int>> matrix2;
//         int n1 = matrix1.size();
//         if( rows_in_page == 0 )
//             rows_in_page = n1;
//         int m1 = matrix1[0].size();
//         int n2 = 0, m2 = 0;
//         cout << "page:" << page_num << " " << row << " " << n1 << m1 << endl;
        
//         for( int i = 0; i < n1; i++ ){
//             for( int j = row+1; j < m1; j++ ){
                
//                 if( j < ((page_num+1)*rows_in_page) ){
//                     // cout << j << "-" << (page_num+1)*rows_in_page << endl;
//                     swap( matrix1[i][j], matrix1[j % rows_in_page][ page_num*rows_in_page + i]);
//                     // cout << matrix1[i][j] << "--" <<  matrix1[j % rows_in_page][ page_num*rows_in_page + i ] << endl;
//                 }
//                 else{
//                     if( n2 == 0 or ((j % rows_in_page) == 0) ){
//                         if( page_num+1 < rows_in_page ){
//                             page_num2 = j / rows_in_page;
//                             matrix2 = read_matrix( table_name, page_num2, read_cnt );
//                             n2 = matrix2.size();
//                             m2 = matrix2[0].size();
//                         }
//                     }
//                     swap( matrix1[i][j], matrix2[ j % rows_in_page ][ page_num*rows_in_page + i] );
//                     // cout << matrix1[i][j] << " " << matrix2[ j % rows_in_page ][ page_num*rows_in_page + i] << endl;
//                     if( (j % rows_in_page) == n2-1 ){
//                         writePage( table_name, page_num2, matrix2, write_cnt );
//                         // cout << "write matrix 2: " << page_num2 << endl;
//                     }
//                 }
//             }
//             row++;
//         }
//         writePage( table_name, page_num, matrix1, write_cnt );
//         cout << "write matrix 1" << endl;
//     }
//     printAccessBlocksInfo(read_cnt, write_cnt);
// }

// int main() {
    
//     executeTRANSPOSE_MATRIX();



// }



// int main(){

//     string table_name = "A";
//     int num_of_pages = 3;
//     int page_num2 = 0;
//     int row = 0;
//     int rows_in_page = 0;
//     int read_cnt = 0;
//     int write_cnt = 0;

//     for( int page_num = 0; page_num < num_of_pages; page_num++ ){

//         vector<vector<int>> matrix1 = read_matrix( table_name, page_num, read_cnt);
//         vector<vector<int>> matrix2;
//         int n1 = matrix1.size();
//         if( rows_in_page == 0 )
//             rows_in_page = n1;
//         int m1 = matrix1[0].size();
//         int n2 = 0, m2 = 0;
//         cout << "page:" << page_num << " " << row << " " << n1 << m1 << endl;
        
//         for( int i = 0; i < n1; i++ ){
//             for( int j = row+1; j < m1; j++ ){
                
//                 if( j < ((page_num+1)*rows_in_page) ){
//                     // cout << j << "-" << (page_num+1)*rows_in_page << endl;
//                     cout << matrix1[i][j] << "--" <<  matrix1[j % rows_in_page][ page_num*rows_in_page + i ] << endl;
//                     swap( matrix1[i][j], matrix1[j % rows_in_page][ page_num*rows_in_page + i]);
                    
//                 }
//                 else{
//                     if( n2 == 0 or ((j % rows_in_page) == 0) ){
//                         if( page_num+1 < rows_in_page ){
//                             page_num2 = j / rows_in_page;
//                             matrix2 = read_matrix( table_name, page_num2, read_cnt );
//                             n2 = matrix2.size();
//                             m2 = matrix2[0].size();
//                         }
//                     }
//                     cout << matrix1[i][j] << " " << matrix2[ j % rows_in_page ][ page_num*rows_in_page + i] << " "<< j % rows_in_page << " " << page_num*rows_in_page + i << endl;
//                     swap( matrix1[i][j], matrix2[ j % rows_in_page ][ page_num*rows_in_page + i] );
                    
//                     if( (j % rows_in_page) == n2-1 ){
//                         writePage( table_name, page_num2, matrix2, write_cnt );
//                         cout << "write matrix 2: " << page_num2 << endl;
//                     }
//                 }
//             }
//             row++;
//         }
//         writePage( table_name, page_num, matrix1, write_cnt );
//         cout << "write matrix 1" << endl;
//     }
//     printAccessBlocksInfo(read_cnt, write_cnt);
// }








// int main() {
//     vector<vector<int>> matrix = read_matrix( "A", 0);
//     // Print the 2D vector
//     for (const auto& row : matrix) {
//         for (int num : row) {
//             std::cout << num << " ";
//         }
//         std::cout << std::endl;
//     }
//     matrix[0][0] = 7;
//     matrix[0][1] = 7;

//     writePage( "A", 0, matrix );

//     std::cout << std::endl;

//     for (const auto& row : matrix) {
//         for (int num : row) {
//             std::cout << num << " ";
//         }
//         std::cout << std::endl;
//     }
//     return 0;
// }
