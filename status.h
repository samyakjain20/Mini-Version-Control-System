#include <iostream>
#include <dirent.h>
#include <filesystem>
#include <string>
#include <string.h>
#include <bits/stdc++.h>
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
#include <openssl/sha.h>

namespace status
{
    using namespace std;
    namespace fs = std::filesystem;
    string vcspath1 = "./.vcs/tracked_current.txt";
    string vcspath2 = "./.vcs/tracked_history.txt";
    map<string, string> previousSHA;
    map<string, string> currSHA;
    map<string, string> currTrackedSHA;

    void getCurrentFileSHA(string fileLoc)
    {

        unsigned char completeFileSHA[SHA_DIGEST_LENGTH];
        FILE *inFile = fopen(fileLoc.c_str(), "rb");
        SHA_CTX shaContext;
        int bytes;
        unsigned char data[524288];

        if (inFile == NULL)
        {
            cout << "Cannot open " << fileLoc << endl;
            return;
        }

        // vector<string> chunkSHA1;
        SHA1_Init(&shaContext);
        while ((bytes = fread(data, 1, 524288, inFile)) != 0)
        {
            SHA1_Update(&shaContext, data, bytes);
        }
        SHA1_Final(completeFileSHA, &shaContext);
        fclose(inFile);
        stringstream ss;
        for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
        {
            ss << hex << setw(2) << setfill('0') << (int)completeFileSHA[i];
        }
        string fullFileSHA1 = ss.str();
        cout << fileLoc << " sha is " << fullFileSHA1 << endl;
        currSHA[fileLoc] = fullFileSHA1;
    }

    // get previous files SHA from version.info
    void getPrevFilesSHA()
    {
        cout<<"-----------------------------------"<<endl;
        string line;

        ifstream myfile;
        myfile.open(vcspath2);
        int cnt = 0;

        if (!myfile.is_open())
        {
            perror("Error open");
            return;
        }

        // get files SHA from tracked_history
        while (getline(myfile, line))
        {
            string fileData = line;
            int pos = fileData.find("$");

            string fileLoc = fileData.substr(0, pos);
            string fileSHA = fileData.substr(pos + 1);
            cout << fileLoc << " " << fileSHA << endl;
            char resolved[2000];
            realpath(fileLoc.c_str(), resolved);
            string abs_path = string(resolved);
            previousSHA[abs_path] = fileSHA;
        }
        myfile.close();

        ifstream myfile1;
        cout << "going for path1 ?" << endl;
        myfile1.open(vcspath1);
        if (!myfile1.is_open())
        {
            perror("Error open");
            return;
        }
        // get files SHA from tracked_current
        while (getline(myfile1, line))
        {
            cout<<"COMING INSIDE";

            string fileData = line;
            int pos = fileData.find("$");
            string fileLoc = fileData.substr(0, pos);
            string fileSHA = fileData.substr(pos + 1);
            char resolved[2000];
            realpath(fileLoc.c_str(), resolved);
            string abs_path = string(resolved);
            cout << fileLoc << " " << fileSHA << endl;
            previousSHA[abs_path] = fileSHA;
            cout << "Adding SHA to " << abs_path << endl;
            currTrackedSHA[abs_path] = fileSHA;
        }

        myfile1.close();
    }

    void traverseFiles(string cwd)
    {
        cout<<"+++++++++++++++++++++++++++++++++++++++"<<endl;
        // get current files SHA
        DIR *folder = opendir(cwd.c_str());
        if (folder == NULL)
        {
            perror("Error open");
            return;
        }
        // reading files in a directory
        struct dirent *file = readdir(folder);
        while (file != NULL)
        {
            if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0 || strcmp(file->d_name, ".vcs") == 0 || strcmp(file->d_name, ".git") == 0 || strcmp(file->d_name, ".vscode") == 0 || strcmp(file->d_name, "main.cpp") == 0 || strcmp(file->d_name, "add.h") == 0 || strcmp(file->d_name, "commit.h") == 0 || strcmp(file->d_name, "diff.h") == 0 || strcmp(file->d_name, "status.h") == 0 || strcmp(file->d_name, "a.out") == 0)
                cout << "ignore" << endl;
            else
            {
                string filepath = cwd + "/" + file->d_name;
                string new_path = "./.vcs/0/" + string(file->d_name);
                if (file->d_type == DT_DIR)
                {
                    cout << "Directory" << filepath << endl;
                    traverseFiles(filepath);
                }
                else
                {
                    getCurrentFileSHA(filepath);
                }
            }
            file = readdir(folder);
        }
        closedir(folder);
    }

    void git_status(map<string, string> &previous_sha, map<string, string> &current_sha)
    {
        string un_tracked = "Untracked Files:\n";
        string deleted_file = "Deleted Files:\n";
        string modified_file = "Modified Files:\n";
        string tracked_file = "Tracked Files: \n";
        string name_of_un = "";
        string name_of_del = "";
        string name_of_mod = "";
        string name_of_track = "";
        if (previous_sha.size() != 0)
        {
            cout << "Inside" << endl;
            for (auto i : previous_sha)
            {
                cout << "file path is:--> " << i.first << endl;
                if (current_sha.count(i.first) > 0)
                {
                    if (current_sha[i.first] == previous_sha[i.first])
                    {
                        // cout << "Here--->>" << i.first << endl;
                        //  name_of_track += i.first + "\n";
                    }
                    else
                    {
                        name_of_mod += (i.first);
                        name_of_mod += "\n";
                        // cout << "Modified file-->>" << name_of_mod;
                    }
                }
                else
                {
                    name_of_del += (i.first);
                    name_of_del += "\n";
                }
            }
            for (auto i : current_sha)
            {
                if (previous_sha.count(i.first) == 0)
                {
                    name_of_un += (i.first);
                    name_of_un += "\n";
                }
            }
            for (auto i : currTrackedSHA)
            {

                name_of_track += (i.first);
                name_of_track += "\n";
            }
        }

        string output = "";
        if (currTrackedSHA.size() == 0)
        {
            output = "Nothing to commit\n";
        }
        else
        {
            output += tracked_file;
            output += name_of_track;
            output += un_tracked;
            output += name_of_un;
            output += "\n";
            output += deleted_file;
            output += name_of_del;
            output += "\n";
            output += modified_file;
            output += name_of_mod;
        }

        cout << output;
    }

    void vcsCmndStatus()
    {
        cout << "changes being made" << endl;
        getPrevFilesSHA();
        string cwd = fs::current_path();
        traverseFiles(cwd);
        // cout << "Current SHA vals" << endl;
        // for(auto itr = currSHA.begin();itr != currSHA.end();itr++)
        //     cout << itr->first << " " << itr->second << endl;

        // cout << "Previous SHA vals" << endl;
        // for(auto itr = previousSHA.begin();itr != previousSHA.end();itr++)
        //     cout << itr->first << " " << itr->second << endl;

        git_status(previousSHA, currSHA);
    }

}