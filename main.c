#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// stdbool.h包含true这个字段 否则c不识别true
#include <stdbool.h>
// getline中使用的库
#include <errno.h>
typedef struct
{
    char *buffer;
    size_t buffer_length;
    size_t input_length;
} InputBuffer;

//初始化
InputBuffer *new_input_buffer()
{
    InputBuffer *input_buffer = (InputBuffer *)malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;
    return input_buffer;
}
//向用户打印命令行之前，先输出“db>”
void print_promt()
{
    printf("db > ");
}
// windows平台中c不支持getline所以直接将该部分代码放置过来
ssize_t getdelim(char **linep, size_t *n, int delim, FILE *fp)
{
    int ch;
    size_t i = 0;
    if (!linep || !n || !fp)
    {
        errno = EINVAL;
        return -1;
    }
    if (*linep == NULL)
    {
        if (NULL == (*linep = malloc(*n = 128)))
        {
            *n = 0;
            errno = ENOMEM;
            return -1;
        }
    }
    while ((ch = fgetc(fp)) != EOF)
    {
        if (i + 1 >= *n)
        {
            char *temp = realloc(*linep, *n + 128);
            if (!temp)
            {
                errno = ENOMEM;
                return -1;
            }
            *n += 128;
            *linep = temp;
        }
        (*linep)[i++] = ch;
        if (ch == delim)
            break;
    }
    (*linep)[i] = '\0';
    return !i && ch == EOF ? -1 : i;
}
ssize_t getline(char **linep, size_t *n, FILE *fp)
{
    return getdelim(linep, n, '\n', fp);
}
//读取用户输入
void read_input(InputBuffer *input_buffer)
{
    // stdin是标准输入，默认是从键盘输入,ssize_t有符号数字
    ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);
    if (bytes_read <= 0)
    {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }
    // 忽略尾部换行符
    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = 0;
}
//释放内存
void close_input_buffer(InputBuffer *input_buffer)
{
    free(input_buffer->buffer);
    free(input_buffer);
}

int main(int argc, char *argv[])
{
    InputBuffer *inputBuffer = new_input_buffer();
    while (true)
    {
        print_promt();
        read_input(inputBuffer);
        if (strcmp(inputBuffer->buffer, ".exit") == 0)
        {
            close_input_buffer(inputBuffer);
            exit(EXIT_SUCCESS);
        }
        else
        {
            printf("Unrecognized command  '%s'. \n ", inputBuffer->buffer);
        }
    }
}
