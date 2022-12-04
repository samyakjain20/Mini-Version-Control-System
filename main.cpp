#include <bits/stdc++.h>
#include <dirent.h>
#include <sys/stat.h>    // imported to create directory
#include <sys/types.h>   // imported to create directory
#include <fstream>       // filehandling
#include <openssl/sha.h> // SHA
#include <ctime>         // using for timestamp
#include <filesystem>
#include "add.h"
#include "diff.h"
#include "status.h"
#include "rollback.h"
using namespace std;

namespace fs = std::filesystem;
bool vcs = false; // indicating if there is vcsfolder cerated
int versionNo;
string branchName = "main";

bool validPath(string &path)
{
    char resPath[300];
    realpath(path.c_str(), resPath); // absolute path
    string temp(resPath);
    path = temp;
    // cout << path <<"\n";

    struct stat sfile;
    int exist = stat(path.c_str(), &sfile);
    // cout<<path<<" "<<exist<<endl;
    if (exist == -1)
        return false;
    else
        return true;
}

struct commitData
{
    int commitNo;
    char commitSHA[50], message[200], timeStamp[50], hexCode[10];
};

map<int, commitData> commitInfoMap; // <commitNo, struct commitData>
map<string, int> commitHexMap;      // <hex, commitNo>
void loadCommitData()
{
    // cout << "Inside commit Data load\n";
    FILE *commitFile;
    commitFile = fopen("./.vcs/commit.info", "r");

    commitData cmt;
    while (fread(&cmt, sizeof(commitData), 1, commitFile))
    {
        commitInfoMap[cmt.commitNo] = cmt;
        commitHexMap[cmt.hexCode] = cmt.commitNo;

        // cout << cmt.commitNo << " " << cmt.hexCode << " " << cmt.message << endl;
    }
    fclose(commitFile);
}

bool checkVcs()
{
    string path = "./.vcs";
    char resPath[300];
    realpath(path.c_str(), resPath); // absolute path
    string temp(resPath);
    path = temp;

    struct stat sfile;
    int exist = stat(path.c_str(), &sfile);
    if (exist == -1)
        return vcs = false;

    string fileName = "./.vcs/version.info";
    fstream file(fileName.c_str());

    file >> temp;

    cout << endl
         << endl;
    cout << "\033[1;37m";
    cout << "\033[1;7m";
    cout << "\t\t\t\t\t\t\t\t\t Current Version no : " << temp << " " << endl
         << endl;
    cout << "\033[0m";
    // printf("\033[%d;%dH\u001b[0m\u001b[42m Version No. \u001b[0m %s",0,50,temp);
    versionNo = stoi(temp);

    loadCommitData();
    return vcs = true;
}

void handleInit()
{
    // Creating a directory
    if (checkVcs())
    {
        cout << "VCS initialised already...\n";
        return;
    }
    int err = mkdir(".vcs", 0777);
    if (err != -1)
    {
        string temp = "./.vcs/" + to_string(versionNo);
        if (mkdir(temp.c_str(), 0777) == -1)
        {
            cerr << "Error creating vcs -> 0 :  " << strerror(errno) << endl;
            return;
        }
    }
    else
    {
        cerr << "Error creating vcs :  " << strerror(errno) << endl;
        return;
    }
    cout << "\033[1;32m";
    cout << "VCS initialised...\n";
    cout << "\033[0m";
    vcs = true;

    string fileName = "./.vcs/version.info", temp = "0";
    fstream file(fileName.c_str(), ios::in | ios::app);
    file.write(temp.c_str(), temp.size());
    file.close();
    fileName = "./.vcs/commit.info";
    fstream file2(fileName.c_str(), ios::in | ios::app);
    file2.close();
    fileName = "./.vcs/tracked_history.txt";
    fstream file3(fileName.c_str(), ios::in | ios::app);
    file3.close();

    fileName = "./.vcs/tracked_current.txt";
    fstream file4(fileName.c_str(), ios::in | ios::app);
    file4.close();
}

string generateHex()
{
    char chars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    string hex;
    srand((unsigned)time(NULL));
    for (int i = 0; i < 7; i++)
        hex += chars[rand() % 16];
    return hex;
}

void createForPatch(string path1, string path2)
{
    string command = "diff ";
    string filename, destPath;
    int found = path1.find_last_of(".");
    filename = path1.substr(0, found);
    command = command + path2 + " " + path1 + " >> " + path1 + ".patch";
    system(command.c_str());
    string remove_command = "rm " + path1;
    system(remove_command.c_str());
}

