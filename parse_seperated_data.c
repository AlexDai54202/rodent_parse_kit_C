#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int is_tabular(FILE *filepointer); // 1 if tabular 0 if not

char **full_parse(FILE *filepointer); // return all content-columns with trailers removed

char *parse_line(char *line, int *trail_length, int col_len);

char *parse_cell(char *cell, int next_len, int is_end, int col_len);

char *parse_cell_auxilary(char *cell, int current_pointer);

int checkIfStringSpecial(char *string);

void replaceNewlineWithSpace(char *str)
{
    int length = strlen(str);

    for (int i = 0; i < length; i++)
    {
        if (str[i] == '\n')
        {
            str[i] = ' ';
        }
    }
}

int static_file_len(int size)
{ // if you pass in 0, you get the number.
    static int count = 1;
    if (size != 0)
    {
        count = size;
    }
    return count;
}

int hexToDec(char hex)
{
    if (hex >= '0' && hex <= '9')
        return hex - '0';
    else if (hex >= 'A' && hex <= 'F')
        return hex - 'A' + 10;
    else if (hex >= 'a' && hex <= 'f')
        return hex - 'a' + 10;
    else
        return -1; // Invalid hexadecimal character
}

/* Returns 0/1/2/3
0 : No single quote nor double quote in string. 'STRING' in list, STRING else.
1 : Single quote case. ""STRING"" in list, STRING else.
2 : Double quote case. 'STRING WITH REPLACEMENT(","")' in list, "STRING WITH REPLACEMENT(","")" else.
3 : Both case. 'STRING WITH REPLACEMENT(","")(',\')' in list, "STRING WITH REPLACEMENT(","")" else.
*/
int checkIfStringSpecial(char *string)
{
    int hassingle = 0;
    int hasdouble = 0;
    for (int i = 0; i < strlen(string); i++)
    {
        if (string[i] == '\'')
        {
            hassingle = 1;
        }
        if (string[i] == '\"')
        {
            hasdouble = 1;
        }
    }
    if (hassingle && hasdouble)
    {
        return 3;
    }
    else if (hasdouble)
    {
        return 2;
    }
    else if (hassingle)
    {
        return 1;
    }
    return 0;
}

char *hexToUtf8(const char *hexString)
{
    size_t length = strlen(hexString);
    if (length % 2 != 0)
    {
        printf("Invalid input: The hexadecimal string must have an even number of characters.\n");
        return NULL;
    }

    size_t utf8Length = length / 2;
    char *utf8String = (char *)malloc(utf8Length + 1); // +1 for the null terminator
    if (utf8String == NULL)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    for (size_t i = 0; i < utf8Length; i++)
    {
        int decValue = hexToDec(hexString[2 * i]);
        if (decValue == -1)
        {
            printf("Invalid input: Non-hexadecimal character found.\n");
            free(utf8String);
            return NULL;
        }
        decValue = decValue * 16 + hexToDec(hexString[2 * i + 1]);
        utf8String[i] = (char)decValue;
    }
    utf8String[utf8Length] = '\0'; // Null terminator

    return utf8String;
}

int main(int argc, char **argv)
{
    // Usage: exe_name file_name
    FILE *fp = fopen(argv[1], "r");

    // checking if file exists
    if (fp == NULL)
    {
        fputs("Unable to open file!", stdout);
        exit(1);
    }
    // file exists.

    // checking if file is tabular
    if (!is_tabular(fp))
    {
        fputs("Not tabular", stdout);
        exit(1);
    }
    // file is tabular.

    fseek(fp, 0, SEEK_SET); // reset file pointer to start of file

    // remove trailers.
    char **content = full_parse(fp);
    // trailers removed

    fclose(fp);
    exit(0);
}

