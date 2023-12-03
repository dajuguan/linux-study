#include <stdio.h>
#include <dirent.h>
#include <string.h>

int list_dir(const char *dirpath) {
    struct dirent *files;
    DIR *dir = opendir(dirpath);
    int count = 0;
    if (dir == NULL)
    {
        perror("Directory cannot be opened!");
        exit(0);
    }
    while ((files = readdir(dir)) != NULL)
    {
        const char *file = files->d_name;
        if (strcmp(file, "..") != 0 && strcmp(file, ".") != 0) {
            printf("%s\n", file);
            count += 1;
        }
    }
    closedir(dir);
    return count;
}

int main(void)
{
    const char dirpath[] = "/root/now/";
    int count = list_dir(dirpath);
    printf("dir: %s, contains %d files and sub directories.\n", dirpath, count);
    return 0;
}