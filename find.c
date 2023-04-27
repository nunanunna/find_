#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <locale.h>
#include "wildcard.h"
#include "volume.h"
#include "printdata.h"
#include "queue.h"

enum{
    MAX_DIRECTORY_LENGTH = 1024,
    MAX_FILE_NAME_LENGTH = 260,
    FILE_NAME = 1,
    OPTION = 2,
    CMD_ONLY = 1,
    FILE_ENTERED = 2,
    OPTION_ENTERED = 3
};

void PrintDirItems(char* file_name, DIR* file_dir, char* cwd, struct stat* file_status, DirInfo* dir_info);
int PrintDirInclOpt(struct dirent* file, DIR* file_dir, char* cwd, struct stat* file_status, DirInfo* dir_info, char* arg_file);
int PrintDirExclOpt(struct dirent* file, DIR* file_dir, char* cwd, struct stat* file_status, DirInfo* dir_info, char* arg_file);

int main(int argc, char *argv[]) {
    setlocale(LC_NUMERIC, "");

    char *cwd = (char *)malloc(sizeof(char) * MAX_DIRECTORY_LENGTH); // 현재 경로 저장 변수
    char *arg_file = (char *)malloc(sizeof(char) * MAX_DIRECTORY_LENGTH); // argv 저장 변수
    getcwd(cwd, MAX_DIRECTORY_LENGTH); // 현재경로 읽어오기

    struct stat file_status;
    DIR *file_dir = NULL;
    struct dirent *file = NULL;

    VolInfo volume_info;
    DirInfo dir_info = {0, };

    // 볼륨 정보 읽어와서 출력
    GetVolumeInfo(&volume_info, cwd);
    PrintVolumeInfo(&volume_info, cwd);

    file_dir = opendir(cwd);

    switch(argc) {
        case CMD_ONLY:
            while ((file = readdir(file_dir)) != NULL) {
                PrintCwd(cwd);
                PrintDirItems(file->d_name, file_dir, cwd, &file_status, &dir_info);
            }
            break;

        case FILE_ENTERED:
            PrintCwd(cwd);
            if(PrintDirInclOpt(file, file_dir, cwd, &file_status, &dir_info, argv[FILE_NAME]) == -1) {
                PrintFileNotFoundError();
                return 0;
            }
            break;
        case OPTION_ENTERED:
            PrintDirExclOpt(file, file_dir, cwd, &file_status, &dir_info, argv[OPTION]);
            break;
        default:
    }

    PrintDirSize(cwd, &dir_info);

    free(cwd);
    free(arg_file);
    closedir(file_dir);
}

void PrintDirItems(char* file_name, DIR* file_dir, char* cwd, struct stat* file_status, DirInfo* dir_info) {
    char path[MAX_DIRECTORY_LENGTH];

    snprintf(path, MAX_DIRECTORY_LENGTH, "%s\\%s", cwd, file_name); //파일 경로 생성
    stat(path, file_status); // 파일 stat 읽기
    PrintDirData(file_name, file_status); // 날짜/시간/디렉토리 여부 출력

    if(S_ISDIR(file_status->st_mode)) // 디렉토리면 dir_count++
        dir_info->dir_count++;
    if(S_ISREG(file_status->st_mode))  // 파일이면 file_count++
        dir_info->file_count++;

    dir_info->total_file_size += file_status->st_size; // 디렉토리 파일 총합 크기 변수에 현재 파일 크기 더함
}

int PrintDirInclOpt(struct dirent* file, DIR* file_dir, char* cwd, struct stat* file_status, DirInfo* dir_info, char* arg_file) {
    if(strcmp(arg_file, "-s")) {
        
    }
    return 0;
}

int PrintDirExclOpt(struct dirent* file, DIR* file_dir, char* cwd, struct stat* file_status, DirInfo* dir_info, char* arg_file) {
    while ((file = readdir(file_dir)) != NULL) {
        if(WildMatch(arg_file, file->d_name))
            PrintDirItems(file->d_name, file_dir, cwd, file_status, dir_info);
    if(!dir_info->dir_count && !dir_info->file_count) {
        return -1;
    }
    return 0;
}