int is_tabular(FILE *filepointer)
{
    char chunk[2048];
    int commas = 0;
    int cur_commas = 0;
    fgets(chunk, sizeof(chunk), filepointer);

    for (int i = 0; chunk[i] != '\0'; i++)
    {
        if (chunk[i] == ',')
        {
            commas++;
        }
    }

    int numlines = 1;
    while (fgets(chunk, sizeof(chunk), filepointer) != NULL)
    {
        // check if number of commas are constant.
        for (int i = 0; chunk[i] != '\0'; i++)
        {
            if (chunk[i] == ',')
            {
                cur_commas++;
            }
        }
        // printf("Commas: %d, Cur commas: %d\n", commas, cur_commas);
        if (cur_commas != commas)
        {
            return 0;
        }
        cur_commas = 0;
        numlines++;
    }
    static_file_len(numlines);
    return 1;
}

char **full_parse(FILE *filepointer)
{
    char line[2048];                                              // define line max length
    char *col_name_line = fgets(line, sizeof(line), filepointer); // get first column
    col_name_line[strlen(col_name_line) - 1] = '\0';              // remove the newline character.
    // printf("Column line: %s\n", col_name_line);                       // get column names column
    int col_numbers = 1;

    for (int i = 0; line[i] != '\0'; i++)
    {
        if (line[i] == ',')
        {
            col_numbers++;
        }
    }
    // printf("Total columns: %d\n", col_numbers);
    int *col_name_len = calloc(col_numbers, sizeof(int));
    char *token = strtok(col_name_line, ",");
    for (int i = 0; i < col_numbers; i++)
    {
        printf("%s", token);
        if (i != col_numbers - 1)
        {
            printf(",");
        }
        col_name_len[i] = strlen(token);
        token = strtok(NULL, ",");
    }
    printf("\n");

    int cur_line = 1;
    while (fgets(line, sizeof(line), filepointer) != NULL)
    {
        cur_line++;
        // error here.
        if (cur_line == static_file_len(0))
        {
            static_file_len(-1);
        }
        parse_line(line, col_name_len, col_numbers);
    }

    return NULL;
}

char *parse_line(char *line, int *trail_length, int col_len)
{
    if (line[strlen(line) - 1] == '\n')
    {
        line[strlen(line) - 1] = '\0';
    }
    char *token = strtok(line, ",");
    char *parsed_tok;
    for (int i = 0; token != NULL; i++)
    {
        parsed_tok = parse_cell(token, trail_length[(i + 1) % col_len], (i == (col_len - 1) ? 1 : 0), col_len);
        // printf("%d: %s\n",i, parsed_tok);
        printf("%s", parsed_tok);
        if (i != (col_len - 1))
        {
            printf(",");
        }
        token = strtok(NULL, ",");
    }
    printf("\n");
}

int static_var_size(int size)
{ // if you pass in 0, you get the number.
    static int count = 0;
    if (size)
    {
        count = size;
    }
    if (size < 0)
    {
        count = 0;
    }
    return count;
}

char *parse_cell(char *cell, int next_len, int is_end, int col_len)
{
    static_var_size(-1);
    if (0)
    {
        printf("******Cell content: %s\n", cell);
        printf("******Next label length: %d\n", next_len);
        printf("******is end label: %d\n", is_end);
        printf("******Col length: %d\n", col_len);
    }
    if (static_file_len(0) >= 0)
    {
        if (next_len >= 16)
        {
            cell[strlen(cell) - 3 * 2] = '\0';
        }
        else
        {
            if (cell[strlen(cell) - 2] == 'a')
            {
                cell[strlen(cell) - 1 * 2] = '\0';
            }
            else
            {
                cell[strlen(cell) - 3 * 2] = '\0';
            }
        }
        if (is_end)
        {
            if (col_len >= 16)
            {
                cell[strlen(cell) - 3 * 2] = '\0';
            }
            else
            {
                if (cell[strlen(cell) - 2] == '8')
                {
                    cell[strlen(cell) - 1 * 2] = '\0';
                }
                else
                {
                    cell[strlen(cell) - 3 * 2] = '\0';
                }
            }
        }
    }
    else
    {
        if (is_end)
        {
            cell[strlen(cell) - 2] = '\0'; // last entry, special case.
        }
        else
        {
            if (next_len >= 16)
            {
                cell[strlen(cell) - 3 * 2] = '\0';
            }
            else
            {
                if (cell[strlen(cell) - 2] == 'a')
                {
                    cell[strlen(cell) - 1 * 2] = '\0';
                }
                else
                {
                    cell[strlen(cell) - 3 * 2] = '\0';
                }
            }
        }
    }

    // in theory trailers should be all removed.
    return parse_cell_auxilary(cell, 0);
}

