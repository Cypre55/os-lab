#ifndef HISTORY_H
#define HISTORY_H

#include <bits/stdc++.h>

using namespace std;

class History {
    
    int history_len = 10000;
    int return_len = 1000;

    int last_entered;
    bool exceeded = false;

    vector<string> list;

    string filename;

    // Read history file
    int read_history();

    void write_history();

public:
    string longest_substring(string str1, string str2);
    

    ~History();

    History();

    // Add to history
    void addToHistory(string line);

    // Get 1000 latest
    void getLatest();

    // Search for best answer
    string search(string item);
};

#endif