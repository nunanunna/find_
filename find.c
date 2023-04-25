#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <windows.h>
#include <stdbool.h>
#include <locale.h>

enum{
    MAX_DIRECTORY_LENGTH = 1024,
    MAX_SERIAL_NUM_LENGTH = 9
};

typedef struct VolumeInfo {
    char volume_name[MAX_PATH + 1];
    char volume_serial[MAX_SERIAL_NUM_LENGTH];
} VolInfo;

typedef struct DirectoryInfo {
    char file_count;
    char dir_count;
    long total_file_size;
} DirInfo;

void GetVolumeInfo(VolInfo* volume_info, char* cwd); 
void PrintDirData(char* file_name, struct stat *stat_data);
void PrintVolumeInfo(VolInfo* volume_info, char* cwd);
unsigned long long int GetVolumeSize(char* cwd);
void PrintDirItems(char* file_name, DIR* file_dir, char* cwd, struct stat* file_status, DirInfo* dir_info);
void PrintDirSize(char* cwd, DirInfo* dir_info);
bool WildMatch(char *wildcard_str, char *filename_str);


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

    if (argc != 1) {
        strcpy(arg_file, argv[1]);

        while ((file = readdir(file_dir)) != NULL) {  
            if(WildMatch(arg_file, file->d_name))
                PrintDirItems(file->d_name, file_dir, cwd, &file_status, &dir_info);
        }

        if(!dir_info.dir_count && !dir_info.file_count) {
            printf("파일을 찾을 수 없습니다.\n");
            return 0;
        }
    }

    else if(argc == 1) {
        while ((file = readdir(file_dir)) != NULL) {    
            PrintDirItems(file->d_name, file_dir, cwd, &file_status, &dir_info);
        }
    }

    PrintDirSize(cwd, &dir_info);

    free(cwd);
    free(arg_file);

    closedir(file_dir);
    return 0;
}

void PrintDirItems(char* file_name, DIR* file_dir, char* cwd, struct stat* file_status, DirInfo* dir_info) {
    strcat(cwd, "\\");
    strcat(cwd, file_name); //현재 경로 문자열로 파일 경로 생성

    stat(cwd, file_status); // 파일 stat 읽기
    PrintDirData(file_name, file_status); // 날짜/시간/디렉토리 여부 출력
    getcwd(cwd, MAX_DIRECTORY_LENGTH); // 다시 실행한 경로로 덮어쓰기

    if(S_ISDIR(file_status->st_mode)) // 디렉토리면 dir_count++
        dir_info->dir_count++;
    if(S_ISREG(file_status->st_mode)) // 파일이면 file_count++
        dir_info->file_count++;

    dir_info->total_file_size += file_status->st_size; // 디렉토리 파일 크기 변수에 현재 파일 크기 더함
}


void PrintDirData(char* file_name, struct stat *stat_data)
{
    // 날짜/시간 출력
    struct tm* t = localtime(&stat_data->st_atime);
    
    printf("%d-%02d-%02d  ", t->tm_year+1900, t->tm_mon+1, t->tm_mday);
    if(t->tm_hour == 12)
        printf("오후 %02d:%02d    ", 12, t->tm_min);
    else if(t->tm_hour > 12)
        printf("오후 %02d:%02d    ", t->tm_hour-12, t->tm_min);
    else
        printf("오전 %02d:%02d    ", t->tm_hour, t->tm_min);
    
    // 디렉터리 여부 출력
    if(S_ISDIR(stat_data->st_mode))
        printf("<DIR>          ");
    else
        printf("%'14d ", stat_data->st_size);

    printf("%s\n", file_name);
}

void GetVolumeInfo(VolInfo* volume_info, char* cwd) {
    // 볼륨 시리얼 입력받아서 8진수로 변환 후 저장
    char rootPath[] = {cwd[0], cwd[1], cwd[2]}; 
    DWORD vol_serial; 
    DWORD max_comp_len; 
    DWORD fs_flags; 
    char fs_name[MAX_PATH + 1]; 

    BOOL result = GetVolumeInformationA(
        rootPath,
        volume_info->volume_name, 
        sizeof(volume_info->volume_name),
        &vol_serial, 
        &max_comp_len,
        &fs_flags, 
        fs_name,
        sizeof(fs_name));

    sprintf(volume_info->volume_serial, "%08X", vol_serial);
}

void PrintVolumeInfo(VolInfo* volume_info, char* cwd) {
    if(volume_info->volume_name == NULL)
        printf(" %c 드라이브의 볼륨에는 이름이 없습니다.\n", cwd[0]);
    else
        printf(" %c 드라이브의 볼륨: %s\n", cwd[0], volume_info->volume_name);
    
    printf(" 볼륨 일련 번호: ");
    for(int i=0; i<8; i++) {
        printf("%c", volume_info->volume_serial[i]);
        if(i==3) printf("-");
    }

    printf("\n\n %s 디렉터리\n\n", cwd);
}

unsigned long long int GetVolumeSize(char* cwd) {
    ULARGE_INTEGER total, free;
    GetDiskFreeSpaceEx("C:\\", NULL, &total, &free);// C드라이브의 남은 공간 정보를 얻음

    return free.QuadPart;
}

void PrintDirSize(char* cwd, DirInfo* dir_info) {
    printf("%'16d개 파일%'20d 바이트\n", dir_info->file_count, dir_info->total_file_size);
    printf("%'16d개 디렉터리%'17llu 바이트 남음\n", dir_info->dir_count, GetVolumeSize(cwd));
}

bool WildMatch(char *wildcard_str, char *filename_str) {
    int i = 0;
    int j = 0;
    int wildcard_pos = -1;
    int filename_pos = -1;
    int wildcard_len = strlen(wildcard_str);
    int filename_len = strlen(filename_str);

    // 와일드카드 문자가 '?'거나 파일이름과 일치하면 i, j ++
    // 다르다면 '*'인지 여부를 확인하고 wildcard_pos와 filename_pos에 현재 문자 위치를 저장해두고 j++
    // 다음 wildcard_str[j]가 일치하지 않으면 저장해두었던 위치로 돌아가서 일치할때까지 i++

    // 하나도 일치하지 않으면 false 반환, 마지막 문자가 *라면 그만큼 j++
    // j == wildcard_len이라면 모두 일치니까 true 반환, 아니면 false 
    while (i < filename_len) {
        if (j < wildcard_len && (wildcard_str[j] == '?' || wildcard_str[j] == filename_str[i])) {
            i++;
            j++;
        } else if (j < wildcard_len && wildcard_str[j] == '*') {
            wildcard_pos = j;
            filename_pos = i;
            j++;
        } else if (wildcard_pos != -1) {
            j = wildcard_pos + 1;
            i = ++filename_pos;
        } else {
            return false;
        }
    }

    while (j < wildcard_len && wildcard_str[j] == '*') {
        j++;
    }

    return j == wildcard_len ? true : false;
}