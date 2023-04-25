#include <string.h>

    // 와일드카드 문자가 '?'거나 파일이름과 일치하면 i, j ++
    // 다르다면 '*'인지 여부를 확인하고 wildcard_pos와 filename_pos에 현재 문자 위치를 저장해두고 j++
    // 다음 wildcard_str[j]가 일치하지 않으면 저장해두었던 위치로 돌아가서 일치할때까지 i++

    // 하나도 일치하지 않으면 false 반환, 마지막 문자가 *라면 그만큼 j++
    // j == wildcard_len이라면 모두 일치니까 true 반환, 아니면 false 
bool WildMatch(char *wildcard_str, char *filename_str) {
    int i = 0;
    int j = 0;
    int wildcard_pos = -1;
    int filename_pos = -1;
    int wildcard_len = strlen(wildcard_str);
    int filename_len = strlen(filename_str);

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