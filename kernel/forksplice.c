#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

void hook(void *ptr);

void processexe(const char *path)
{
    if (fork() == 0)
    {
        // we're the child lets splice in the file's code
        FILE *f = fopen(path, "rb");
        if (f == NULL)
            exit(EXIT_FAILURE);
        // set a random offset modulo the exe size
        fseek(f, 0, SEEK_END);
        off_t size = ftell(f);

        fseek(f, rand() % size, SEEK_SET);  // seek to a random portion
        
        size_t cbRead = rand() % 1024;
        if (cbRead == 0)
            exit(EXIT_FAILURE);

        char *buffer = malloc(cbRead);
        fread(buffer, cbRead, 1, f);
        hook(buffer);
        free(buffer);
        
        fclose(f);
    }
}

void processdir(const char *path)
{
    struct dirent *de;
    DIR *d = opendir(path);
    if (d == NULL)
        return;
        
    while ((de = readdir(d)) != NULL)
    {
        if (de->d_name[0] == '.')
            continue;

        char fpath[1024];
        strcpy(fpath, path);
        strcat(fpath, "/");
        strcat(fpath, de->d_name);
        
        if (de->d_type == DT_DIR)
        {
            processdir(fpath);
        }
        else if (de->d_type == DT_REG)
        {
            // is the file executable?
            struct stat sb;
            if (stat(fpath, &sb) == 0 && sb.st_mode & S_IXUSR)
            {
                processexe(fpath);
            }
        }
    }
    closedir(d);
}

int main(int argc, char *argv[])
{
    if (argc == 1)
        return EXIT_FAILURE;

    for (;;)
    {
        // randomly scan the filesystem for executable files
        for (int idir = 1; idir < argc; ++idir)
        {
            processdir(argv[idir]);
        }
    }
    return EXIT_SUCCESS;
}
