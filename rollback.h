#include <iostream>
#include <dirent.h>
#include <filesystem>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <dirent.h>
#include <vector>
#include <string>
#include <bits/stdc++.h>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utility>
// using namespace std;

namespace rollback
{

    using namespace std;
    namespace fs = std::filesystem;

    string vcspath = "./.vcs";

    int createdir(string path1){
        int check;
        check = mkdir(path1.c_str(), 0777);
        if (check == 0)
            return 1;
        else
            return -1;
    }

    void create_dir_structure(string check_path){
        string cwd = fs::current_path();
        char resolved[3000];
        realpath(check_path.c_str(), resolved);
        string abs_path = string(resolved);
        cout << "Absolute Path: " << abs_path << endl;
        vector<string> directories_in_path;
        string temp;
        for (int i = 0; i < (int)abs_path.size(); i++)
        {
            if (abs_path[i] != '/')
            {
                temp += abs_path[i];
            }
            else
            {
                directories_in_path.push_back(temp);
                temp = "";
            }
        }
        string temp_path = cwd;
        for (auto i : directories_in_path)
        {
            // cout << "Here " << i << endl;
            temp_path = temp_path + "/" + i;
            if (fs::exists(temp_path))
            {
                continue;
            }
            else
            {
                string create_command = "mkdir " + temp_path;
                system(create_command.c_str());
            }
        }
    }

    void getFileRecursive(vector<string> &st, string path, string dirName)
    {
        DIR *dir;
        struct dirent *sd;
        // cout << "Inside dir: ";
        string fullPath = path + dirName;
        // cout << fullPath << endl;
        dir = opendir(fullPath.c_str());

        while ((sd = readdir(dir)) != NULL)
        {
            string currFile = sd->d_name;
            if (currFile == ".." || currFile == "." || currFile == ".vcs" || currFile == "add.h" || currFile == "commit.h" || currFile == "status.h" || currFile == "diff.h" || currFile == "a.out" || currFile == ".git" || currFile == ".vscode" || currFile == "main.cpp" || currFile == "rollback.h")
                continue;
            else
            {
                struct stat sfile;
                currFile = path + dirName + "/" + sd->d_name;
                stat(currFile.c_str(), &sfile);
                // cout << currFile << endl;

                if (fs::is_directory(currFile.c_str()))
                {
                    // cout << "recur " << dirName + "/" + sd->d_name << endl;
                    getFileRecursive(st, path, dirName + "/" + sd->d_name);
                }
                else
                    st.push_back(dirName + "/" + sd->d_name);
            }
        }
        // cout << "going out\n";
    }

