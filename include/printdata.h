#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>

typedef struct DirectoryInfo {
    char file_count;
    char dir_count;
    long total_file_size;
} DirInfo;

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

void PrintDirSize(char* cwd, DirInfo* dir_info) {
    printf("%'16d개 파일%'20d 바이트\n", dir_info->file_count, dir_info->total_file_size);
    printf("%'16d개 디렉터리%'17llu 바이트 남음\n", dir_info->dir_count, GetVolumeSize(cwd));
}

void PrintFileNotFoundError() {
    printf("파일을 찾을 수 없습니다.\n");
}