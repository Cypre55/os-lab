#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/wait.h>
#include <dirent.h>

#include <fcntl.h> // for open, close etc
#include <unistd.h> // for exec family of commands

// TODO
// make an tokenise command function which takes care of ' and "
// remove bits/stdc++.h
// Before returning termios string, always reset
// implement multiwatch
// Ctrl+C and Ctrl+Z handlers

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

using namespace std;

struct termios saved_attributes;

static volatile int isBackground = 0;


void resetInputMode(void) {
  saved_attributes.c_lflag |= (ICANON | ECHO);
  saved_attributes.c_cc[VMIN] = 0;
  saved_attributes.c_cc[VTIME] = 1;
  tcsetattr(STDIN_FILENO, TCSANOW, &saved_attributes);
}

void setInputMode(void) {
  struct termios tattr;
  tcgetattr(STDIN_FILENO, &saved_attributes);
  atexit(resetInputMode);
  tcgetattr(STDIN_FILENO, &tattr);
  tattr.c_lflag &= ~(ICANON | ECHO);
  tattr.c_cc[VMIN] = 1;
  tattr.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &tattr);
}

class History {
    
    int history_len = 10000;
    int return_len = 1000;

    int last_entered;
    bool exceeded = false;

    vector<string> list;

    int filename_len = 1000;
    string filename;

    // Read history file
    int read_history() {

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

    void write_history() {
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

public:
    string longest_substring(string str1, string str2) {
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
    

    ~History() {
        write_history();
    }

    History() : filename("/.nicsh_history") {
        char fn[filename_len];
        getcwd(fn, filename_len);
        string fn_str = string(fn);
        fn_str.append(filename);
        filename = fn_str;
        cout << filename << endl;
        list.resize(history_len);
        last_entered = read_history();
    }

    // Add to history
    void addToHistory(string line) {
        list[(last_entered++)%history_len] = line;

        if (last_entered == history_len)
        {
            last_entered = 0;
            exceeded = true;
        }
    }

    // Get 1000 latest
    void getLatest() {
        cout << "History: " << endl;
        for (int i = 0; i < return_len; i++)
        {
            int ind = (last_entered-i+history_len)%history_len;
            string str = list[ind];
            if (str != "")
                cout << "\t" << str << endl;
        }
    }

    // Search for best answer
    set<string> search(string item) {
        set<string> result;

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
                    result.clear();
                    max_len = lc.length();
                    result.insert(list[ind]);
                }
                else if (lc.length() == max_len)
                {
                    result.insert(list[ind]);
                }
            }
                
        }
        if (max_len >= 2)
            return result;
        else {
            result.clear();
            return result;
        }
    }

    void performSearch() {
        cout << "\nEnter the search term: ";
        string searchString;

        getline(cin, searchString);

        set<string> result = search(searchString);

        if (result.size() == 0)
            cout << "No match found in history" << endl;
        else {
            for (string s: result) {
                cout << s << endl;
            }
        }
    }
};

History history;

void autoComplete(string &line) {

    string printedString;
    ostringstream printedStream(printedString);

    istringstream ss(line);
  
    string last_token; // for storing each word
  
    while (ss >> last_token) 
        ;

    vector <string> fileList;
	struct dirent * file;
	DIR * dir = opendir(".");
	while ((file = readdir(dir)) != NULL)
	{
		fileList.push_back(string(file->d_name));
	}

    printedStream << "\n";

    vector <string> options;
	for (auto x : fileList)
	{
		// check if queryString is a prefix
		if (x.length() >= last_token.length() && x.substr(0, last_token.length()) == last_token)
		{
			options.push_back(x);
		}
	}

    string answer = "";
    if (options.size() == 0)
    {
        ;
    }
    else if (options.size() == 1)
	{
		answer = options[0]; 
	}
	else
	{
		int count = 0;
		for (auto x : options)
		{
			printedStream << ++count << ") " << x << "\n"; 
		}

        printedStream << "Choice: ";
        cout << printedStream.str();
        cout.flush();

		string choice_string;
		getline(cin, choice_string);

        printedStream << choice_string << "\n";

        
        int choice = stoi(choice_string);
		answer = options[choice - 1];
        if (choice > 0 && choice <= options.size())
		{
			answer = options[choice - 1];
		}
	
    
    }

    // cout << answer << endl;
    // sleep(1);
    printedStream << line << "\t";

    int str_len = printedStream.str().length();

    // cout << str_len << endl;

    // for (int i = 0; i < str_len; i++) {
    //     cout << "\b \b";
    //     cout.flush();
    // }


    if (answer != "")
        line = line.substr(0, line.length()-last_token.length());

    line.append(answer);

}

