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
// fcntl.h provides constants for read,write
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utility>
using namespace std;
void copydirectory(string path1, string path2);
void copyfile(string path1, string path2);

namespace fs = std::filesystem;
string vcspath = "./.vcs"; // add .vcs
string cwd = fs::current_path();
int createdir(string path1)
{
    int check;
    check = mkdir(path1.c_str(), 0777);
    if (check == 0)
        return 1;
    else
        return -1;
}
void copy_version()
{
    string version;
    // string ver=to_string(version);
    // string command1="mkdir  ./.vcs/"+ver;
    // system(command1.c_str());
    // system("touch ./.vcs/version.info");
    ifstream vfile("./.vcs/version.info");
    vfile >> version;
    vfile.close();
    DIR *folder = opendir(cwd.c_str());
    if (folder == NULL)
        return;
    // reading files in a directory
    struct dirent *file = readdir(folder);
    while (file != NULL)
    {

        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0 || strcmp(file->d_name, ".vcs") == 0 || strcmp(file->d_name, "main.cpp") == 0)
            cout << "ignore" << endl;
        else
        {

            string filepath = cwd + "/" + file->d_name;
            string new_path = "./.vcs/"+version+"/"+ string(file->d_name);
            if (file->d_type == DT_DIR)
            {
                cout << "Directory" << filepath << endl;

                // cout<<"directory_detected";
                // create_dir_structure(new_path,"0");
                copydirectory(filepath, new_path);
            }

            else
            {
                // create_dir_structure(new_path,"0");
                copyfile(filepath, new_path);
            }
        }
        file = readdir(folder);
    }
    closedir(folder);
}
void create_dir_structure(string check_path,string version)
{
    // fs::vcspath p=vcspath;
    // cout<<p.root_path()<<endl;
    // cout<<fs::absolute(p)<<endl;
    char resolved[2000];
    realpath(check_path.c_str(),resolved);
    // cout<<
    string abs_path=string(resolved);
    cout << "Absolute Path: " << abs_path << endl;
    vector<string> directories_in_path;
    string temp;
    for(int i=cwd.size()+1;i<abs_path.size();i++)
    {
        if(abs_path[i]!='/')
        {
            temp+=abs_path[i];
        }
        else
        {
            directories_in_path.push_back(temp);
            temp="";
        }
        
    }
    string temp_path=vcspath+"/"+version;
    int count=0;
    for(auto i:directories_in_path)
    {
        cout << "Here " << i << endl;
        // if (count<2)
        //     {
        //         count++;
        //         temp_path=temp_path+"/"+i;
        //         continue;
        //     }
        temp_path=temp_path+"/"+i;
        if(fs::exists(temp_path))
        {
            continue;
        }
        else
        {
            string create_command="mkdir "+temp_path;
            system(create_command.c_str());
        }
    }


}
void copyfile(string path1, string path2)
{
    cout << path1 << endl;
    cout << path2 << endl;

    char buffer[1000];
    int filecheck1, filecheck2;
    struct stat buff;
    int exists = stat(path1.c_str(), &buff);

    // opening the first file
    filecheck1 = open(path1.c_str(), O_RDONLY);

    if (filecheck1 == -1)
    {
        printf("Error opening first_file\n");
        close(filecheck1);
        return;
    }

    filecheck2 = open(path2.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (filecheck2 == -1)
    {
        cout << "error" << endl;
        return;
    }

    while (read(filecheck1, buffer, 1000))
    {
        write(filecheck2, buffer, 1000);
    }
    chmod(path2.c_str(), buff.st_mode);

    printf("Successful copy");

    close(filecheck1);
    close(filecheck2);
}
void copydirectory(string path1, string path2)
{
    struct stat mbuff;
    int exists = stat(path1.c_str(), &mbuff);
    // opening the folder
    DIR *folder = opendir(path1.c_str());
    // creating a directory with the same name at specified vcspath
    if (createdir(path2) == -1)
    {
        cout << "no dir";
        return;
    }

    if (folder == NULL)
        return;
    // reading files in a directory
    struct dirent *file;
    file = readdir(folder);

    while (file != NULL)
    {

        // if(strcmp(file->d_name,name.c_str())==0){

        // 	cout<<name<<file->d_name<<endl;
        // 	return true;
        // 	}

        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
            cout << ". or .. detected" << endl;
        else
        {

            // string filepath= path1 +"/"+file->d_name;
            if (file->d_type == DT_DIR)
            {
                cout << "Directory " << path1 + "/" + file->d_name << endl;
                pair<string, string> PAIR;
                PAIR.first = path1 + "/" + file->d_name;
                PAIR.second = path2 + "/" + file->d_name;
                struct stat buff;
                int exists = stat(path1.c_str(), &buff);
                copydirectory(PAIR.first, PAIR.second);

            }
            else
            {
                pair<string, string> PAIR;
                PAIR.first = path1 + "/" + file->d_name;
                PAIR.second = path2 + "/" + file->d_name;
                struct stat buff;
                int exists = stat(path1.c_str(), &buff);
                copyfile(PAIR.first, PAIR.second);
            }
        }
        file = readdir(folder);

        chmod(path2.c_str(), mbuff.st_mode);
    }

    closedir(folder);
}



void addFile(string addPath, string version) { 
    string filepath = cwd + "/" + string(fs::path(addPath.c_str()));
    string new_path = "./.vcs/"+version+"/"+ string(fs::path(addPath.c_str()));
    if (fs::is_directory(addPath.c_str()))
    {
        cout << "Directory" << addPath << endl;
        create_dir_structure(addPath, version);
        copydirectory(addPath, new_path);
    }

    else
    {
        create_dir_structure(addPath, version);
        copyfile(filepath, new_path);
    }
}

int main()
{
    // string vcspath = "./.vcs"; // add .vcs
    // string cwd = fs::current_path();

    // create_dir_structure("./folder1/folder2/folder3/folder4");
    // return 0;
    // copyfile("./folder1/folder2/folder3/folder4/m5.txt", "./.vcs/0/folder1/folder2/folder3/folder4/m5.txt");
    addFile("Folder1/demo1.txt","0");
    return 0;
    if (fs::is_empty(vcspath.c_str()) == 1)
    {
        system("mkdir  ./.vcs/0");
        system("touch ./.vcs/version.info");
        ofstream vfile("./.vcs/version.info");
        vfile << "0";
        vfile.close();
        DIR *folder = opendir(cwd.c_str());
        if (folder == NULL)
            return 0;
        // reading files in a directory
        struct dirent *file = readdir(folder);
        while (file != NULL)
        {

            if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0 || strcmp(file->d_name, ".vcs") == 0 || strcmp(file->d_name, "main.cpp") == 0)
                cout << "ignore" << endl;
            else
            {

                string filepath = cwd + "/" + file->d_name;
                string new_path = "./.vcs/0/" + string(file->d_name);
                if (file->d_type == DT_DIR)
                {
                    cout << "Directory" << filepath << endl;
                    
                    copydirectory(filepath, new_path);
                }

                else
                {
                    copyfile(filepath, new_path);
                }
            }
            file = readdir(folder);
        }
        closedir(folder);


    }
    else
    {
        copy_version();
    }

    return 0;
}