#include <iostream>
#include <string>
// #include <cstring>
#include <bits/stdc++.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

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

    arr = new char*[size];

    for (int i = 0; i < size; i++) {

        int stringSize = vect[i].length();
        arr[i] = new char[stringSize];
        strcpy(arr[i], vect[i].c_str());

    }

    return size;
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

        for (int i = 0; i < cmd_len; i++) {

            cout << cmd_arr[i] << endl;

        } 

        // pid_t pid = fork();
        // pid_t wpid;

        // if (pid < 0) {

        //     cout << "Error in forking\n";

        // }

        // if (pid == 0) {

        //     execvp(cmd_arr[0], cmd_arr);

        //     cout << "Ran Command\n";

        // }
        // else {
        //     if(strcmp(cmd_arr[cmd_len-1],"&")!=0)
        //     {
        //         do{
        //             wpid = waitpid(pid,&status,WUNTRACED);
        //         }while(!WIFEXITED(status) && !WIFSIGNALED(status));
        //     }
        //     cout << "Done Waiting\n";
        // }

    }

}