void executeMultiWatch(char *cmd_arr[], int cmd_len) {
    // syntax is multiWatch ["cmd1", "cmd2", "cmd3"]

    // char **

    // for (int i = 0; i < cmd_len; )

    // vector <string> commands;
    // int n = command.length();
    // for (int i = 0; i < n; )
    // {
    //     if (command[i] == '\"')
    //     {
    //         // TODO check this
    //         // find the next ','
    //         // better than looking for '\"' as these can be nested
    //         int j;
    //         bool found = false;
    //         for (j = i + 1; j < n; j++)
    //         {
    //             if (command[j] == ',')
    //             {
    //                 commands.push_back(command.substr(i + 1, j - i - 2));
    //                 found = true;
    //                 i = j;
    //                 break;
    //             }
    //         }
    //         if (!found)
    //         {
    //             commands.push_back(command.substr(i + 1, n - i - 3));
    //             i = n;
    //         }
    //     }
    // }

    // DEBUG
    // cout << "multiwatch commands are:" << endl;
    // for (auto x : commands)
    // {
    //     cout << x << endl;
    // }
    // cout << endl;

    // int command_count = commands.size();

    // int inotify_fd = inotify_init();
    // if (inotify_fd == -1)
    // {
    //     perror("inotify_init");
    // }

    // vector <int> readFD (command_count), writeFD (command_count), inotify_WD (command_count);
    // for (int i = 0; i < command_count; i++)
    // {
    //     string filename = ".temp.PID" + to_string(i + 1) + ".txt";
    //     writeFD[i] = open(filename.c_str(), O_WRONLY | O_CREAT, 0666);
    //     readFD[i] = open(filename.c_str(), O_RDONLY, 0666);
    //     int inotify_wd = inotify_add_watch(inotify_fd, filename.c_str(), IN_MODIFY);
    //     inotify_WD[i] = inotify_wd;
    // }

    // vector <pid_t> process_ids (command_count);
    // for (int i = 0; i < command_count; i++)
    // {
    //     process_ids[i] = fork();
    //     if (process_ids[i] < 0)
    //     {
    //         printf("Error in generating the process.\n");
    //         exit(0);
    //     }
    //     if (process_ids[i] == 0)
    //     {
    //         // TODO execute commands[i].
    //         dup2(writeFD[i], STDOUT_FILENO);
    //     }
    // }

    // int length, i = 0;
    // char buffer[BUF_LEN];

    // while (true)
    // {
    //     __attribute__((aligned((struct inotify_event))));
    //     length = read(inotify_fd, buffer, BUF_LEN);

    //     if (length < 0)
    //     {
    //         perror("read");
    //     }

    //     while (i < length)
    //     {
    //         struct inotify_event *event = (struct inotify_event *) &buffer[i];

    //         if (event->len)
    //         {
    //             if (event->mask & IN_MODIFY)
    //             {
    //                 int file_WD = event->wd;
    //                 int file_index = -1;
    //                 for (int x = 0; x < command_count; x++)
    //                 {
    //                     if (inotify_WD[x] == file_WD)
    //                     {
    //                         file_index = x;
    //                     }
    //                 }
    //             }

    //             // print the output of file_index in this format
    //         }
    //         i += sizeof(struct inotify_event) + event->len;
    //     }

    // }

}

void splitCommand (string str, vector<string> &cmd) {

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
    // Handling Custom Commands
    string command_name = string(cmd_arr[0]);
    if (command_name == "cd") {
        if(chdir(cmd_arr[1])!=0)
        {
            cout << "Error: directory not found\n";
        }
        return;
    }
    else if (command_name == "history") {
        history.getLatest();
        return;
    }
    else if (command_name == "exit") {
        exit(0);
        return;
    }
    else if (command_name == "multiwatch") {

        return;
    }

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
            outfd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (outfd == -1)
            {
                cout << "cannot open the output file" << endl;
                return;
            }
            dup2(outfd, STDOUT_FILENO);
        }
        
        // cout << "PID of child = " << getpid() << endl;
        if (fork() == 0)
        {
            execvp(cmd_arr[0], cmd_arr);
            exit(0);
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

string getLine () {
    string result = "";

    // 0: Canonical
    // 1: Non-Canonical
    int inputMode = 0;

    char c;

    while (true) {

        if (inputMode == 0) {
            inputMode = 1;
            setInputMode();
        }

        c = getchar();

        if ((int) c == 127) {

            if (result.length() == 0)
                continue;
            
            result.pop_back();
            cout << "\b \b";
            cout.flush();
        }
        else if ((int) c == 18) {

            // TODO: Integrate History
            int str_len = result.length();

            for (int i = 0; i < str_len; i++) {
                cout << "\b \b";
                cout.flush();
            }

            inputMode = 0;
            resetInputMode();
            // cout << "Ctrl + R Pressed" << endl;
            history.performSearch();

            return string("");
        }
        else if ((int) c == 9) {

            inputMode = 0;
            resetInputMode();
            // cout << "Tab Pressed" << endl;
            // TODO: AutoComplete Backend

            autoComplete(result);
            // Return appropitaly
            cout << "\n\nnicsh >>> ";
            cout << result;

        }
        else if ((int) c == 4) {
            result.clear();
            result.append("<<>>");
            resetInputMode();
            return result;
        }
        else if (c == '\n' || c == EOF) {
            break;
        }
        else {
            result.push_back(c);
            cout << c;
            cout.flush();
        }

    }

    resetInputMode();
    cout << "\n";
    return result;

}

void ctrl_C_Handler(int dum) {
//   stopWatch = 1;
  fprintf(stdout, " %d Ctrl C Detected.\n", getpid());
//   close(inotifyFd);
  return;
}

void ctrl_Z_Handler(int dum) {
  fprintf(stdout, " %d Ctrl Z Detected.\n", getpid());
  isBackground = 1;
}

int main (void) {

    signal(SIGINT, ctrl_C_Handler);
    signal(SIGTSTP, ctrl_Z_Handler);

    cout << getpid() << endl;

    string prompt = "\nnicsh >>> ";
    string line;
    int number_pipes;
    char ** cmd_arr;
    int cmd_len;

    int status;

    while (1) {
        
        cout << prompt;

        // getline(cin, line);
        // if(line.empty() && !cin.eof()) continue;
        // if (cin.eof()) {
        //     cout << "Exit.\n";
        //     break;
        // }  

        string line = getLine();

        if (line == "<<>>") {
            cout << "exit\n";
            break;
        }
        
        if (line == "") {
            continue;

        }

        if (isBackground == 1)
        {
            cout << "Ctrl + Z pressed\n";
            isBackground = 0;
        }

        vector<string> cmd;
        vector<vector<string>> pipedCommand;

        history.addToHistory(line);
        splitCommand(line, cmd);

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