    void go_to_previous(string version_path, string curr_path)
    {
        DIR *dir1, *dir2, *dir;
        struct dirent *sd;

        cout << "Version Path : " << version_path << endl;
        cout << "Current path : " << curr_path << endl;
        dir1 = opendir(version_path.c_str());
        dir2 = opendir(curr_path.c_str());

        if (dir1 == NULL || dir2 == NULL)
        {
            // cout << "Here" << endl;
            cout << "'.vcs' folder corrupted.\n";
            return;
        }

        // set<string> previous, current; // stores path of all the files / directories

        // dir = dir2;
        // while ((sd = readdir(dir)) != NULL)
        // {
        //     string currFile = sd->d_name;
        //     if (currFile == ".." || currFile == "." || currFile == ".vcs" || currFile == "main.cpp" || currFile == "rollback.h" )
        //         continue;
        //     else
        //         current.insert(sd->d_name);
        //     cout << "latest " << sd->d_name << endl;
        // }

        // dir = dir1;
        // while ((sd = readdir(dir)) != NULL)
        // {
        //     string currFile = sd->d_name;

        //     if (currFile == ".." || currFile == "." || currFile == ".vcs" || currFile == "main.cpp"  || currFile == "rollback.h" )
        //         continue;
        //     else
        //         previous.insert(sd->d_name);
        //     cout << "old " << sd->d_name << endl;
        // }

        // for (auto it1 : previous)
        // {
        //     struct stat sfile;
        //     string currFile = version_path + "/" + it1;
        //     int a = stat(currFile.c_str(), &sfile);
        //     cout << currFile << " " << endl;
        //     if ((sfile.st_mode & S_IFDIR))
        //     {

        //         DIR *dir;
        //         struct dirent *sd;
        //         dir = opendir(currFile.c_str());
        //         previous.erase(previous.find(it1));

        //         while ((sd = readdir(dir)) != NULL)
        //             if (strcmp(sd->d_name, "..") == 0 || strcmp(sd->d_name, ".") == 0)
        //                 continue;
        //             else
        //             {
        //                 cout << sd->d_name << endl;
        //                 previous.insert(it1 + "/" + sd->d_name);
        //             }
        //     }
        // }
        // cout << "1st done\n\n";

        // for (auto it2 : current)
        // {
        //     struct stat sfile;
        //     string currFile = curr_path + "/" + it2;
        //     int a = stat(currFile.c_str(), &sfile);

        //     cout << currFile << endl;

        //     if ((sfile.st_mode & S_IFDIR))
        //     {

        //         DIR *dir;
        //         struct dirent *sd;
        //         dir = opendir(currFile.c_str());
        //         current.erase(current.find(it2));

        //         while ((sd = readdir(dir)) != NULL)
        //             if (strcmp(sd->d_name, "..") == 0 || strcmp(sd->d_name, ".") == 0)
        //                 continue;
        //             else
        //             {
        //                 // cout << sd->d_name << endl;
        //                 current.insert(it2 + "/" + sd->d_name);
        //             }
        //     }
        // }

        vector<string> old, latest; // stores path of all the files / directories
        dir = dir2;
        while ((sd = readdir(dir)) != NULL)
        {
            string currFile = sd->d_name;
            // string fileDetails = getFileDetails(dir_to_search + '/'+sd->d_name);
            if (currFile == ".." || currFile == "." || currFile == ".vcs" || currFile == "add.h" || currFile == "commit.h" || currFile == "status.h" || currFile == "diff.h" || currFile == "a.out" || currFile == ".git" || currFile == ".vscode" || currFile == "main.cpp" || currFile == "rollback.h")
                continue;
            else
            {
                struct stat sfile;
                currFile = curr_path + sd->d_name;
                stat(currFile.c_str(), &sfile);

                if (fs::is_directory(currFile.c_str()))
                    getFileRecursive(latest, curr_path, sd->d_name);
                else
                    latest.push_back(sd->d_name);
            }
            // cout << "latest " << sd->d_name << endl;
        }

        dir = dir1;
        while ((sd = readdir(dir)) != NULL)
        {
            string currFile = sd->d_name;
            // string fileDetails = getFileDetails(dir_to_search + '/'+sd->d_name);
            if (currFile == ".." || currFile == "." || currFile == ".vcs" || currFile == "add.h" || currFile == "commit.h" || currFile == "status.h" || currFile == "diff.h" || currFile == "a.out" || currFile == ".git" || currFile == ".vscode" || currFile == "main.cpp")
                continue;
            else
            {
                struct stat sfile;
                currFile = version_path + currFile;
                stat(currFile.c_str(), &sfile);

                if (fs::is_directory(currFile.c_str()))
                    getFileRecursive(old, version_path, sd->d_name);
                else
                    old.push_back(sd->d_name);
            }
            // cout << "old " << sd->d_name << endl;
        }


        for(auto it : old)
        {
            cout<<"old : "<<it<<endl;
        }

        for(auto it : latest)
        {
            cout<<"current : "<<it<<endl;
        }

        for (auto it : latest)
        {
            string add_patch = it + ".patch";

            auto prev_itr = find(old.begin(), old.end(), add_patch);
            if (prev_itr != old.end())
            {
                string command = "patch " + it + " " + version_path + add_patch;
                system(command.c_str());
                old.erase(prev_itr);
            }

            else
            {
                string command = "rm " + it;
                cout << command << endl;
                system(command.c_str());
            }
        }

        for (auto it : old)
        {
            create_dir_structure(it);
            string command = "cp " + version_path + it + " " + "./" + it;
            system(command.c_str());
        }
    }

    void rollback(string version_no, string curr_path, int totalCommitsByFar)
    {
        string version_path = "./.vcs/" + version_no + "/";

        int count = 1; // Number of times to rollback

        while (count > 0)
        {
            go_to_previous(version_path, curr_path);
            count--;
        }

        return;
    }

}