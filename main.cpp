#include<bits/stdc++.h>
#include <dirent.h>
#include <sys/stat.h> // imported to create directory
#include <sys/types.h> // imported to create directory
#include <fstream> // filehandling
#include <openssl/sha.h> // SHA 
#include <ctime>  // using for timestamp
#include "add.h"
#include "diff.h"
#include "status.h"
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

struct commitData{
    int commitNo;
    char commitSHA[50], message[200], timeStamp[50], hexCode[10];
};

map<int, commitData> commitInfoMap; // <commitNo, struct commitData>
map<string, int> commitHexMap; // <hex, commitNo> 
void loadCommitData(){
    // cout << "Inside commit Data load\n";
    FILE *commitFile;
    commitFile = fopen("./.vcs/commit.info", "r");

    commitData cmt;
    while (fread(&cmt, sizeof(commitData), 1, commitFile)){
        commitInfoMap[cmt.commitNo] = cmt;
        commitHexMap[cmt.hexCode] = cmt.commitNo;
        // cout << c.commitNo << " " << c.message << endl;
    }
    fclose(commitFile);
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

    loadCommitData();
    return vcs = true;
}

void handleInit(){
    // Creating a directory
    if(checkVcs()){
        cout << "VCS initialised already...\n";
        return;
    }
    int err = mkdir(".vcs", 0777);
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
    fileName = "./.vcs/commit.info";
    fstream file2(fileName.c_str(), ios::in | ios:: app);
    file2.close();
}

string generateHex(){
    char chars[]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
  
    string hex;
    for(int i = 0; i < 7; i++)
        hex += chars [rand() % 16];
    return hex;
}

void createForPatch(string path1, string path2){
    string command = "diff ";
    string filename, destPath;
    int found = path1.find_last_of(".");
    filename = path1.substr(0, found);

    cout<<filename<<endl;
    cout << path1 << " " << path2 << "  "<< filename<<endl;
    command = command + path2 + " " + path1 + " >> " + path1+".patch";
    system(command.c_str());
    string remove_command = "rm " + path1;
    system(remove_command.c_str());
    
}

string calculateFileSHA(string cmtData){
    unsigned char completeFileSHA[SHA_DIGEST_LENGTH];
    SHA_CTX shaContext;

    SHA1_Init(&shaContext);
    SHA1_Update(&shaContext, cmtData.c_str(), cmtData.size());
    SHA1_Final(completeFileSHA, &shaContext);
    stringstream ss;
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
        ss << hex << setw(2) << setfill('0') << (int)completeFileSHA[i];

    string commitSHA = ss.str();
    cout << commitSHA << endl;
    return commitSHA;
}

void handleCommit(string commitMsg){
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
            if(currFile == ".." || currFile == "." || currFile == ".vcs" || currFile == "add.h" || currFile == "commit.h" || currFile == "status.h" || currFile == "diff.h"|| currFile == "a.out" || currFile == ".git" || currFile == ".vscode" || currFile == "main.cpp")
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
            if(currFile == ".." || currFile == "." || currFile == ".vcs" || currFile == "add.h" || currFile == "commit.h" || currFile == "status.h" || currFile == "diff.h"|| currFile == "a.out"||currFile==".git"|| currFile == ".vscode" || currFile == "main.cpp")
                continue;
            else
                old.insert(sd->d_name);
            cout << "old " << sd->d_name << endl;
        }
        
        cout << "out of loop\n\n";
        for(auto it1 : old){
            struct stat sfile;
            string currFile = path1  + it1;
            stat(currFile.c_str(), &sfile);

            if((sfile.st_mode & S_IFDIR)){
                cout << currFile << " ";
                DIR *dir;
                struct dirent *sd;
                dir = opendir(currFile.c_str());
                old.erase(old.find(it1));
                string tempPath = it1;

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
            stat(currFile.c_str(), &sfile);

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
    string fileName = "./.vcs/version.info", hexCode;
    temp = to_string(versionNo);
    remove(fileName.c_str());
    fstream file(fileName.c_str(), ios::in | ios:: app);
    file.write(temp.c_str(), temp.size());
    file.close();

    string currHex = generateHex();
    while(commitHexMap.find(currHex) != commitHexMap.end())
        currHex = generateHex();

    string commitHex = calculateFileSHA(to_string(versionNo-1) + commitMsg + currHex);

    auto t = std::chrono::system_clock::now(); 
    std::time_t currTime = std::chrono::system_clock::to_time_t(t);

    struct commitData cd;
    string timeStamp(std::ctime(&currTime));
    cout << endl << timeStamp << endl;
	cd.commitNo = versionNo-1;
	strcpy(cd.commitSHA, commitHex.c_str());
    strcpy(cd.message, commitMsg.c_str());
    strcpy(cd.timeStamp, timeStamp.c_str());
    strcpy(cd.hexCode, currHex.c_str());
    commitHexMap[commitHex] = versionNo-1;
    commitInfoMap[versionNo-1] = cd;

	FILE *commitFile;
	commitFile = fopen("./.vcs/commit.info", "a");
	if(!commitFile) {
		cout << "Invalid file" << endl;
		return;
	}
	if(	fwrite(&cd, sizeof(struct commitData), 1, commitFile) == 0) {
		string msg = "commit File Handling Error!!\n";
		cout << msg << endl;
	}
	fclose(commitFile);
    cout << "Commit " << versionNo - 1 << " is added with " << commitHex <<" .\n";
}

void log(){
    for(auto it = commitInfoMap.begin(); it != commitInfoMap.end(); it++){
        struct commitData cmt = it->second;
        cout << "commit " << cmt.commitSHA << endl <<  "Date: " << cmt.timeStamp  << "\t" << cmt.message << "\n\n";
    }
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

int main(int argc, char *argv[]){
    string cmnd = "";
    for(int i= 1 ; i < argc; i++){
        string temp(argv[i]);
        cmnd += temp + " ";
    }
    cmnd = cmnd.substr(0, cmnd.size() - 1);

    checkVcs();
 
    vector<string> cmndArgs = parseCommands(cmnd);
    if(cmndArgs[0] == "vcs" && cmndArgs[1] == "init")
        handleInit(); // passing path as argument
    else if(cmndArgs[0] == "validate" ){
        validPath(cmndArgs[1]); // passing path as argument
    }
    else if(cmndArgs[0] == "vcs" && cmndArgs[1] == "commit"){
        string commitMsg = "";
        for(int i = 2; i < (int)cmndArgs.size(); i++)
            commitMsg += cmndArgs[i] + " ";
        handleCommit(commitMsg.substr(0, commitMsg.size() - 1)); // passing path as argument
    }
    else if(cmndArgs[0] == "vcs" && cmndArgs[1] == "add" && cmndArgs.size() > 2 && cmndArgs[2] == ".")
        add::addComplete(); // passing path as argument
    else if(cmndArgs[0] == "vcs" &&  cmndArgs[1] == "diff")
        diff::difference_between_two(cmndArgs[1]);
    else if(cmndArgs[0] == "vcs" && cmndArgs[1] == "log")
        log();
    else if(cmndArgs[0] == "vcs" && cmndArgs[1] == "status")
        status::vcsCmndStatus();
    else if(cmndArgs[0] == ":exit")
        return 0;
    else
        cout << "Invalid Command\n";

    return 0;
}
