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

int getPipedCommand (vector<string> &cmd, vector<vector<string>> &pipedCommands) {

    int start = 0;
    string str;
    for (int i = 0; i < cmd.size(); i++) {

        str = cmd[i];
        if (str == "|") {
            // cout << cmd[i-1] << " ";
            vector<string> sub_vect (cmd.begin() + start, cmd.begin() + i);
            pipedCommands.push_back(sub_vect);
            start = i+1;
        }

    }

    vector<string> sub_vect (cmd.begin() + start, cmd.end());
    pipedCommands.push_back(sub_vect);

    // for (int i = 0; i < pipedCommands.size(); i++) {
    //     for (int j = 0; j < pipedCommands[i].size(); j++) {
    //         cout << pipedCommands[i][j] << " ";
    //     }
    //     cout << endl;
    // }

    return pipedCommands.size();

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

void executeCommand(char *cmd_arr[], int cmd_len, int infd = 0, int outfd = 1)
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

    // int infd = 0, outfd = 1;

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

        if (infd != 0) {

            // cout << "Taking input from pipe" << endl;
            dup2(infd, STDIN_FILENO);
            close(infd);
        }

        if (outfd != 1) {

            // cout << "Sending output to pipe" << endl;
            dup2(outfd, STDOUT_FILENO);
            close(outfd);
        }

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
        
        // cerr << "PID of child = " << getpid() << endl;
        execvp(cmd_arr[0], cmd_arr);
        exit(0);
    }
    else if (x > 0)
    {
        // cout << "PID of parent = " << getpid() << endl;
        if (!bg)
        {
            // cerr << "Waiting\n";
            wait(NULL);
        }
        // cout << "Executed" << endl;
    }
    else
    {
        cout << "ded" << endl;
    }
}


int main (void) {

    string prompt = "nicsh >>> ";
    string line;
    int number_pipes;
    char ** cmd_arr;
    int cmd_len;

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
        vector<vector<string>> pipedCommand;

        getCommand(line, cmd);

        number_pipes = getPipedCommand(cmd, pipedCommand);

        if (number_pipes == 1) {

            cmd_len = convertVectorToArray(pipedCommand[0], cmd_arr);

            executeCommand(cmd_arr, cmd_len);
        }
        else if (number_pipes > 1) {

            int infd = 0;
            int FD[2];
            int pipeError = 0;

            for (int i = 0; i < number_pipes - 1; i++) {

                if (pipe(FD) == -1) {
                    pipeError = 1;
                    break;
                }

                cmd_len = convertVectorToArray(pipedCommand[i], cmd_arr);

                executeCommand(cmd_arr, cmd_len, infd, FD[1]);

                close(FD[1]);
                infd = FD[0];

            }
            if(!pipeError)
            {
                cmd_len = convertVectorToArray(pipedCommand[number_pipes-1], cmd_arr);

                executeCommand(cmd_arr, cmd_len, infd);
            }

        }


    }

}