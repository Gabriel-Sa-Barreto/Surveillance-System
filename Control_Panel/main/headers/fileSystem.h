#ifndef __FILE_SYSTEM_H
    #define __FILE_SYSTEM_H
    #include <stdio.h>
    #include <string.h>
    #include <sys/stat.h>
    #include <dirent.h>
    #include <errno.h>
    #include <unistd.h>
    #include "esp_err.h"
    #include "esp_log.h"
    #include "esp_littlefs.h"
    #include "general.h"

    bool  initFileSystem();
    FILE* openFile(char *path, char *mode);
    bool  fileExist(char *path);
    bool  dirExist(char *path);
    bool  deleteFile(char *path);
    bool  deleteDir(char *path);
    bool  renameFile_or_Dir(char *lastPathName, char *newPathName);
    bool  unmountFileSystem();
#endif