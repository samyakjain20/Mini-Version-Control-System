#include<bits/stdc++.h>
#include <dirent.h>
#include <sys/stat.h> // imported to create directory
#include <sys/types.h> // imported to create directory
using namespace std;

bool vcs = false; // indicating if there is vcsfolder cerated

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
    return vcs = true;
}

void handleInit(){
    // Creating a directory
    if(checkVcs()){
        cout << "VCS initialised already...\n";
        return;
    }
    if (mkdir("vcs", 0777) == -1)
        cerr << "Error creating vcs :  " << strerror(errno) << endl;
    else
        vcs = true;
}

// For vsc add
// void copyFileDir(string filePath, string destDir, int &err){

//     if(!validPath(filePath)){
//         // cout<<filePath<<endl;
//         cmd = "Invalid Path";
//         return;
//     }
//     string line, fileDetails = getFileDetails(filePath);
//     string file = getFileName(filePath);
//     cout<<fileDetails[0]<<"\n";
//     if(fileDetails[0] == 'd'){
//         string destRecurDir = destDir + '/' + file;
//         if (mkdir(destRecurDir.c_str(), 0777) == -1) cmd = " Error Occured";

//         DIR *dirToBeCopied;
//         string currDir = filePath;
//         dirToBeCopied = opendir(currDir.c_str());
        
//         struct dirent *sd;
//         while( (sd=readdir(dirToBeCopied)) != NULL ){
//         string currFile = sd->d_name;
//         if(currFile == ".." || currFile == ".")
//             continue;
//         else
//             copyFileDir(filePath + '/' +currFile, destRecurDir, errFlag);
//         }
//     }
//     else{
//         string sourceFile = filePath, targetFile = destDir + '/' + getFileName(filePath);
//         ifstream ini_file{ sourceFile }; // This is the original file
//         ofstream out_file{ targetFile };
//         // cout<<sourceFile<<" "<<targetFile<<" sdzcsdzcscx";
//         if (ini_file && out_file)
//         while (getline(ini_file, line)) 
//             out_file << line << "\n";
//         else errFlag = 1;

//         // Closing file
//         ini_file.close();
//         out_file.close();
//     }
//     if(errFlag) err = 1;
// }

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
    else if(cmndArgs[0] == ":exit")
        return 0;
    else
        cout << "Invalid Command\n";

    return 0;
}