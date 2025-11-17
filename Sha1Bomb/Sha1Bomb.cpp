// Sha1Bomb.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// ============================================================================
// PASSWORD CRACKER - SHA-1 Password Cracking Tool
// ============================================================================
// Readable C++ implementation with full SHA-1 built-in
// Achieves 70% success rate (14/20 passwords) using 4 attack strategies
// ============================================================================

#include <iostream>   // For input/output (cout, cin)
#include <fstream>    // For file operations (ifstream, ofstream)
#include <sstream>    // For string streams (istringstream)
#include <string>     // For string class
#include <vector>     // For dynamic arrays
#include <map>        // For hash maps (key-value pairs)
#include <algorithm>  // For transform, sort
#include <iomanip>    // For hex formatting (setw, setfill)
#include <chrono>     // For timing (high_resolution_clock)
#include <cstdint>    // For uint32_t, uint64_t types

using namespace std;
using namespace chrono;

// ============================================================================
// SHA-1 IMPLEMENTATION
// ============================================================================
// Implements the SHA-1 hashing algorithm from scratch (no external libraries)
// SHA-1 produces a 160-bit (20-byte) hash from any input

class SHA1 {
    // Private member variables
    uint32_t hash[5];        // Five 32-bit hash values (160 bits total)
    uint8_t block[64];       // 64-byte message block buffer
    size_t block_len;        // Current position in block
    uint64_t total_len;      // Total bytes processed

    // Rotate bits left - used in SHA-1 algorithm
    uint32_t rotate_left(uint32_t value, int bits) {
        return (value << bits) | (value >> (32 - bits));
    }

    // Process a single 64-byte block through SHA-1 algorithm
    void process_block() {
        // Expand 64 bytes into 80 32-bit words
        uint32_t w[80];

        // First 16 words: convert 4 bytes to 32-bit word (big-endian)
        for (int i = 0; i < 16; i++)
            w[i] = (block[i * 4] << 24) | (block[i * 4 + 1] << 16) | (block[i * 4 + 2] << 8) | block[i * 4 + 3];

        // Extend to 80 words using XOR and rotation
        for (int i = 16; i < 80; i++)
            w[i] = rotate_left(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);

        // Initialize working variables with current hash
        uint32_t a = hash[0], b = hash[1], c = hash[2], d = hash[3], e = hash[4];

        // Main SHA-1 loop (80 rounds)
        for (int i = 0; i < 80; i++) {
            uint32_t f, k;

            // Different functions and constants for each 20-round section
            if (i < 20) {
                // Rounds 0-19: (B AND C) OR ((NOT B) AND D)
                f = (b & c) | (~b & d);
                k = 0x5A827999;
            }
            else if (i < 40) {
                // Rounds 20-39: B XOR C XOR D
                f = b ^ c ^ d;
                k = 0x6ED9EBA1;
            }
            else if (i < 60) {
                // Rounds 40-59: (B AND C) OR (B AND D) OR (C AND D)
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDC;
            }
            else {
                // Rounds 60-79: B XOR C XOR D
                f = b ^ c ^ d;
                k = 0xCA62C1D6;
            }

            // Update working variables
            uint32_t temp = rotate_left(a, 5) + f + e + k + w[i];
            e = d;
            d = c;
            c = rotate_left(b, 30);
            b = a;
            a = temp;
        }

        // Add this block's hash to result
        hash[0] += a;
        hash[1] += b;
        hash[2] += c;
        hash[3] += d;
        hash[4] += e;
    }

public:
    // Constructor: Initialize hash with SHA-1 standard values
    SHA1() : block_len(0), total_len(0) {
        hash[0] = 0x67452301;
        hash[1] = 0xEFCDAB89;
        hash[2] = 0x98BADCFE;
        hash[3] = 0x10325476;
        hash[4] = 0xC3D2E1F0;
    }

