/* This program prints the size of all files in the current directory. */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

int main()
{
  DIR *d;                   /* Return value of opendir(). */
  struct dirent *de;        /* Return value of each readdir() call. */
  struct stat buf;          /* The information about each file returned by stat() */
  int exists;               /* Return value of stat on each file. */
  int total_size;          /* The total size of all files. */
  
  char path[] = "/root/now/linux-study/04-ipc";
  d = opendir(path);         /* Open "." to list all the files. */
  if (d == NULL) {
    perror(".");
    exit(1);
  }
 
  total_size = get_dir_size(&path);
  printf("total size is: %u\n", total_size);

  /* Run through the directory and run stat() on each file, 
     keeping track of the total size of all of the files.  */

}

int get_dir_size(const char*pathname)
{
  struct stat s_buf;
  stat(pathname,&s_buf);
  int size = 0;
  if(S_ISDIR(s_buf.st_mode)) {
      struct dirent *entry;
      int parent_dir_len = strlen(pathname);
      DIR *dir = opendir(pathname);
    	while(entry = readdir(dir)) {
        if ((strcmp(entry->d_name,".") == 0 ||  strcmp(entry->d_name,"..") == 0)) {
          continue;
        }
        char *fullpath = malloc(parent_dir_len + strlen(entry->d_name) + 2);
        sprintf(fullpath, "%s/%s", pathname, entry->d_name);
        size += get_dir_size(fullpath);
      }
  } else {
    size += s_buf.st_size;
  }
  return size;
}