#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include "history.hpp"

using namespace std;

int main()
{
    // int stdin_copy = dup(0);
    // int stdout_copy = dup(1);
    // // close(0);
    // // close(1);

    // int file1 = open("./input", O_RDWR | O_EXCL);
    // dup2(file1, 0);
    // int file2 = open("./output", O_RDWR | O_EXCL);
    // dup2(file2, 1);
    // cout << file2 << endl;
    // char* cmd[] = { "python", "t.py", NULL };
    // execvp(cmd[0], cmd);

    // close(file1);
    // close(file2);
    // dup2(stdin_copy, 0);
    // dup2(stdout_copy, 1);
    // close(stdin_copy);
    // close(stdout_copy);

    // History history;

    // // history.addToHistory("Start");
    // // history.getLatest();

    // string result = history.search("liber");

    // cout << result << endl;
    
    cout << "Hello";
    cout.flush();
    sleep(1);
    cout << "\b \b\n";

}