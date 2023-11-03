#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <regex>
#include <map>

std::vector<std::string> loadFile(const std::string& filePath) {
    std::vector<std::string> phrases;

    // Open the file
    std::ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open input file." << std::endl;
        return phrases;
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        phrases.push_back(line);
    }

    inputFile.close();
    return phrases;
}


std::pair<std::vector<std::string>, std::vector<std::string> > score(const std::vector<std::string>& phrases) {
    std::vector<std::string> output;
    std::vector<std::string> labelScores;

    // Loop through each row in the input csv
        // assign type, name, context vars
        // send off each line to 'phrase score' function, return report
        // next add the score label ("good" "bad" etc.)
    //return report


    return std::make_pair(output, labelScores);
}

int main() {
    const std::string inputFilePath = "output.csv";
    const std::string outputFilePath = "output.txt";
    const std::string labelFilePath = "labels.txt";

    // Load the input file
    std::vector<std::string> phrases = loadFile(inputFilePath);

    // // Score the phrases
    // std::pair<std::vector<std::string>, std::vector<std::string>> result = score(phrases);
    std::vector<std::string> output = phrases;
    // std::vector<std::string> labelScores = result.second;

    // Write the output to files
    std::ofstream outputFile(outputFilePath);
    if (outputFile.is_open()) {
        outputFile << "Name,Score,Comments" << std::endl;
        for (const std::string& line : output) {
            outputFile << line << std::endl;
        }
        outputFile.close();
    } else {
        std::cerr << "Error: Could not open output file." << std::endl;
    }

    return 0;
}


