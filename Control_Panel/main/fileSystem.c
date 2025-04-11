#include "headers/fileSystem.h"

esp_vfs_littlefs_conf_t conf;

bool initFileSystem()
{
    ESP_LOGI(FILE_SYSTEM_TAG, "Initializing LittleFS...");

    conf.base_path              = "/littlefs";
    conf.partition_label        = "storage";
    conf.format_if_mount_failed = false;
    conf.dont_mount             = false;

    esp_err_t ret = esp_vfs_littlefs_register(&conf);

    if(ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
            ESP_LOGE(FILE_SYSTEM_TAG, "Failed to mount or format filesystem");
        else if (ret == ESP_ERR_NOT_FOUND)
            ESP_LOGE(FILE_SYSTEM_TAG, "Failed to find LittleFS partition");
        else
            ESP_LOGE(FILE_SYSTEM_TAG, "Failed to initialize LittleFS (%s)", esp_err_to_name(ret));
        return false;
    }

    size_t total = 0, used = 0;
    ret = esp_littlefs_info(conf.partition_label, &total, &used);
    if(ret != ESP_OK)
    {
        ESP_LOGE(FILE_SYSTEM_TAG, "Failed to get LittleFS partition information (%s)", esp_err_to_name(ret));
        unmountFileSystem();
        return false;
    }else
        ESP_LOGI(FILE_SYSTEM_TAG, "Partition size: total: %d, used: %d", total, used);
        
    return true;
}

FILE* openFile(char *path, char *mode)
{
    ESP_LOGI(FILE_SYSTEM_TAG, "Opening file: %s", path);
    FILE *prt_file = fopen(path, mode);
    if(prt_file == NULL)
    {
        ESP_LOGE(FILE_SYSTEM_TAG, "Failed to open file: %s", path);
        return NULL;
    }
    return prt_file;
}

bool fileExist(char *path)
{
    struct stat st;
    if(stat(path, &st) == 0) {
        return true;
    }
    return false;
}

bool dirExist(char *path)
{
    DIR* dir = opendir(path);
    if(dir)
    {
        closedir(dir);
        return true;
    }else if(ENOENT == errno)
        ESP_LOGE(FILE_SYSTEM_TAG, "Directory don't exist: %s", path);
    else
        ESP_LOGE(FILE_SYSTEM_TAG, "Failed to open directory: %s", path);
    return false;
}

bool deleteFile(char *path)
{
    struct stat st;
    if (stat(path, &st) == 0)
    {
        if(unlink(path) == 0)
        {
            ESP_LOGI(FILE_SYSTEM_TAG, "File deleted: %s", path);
            return true;
        }else
            ESP_LOGE(FILE_SYSTEM_TAG, "Error to delete file: %s", path);
    }
    return false;
}

bool deleteDir(char *path)
{
    struct stat st;
    if(rmdir(path) == 0)
    {
        ESP_LOGI(FILE_SYSTEM_TAG, "Directory deleted: %s", path);
        return true;
    }else
        ESP_LOGE(FILE_SYSTEM_TAG, "Error to delete file: %s", path);
    return false;
}

bool renameFile_or_Dir(char *lastPathName, char *newPathName)
{
    if(rename(lastPathName, newPathName) == 0)
    {
        ESP_LOGI(FILE_SYSTEM_TAG, "Rename sucessfully: %s", newPathName);
        return true;
    }else
        ESP_LOGE(FILE_SYSTEM_TAG, "Rename failed: %s", lastPathName);
    return false;
}

bool unmountFileSystem()
{
    // unmount partition and disable LittleFS
    if(esp_vfs_littlefs_unregister(conf.partition_label) == ESP_OK)
    {
        ESP_LOGI(FILE_SYSTEM_TAG, "File System unmounted.");
        return true;
    }else
        ESP_LOGE(FILE_SYSTEM_TAG, "Error in the unmount process.");
    return false;
}