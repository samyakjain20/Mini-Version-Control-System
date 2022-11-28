# Mini - Version Control System
We built a mini version control system. Version control software keeps track of every modification to the code in a special kind of database. If a mistake is made, developers can turn back the clock and compare earlier versions of the code to help fix the mistake while minimizing disruption to all team members. 

We can run commands like 'init', 'add', 'commit', 'log', 'status, 'diff', 'rollback', 'retrieve -a' and 'retrieve SHA'.



## Following are commands that are implemented:

### ./vcs init
```
./vcs init 
```

This command will create a .vcs repository and the required structure. This folder will contain all the version control information. 

### ./vcs status
```
./vcs status 
```
The following command will show the status of the current working tree. Initially all will be untracked. 

### ./vcs add [file name/s]
```
./vcs add .
```
This command will add the file/files in the current directory to the staging area. 

### ./vcs commit [message]
```
./vcs commit “first commit” 
```
The following command will commit all the files in the staging area. 

### ./vcs diff [file name]
```
./vcs diff temp.txt 
```
The following command shows the difference for temp.txt. As described above. 


### ./vcs log
```
./vcs log 
```
The following command will log all the previous commit data. 

### ./vcs rollback [commmitId]
```
./vcs rollback 76931fa 
```
This will first commit the current staging area, and then rollback to the version corresponding to the commit id given. 

### ./vcs retrieve -a [versionNo]
```
./vcs retrieve -a 0 
```
This command will return the “commit id” of the version no, if the version no is valid. 

### ./vcs retrieve SHA [versionNo]
```
./vcs retrieve SHA 0 
```
This command will return the “commit SHA” of the version no, if the version no is valid. 
