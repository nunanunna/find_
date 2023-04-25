#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <windows.h>

enum{
    MAX_SERIAL_NUM_LENGTH = 9
};

typedef struct VolumeInfo {
    char volume_name[MAX_PATH + 1];
    char volume_serial[MAX_SERIAL_NUM_LENGTH];
} VolInfo;

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

unsigned long long int GetVolumeSize(char* cwd) {
    ULARGE_INTEGER total, free;
    GetDiskFreeSpaceEx("C:\\", NULL, &total, &free);// C드라이브의 남은 공간 정보를 얻음

    return free.QuadPart;
}