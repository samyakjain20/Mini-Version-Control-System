
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
    #include <openssl/sha.h>
    #include <utility>
    using namespace std;
    void copydirectory(string path1, string path2);
    void copyfile(string path1, string path2);

    namespace fs = std::filesystem;
    string vcspath = "./.vcs"; // add .vcs
    
    void calculateFileSHA(string fileLoc)
    {
        unsigned char completeFileSHA[SHA_DIGEST_LENGTH];
        int i;
        FILE *inFile = fopen(fileLoc.c_str(), "rb");
        SHA_CTX shaContext, shaPieceContext;
        int bytes;
        // 524288
        unsigned char data[524288];

        if (inFile == NULL)
        {
            cout << "Cannot open " << fileLoc << endl;
            return;
        }

        // vector<string> chunkSHA1;
        int chunks = 0;
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
        

        string appendData = "\n" + fileLoc + "$" + fullFileSHA1;
        string cwd = fs::current_path();
        string version;
        ofstream vfile;
        vfile.open("./.vcs/version.info", std::ios_base::app); // append instead of overwrite
        vfile << appendData;
        vfile.close();
    }


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
        string cwd = fs::current_path();
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
                string new_path = "./.vcs/" + version + "/" + string(file->d_name);
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
    void create_dir_structure(string check_path, string version)
    {
        string cwd = fs::current_path();
        // fs::vcspath p=vcspath;
        // cout<<p.root_path()<<endl;
        // cout<<fs::absolute(p)<<endl;
        char resolved[2000];
        realpath(check_path.c_str(), resolved);
        // cout<<
        string abs_path = string(resolved);
        cout << "Absolute Path: " << abs_path << endl;
        vector<string> directories_in_path;
        string temp;
        for (int i = cwd.size() + 1; i < abs_path.size(); i++)
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
        string temp_path = vcspath + "/" + version;
        int count = 0;
        for (auto i : directories_in_path)
        {
            cout << "Here " << i << endl;
            // if (count<2)
            //     {
            //         count++;
            //         temp_path=temp_path+"/"+i;
            //         continue;
            //     }
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
    void copyfile(string path1, string path2)
    {
        string cwd = fs::current_path();
        cout << path1 << endl;
        cout << path2 << endl;
        calculateFileSHA(path1);
        char buffer[1000];
        memset(&buffer[0], 0, sizeof(buffer));
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
        int a = 0;
        while (a = read(filecheck1, buffer, 1000))
        {
            if(a != -1)
                write(filecheck2, buffer, a);
            else break;
        }
        chmod(path2.c_str(), buff.st_mode);

        printf("Successful copy");

        close(filecheck1);
        close(filecheck2);
        // fstream f1;
        // fstream f2;
        // string ch;
    
        // // opening first file to read the content
        // f1.open(path1, ios::in);
    
        // // opening second file to write
        // // the content copied from
        // // first file
        // f2.open(path2, ios::out);
    
        // while (!f1.eof()) {
    
        //     // extracting the content of
        //     // first file line by line
        //     getline(f1, ch);
    
        //     // writing content to second
        //     // file line by line
        //     f2 << ch << endl;
        // }
    
        // // closing the files
        // f1.close();
        // f2.close();
    }
    void copydirectory(string path1, string path2)
    {
        string cwd = fs::current_path();
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

    void addFile(string addPath, string version)
    {
        string cwd = fs::current_path();
        string filepath = cwd + "/" + string(fs::path(addPath.c_str()));
        string new_path = "./.vcs/" + version + "/" + string(fs::path(addPath.c_str()));
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

    void addComplete()
    {
        string cwd = fs::current_path();
        if (fs::is_empty(vcspath.c_str()) == 1)
        {
            system("mkdir  ./.vcs/0");
            system("touch ./.vcs/version.info");
            ofstream vfile("./.vcs/version.info");
            vfile << "0";
            vfile.close();
            cout << "mkdir done..." << endl;
            DIR *folder = opendir(cwd.c_str());
            if (folder == NULL)
                return ;
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


    }

    int main()
    {
        // string vcspath = "./.vcs"; // add .vcs
        // string cwd = fs::current_path();

        // create_dir_structure("./folder1/folder2/folder3/folder4");
        // return 0;
        // copyfile("./folder1/folder2/folder3/folder4/m5.txt", "./.vcs/0/folder1/folder2/folder3/folder4/m5.txt");
        // addFile("Folder1/demo1.txt", "0");
        addComplete();
        
        return 0;
    }
