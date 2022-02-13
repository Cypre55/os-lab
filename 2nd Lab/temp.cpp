#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include <fcntl.h> // for open, close etc
#include <unistd.h> // for exec family of commands

// TODO
// make an tokenise command function which takes care of ' and "
// remove bits/stdc++.h

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))


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
            outfd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC);
            if (outfd == -1)
            {
                cout << "cannot open the output file" << endl;
                return;
            }
            dup2(outfd, STDOUT_FILENO);
        }
        
        cout << "PID of child = " << getpid() << endl;
        if (fork() == 0)
        {
            execvp(cmd_arr[0], cmd_arr);
        }
        else
        {
            wait(NULL);
            // dup2(infd, 0);
            // close(infd);
            // dup2(outfd, 1);
            // close(outfd);
            exit(0);
        }
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

void executeMultiWatch(string command)
{
    // syntax is multiWatch ["cmd1", "cmd2", "cmd3"]
    vector <string> commands;
    int n = command.length();
    for (int i = 0; i < n; )
    {
        if (command[i] == '\"')
        {
            // TODO check this
            // find the next ','
            // better than looking for '\"' as these can be nested
            int j;
            bool found = false;
            for (j = i + 1; j < n; j++)
            {
                if (command[j] == ',')
                {
                    commands.push_back(command.substr(i + 1, j - i - 2));
                    found = true;
                    i = j;
                    break;
                }
            }
            if (!found)
            {
                commands.push_back(command.substr(i + 1, n - i - 3));
                i = n;
            }
        }
    }

    // DEBUG
    cout << "multiwatch commands are:" << endl;
    for (auto x : commands)
    {
        cout << x << endl;
    }
    cout << endl;

    int command_count = commands.size();

    int inotify_fd = inotify_init();
    if (inotify_fd == -1)
    {
        perror("inotify_init");
    }

    vector <int> readFD (command_count), writeFD (command_count), inotify_WD (command_count);
    for (int i = 0; i < command_count; i++)
    {
        string filename = ".temp.PID" + to_string(i + 1) + ".txt";
        writeFD[i] = open(filename.c_str(), O_WRONLY | O_CREAT, 0666);
        readFD[i] = open(filename.c_str(), O_RDONLY, 0666);
        int inotify_wd = inotify_add_watch(inotify_fd, filename.c_str(), IN_MODIFY);
        inotify_WD[i] = inotify_wd;
    }

    vector <pid_t> process_ids (command_count);
    for (int i = 0; i < command_count; i++)
    {
        process_ids[i] = fork();
        if (process_ids[i] < 0)
        {
            printf("Error in generating the process.\n");
            exit(0);
        }
        if (process_ids[i] == 0)
        {
            // TODO execute commands[i].
            dup2(writeFD[i], STDOUT_FILENO);
        }
    }

    int length, i = 0;
    char buffer[BUF_LEN];

    while (true)
    {
        __attribute__((aligned((struct inotify_event))));
        length = read(inotify_fd, buffer, BUF_LEN);

        if (length < 0)
        {
            perror("read");
        }

        while (i < length)
        {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];

            if (event->len)
            {
                if (event->mask & IN_MODIFY)
                {
                    int file_WD = event->wd;
                    int file_index = -1;
                    for (int x = 0; x < command_count; x++)
                    {
                        if (inotify_WD[x] == file_WD)
                        {
                            file_index = x;
                        }
                    }
                }

                // print the output of file_index in this format
            }
            i += sizeof(struct inotify_event) + event->len;
        }

    }

}