#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <regex>
#include <map>

/*

// Function to split a string by underscores, hyphens, or spaces
std::vector<std::string> splitString(const std::string& input) {
    std::regex regex("[_\\-\\s]+");
    std::sregex_token_iterator iterator(input.begin(), input.end(), regex, -1);
    std::vector<std::string> tokens(iterator, {});
    return tokens;
}

// Function to check if a string follows the underscore naming rule
bool followsUnderscoreRule(const std::string& text) {
    if (text.empty() || text.front() == '_' || text.back() == '_')
        return false;

    int continuousUnderscoreCount = 0;
    for (char letter : text) {
        if (std::isupper(letter))
            return false;
        if (std::isdigit(letter))
            continue;
        if (letter == '_')
            continuousUnderscoreCount++;
        else
            continuousUnderscoreCount = 0;
        if (continuousUnderscoreCount >= 2)
            return false;
    }
    return true;
}

// Function to check if a string follows the KebabCase naming rule
bool followsKebabCaseRule(const std::string& text) {
    if (text.empty() || text.front() == '-' || text.back() == '-')
        return false;

    int continuousDashCount = 0;
    for (char letter : text) {
        if (std::isupper(letter))
            return false;
        if (std::isdigit(letter))
            continue;
        if (letter == '-')
            continuousDashCount++;
        else
            continuousDashCount = 0;
        if (continuousDashCount >= 2)
            return false;
    }
    return true;
}

// Function to check if a string follows the camelCase naming rule
bool followsCamelCaseRule(const std::string& text) {
    if (text.find('_') != std::string::npos || text.find('-') != std::string::npos)
        return false;
    if (!std::islower(text[0]))
        return false;

    bool isUpper = false;
    int continuousUpperLetterCount = 0;
    for (size_t i = 1; i < text.length(); i++) {
        if (std::isupper(text[i])) {
            continuousUpperLetterCount++;
            isUpper = true;
        } else {
            continuousUpperLetterCount = 0;
        }
        if (continuousUpperLetterCount >= 2)
            return false;
    }
    return isUpper;
}

// Function to check if a string follows the PascalCase naming rule
bool followsPascalCaseRule(const std::string& text) {
    if (text.find('_') != std::string::npos || text.find('-') != std::string::npos)
        return false;
    if (!std::isupper(text[0]))
        return false;

    bool isLower = false;
    int continuousUpperLetterCount = 0;
    for (size_t i = 1; i < text.length(); i++) {
        if (std::isupper(text[i]))
            continuousUpperLetterCount++;
        else
            continuousUpperLetterCount = 0;
        if (continuousUpperLetterCount >= 2)
            return false;
        isLower = true;
    }
    return isLower;
}

// Function to handle exceptions
bool handleException(const std::string& text, const std::vector<std::string>& exceptionList) {
    if (text.front() == '_' || text.front() == '-' || text.back() == '_' || text.back() == '-')
        return false;

    for (const std::string& word : exceptionList) {
        if (text.find(word) != std::string::npos)
            return true;
    }
    return false;
}

// Function to check if a word is in WordNet or the exception list
bool wordnetOrExceptionWordRule(const std::string& phrase, const std::vector<std::string>& exceptionList) {
    std::vector<std::string> words = splitString(phrase);

    for (const std::string& word : words) {
        // Check WordNet
        // Note: You would need a C++ library for WordNet integration to implement this part.
        // If you can find a suitable library, you can replace this comment with the code for checking WordNet.

        // Check the exception list
        if (std::find(exceptionList.begin(), exceptionList.end(), word) == exceptionList.end())
            return false;
    }

    return true;
}

// Function to check the grammatical rule
bool grammarRule(const std::string& phrase, const std::vector<std::string>& prepositions) {
    std::vector<std::string> words = splitString(phrase);

    if (!words.empty() && (std::find(prepositions.begin(), prepositions.end(), words.front()) != prepositions.end() ||
        std::find(prepositions.begin(), prepositions.end(), words.back()) != prepositions.end())) {
        return false;
    }

    return true;
}

// Function to check if a phrase contains a verb
bool containsVerb(const std::string& phrase, const std::map<std::string, std::string>& abbreviationFullForm) {
    std::vector<std::string> words = splitString(phrase);

    for (const std::string& word : words) {
        std::string lowercaseWord = word;
        std::transform(lowercaseWord.begin(), lowercaseWord.end(), lowercaseWord.begin(), ::tolower);

        auto it = abbreviationFullForm.find(lowercaseWord);
        if (it != abbreviationFullForm.end()) {
            lowercaseWord = it->second;
        }

        // Check if the word is a verb
        // Note: You would need a library or an API for natural language processing to implement this part.
        // You can replace this comment with the code for checking if a word is a verb.
        // This may require the use of an NLP library like NLTK or spaCy in Python.

        // For simplicity, we'll assume all words are nouns in this example.
        if (true) {
            return true;
        }
    }

    return false;
}

// Function to check for single-letter words
bool containsSingleLetterWord(const std::string& phrase) {
    std::vector<std::string> words = splitString(phrase);

    for (const std::string& word : words) {
        if (word.length() == 1) {
            return true;
        }
    }

    return false;
}

// Function to check if a phrase starts with a special word
bool startsWithSpecialWord(const std::string& phrase, const std::vector<std::string>& specialList) {
    std::vector<std::string> words = splitString(phrase);

    if (!words.empty()) {
        for (const std::string& specialWord : specialList) {
            if (words.front() == specialWord) {
                return true;
            }
        }
    }

    return false;
}
*/

// // Define a function to load a file
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

// Define a function to score phrases
std::pair<std::vector<std::string>, std::vector<std::string> > score(const std::vector<std::string>& phrases) {
    std::vector<std::string> output;
    std::vector<std::string> labelScores;

    // Implement your scoring logic here

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

    // std::ofstream labelFile(labelFilePath);
    // if (labelFile.is_open()) {
    //     labelFile << "Name,Label,Comments" << std::endl;
    //     for (const std::string& line : labelScores) {
    //         labelFile << line << std::endl;
    //     }
    //     labelFile.close();
    // } else {
    //     std::cerr << "Error: Could not open label file." << std::endl;
    // }

    return 0;
}


