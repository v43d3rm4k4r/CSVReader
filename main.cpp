#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>

#define DEFAULT_CSV_FILE "../Examples/example2.txt"

using std::vector;
using std::string;
using std::cout;
using std::endl;

uint16_t columns = 1;
uint16_t rows = 0;

const char ops[] = "+-*/";

string calculate(const string& str, const vector<vector<string>>& Vector);

int main(int argc, char* argv[])
{
    if (argc < 2)
        strcpy(argv[1], DEFAULT_CSV_FILE);

    FILE* file = fopen(argv[1], "r");
    if (!file)
    {
        fprintf(stderr, "Cannot open the file \"%s\".\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("File \"%s\" successfully opened.\n", argv[1]);
    }

    // find out the size of the desired vector
    bool first_row_ended = false;
    uint16_t global_comma_count = 0;
    char ch = 0;
    while ((ch = getc(file)) != EOF)
    {
        if (ch == '\n')
            first_row_ended = true;

        if (ch == ',' || ch == ';')
        {
            ++global_comma_count;

            if(!first_row_ended)
                ++columns;
        }
    }
    rows = (global_comma_count+columns) / columns;

    // initializing the desired vector
    vector<vector<string>> field(rows);
    for (uint16_t i = 0; i < rows; ++i)
    {
        field[i].resize(columns);
    }

    rewind(file);

    // filter input
    uint16_t i = 0; // rows
    uint16_t j = 0; // columns
    while ((ch = getc(file)) != EOF)
    {
        if (ch == ' ')
            continue;
        if (ch != ',' && ch != '\n' && ch != ';')
            field[i][j] += ch;
        if (ch == ',' || ch == ';')
            ++j;
        if (ch == '\n')
        {
            ++i;
            j = 0;
        }
    }

    // if some cell has '=', but there are no valid operators, exit the program
    // and show invalid cell
    for (uint16_t i = 0; i < rows; ++i)
    {
        for (uint16_t j = 0; j < columns; ++j)
        {
            if (field[i][j][0] == '=' && !strpbrk(field[i][j].c_str(), ops))
            {
                fprintf(stderr, "\nError, no valid operator found in (row=%d,col=%d) cell.\n", i, j);
                system("pause");
                exit(EXIT_FAILURE);
            }
        }
    }

    cout << endl << "INPUT:" << endl << endl;
    for (uint16_t i = 0; i < rows; ++i)
    {
        for (uint16_t j = 0; j < columns; ++j)
        {
            if (j != columns-1)
                cout << field[i][j] << ",";
            else
                cout << field[i][j];
        }
        cout << endl;
    }

    // calculate the cells if needed
    for (uint16_t i = 0; i < rows; ++i)
    {
        for (uint16_t j = 0; j < columns; ++j)
        {
            if (field[i][j][0] == '=')
            {
                field[i][j] = calculate(field[i][j], field);
            }
        }
    }

    cout << endl;
    cout << "OUTPUT:" << endl << endl;
    for (uint16_t i = 0; i < rows; ++i)
    {
        for (uint16_t j = 0; j < columns; ++j)
        {
            if (j != columns-1)
                cout << field[i][j] << ",";
            else
                cout << field[i][j];
        }
        cout << endl;
    }

    fclose(file);
    return 0;
}
//==============================================================================
string calculate(const string& str, const vector<vector<string>>& field)
{
    char* op = nullptr;

    char cStr[str.size()+1];

    memcpy(cStr, str.c_str(), sizeof(cStr));

    // find operator
    op = strpbrk(cStr, ops);

    // divide the remaining line by ARG1 and ARG2
    uint16_t i = 1; // to ignore '='
    string ARG1, ARG2;
    while (str[i] != *op)
    {
        ARG1 += str[i];
        ++i;
    }

    i = ARG1.size()+2;

    while (str[i])
    {
       ARG2 += str[i];
       ++i;
    }

    // find the parts of the arguments
    string ARG1_row_name; // 1
    string ARG1_col_name; // A

    string ARG2_row_name; // 30
    string ARG2_col_name; // Cell

    for (uint16_t i = 0; i < ARG1.size(); ++i)
    {
        // until we come across numbers, we add
        if (!isdigit(ARG1[i]))
            ARG1_col_name += ARG1[i];
        // write the rest in row
        else
        {
            ARG1_row_name += ARG1[i];
        }
    }
    for (uint16_t i = 0; i < ARG2.size(); ++i)
    {
        // the rest until we come across numbers, we add
        if (!isdigit(ARG2[i]))
            ARG2_col_name += ARG2[i];
        // write the rest in row
        else
        {
            ARG2_row_name += ARG2[i];
        }
    }

    // find cells by indexes
    uint16_t ARG1_row_index = 0;
    uint16_t ARG1_col_index = 0;

    uint16_t ARG2_row_index = 0;
    uint16_t ARG2_col_index = 0;

    for (uint16_t i = 1; i < rows; ++i)
    {
        if (ARG1_row_name == field[i][0])
        {
            ARG1_row_index = i;
            break;
        }
    }
    for (uint16_t j = 0; j < columns; ++j)
    {
        if (ARG1_col_name == field[0][j])
        {
            ARG1_col_index = j;
            break;
        }
    }
    for (uint16_t i = 1; i < rows; ++i)
    {
        if (ARG2_row_name == field[i][0])
        {
            ARG2_row_index = i;
            break;
        }
    }
    for (uint16_t j = 0; j < columns; ++j)
    {
        if (ARG2_col_name == field[0][j])
        {
            ARG2_col_index = j;
            break;
        }
    }

    // finally compute the value
    int result = 0;
    switch (*op)
    {
    case '+':
        result = atoi(field[ARG1_row_index][ARG1_col_index].c_str()) +
                 atoi(field[ARG2_row_index][ARG2_col_index].c_str());
        break;
    case '-':
        result = atoi(field[ARG1_row_index][ARG1_col_index].c_str()) -
                 atoi(field[ARG2_row_index][ARG2_col_index].c_str());
        break;
    case '*':
        result = atoi(field[ARG1_row_index][ARG1_col_index].c_str()) *
                 atoi(field[ARG2_row_index][ARG2_col_index].c_str());
        break;
    case '/':
        result = atoi(field[ARG1_row_index][ARG1_col_index].c_str()) /
                 atoi(field[ARG2_row_index][ARG2_col_index].c_str());
        break;
    default:
        break;
    }

    return string(std::to_string(result));
}
