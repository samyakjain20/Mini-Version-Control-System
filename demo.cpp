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
	cd.commitNo = 0;
	strcpy(cd.commitSHA, "1qwedc0d6b769ebddc6btyhvs1e5b483971be5a0");
    strcpy(cd.message, "Second Commit");
    strcpy(cd.timeStamp, "Thu Nov 24 15:55:55 2022 +0530");
    strcpy(cd.hexCode, "acd6wpoq");

	FILE *datafile;
	datafile = fopen(commitDatapath.c_str(), "a");
	if(!datafile) {
		cout << "Invalid file" << endl;
		return 0;
	}
	fwrite(&cd, sizeof(struct commitData), 1, datafile);
	if(fwrite == 0) {
		string msg = "File Handling Error!!\n";
		cout << msg << endl;
	}
	fclose(datafile);

    return 0;
}