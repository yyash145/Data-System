#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

void appendToCSV(const std::vector<int>& nums, const std::string& fileName) {
    // Open the CSV file in append mode (ios::app), which creates the file if it doesn't exist
    std::ofstream file(fileName, std::ios::app);

    if (!file.is_open()) {
        std::cerr << "Error opening the file: " << fileName << std::endl;
        return;
    }

    // Create a stringstream to build the CSV row
    std::stringstream ss;

    // Append each number from the vector to the stringstream
    for (size_t i = 0; i < nums.size(); ++i) {
        ss << nums[i];
        
        // Add a comma (,) if it's not the last element
        if (i < nums.size() - 1) {
            ss << ",";
        }
    }
    
    // Add a newline character to mark the end of the row
    ss << "\n";

    // Write the stringstream to the file
    file << ss.str();

    // Close the file
    file.close();

    std::cout << "Data appended to " << fileName << std::endl;
}

int main() {
    std::vector<int> nums = {1, 2, 3, 5, 5};
    std::string fileName = "data.csv";

    // Call the appendToCSV function
    appendToCSV(nums, fileName);

    return 0;
}