    // Add data to hash (can be called multiple times)
    void update(const uint8_t* data, size_t len) {
        for (size_t i = 0; i < len; i++) {
            block[block_len++] = data[i];

            // Process block when full (64 bytes)
            if (block_len == 64) {
                process_block();
                block_len = 0;
            }
        }
        total_len += len;
    }

    // Finalize hash and return hex string
    string finalize() {
        // Add padding: append 0x80 byte
        block[block_len++] = 0x80;

        // If not enough space for length, pad and process
        if (block_len > 56) {
            while (block_len < 64) block[block_len++] = 0;
            process_block();
            block_len = 0;
        }

        // Pad with zeros until 56 bytes
        while (block_len < 56) block[block_len++] = 0;

        // Append original message length in bits (8 bytes, big-endian)
        uint64_t bit_len = total_len * 8;
        for (int i = 7; i >= 0; i--) {
            block[56 + i] = bit_len & 0xFF;
            bit_len >>= 8;
        }

        // Process final block
        block_len = 64;
        process_block();

        // Convert hash to hex string (40 characters)
        stringstream result;
        for (int i = 0; i < 5; i++)
            result << hex << setw(8) << setfill('0') << hash[i];

        return result.str();
    }
};

// ============================================================================
// SHA-1 WRAPPER FUNCTION
// ============================================================================
// Simple wrapper to hash a string and return hex digest
string sha1(const string& input) {
    SHA1 hasher;
    hasher.update((uint8_t*)input.c_str(), input.size());
    return hasher.finalize();
}

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================
map<string, string> hashes;    // Hash -> User ID mapping (target hashes)
map<string, string> cracked;   // Hash -> Password mapping (found passwords)
long long attempts = 0;        // Total number of password attempts

// ============================================================================
// CHECK PASSWORD FUNCTION
// ============================================================================
// Try a password against all hashes, print and store if found
bool check(const string& password) {
    attempts++;
    string hash_value = sha1(password);

    // Check if this hash is in our target list and not already cracked
    if (hashes.count(hash_value) && !cracked.count(hash_value)) {
        cracked[hash_value] = password;
        cout << "[+] User " << hashes[hash_value] << ": " << password << endl;
        return true;
    }

    return false;
}

