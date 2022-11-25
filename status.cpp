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
using namespace std;
namespace fs = std::filesystem;
string vcspath = "./.vcs/version.info";
map<string, string> previousSHA;
map<string, string> currSHA;


void getCurrentFileSHA(string fileLoc)
{
    unsigned char completeFileSHA[SHA_DIGEST_LENGTH];
    int i;
    FILE *inFile = fopen(fileLoc.c_str(), "rb");
    SHA_CTX shaContext, shaPieceContext;
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


//get previous files SHA from version.info
void getPrevFilesSHA() {
    string line;
    ifstream myfile;
    myfile.open(vcspath);

    if (!myfile.is_open())
    {
        perror("Error open");
        return;
    }

    // get files SHA from version.info
    int cnt = 0;
    while (getline(myfile, line))
    {
        if (cnt == 0)
        {
            cnt++;
            continue;
        }
        string fileData = line;
        int pos = fileData.find("$");
        string fileLoc = fileData.substr(0, pos);
        string fileSHA = fileData.substr(pos + 1);
        cout << fileLoc << " " << fileSHA << endl;
        previousSHA[fileLoc] = fileSHA;
    }
}



void traverseFiles(string cwd) {
    // get current files SHA
    DIR *folder = opendir(cwd.c_str());
    if (folder == NULL) {
        perror("Error open");
        return;
    }
    // reading files in a directory
    struct dirent *file = readdir(folder);
    while (file != NULL)
    {

        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0 || strcmp(file->d_name, ".git") == 0 || strcmp(file->d_name, ".vcs") == 0 || strcmp(file->d_name, "main.cpp") == 0 || strcmp(file->d_name, ".vscode") == 0)
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
    string un_tracked="Untracked Files:\n";
    string deleted_file="Deleted Files:\n";
    string modified_file="Modified Files:\n";
    string name_of_un="";
    string name_of_del="";
    string name_of_mod="";
    

    for(auto i: previous_sha )
    {
        if(current_sha.count(i.first)>0)
        {
            if(current_sha[i.first]==previous_sha[i.first])
            {
                continue;
            }
            else
            {
                name_of_mod+=(i.first);
                name_of_mod+="\n";
            }
        }
        else
        {
            name_of_del+=(i.first);
            name_of_del+="\n";
        }

    }
    for(auto i: current_sha )
    {
        if(previous_sha.count(i.first)==0)
        {
            name_of_un+=(i.first);
            name_of_un+="\n";
        }

    }

    string output="";
    if(name_of_un=="" && name_of_del=="" && name_of_un=="")
    {
        output="Nothing to commit\n";
    }
    else
    {
        output+=un_tracked;
        output+=name_of_un;
        output+="\n";
        output+=deleted_file;
        output+=name_of_del;
        output+="\n";
        output+=modified_file;
        output+=name_of_mod;
    }

    cout<<output;

}



int main()
{
    getPrevFilesSHA();
    string cwd = fs::current_path();
    traverseFiles(cwd);
    cout << "Current SHA vals" << endl;
    for(auto itr = currSHA.begin();itr != currSHA.end();itr++)
        cout << itr->first << " " << itr->second << endl;

    cout << "Previous SHA vals" << endl;
    for(auto itr = previousSHA.begin();itr != previousSHA.end();itr++)
        cout << itr->first << " " << itr->second << endl;

    git_status(previousSHA,currSHA);
    return 0;
}