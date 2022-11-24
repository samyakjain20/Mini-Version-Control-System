namespace add{
    #include<bits/stdc++.h>
    #include <dirent.h>
    #include <sys/stat.h> // imported to create directory
    #include <sys/types.h> // imported to create directory
    #include <fstream> // filehandling
    using namespace std;

    bool vcs = false; // indicating if there is vcsfolder cerated
    int versionNo;

    bool validPath(string &path){
        char resPath[300];
        realpath(path.c_str(), resPath); // absolute path
        string temp(resPath);
        path = temp;
        cout << path <<"\n";

        struct stat sfile;
        int exist = stat(path.c_str(), &sfile);
        // cout<<path<<" "<<exist<<endl;
        if(exist == -1)
            return false;
        else 
            return true;  
    }

    bool checkVcs(){
        string path = "./.vcs";
        char resPath[300];
        realpath(path.c_str(), resPath); // absolute path
        string temp(resPath);
        path = temp;
        
        struct stat sfile;
        int exist = stat(path.c_str(), &sfile);
        if(exist == -1)
            return vcs = false;

        string fileName = "./.vcs/version.info";
        fstream file(fileName.c_str());

        file >> temp;
        cout << "version no: " << temp << endl; 
        versionNo = stoi(temp);

        return vcs = true;
    }

    void handleInit(){
        // Creating a directory
        if(checkVcs()){
            cout << "VCS initialised already...\n";
            return;
        }
        int err = mkdir(".vcs", 0777);
        bool tempFlag = false;
        if (err != -1){
            string temp = "./.vcs/" + to_string(versionNo);
            if (mkdir(temp.c_str(), 0777) == -1){
                cerr << "Error creating vcs -> 0 :  " << strerror(errno) << endl;
                return ;
            }
        }
        else{
            cerr << "Error creating vcs :  " << strerror(errno) << endl;
            return ;
        }
        cout << "VCS initialised..\n" ;
        vcs = true;

        string fileName = "./.vcs/version.info", temp = "0";
        fstream file(fileName.c_str(), ios::in | ios:: app);
        file.write(temp.c_str(), temp.size());
        file.close();

    }


    void createForPatch(string path1, string path2){
        string command = "diff ";
        string filename, destPath;
        int found = path1.find_last_of(".");
        filename = path1.substr(0, found);

        cout<<filename<<endl;
        cout << path1 << " " << path2 << "  "<< filename<<endl;
        command = command + path2 + " " + path1 + " >> " + filename+".patch";
        system(command.c_str());
        string remove_command = "rm " + path1;
        system(remove_command.c_str());
        
    }

    void handleCommit(){
        // will create patch
        if(versionNo > 0){
            string path1 = "./.vcs/" + to_string(versionNo - 1) + "/", path2 = "./.vcs/" + to_string(versionNo) + "/";
            cout << path1 << " " << path2 << endl;

            DIR *dir1, *dir2, *dir;
            struct dirent *sd;
            dir1 = opendir(path1.c_str());
            dir2 = opendir(path2.c_str());
            
            if(dir1 == NULL || dir2 == NULL){
                cout << "'.vcs' folder corrupted.\n" ;
                return;
            }

            set<string> old, latest; // stores path of all the files / directories
            dir = dir2;
            while( (sd = readdir(dir)) != NULL ){
                string currFile = sd->d_name;
                // string fileDetails = getFileDetails(dir_to_search + '/'+sd->d_name); 
                if(currFile == ".." || currFile == "." || currFile == ".vcs" || currFile == "main.cpp")
                    continue;
                else
                    latest.insert(sd->d_name);
                cout << "latest " << sd->d_name << endl;
            }
            if(latest.empty()){
                cout << "Nothing to commit..\n";
                return;
            }

            dir = dir1;
            while( (sd = readdir(dir)) != NULL ){
                string currFile = sd->d_name;
                // string fileDetails = getFileDetails(dir_to_search + '/'+sd->d_name); 
                if(currFile == ".." || currFile == "." || currFile == ".vcs" || currFile == "main.cpp")
                    continue;
                else
                    old.insert(sd->d_name);
                cout << "old " << sd->d_name << endl;
            }
            
            cout << "out of loop\n\n";
            for(auto it1 : old){
                struct stat sfile;
                string currFile = path1  + it1;
                int a = stat(currFile.c_str(), &sfile);

                if((sfile.st_mode & S_IFDIR)){
                    cout << currFile << " ";
                    DIR *dir;
                    struct dirent *sd;
                    dir = opendir(currFile.c_str());
                    old.erase(old.find(it1));

                    while( (sd = readdir(dir)) != NULL )
                        if(strcmp(sd->d_name, "..") == 0 || strcmp(sd->d_name, ".") == 0)
                            continue;
                        else{
                            cout << sd->d_name << endl;
                            old.insert(it1 + "/" + sd->d_name);
                        }
                }
            }
            cout << "1st done\n\n";
            for(auto it2 : latest){
                struct stat sfile;
                string currFile = path2  + it2;
                int a = stat(currFile.c_str(), &sfile);

                if((sfile.st_mode & S_IFDIR)){
                    cout << currFile << " ";
                    DIR *dir;
                    struct dirent *sd;
                    dir = opendir(currFile.c_str());
                    latest.erase(latest.find(it2));

                    while( (sd = readdir(dir)) != NULL )
                        if(strcmp(sd->d_name, "..") == 0 || strcmp(sd->d_name, ".") == 0)
                            continue;
                        else{
                            cout << sd->d_name << endl;
                            latest.insert(it2 + "/" + sd->d_name);
                        }
                }
            }
            cout << "out of dir thing\n\n";

            for(auto it1 : old)
                cout << it1 << "\n";
            for(auto it2 : latest)
                cout << it2 << "\n"; 
            cout << endl;
            auto it1 = old.begin(), it2 = latest.begin();
            for(auto it1 : old){
                auto it3 = latest.find(it1);
                if(it3 != latest.end())
                    createForPatch(path1 + it1, path2 + *it3);
            }
        }
        // increasing versionNo
        versionNo++;
        string temp = "./.vcs/" + to_string(versionNo);
        if (mkdir(temp.c_str(), 0777) == -1){
            cerr << "Error creating new version folder:  " << strerror(errno) << endl;
            return ;
        }
        string fileName = "./.vcs/version.info";
        temp = to_string(versionNo);
        remove(fileName.c_str());
        fstream file(fileName.c_str(), ios::in | ios:: app);
        file.write(temp.c_str(), temp.size());
        file.close();
        cout << "Commit " << versionNo - 1 << " is added.\n";
    }

    vector<string> parseCommands(string cmnd){

        vector<string> args;
        int i = 0, j = 0, n = cmnd.size();
        while (i < n){
            if (i == j && cmnd[i] == ' ')
                j++;
            else if (cmnd[i] == ' '){
                args.push_back(cmnd.substr(j, i - j));
                j = i + 1;
            }
            i++;
        }
        args.push_back(cmnd.substr(j, i - j));
        return args;
    }

    int main(){
        checkVcs();

        // for now we are taking command line input
        string cmnd;
        cout << ">> ";
        getline(cin, cmnd, '\n');
        // cin >> cmnd;

        vector<string> cmndArgs = parseCommands(cmnd);
        if(cmndArgs[0] == "vcs" && cmndArgs[1] == "init")
            handleInit(); // passing path as argument
        else if(cmndArgs[0] == "validate" ){
            validPath(cmndArgs[1]); // passing path as argument
        }
        else if(cmndArgs[0] == "vcs" && cmndArgs[1] == "commit")
            handleCommit(); // passing path as argument
        else if(cmndArgs[0] == ":exit")
            return 0;
        else
            cout << "Invalid Command\n";

        return 0;
}
}