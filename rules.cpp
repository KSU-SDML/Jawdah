#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <regex>
#include <map>
#include <cctype>


struct Report {
    std::string name;
    int score = 10;
    std::string label;
    std::string comments = "";
};


// Syntax Rules

// Naming Style Methods
bool followPascal(const std::string& str) {
    int consecutiveUpper = 0;
    for (int i = 0; i < str.length() - 1; ++i) {
        if (isupper(str[i])) ++consecutiveUpper;
        else consecutiveUpper = 0;
        if (consecutiveUpper >= 2) return false;
    }
    return true;
}

bool followCamel(const std::string& str) {
    return false;
}

bool followKebab(const std::string& str) {
    return false;
}

bool followUnderscore(const std::string& str) {
    return false;
}

void followStyle(const std::string& str, Report& report) {
    if (followPascal(str) || followUnderscore(str) || followKebab(str) || followCamel(str)) {
        
    } else {
        report.score -= 1;
        report.comments += "Follow a naming style,";
    }
}

void hasConsecutiveUnderscores(const std::string& str, Report& report) {
    for (int i = 0; i < str.length() - 1; ++i) {
        if (str[i] == '_' && str[i + 1] == '_') {
            report.score -= 1;
            report.comments += "remove consecutive underscores,";
        }
    }
}

void hasLeadingTrailingUnderscore(const std::string& str, Report& report) {
    if ((str[0] == '_') || (str[str.length() - 1] == '_')) {
        report.score -= 1;
        report.comments += "remove leading/trailing underscores,";
    }
}

void maxCharacters(const std::string& str, Report& report) {
    if (str.length() > 20) {
        report.score -= 1;
        report.comments += "name length must be <= 20 characters,";
    }
}

// Vocabulary Rules


// Data Type Rules


// reads in each line of identifier information 
std::vector<std::string> loadFile(const std::string& filePath) {
    
    std::vector<std::string> lines;

    // Open the file
    std::ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open input file." << std::endl;
        return lines;
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        lines.push_back(line);
    }

    inputFile.close();
    return lines;
}

// Checks identifier for each rule
Report phraseScore(std::string& name, std::string& context) {

    Report phraseReport;    
    phraseReport.name = name;

    // naming style rule (1)
    followStyle(name, phraseReport);

    // underscore rule (4)
    hasConsecutiveUnderscores(name, phraseReport);

    // leading/Trailing rule (5)
    hasLeadingTrailingUnderscore(name, phraseReport);

    // character limit rule (7)
    maxCharacters(name, phraseReport);

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
    
    // Holds report for each identifier
    std::vector<Report> reports;

    // Loop through each row in the input csv
    for (const std::string& entry : phrases) {
        std::stringstream ss(entry);
        std::string name, context, restOfPhrase;

        // assign name, context vars
        std::getline(ss, name, ',');
        std::getline(ss, context, ',');
        ss >> restOfPhrase;
        
        // send off each line to be scored, return report
        Report identifierReport =  phraseScore(name, context);
        reports.push_back(identifierReport);
    }
    
    // next add the score label ("good" "bad" etc.)
    for (Report& report : reports) {
        labelScore(report);
    }

    return reports;
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


