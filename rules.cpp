#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <regex>
#include <map>


// Syntax

bool hasConsecutiveUnderscores(const std::string& str) {
    for (int i = 0; i < str.length() - 1; ++i) {
        if (str[i] == '_' && str[i + 1] == '_') {
            return true;
        }
    }
    return false;
}

// Vocabulary


// Data Type


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

// report data sctructure needs:
// numerical score, comments on naming violations
struct Report {
    std::string name;
    int score = 10;
    std::string label;
    std::string comments = "";
};

// 
Report phraseScore(std::string& name, std::string& context) {

    Report phraseReport;    
    phraseReport.name = name;

    // underscore rule (4)
    if (hasConsecutiveUnderscores(name)){
        phraseReport.score -= 1;
        phraseReport.comments += "remove consecutive underscores,";
    } 

    return phraseReport;
}

void labelScore(Report& report) {
    
    if (report.score < 10) {
        report.label = "Fair Method Name";
    } else {
        report.label = "Excellent Method Name";
    }
}


std::vector<Report> score(const std::vector<std::string>& phrases) {
    std::vector<Report> output;
    // std::vector<std::string> labelScores;

    // Loop through each row in the input csv
    for (const std::string& entry : phrases) {
        std::stringstream ss(entry);
        std::string name, context, restOfPhrase;

        // assign type, name, context vars
        std::getline(ss, name, ',');
        std::getline(ss, context, ',');
        ss >> restOfPhrase;
        
        // send off each line to 'phrase score' function, return report
        Report identifierReport =  phraseScore(name, context);
        output.push_back(identifierReport);
    }
    
    // next add the score label ("good" "bad" etc.)
    for (Report& report : output) {
        labelScore(report);
    }

    return output;
}

int main() {
    const std::string inputFilePath = "output.csv";
    const std::string outputFilePath = "output.txt";
    const std::string labelFilePath = "labels.txt";

    // Load the input file line-by-line
    std::vector<std::string> phrases = loadFile(inputFilePath);

    // Score the phrases
    std::vector<Report> result = score(phrases);

    // Write the output to file
    std::ofstream outputFile(outputFilePath);
    if (outputFile.is_open()) {
        outputFile << "Name,Label,Comments" << std::endl;
        for (const Report& report : result) {
            outputFile << report.name << "," << report.label << "," << report.comments << std::endl;
        }
        outputFile.close();
    } else {
        std::cerr << "Error: Could not open output file." << std::endl;
    }

    return 0;
}