char *parse_cell_auxilary(char *cell, int is_list)
{
    if (strlen(cell) % 2 != 0 || strlen(cell) < 2)
    {
        printf("General Error Parsing, Cell length odd or less than 2!\n");
        exit(1);
    }
    char *ret_str;
    // if(is_list) {
    //     printf("%s\n", cell);
    // }
    if (cell[0] == '9')
    {
        // Array case 1
        // 9x [x*2 content]
        ret_str = calloc(strlen(cell) * 2 + 1, 1);
        char substr[1 + 1];
        memcpy(substr, &cell[1], 1);
        substr[1] = '\0';
        int lst_len = (int16_t)strtol(substr, NULL, 16);

        int item_enty = 0;
        int total_lapsed_length = 2;
        if (!is_list)
        {
            strcat(ret_str, "\"");
        }
        strcat(ret_str, "[");
        for (int i = 0; item_enty < lst_len; i++)
        {
            // parse each item, add the size of the parsed item to the length.
            if (total_lapsed_length >= strlen(cell))
            {
                printf("ERROR PARSING LIST, TOO MANY ENTRIES: %s", cell);
                exit(1);
            }
            char *item = parse_cell_auxilary(cell + total_lapsed_length, 1);

            strcat(ret_str, item);

            if (item_enty != lst_len - 1)
            {
                strcat(ret_str, ",");
            }
            item_enty += 1;
            total_lapsed_length += static_var_size(0);
        }
        strcat(ret_str, "]");
        if (!is_list)
        {
            strcat(ret_str, "\"");
        }
        if (total_lapsed_length != strlen(cell) && !is_list)
        {
            printf("ERROR PARSING LIST: %d, %d\n", total_lapsed_length, strlen(cell));
        }
        static_var_size(total_lapsed_length);
    }
    else if (cell[0] == 'd' && cell[1] == 'c')
    {
        // Array case 2
        // dc xx xx [x*2 content]
        ret_str = calloc(strlen(cell) * 2 + 1, 1);
        char substr[4 + 1];
        memcpy(substr, &cell[2], 4);
        substr[4] = '\0';
        int lst_len = (int16_t)strtol(substr, NULL, 16);

        int item_enty = 0;
        int total_lapsed_length = 6;
        if (!is_list)
        {
            strcat(ret_str, "\"");
        }
        strcat(ret_str, "[");
        for (int i = 0; item_enty < lst_len; i++)
        {
            // parse each item, add the size of the parsed item to the length.
            if (total_lapsed_length >= strlen(cell))
            {
                printf("ERROR PARSING LIST: %s", cell);
                exit(1);
            }
            char *item = parse_cell_auxilary(cell + total_lapsed_length, 1);

            strcat(ret_str, item);

            if (item_enty != lst_len - 1)
            {
                strcat(ret_str, ",");
            }
            item_enty += 1;
            total_lapsed_length += static_var_size(0);
        }
        strcat(ret_str, "]");
        if (!is_list)
        {
            strcat(ret_str, "\"");
        }
        static_var_size(total_lapsed_length);
    }
    else if (cell[0] == 'c' && cell[1] == 'a')
    {
        // float case
        // ca xx xx xx xx
        ret_str = calloc(40, 1);
        if (strlen(cell) != 10 && !is_list)
        {
            printf("Error with: %s", cell);
            exit(1);
        }
        char substr[8 + 1];
        memcpy(substr, &cell[2], 8);
        substr[8] = '\0';
        unsigned long hexValue = strtoul(substr, NULL, 16);
        float floatValue = *((float *)&hexValue);

        sprintf(ret_str, "%.3f", floatValue);
        static_var_size(10);
    }
    else if (cell[0] == 'a')
    {
        // string case 1
        // ax [x*2 content]
        char substr[1 + 1];
        memcpy(substr, &cell[1], 1);
        substr[1] = '\0';
        int str_len = (int16_t)strtol(substr, NULL, 16);

        ret_str = calloc(str_len * 2, 1);
        if (str_len != 0)
        {
            if (strlen(cell) != (2 + str_len * 2) && !is_list)
            {
                printf("Error with: %s\n", cell);
                exit(1);
            }
            char *content = calloc(str_len * 2 + 1, 1);
            memcpy(content, &cell[2], str_len * 2);
            content[str_len * 2] = '\0';
            char *utf8String = hexToUtf8(content);

            int quotesid = checkIfStringSpecial(utf8String);
            int usedoublequotesurroundoutlist = 0;
            int usedoublequotesurroundinlist = 0;
            int replacedoublequoteswithdoubledoublequotes = 0;
            int replacesinglequotewithslashsinglequote = 0;
            // int numextraspaces = 0;

            if (quotesid == 1) // single quote in string case
            {
                usedoublequotesurroundinlist = 1; // only in list.
            }
            if (quotesid == 2)
            {
                usedoublequotesurroundoutlist = 1;
                // REPLACE " WITH ""
                replacedoublequoteswithdoubledoublequotes = 1;
            }
            if (quotesid == 3)
            {
                usedoublequotesurroundoutlist = 1;
                // REPLACE " WITH "" and ' with \'
                replacedoublequoteswithdoubledoublequotes = 1;
                replacesinglequotewithslashsinglequote = 1;
            }

            if (is_list) // inside list
            {
                if (usedoublequotesurroundinlist)
                {
                    strcat(ret_str, "\"\"");
                }
                else
                {
                    strcat(ret_str, "\'");
                }
            }
            else if (usedoublequotesurroundoutlist)
            {
                strcat(ret_str, "\"\"");
            }

            for (int i = 0; i < strlen(utf8String); i++)
            {
                if (utf8String[i] == '\'' && replacesinglequotewithslashsinglequote)
                {
                    strcat(ret_str, "\\\'");
                }
                else if (utf8String[i] == '\"' && replacedoublequoteswithdoubledoublequotes)
                {
                    strcat(ret_str, "\"\"");
                }
                else
                {
                    strncat(ret_str, &(utf8String[i]), 1);
                }
            }

            if (is_list)
            {
                if (usedoublequotesurroundinlist)
                {
                    strcat(ret_str, "\"\"");
                }
                else
                {
                    strcat(ret_str, "\'");
                }
            }
            else if (usedoublequotesurroundoutlist)
            {
                strcat(ret_str, "\"\"");
            }
        }
        else
        {
            if (is_list)
            {
                strcpy(ret_str, "\'\'");
            }
            else
            {
                strcpy(ret_str, "");
            }
        }
        replaceNewlineWithSpace(ret_str);
        // printf("returned string: %s\n", ret_str);

        static_var_size(2 + str_len * 2);
    }
    else if (cell[0] == 'd' && cell[1] == 'a')
    {
        // string case 2
        // da xx xx [x*2 content]
        char substr[4 + 1];
        memcpy(substr, &cell[2], 4);
        substr[4] = '\0';
        int str_len = (int32_t)strtol(substr, NULL, 16);
        ret_str = calloc(str_len * 2 + 1, 1);
        if (str_len != 0)
        {
            if (strlen(cell) != (6 + str_len * 2) && !is_list)
            {
                printf("Error with: %s", cell);
                exit(1);
            }
            char *content = calloc(2 * str_len + 1, 1);
            memcpy(content, &cell[6], 2 * str_len);
            char *utf8String = hexToUtf8(content);
            int quotesid = checkIfStringSpecial(utf8String);
            int usedoublequotesurroundoutlist = 0;
            int usedoublequotesurroundinlist = 0;
            int replacedoublequoteswithdoubledoublequotes = 0;
            int replacesinglequotewithslashsinglequote = 0;
            // int numextraspaces = 0;

            if (quotesid == 1) // single quote in string case
            {
                usedoublequotesurroundinlist = 1; // only in list.
            }
            if (quotesid == 2)
            {
                usedoublequotesurroundoutlist = 1;
                // REPLACE " WITH ""
                replacedoublequoteswithdoubledoublequotes = 1;
            }
            if (quotesid == 3)
            {
                usedoublequotesurroundoutlist = 1;
                // REPLACE " WITH "" and ' with \'
                replacedoublequoteswithdoubledoublequotes = 1;
                replacesinglequotewithslashsinglequote = 1;
            }

            if (is_list) // inside list
            {
                if (usedoublequotesurroundinlist)
                {
                    strcat(ret_str, "\"\"");
                }
                else
                {
                    strcat(ret_str, "\'");
                }
            }
            else if (usedoublequotesurroundoutlist)
            {
                strcat(ret_str, "\"\"");
            }

            for (int i = 0; i < strlen(utf8String); i++)
            {
                if (utf8String[i] == '\'' && replacesinglequotewithslashsinglequote)
                {
                    strcat(ret_str, "\\\'");
                }
                else if (utf8String[i] == '\"' && replacedoublequoteswithdoubledoublequotes)
                {
                    strcat(ret_str, "\"\"");
                }
                else
                {
                    strncat(ret_str, &(utf8String[i]), 1);
                }
            }

            if (is_list)
            {
                if (usedoublequotesurroundinlist)
                {
                    strcat(ret_str, "\"\"");
                }
                else
                {
                    strcat(ret_str, "\'");
                }
            }
            else if (usedoublequotesurroundoutlist)
            {
                strcat(ret_str, "\"");
            }
        }
        else
        {
            if (is_list)
            {
                strcpy(ret_str, "\'\'");
            }
            else
            {
                strcpy(ret_str, "");
            }
        }
        replaceNewlineWithSpace(ret_str);

        static_var_size(6 + str_len * 2);
    }
    else if (cell[0] == 'd' && cell[1] == '1')
    {
        // integer case 1
        // d1 xx xx
        ret_str = calloc(40, 1);
        if (strlen(cell) != 6 && !is_list)
        {
            printf("Error with: %s", cell);
            exit(1);
        }
        char substr[4 + 1];
        memcpy(substr, &cell[2], 4);
        substr[4] = '\0';
        sprintf(ret_str, "%d", (int16_t)strtol(substr, NULL, 16));
        static_var_size(6);
    }
    else if (cell[0] == 'd' && cell[1] == '2')
    {
        // integer case 2
        // d2 xx xx xx xx
        ret_str = calloc(40, 1);
        if (strlen(cell) != 10 && !is_list)
        {
            printf("Error with: %s", cell);
            exit(1);
        }
        char substr[8 + 1];
        memcpy(substr, &cell[2], 8);
        substr[8] = '\0';
        sprintf(ret_str, "%d", (int32_t)strtol(substr, NULL, 16));
        static_var_size(10);
    }
    else if (cell[0] == 'c' && cell[1] == 'c')
    {
        // enum case 1
        // cc xx
        ret_str = calloc(5, 1);
        if (strlen(cell) != 4 && !is_list)
        {
            printf("Error with: %s", cell);
            exit(1);
        }
        char substr[3];
        memcpy(substr, &cell[2], 2);
        substr[2] = '\0';
        sprintf(ret_str, "%d", (int8_t)strtol(substr, NULL, 16));
        static_var_size(4);
    }
    else
    {
        // integer case
        // xx
        ret_str = calloc(5, 1);
        if (strlen(cell) != 2 && !is_list)
        {
            printf("Error with: %s", cell);
            exit(1);
        }
        char substr[3];
        memcpy(substr, &cell[0], 2);
        substr[2] = '\0';
        // printf("Item: %s\n",substr);
        sprintf(ret_str, "%d", (int8_t)strtol(substr, NULL, 16));
        static_var_size(2);
    }
    return ret_str;
}