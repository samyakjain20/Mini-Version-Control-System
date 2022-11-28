#include <iostream>
#include <dirent.h>
#include <filesystem>
#include <string>
#include <string.h>
// #include <bits/stdc++.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <dirent.h>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utility>
// using namespace std;

namespace diff
{
    using namespace std;
    namespace fs = std ::filesystem;
    string cwd = fs ::current_path();
    string vcspath = "./.vcs"; // add .vcs

    void error_print(string print)
    {
        cout << "\033[1;31m";
        cout << print << endl;
        cout << "\033[0m";
        return;
    }

    bool check_directory(string path1)
    {
        DIR *location = opendir(path1.c_str());

        if (location == NULL)
            return false;

        struct dirent *file = readdir(location);
        if (file->d_type == DT_DIR)
            return true;
        else
            return false;
    }

    void difference_between_two(string file)
    {
        int versionNo;
        string temp;
        ifstream vfile("./.vcs/version.info");
        vfile >> temp;
        vfile.close();
        versionNo = stoi(temp);
        string path1 = "./" + file, path2 = "./.vcs/" + to_string(versionNo) + "/" + file;

        char resolved[2000];
        realpath(path1.c_str(), resolved);
        path1 = string(resolved);

        realpath(path2.c_str(), resolved);
        path2 = string(resolved);

        // cout << path1 << " " << path2 << endl;
        if (fs::exists(path1) == false)
        {
            // cout << "File not present\n";
            error_print("File not present");
            return;
        }
        else if (fs::exists(path2) == false)
        {
            path2 = "./.vcs/" + to_string(versionNo - 1) + "/" + file;
            realpath(path2.c_str(), resolved);
            path2 = string(resolved);

            // cout << path2 << endl;
            if (fs::exists(path2) == false)
            {
                cout << "File added recently\n";
                return;
            }
        }

        if ((!check_directory(path1) && !check_directory(path2)) || (check_directory(path1) && check_directory(path2)))
        {
            string command = "diff ";
            command += path1 + " " + path2;
            system(command.c_str());
        }
    }
}
