#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1
#endif

static void free_memory(char **ptr)
{
    if (*ptr)
    {
        free(*ptr);
        *ptr = NULL;
    }
}

static char *read_line(int fd, char **leftover)
{
    char *buffer = malloc(BUFFER_SIZE + 1);
    char *temp_line;
    int bytes_read;

    if (!buffer)
        return NULL;

    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0)
    {
        buffer[bytes_read] = '\0';
        temp_line = *leftover ? strdup(*leftover) : strdup("");
        free_memory(leftover);

        *leftover = malloc(strlen(temp_line) + bytes_read + 1);
        if (!(*leftover))
        {
            free(buffer);
            return NULL;
        }
        strcpy(*leftover, temp_line);
        strcat(*leftover, buffer);
        free(temp_line);

        if (strchr(buffer, '\n'))
            break;
    }
    free(buffer);
    if (bytes_read == 0 && (*leftover == NULL || *(*leftover) == '\0')) {
        free_memory(leftover);
        return NULL;
    }
    return (bytes_read < 0) ? NULL : *leftover;
}

char *get_next_line(int fd)
{
    static char *leftover = NULL;
    char *line, *newline;
    size_t len;

    if (fd < 0 || BUFFER_SIZE <= 0)
        return NULL;

    if (!leftover)
        leftover = strdup("");

    leftover = read_line(fd, &leftover);
    if (!leftover)
        return NULL;

    newline = strchr(leftover, '\n');
    len = newline ? (size_t)(newline - leftover + 1) : strlen(leftover);

    line = malloc(len + 1);
    if (!line)
    {
        free_memory(&leftover);
        return NULL;
    }

    strncpy(line, leftover, len);
    line[len] = '\0';

    char *temp = strdup(newline ? newline + 1 : "");
    free_memory(&leftover);
    leftover = temp;

    return line;
}

// int main(void)
// {
//     int fd;

//     fd = open("text.txt", O_RDONLY);
//     printf("%s", get_next_line(fd));
//     printf("%s", get_next_line(fd));
//     printf("%s", get_next_line(fd));
//     printf("%s", get_next_line(fd));
//     printf("%s", get_next_line(fd));
//     printf("%s", get_next_line(fd));
//     printf("%s", get_next_line(fd));
//     printf("%s", get_next_line(fd));
//     printf("%s", get_next_line(fd));

//     close(fd);
//     return (0);
// }