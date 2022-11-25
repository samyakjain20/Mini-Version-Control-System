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
string commitDatapath = "./.vcs/commit.info";

struct commitData{
    int commitNo;
    char commitSHA[50], message[200], timeStamp[50], hexCode[10];
};


int main() {


    struct commitData cd;
    FILE *inpFile = fopen(commitDatapath.c_str(), "r");
    while (fread(&cd, sizeof(struct commitData), 1, inpFile)) {
    	int commitNo = cd.commitNo;
    	string commitSHA = cd.commitSHA;
    	string commitMessage = cd.message;
    	string commitTimeStamp = cd.timeStamp;
    	string commitHexCode = cd.hexCode;
        cout << "commit Number: " << commitNo << endl;
    	cout << "commit " << commitSHA << endl;
        cout << "date: " << commitTimeStamp << endl;
        cout << "message: " << commitMessage << endl;
        cout << "hexcode: " << commitHexCode << endl;
        cout << endl;
    }
    fclose(inpFile);
    return 0;
}