string calculateFileSHA(string cmtData)
{
    unsigned char completeFileSHA[SHA_DIGEST_LENGTH];
    SHA_CTX shaContext;

    SHA1_Init(&shaContext);
    SHA1_Update(&shaContext, cmtData.c_str(), cmtData.size());
    SHA1_Final(completeFileSHA, &shaContext);
    stringstream ss;
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
        ss << hex << setw(2) << setfill('0') << (int)completeFileSHA[i];

    string commitSHA = ss.str();
    // cout << commitSHA << endl;
    return commitSHA;
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
        if (currFile == ".." || currFile == "." || currFile == ".vcs" || currFile == "add.h" || currFile == "commit.h" || currFile == "status.h" || currFile == "diff.h" || currFile == "vcs" || currFile == ".git" || currFile == ".vscode" || currFile == "main.cpp" || currFile == "rollback.h")
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
void changes_on_commit()
{
    string vcspath1 = "./.vcs/tracked_current.txt";
    string vcspath2 = "./.vcs/tracked_history.txt";
    ifstream vfile1;
    ofstream vfile2;
    vfile1.open(vcspath1);
    vfile2.open(vcspath2, std::ios_base::in | std::ios_base::out | std::ios_base::app);
    if (!vfile1.is_open() && !vfile2.is_open())
    {
        perror("Error open");
        return;
    }
    string line;
    while (getline(vfile1, line))
    {
        vfile2 << line;
        vfile2 << "\n";
    }
    vfile1.close();
    vfile2.close();
    std::ofstream ofs;
    ofs.open(vcspath1, std::ofstream::out | std::ofstream::trunc);
    ofs.close();
}

void handleCommit(string commitMsg)
{
    // will create patch
    if (versionNo > 0)
    {
        string path1 = "./.vcs/" + to_string(versionNo - 1) + "/", path2 = "./.vcs/" + to_string(versionNo) + "/";
        // cout << path1 << " " << path2 << endl;

        DIR *dir1, *dir2, *dir;
        struct dirent *sd;
        dir1 = opendir(path1.c_str());
        dir2 = opendir(path2.c_str());

        if (dir1 == NULL || dir2 == NULL)
        {
            cout << "'.vcs' folder corrupted.\n";
            return;
        }

        vector<string> old, latest; // stores path of all the files / directories
        dir = dir2;
        while ((sd = readdir(dir)) != NULL)
        {
            string currFile = sd->d_name;
            if (currFile == ".." || currFile == "." || currFile == ".vcs" || currFile == "add.h" || currFile == "commit.h" || currFile == "status.h" || currFile == "diff.h" || currFile == "vcs" || currFile == ".git" || currFile == ".vscode" || currFile == "main.cpp" || currFile == "rollback.h")
                continue;
            else
            {
                struct stat sfile;
                currFile = path2 + sd->d_name;
                stat(currFile.c_str(), &sfile);

                if (fs::is_directory(currFile.c_str()))
                    getFileRecursive(latest, path2, sd->d_name);
                else
                    latest.push_back(sd->d_name);
            }
            // cout << "latest " << sd->d_name << endl;
        }
        if (latest.empty())
        {
            cout << "Nothing to commit..\n";
            return;
        }

        dir = dir1;
        while ((sd = readdir(dir)) != NULL)
        {
            string currFile = sd->d_name;
            // string fileDetails = getFileDetails(dir_to_search + '/'+sd->d_name);
            if (currFile == ".." || currFile == "." || currFile == ".vcs" || currFile == "add.h" || currFile == "commit.h" || currFile == "status.h" || currFile == "diff.h" || currFile == "vcs" || currFile == ".git" || currFile == ".vscode" || currFile == "main.cpp" || currFile == "rollback.h")
                continue;
            else
            {
                struct stat sfile;
                currFile = path1 + currFile;
                stat(currFile.c_str(), &sfile);

                if (fs::is_directory(currFile.c_str()))
                    getFileRecursive(old, path1, sd->d_name);
                else
                    old.push_back(sd->d_name);
            }
            // cout << "old " << sd->d_name << endl;
        }

        // cout << "out of dir thing\n\n";

        // for (auto it1 : old)
        //     cout << it1 << "\n";
        // for (auto it2 : latest)
        //     cout << it2 << "\n";
        // cout << endl;

        for (auto it1 : old)
        {
            auto it3 = find(latest.begin(), latest.end(), it1);
            if (it3 != latest.end())
                createForPatch(path1 + it1, path2 + *it3);
        }
    }
    // increasing versionNo
    versionNo++;
    string temp = "./.vcs/" + to_string(versionNo);
    if (mkdir(temp.c_str(), 0777) == -1)
    {
        cerr << "Error creating new version folder:  " << strerror(errno) << endl;
        return;
    }
    string fileName = "./.vcs/version.info", hexCode;
    temp = to_string(versionNo);
    remove(fileName.c_str());
    fstream file(fileName.c_str(), ios::in | ios::app);
    file.write(temp.c_str(), temp.size());
    file.close();
    changes_on_commit();

    string currHex = generateHex();
    while (commitHexMap.find(currHex) != commitHexMap.end())
        currHex = generateHex();

    string commitHex = calculateFileSHA(to_string(versionNo - 1) + commitMsg + currHex);

    auto t = std::chrono::system_clock::now();
    std::time_t currTime = std::chrono::system_clock::to_time_t(t);

    struct commitData cd;
    string timeStamp(std::ctime(&currTime));
    // cout << endl << timeStamp << endl;
    cd.commitNo = versionNo - 1;
    strcpy(cd.commitSHA, commitHex.c_str());
    strcpy(cd.message, commitMsg.c_str());
    strcpy(cd.timeStamp, timeStamp.c_str());
    strcpy(cd.hexCode, currHex.c_str());
    commitHexMap[commitHex] = versionNo - 1;
    commitInfoMap[versionNo - 1] = cd;

    FILE *commitFile;
    commitFile = fopen("./.vcs/commit.info", "a");
    if (!commitFile)
    {
        cout << "Invalid file" << endl;
        return;
    }
    if (fwrite(&cd, sizeof(struct commitData), 1, commitFile) == 0)
    {
        string msg = "commit File Handling Error!!\n";
        cout << msg << endl;
    }
    fclose(commitFile);
    cout << "\nCommit " << versionNo - 1 << " is added with " << commitHex << " .\n";
}
void log()
{
    int i = 0;
    for (auto it = commitInfoMap.begin(); it != commitInfoMap.end(); it++)
    {
        struct commitData cmt = it->second;

        cout << "\033[1;36m";
        cout << "Commit " << cmt.commitSHA;
        cout << "\033[0m";
        cout << "\033[1;31m";
        cout << "  (Version " << i << ")" << endl;
        cout << "\033[0m";
        cout << "\033[1;37m";
        cout << "Date: " << cmt.timeStamp;
        cout << "\033[0m";
        cout << "\033[1;33m";
        cout << "Commit Id: " << cmt.hexCode << endl
             << "\t";
        cout << "\033[0m";
        cout << "\033[1;32m";
        cout << cmt.message << "\n\n";
        cout << "\033[0m";
        i++;
    }
}

void error_print(string print)
{
    cout << "\033[1;31m";
    cout << print << endl;
    cout << "\033[0m";
    return;
}

vector<string> parseCommands(string cmnd)
{
    vector<string> args;
    int i = 0, j = 0, n = cmnd.size();
    while (i < n)
    {
        if (i == j && cmnd[i] == ' ')
            j++;
        else if (cmnd[i] == ' ')
        {
            args.push_back(cmnd.substr(j, i - j));
            j = i + 1;
        }
        i++;
    }
    args.push_back(cmnd.substr(j, i - j));
    return args;
}

bool getFileName(string &fileName)
{
    char resolved[2000];
    realpath(fileName.c_str(), resolved);
    string abs_path = string(resolved);
    string cwd = fs::current_path();

    if (abs_path.substr(0, cwd.size()) == cwd)
    {
        fileName = abs_path.substr(cwd.size() + 1, abs_path.size());
        // cout<<fileName<<endl;
        return true;
    }
    else
    {
        return false;
    }
}

int main(int argc, char *argv[])
{
    string cmnd = "";
    for (int i = 1; i < argc; i++)
    {
        string temp(argv[i]);
        cmnd += temp + " ";
    }
    cmnd = cmnd.substr(0, cmnd.size() - 1);

    checkVcs();

    vector<string> cmndArgs = parseCommands(cmnd);
    if (!vcs && cmndArgs[0] == "init")
    {
        handleInit(); // passing path as argument
    }

    else if (vcs && cmndArgs[0] == "init")
    {
        cout << "VCS already initialized\n";
    }

    else if (cmndArgs[0] == "validate")
    {
        validPath(cmndArgs[1]); // passing path as argument
    }
    else if (!vcs && cmndArgs[0] == "commit" && cmndArgs.size() > 1)
    {
        error_print("VCS not initialized");
    }
    else if (vcs && cmndArgs[0] == "commit")
    {
        if (cmndArgs.size() > 1)
        {
            string commitMsg = "";
            for (int i = 1; i < (int)cmndArgs.size(); i++)
                commitMsg += cmndArgs[i] + " ";
            handleCommit(commitMsg.substr(0, commitMsg.size() - 1)); // passing path as argument
            cout << "Commit done successfully!\n";
        }
        else
        {
            error_print("Invalid command. Commit message not provided.");
        }

        // if(msg)
        //     cout << "All the file/s are added successfully!\n";
        // else
        // cout << "All the remaning file/s are added successfully!\n";
    }
    else if (!vcs && cmndArgs[0] == "add" && cmndArgs.size() > 1)
    {
        error_print("VCS not initialized");
    }
    else if (vcs && cmndArgs[0] == "add" && cmndArgs.size() > 1)
    {
        bool msg = true;
        for (int i = 1; i < (int)cmndArgs.size(); i++)
        {
            if (fs::exists(cmndArgs[i]) == false)
            {
                cout << "File " << cmndArgs[i] << " not present...\n";
                msg = false;
                continue;
            }
            // add::addComplete(branchName, cmndArgs[i]);

            if (cmndArgs[i] == "." || getFileName(cmndArgs[i]))
            {
                add::addComplete(cmndArgs[i]);
                cout << "Adding " << cmndArgs[i] << endl;
            }
            else
            {
                cout << "File " << cmndArgs[i] << " is outside of cwd\n";
                msg = false;
                continue;
            }
            // add::addComplete(cmndArgs[i]);
        }
        if (msg)
        {
            cout << "\033[0;32m";
            cout << "All the file/s are added successfully!\n";
            cout << "\033[0m";
        }
        // else
        // {
        //     cout << "\033[0;32m";
        //     cout << "All the remaning file/s are added successfully!\n";
        //     cout << "\033[0m";
        // }
    }
    else if (!vcs && cmndArgs[0] == "diff")
    {
        error_print("VCS not initialized");
    }
    else if (vcs && cmndArgs[0] == "diff" && cmndArgs.size() == 2)
    {
        diff::difference_between_two(cmndArgs[1]);
    }

    else if (!vcs && cmndArgs[0] == "log")
    {
        error_print("VCS not initialized");
    }
    else if (vcs && cmndArgs[0] == "log")
    {
        log();
    }
    else if (!vcs && cmndArgs[0] == "status")
    {
        error_print("VCS not initialized");
    }
    else if (vcs && cmndArgs[0] == "status")
    {
        status::vcsCmndStatus();
    }
    else if (!vcs && cmndArgs[0] == "rollback" && cmndArgs.size() == 2)
    {
        error_print("VCS not initialized");
    }
    else if (vcs && cmndArgs[0] == "rollback" && cmndArgs.size() == 2)
    {
        // string tempPath = "./.vcs/" + to_string(versionNo);
        // if(!(fs::is_empty(tempPath)){
        //     cout << "Working directory not clean, "
        // }
        if (commitHexMap.find(cmndArgs[1]) == commitHexMap.end())
        {
            error_print("No such commit id found");
            return 0;
        }
        cout << "Commiting current directory status before rolling back\n";
        add::addComplete(".");
        handleCommit("commiting before rollback");
        rollback::rollback(to_string(commitHexMap[cmndArgs[1]]), "./", versionNo);
        cout << "Rollback done to commit " << cmndArgs[1] << endl;
    }
    else if (vcs && cmndArgs[0] == "retrieve" && cmndArgs[1] == "-a" && cmndArgs.size() == 3)
    {
        if (versionNo <= stoi(cmndArgs[2]))
        {
            error_print("Commit not found.");
        }
        else
        {
            cout << "\033[1;36m";
            cout << "Commit Id: " << commitInfoMap[stoi(cmndArgs[2])].hexCode << endl;
            cout << "\033[0m";
        }
    }
    else if (!vcs && cmndArgs[0] == "retrieve" && cmndArgs[1] == "-a" && cmndArgs.size() == 3)
    {
        error_print("VCS not initialized.");
    }
    else if (vcs && cmndArgs[0] == "retrieve" && cmndArgs[1] == "SHA" && cmndArgs.size() == 3)
    {
        if (versionNo <= stoi(cmndArgs[2]))
        {
            error_print("Commit not found.");
        }
        else
        {
            cout << "\033[1;36m";
            cout << "Commit SHA: " << commitInfoMap[stoi(cmndArgs[2])].commitSHA << endl;
            cout << "\033[0m";
        }
    }
    else if (!vcs && cmndArgs[0] == "retrieve" && cmndArgs[1] == "SHA" && cmndArgs.size() == 3)
    {
        error_print("VCS not initialized.");
    }
    else if (vcs && cmndArgs[0] == ":exit")
    {
        return 0;
    }

    else
    {
        error_print("Invalid Command.");
    }
    cout << endl;
    return 0;
}
