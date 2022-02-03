#include <iostream>
#include <string>
// #include <cstring>
#include <bits/stdc++.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include <fcntl.h> // for open, close etc
#include <unistd.h> // for exec family of commands

using namespace std;

void getCommand (string str, vector<string> &cmd) {

    // Used to split string around spaces.
    istringstream ss(str);
  
    string word; // for storing each word
  
    while (ss >> word) 
    {
        cmd.push_back(word);
    }

}

int convertVectorToArray (vector<string> &vect, char** &arr) {

    int size = vect.size();

    arr = new char*[size + 1];

    for (int i = 0; i < size; i++) {

        int stringSize = vect[i].length();
        arr[i] = new char[stringSize];
        strcpy(arr[i], vect[i].c_str());

    }
    arr[size] = nullptr;

    return size + 1;
}

void executeCommand(char *cmd_arr[], int cmd_len)
{
    // check if input or output redirection or if it has "&"
    char *infile = nullptr, *outfile = nullptr;
    bool bg = false;
    int cmd_end = cmd_len - 1;
    for (int i = 0; i < cmd_len - 1; i++)
    {
        if (strcmp(cmd_arr[i], "<") == 0)
        {
            if (i == cmd_len - 1)
            {
                cout << "syntax error" << endl;
                return;
            }
            infile = cmd_arr[i + 1];
            cmd_end = min(cmd_end, i);
        }
        if (strcmp(cmd_arr[i], ">") == 0)
        {
            if (i == cmd_len - 1)
            {
                cout << "syntax error" << endl;
                return;
            }
            outfile = cmd_arr[i + 1];
            cmd_end = min(cmd_end, i);
        }
        if (strcmp(cmd_arr[i], "&") == 0)
        {
            bg = true;
            cmd_end = min(cmd_end, i);
        }
    }

    cmd_arr[cmd_end] = NULL;

    int infd = 0, outfd = 1;

    pid_t x = fork();
    if (x == 0)
    {
        


        // if (infd != 0)
        // {
        //     dup2(infd, STDIN_FILENO);
        // }
        // if (outfd != 1)
        // {
        //     dup2(outfd, STDOUT_FILENO);
        // }

        if (infile != nullptr)
        {
            cout << "Taking input from " << infile << endl;
            infd = open(infile, O_RDONLY);
            if (infd == -1)
            {
                cout << "cannot open the input file" << endl;
                return;
            }
            dup2(infd, STDIN_FILENO);
        }
        if (outfile != nullptr)
        {
            cout << "Writing output to " << outfile << endl;
            outfd = open(outfile, O_WRONLY | O_CREAT);
            if (outfd == -1)
            {
                cout << "cannot open the output file" << endl;
                return;
            }
            dup2(outfd, STDOUT_FILENO);
        }




        
        cout << "PID of child = " << getpid() << endl;
        execvp(cmd_arr[0], cmd_arr);
        exit(0);
    }
    else if (x > 0)
    {
        cout << "PID of parent = " << getpid() << endl;
        if (!bg)
        {
            wait(NULL);
        }
        cout << "Executed" << endl;
    }
    else
    {
        cout << "ded" << endl;
    }
}


int main (void) {

    string prompt = "nicsh >>> ";
    string line;

    int status;

    while (1) {
        
        cout << prompt;

        getline(cin, line);
        if(line.empty() && !cin.eof()) continue;
        if (cin.eof()) {
            cout << "Exit.\n";
            break;
        }  

        vector<string> cmd;

        getCommand(line, cmd);

        char ** cmd_arr;

        int cmd_len = convertVectorToArray(cmd, cmd_arr);

        for (int i = 0; i < cmd_len - 1; i++) {

            cout << cmd_arr[i] << endl;

        } 

        executeCommand(cmd_arr, cmd_len);

    }

}