// ============================================================================
// STRING TRIM UTILITY
// ============================================================================
// Remove whitespace, carriage returns, newlines, tabs from both ends
string trim(string str) {
    str.erase(str.find_last_not_of(" \r\n\t") + 1);  // Trim end
    str.erase(0, str.find_first_not_of(" \r\n\t"));  // Trim beginning
    return str;
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================
int main(int argc, char** argv) {
    // ========================================================================
    // SETUP: Load password hashes from file
    // ========================================================================
    string password_file = (argc > 1) ? argv[1] : "passwords.txt";

    ifstream file(password_file);
    string line;
    int user_id = 1;

    // Read each line from password file
    while (getline(file, line)) {
        line = trim(line);
        istringstream iss(line);
        string first, second;
        iss >> first >> second;

        // Format: either "hash" or "user_id hash"
        if (second.empty()) {
            hashes[first] = to_string(user_id++);  // Just hash, assign ID
        }
        else {
            hashes[second] = first;                 // user_id hash
            user_id++;
        }
    }
    file.close();

    // Start timer
    auto start_time = high_resolution_clock::now();

    // ========================================================================
    // ATTACK 1: BRUTE FORCE DIGITS (1-8 characters)
    // ========================================================================
    // Try all numeric passwords: 0, 1, ..., 9, 00, 01, ..., 99999999
    // Finds: 000, 123456, 987654, 20240501

    for (int length = 1; length <= 8 && cracked.size() < hashes.size(); length++) {
        // Calculate how many numbers of this length (10^length)
        long long max_num = 1;
        for (int i = 0; i < length; i++) max_num *= 10;

        // Try all numbers from 0 to max_num-1
        for (long long num = 0; num < max_num && cracked.size() < hashes.size(); num++) {
            string password = to_string(num);

            // Pad with leading zeros to reach desired length
            while ((int)password.length() < length)
                password = "0" + password;

            check(password);
        }
    }

    // ========================================================================
    // SETUP: Load dictionary for word-based attacks
    // ========================================================================
    ifstream dict("dictionary.txt");
    if (dict.good()) {
        vector<string> words;      // All dictionary words
        vector<string> priority;   // Short words (?14 chars) for combos

        // Read dictionary file
        while (getline(dict, line)) {
            line = trim(line);

            // Strip UTF-8 BOM (Byte Order Mark) if present at start
            // BOM is 3 bytes: 0xEF 0xBB 0xBF
            if (!line.empty() && (uint8_t)line[0] == 0xEF && line.length() >= 3) {
                if ((uint8_t)line[1] == 0xBB && (uint8_t)line[2] == 0xBF)
                    line = line.substr(3);
            }

            if (!line.empty()) {
                words.push_back(line);

                // Priority list: words ?14 chars for 2-word combinations
                if (line.length() <= 14)
                    priority.push_back(line);
            }
        }
        dict.close();

        // Sort priority words by length (shorter first), then alphabetically
        // This optimizes cracking order (shorter combos tried first)
        sort(priority.begin(), priority.end(), [](const string& a, const string& b) {
            return a.length() < b.length() || (a.length() == b.length() && a < b);
            });

        // ====================================================================
        // ATTACK 2: DICTIONARY WORDS
        // ====================================================================
        // Try each word in lowercase
        // Finds: monday, triumphant, bewildered, writings

        for (const auto& word : words) {
            string lower_word = word;
            transform(lower_word.begin(), lower_word.end(), lower_word.begin(), ::tolower);

            if (check(lower_word) && cracked.size() == hashes.size())
                break;
        }

        // ====================================================================
        // ATTACK 3: TWO-WORD COMBINATIONS
        // ====================================================================
        // Try all pairs of short words (word1 + word2)
        // Only test combos between 6-28 characters
        // Finds: bestrings, excitephysician, investigationsaving

        for (size_t i = 0; i < priority.size() && cracked.size() < hashes.size(); i++) {
            for (size_t j = 0; j < priority.size() && cracked.size() < hashes.size(); j++) {
                string combo = priority[i] + priority[j];
                transform(combo.begin(), combo.end(), combo.begin(), ::tolower);

                // Only try reasonable-length combinations
                if (combo.length() >= 6 && combo.length() <= 28)
                    check(combo);
            }
        }

        // ====================================================================
        // ATTACK 4: HYBRID (Word + Digits)
        // ====================================================================
        // Try each word followed by digits 0-9999
        // Examples: word0, word1, ..., word9999
        // Finds: marching2024, writings784, aid123, convince0

        for (const auto& word : words) {
            if (cracked.size() == hashes.size())
                break;

            string lower_word = word;
            transform(lower_word.begin(), lower_word.end(), lower_word.begin(), ::tolower);

            // Try appending 0-9999
            for (int digits = 0; digits < 10000 && cracked.size() < hashes.size(); digits++) {
                check(lower_word + to_string(digits));
            }
        }
    }

    // ========================================================================
    // RESULTS: Display statistics
    // ========================================================================
    auto elapsed = duration_cast<milliseconds>(high_resolution_clock::now() - start_time).count() / 1000.0;

    cout << "\n" << string(60, '=') << endl;
    cout << "Cracked: " << cracked.size() << "/" << hashes.size()
        << " (" << (100 * cracked.size() / hashes.size()) << "%)" << endl;
    cout << "Time: " << fixed << setprecision(1) << elapsed << "s | Attempts: " << attempts << endl;
    cout << string(60, '=') << endl;

    // ========================================================================
    // SAVE: Write results to file
    // ========================================================================
    ofstream output("cracked_passwords.txt");
    output << "USER_ID PASSWORD" << endl;
    for (const auto& [hash, password] : cracked) {
        output << hashes[hash] << " " << password << endl;
    }
    output.close();

    cout << "[+] Results saved to cracked_passwords.txt" << endl;

    return 0;
}