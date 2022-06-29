#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// stdbool.h包含true这个字段 否则c不识别true
#include <stdbool.h>
// getline中使用的库
#include <errno.h>
//用户输入
typedef struct
{
    char *buffer;
    size_t buffer_length;
    size_t input_length;
} InputBuffer;
//虚拟机返回结果
//原命令结果
typedef enum
{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;
//事先对语句的判别结果：可以识别的语句，不可以识别的语句
typedef enum
{
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;
// sql语句
// sql语句类型：增删改查
typedef enum
{
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;
typedef struct
{
    StatementType type;
} Statement;
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

//识别命令
MetaCommandResult do_meta_command(InputBuffer *input_buffer)
{
    if (strcmp(input_buffer->buffer, ".exit") == 0)
    {
        exit(EXIT_SUCCESS);
    }
    else
    {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}
// 对用户输入的命令识别属于哪种sql（增删改查）
PrepareResult prepare_statement(InputBuffer *input_buffer,
                                Statement *statement)
{
    if (strncmp(input_buffer->buffer, "insert", 6) == 0)
    {
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }
    if (strcmp(input_buffer->buffer, "select") == 0)
    {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}
// sql语句执行
void execute_statement(Statement *statement)
{
    switch (statement->type)
    {
    case (STATEMENT_INSERT):
        printf("This is where we would do an insert.\n");
        break;
    case (STATEMENT_SELECT):
        printf("This is where we would do a select.\n");
        break;
    }
}
int main(int argc, char *argv[])
{
    InputBuffer *inputBuffer = new_input_buffer();
    while (true)
    {
        
        print_promt();
        read_input(inputBuffer);
        //非sql语句都为.xxx形式
        if (inputBuffer->buffer[0] == '.')
        {
            switch (do_meta_command(inputBuffer))
            {
                //当switch外面有循环语句时可以用循环语句的特性来结束该case
            case (META_COMMAND_SUCCESS):
                continue;
            case (META_COMMAND_UNRECOGNIZED_COMMAND):
                printf("Unrecognized command '%s'\n", inputBuffer->buffer);
                continue;

            default:
                break;
            }
        }
        Statement statement;
        //判断语句类型
        switch (prepare_statement(inputBuffer, &statement))
        {
        case PREPARE_SUCCESS:
            continue;
        case PREPARE_UNRECOGNIZED_STATEMENT:
            //无法识别的sql关键字
            printf("Unrecognized keyword at start of '%s'. \n", inputBuffer->buffer);
            continue;
        default:
            break;
        }
        //执行语句
        execute_statement(&statement);
        printf("Executed.\n");
    }
}
