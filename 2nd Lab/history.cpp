#include "history.hpp"
// #include <fstream>

History::History() : filename("./.nicsh_history")
{
    list.resize(history_len);
    last_entered = read_history();
}

History::~History()
{
    write_history();
}

int History::read_history()
{

    ifstream infile(filename);

    string line;
    int num_lines = 0;
    while(getline(infile, line)) 
    {
        list[num_lines%history_len] = line; 
        // cout << line << endl;   
        num_lines++;    
    }

    if (num_lines >= history_len)
        exceeded = true;

    return num_lines%history_len;
}

void History::write_history()
{
    ofstream outfile(filename, ios::trunc);

    if (exceeded)
    {
        for (int i = 0; i < history_len; i++)
        {
            outfile << list[(last_entered+i)%history_len] << endl;
        }
    }
    else
    {
        for (int i = 0; i < last_entered; i++)
        {
            outfile << list[i] << endl;
        }
    }
}

void History::addToHistory(string line)
{
    list[(last_entered++)%history_len] = line;

    if (last_entered == history_len)
    {
        last_entered = 0;
        exceeded = true;
    }
}

void History::getLatest()
{
    for (int i = 0; i < return_len; i++)
    {
        int ind = (last_entered-i+history_len)%history_len;
        string str = list[ind];
        if (str != "")
            cout << str << endl;
    }
}

string History::search(string item)
{
    string result = "";

    int max_len = 0;

    for (int i = 0; i < return_len; i++)
    {
        int ind = (last_entered-i+history_len)%history_len;
        string str = list[ind];
        // cout << str << endl;
        if (str != "")
        {
            string lc = longest_substring(str, item);
            if (lc.length() > max_len)
            {
                max_len = lc.length();
                result = list[ind];
            }
        }
            
    }
    return result;
}

string History::longest_substring(string str1, string str2)
{
    string result;
    int m = str1.length();
    int n = str2.length();
    int LCSuff[m + 1][n + 1];
 
    int len = 0;
    int row, col;

    for (int i = 0; i <= m; i++) {
        for (int j = 0; j <= n; j++) {
            if (i == 0 || j == 0)
                LCSuff[i][j] = 0;
 
            else if (str1[i - 1] == str2[j - 1]) {
                LCSuff[i][j] = LCSuff[i - 1][j - 1] + 1;
                if (len < LCSuff[i][j]) {
                    len = LCSuff[i][j];
                    row = i;
                    col = j;
                }
            }
            else
                LCSuff[i][j] = 0;
        }
    }
 
    if (len == 0) {
        result = "";
        return result;
    }
 
    char* resultStr = (char*)malloc((len + 1) * sizeof(char));
    result.resize(len);

    while (LCSuff[row][col] != 0) {
        result[--len] = str1[row - 1]; 
        row--;
        col--;
    }

    return result;

}