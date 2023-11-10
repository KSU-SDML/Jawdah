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
    
    // must start with capital letter
    if (islower(str[0])) return false;

    int consecutiveUpper = 0;
    for (int i = 1; i < str.length() - 1; ++i) {
        if (std::isalpha(str[i])) {
            if (isupper(str[i])) ++consecutiveUpper;
            else consecutiveUpper = 0;
            if (consecutiveUpper >= 2) return false;
        } else {
            return false;
        }
    }
    return true;
}

bool followCamel(const std::string& str) {

    //must start with lowercase letter
    if (isupper(str[0])) return false;

    int consecutiveUpper = 0;
    for (int i = 1; i < str.length() - 1; ++i) {
        if (std::isalpha(str[i])) {
            if (isupper(str[i])) ++consecutiveUpper;
            else consecutiveUpper = 0;
            if (consecutiveUpper >= 2) return false;
        } else {
            return false;
        }
    }
    return true;
}

bool followKebab(const std::string& str) {
    
    if ((str[0] == '-') || (str[str.length() - 1] == '-')) return false;

    int consecutiveDash = 0;
    bool containsDash = false;
    for (int i = 1; i < str.length() - 2; ++i) {
        if (str[i] == '-') ++consecutiveDash;
        else consecutiveDash = 0;
        if (consecutiveDash >= 1) containsDash = true;
        if (consecutiveDash >= 2) return false;
    }
    
    if (containsDash) return true;
    else return false;
}

bool followUnderscore(const std::string& str) {
    
    if ((str[0] == '_') || (str[str.length() - 1] == '_')) return false;

    int consecutiveUnderscore = 0;
    bool containsUnderscore = false;
    for (int i = 1; i < str.length() - 2; ++i) {
        if (str[i] == '_') ++consecutiveUnderscore;
        else consecutiveUnderscore = 0;
        if (consecutiveUnderscore >= 1) containsUnderscore = true;
        if (consecutiveUnderscore >= 2) return false;
    }

    if (containsUnderscore) return true;
    else return false;
}

void followStyle(const std::string& str, Report& report) {
    if (followPascal(str) || followUnderscore(str) || followKebab(str) || followCamel(str)) {
        
    } else {
        report.score -= 1;
        report.comments += "Follow a naming style,";
    }
}

void singleLetterRule(const std::string& str, const std::string& taggedWords, Report& report) {
    // need to anlayze each split word
    std::string splitWords;
    if (taggedWords[0] == '\"') 
        splitWords = taggedWords.substr(1, taggedWords.length()-2);
    else {
        splitWords = taggedWords;
        // this means it is a single word
        // check if length > 1
    }

    std::vector<std::string> words;
    // split into individual words, disregard tags
    int start = 0, end = 0;
    while (end <= splitWords.length() - 1) {
        if (splitWords[end] != '|' && splitWords[end] != ',') ++end;
        else if (splitWords[end] == ',') {
            ++end;
            start = end;
        }
        else {
            words.push_back(splitWords.substr(start, end - start));
            // now reassign start and end after comma
            ++end;
            start = end;
        }
    }
    
    for (const auto& str : words) {
        if (str.length() == 1) {
            report.score -= 1;
            report.comments += "name should not contain single letter \'" + str + "\'";
        }
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
Report phraseScore(std::string& name, std::string& context, std::string& taggedWords) {

    Report phraseReport;    
    phraseReport.name = name;

    // naming style rule (1)
    followStyle(name, phraseReport);

    // single-letter name rule (3)
    singleLetterRule(name, taggedWords, phraseReport);

    // underscore rule (4)
    hasConsecutiveUnderscores(name, phraseReport);

    // leading/Trailing rule (5)
    hasLeadingTrailingUnderscore(name, phraseReport);

    // character limit rule (7)
    maxCharacters(name, phraseReport);

    return phraseReport;
}

void labelScore(Report& report) {
    
    if (report.score < 8) 
        report.label = "Poor Method Name";
    else if (report.score < 9) 
        report.label = "Okay Method Name";
    else if (report.score < 10) 
        report.label = "Fair Method Name";
    else 
        report.label = "Excellent Method Name";
}


std::vector<Report> score(const std::vector<std::string>& phrases) {
    
    // Holds report for each identifier
    std::vector<Report> reports;

    // Loop through each row in the input csv
    for (const std::string& entry : phrases) {
        std::stringstream ss(entry);
        std::string name, context, fileName, location, taggedWords;

        // assign name, context vars
        std::getline(ss, name, ',');
        std::getline(ss, context, ',');
        std::getline(ss, fileName, ',');
        std::getline(ss, location, ',');
        ss >> taggedWords;
        
        // send off each line to be scored, return report
        Report identifierReport =  phraseScore(name, context, taggedWords);
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


