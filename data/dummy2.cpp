#include <iostream>
#include <vector>
#include <sstream>
using namespace std;
#include <fstream>


vector<vector<int>> read_matrix(string table_name, int page_num, int &read_cnt) {
    std::ifstream inputFile("temp2/"+ table_name +"_Page" + to_string(page_num));
    std::vector<std::vector<int>> matrix;
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open the input file." << std::endl;
        return matrix;
    }
    std::string line;
    while (std::getline(inputFile, line)) {
        std::vector<int> row;
        std::istringstream iss(line);
        int num;
        while (iss >> num) {
            row.push_back(num);
        }
        matrix.push_back(row);
    }
    inputFile.close();
    read_cnt++;
    return matrix;
}

int main(){
    // string table_name = "A";
    // int num_of_pages = 3;
    // int read_cnt = 0;

    // auto matrix2 = read_matrix( table_name, 0, read_cnt );

    // int i = 0, j = 0;
    // for( auto vec: matrix2 ){
    //     int j = 0;
    //     for( auto it: vec ){
    //         cout << it << "-(" << i << "," << j << ")" << " | " ;
    //         j++;
    //     }
    //     i++;
    //     cout << endl;
    // }

    // vector<vector<int>> nums(10000,vector<int>(10000,0));
}