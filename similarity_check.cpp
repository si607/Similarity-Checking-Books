#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <string>
#include <cctype>
#include <iomanip>
#include <set>

using namespace std;

// Constants
const int NUM_BOOKS = 4; // Adjust according to the number of books
const int TOP_WORDS = 100; // Limit for top frequent words
const vector<string> COMMON_WORDS = {"A", "AND", "AN", "OF", "IN", "THE"}; // Words to ignore

// Function to normalize words (to uppercase, keeping alphanumeric only)
string normalizeWord(const string& word) {
    string normalized;
    for (char c : word) {
        if (isalnum(c)) normalized += toupper(c);
    }
    return normalized;
}

// Function to check if a word is a common (stop) word
bool isCommonWord(const string& word) {
    return find(COMMON_WORDS.begin(), COMMON_WORDS.end(), word) != COMMON_WORDS.end();
}

// Function to count top frequent words in a text file
unordered_map<string, double> getTopFrequentWords(const string& filename) {
    unordered_map<string, int> wordCount;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return {};
    }

    string word;
    int totalWords = 0;

    // Reading and normalizing words
    while (file >> word) {
        word = normalizeWord(word);
        if (isCommonWord(word) || word.empty()) continue;
        wordCount[word]++;
        totalWords++;
    }

    // Calculate normalized frequencies for top words
    unordered_map<string, double> freqWords;
    vector<pair<string, int>> sortedWords(wordCount.begin(), wordCount.end());
    
    // Sort words by frequency and take the top N
    sort(sortedWords.begin(), sortedWords.end(), [](const auto& a, const auto& b) {
        return b.second < a.second;
    });
    
    for (int i = 0; i < min(TOP_WORDS, static_cast<int>(sortedWords.size())); i++) {
        freqWords[sortedWords[i].first] = static_cast<double>(sortedWords[i].second) / totalWords;
    }

    return freqWords;
}

// Function to calculate similarity score between two books based on word frequency
double calculateSimilarity(const unordered_map<string, double>& words1, const unordered_map<string, double>& words2) {
    double similarity = 0.0;
    for (const auto& [word, freq] : words1) {
        if (words2.find(word) != words2.end()) {
            similarity += min(freq, words2.at(word)); // Sum of common words' normalized frequency
        }
    }
    return similarity;
}

// Main Function
int main() {
    vector<string> filenames = {"book1.txt", "book2.txt", "book3.txt", "book4.txt"}; // File names for books
    vector<vector<double>> similarityMatrix(NUM_BOOKS, vector<double>(NUM_BOOKS, 0.0));
    vector<unordered_map<string, double>> bookWords(NUM_BOOKS);

    // Process each book and extract top frequent words
    for (int i = 0; i < NUM_BOOKS; ++i) {
        bookWords[i] = getTopFrequentWords(filenames[i]);
    }

    // Populate similarity matrix
    for (int i = 0; i < NUM_BOOKS; ++i) {
        for (int j = i + 1; j < NUM_BOOKS; ++j) {
            similarityMatrix[i][j] = calculateSimilarity(bookWords[i], bookWords[j]);
            similarityMatrix[j][i] = similarityMatrix[i][j]; // Matrix is symmetric
        }
    }

    // Display similarity matrix
    cout << "Similarity Matrix:\n";
    for (int i = 0; i < NUM_BOOKS; ++i) {
        for (int j = 0; j < NUM_BOOKS; ++j) {
            cout << fixed << setprecision(3) << similarityMatrix[i][j] << " ";
        }
        cout << endl;
    }

    // Find top 10 similar pairs (excluding self-similarity)
    vector<tuple<int, int, double>> similarPairs;
    for (int i = 0; i < NUM_BOOKS; ++i) {
        for (int j = i + 1; j < NUM_BOOKS; ++j) {
            similarPairs.emplace_back(i, j, similarityMatrix[i][j]);
        }
    }

    // Sort by similarity in descending order
    sort(similarPairs.begin(), similarPairs.end(), [](const auto& a, const auto& b) {
        return get<2>(b) < get<2>(a);
    });

    cout << "\nTop 10 Similar Pairs:\n";
    for (int i = 0; i < min(10, static_cast<int>(similarPairs.size())); i++) {
        auto [book1, book2, similarity] = similarPairs[i];
        cout << "Books " << book1 + 1 << " and " << book2 + 1 << " - Similarity: " << similarity << endl;
    }

    return 0;
}
