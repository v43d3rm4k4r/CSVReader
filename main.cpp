#include <iostream>
#include <vector>
#include <string>
#include <cstdlib> // for atoi() and itoa()
#include <cstring>

#define DEFAULT_CSV_FILE "file2.csv.txt"

//#define DEBUG

using std::vector;
using std::string;
using std::cout;
using std::endl;

size_t columns = 1;
size_t rows = 0;
char ops[] = "+-*/";

string Calculate(const string& str, const vector<vector<string>>& Vector);

int main(int argc, char* argv[])
{
    if (argc != 2)
        argv[1] = (char*)DEFAULT_CSV_FILE;

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
    size_t global_comma_count = 0;
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
    vector<vector<string>> Vector(rows);
    for (size_t i = 0; i < rows; ++i)
    {
        Vector[i].resize(columns);
    }

#ifdef DEBUG
    printf("columns = %d\n", columns);
    printf("rows = %d\n", rows);
    cout << endl;
#endif

    rewind(file);

    // filter input
    size_t i = 0; // rows
    size_t j = 0; // columns
    while ((ch = getc(file)) != EOF)
    {
        if (ch == ' ')
            continue;
        if (ch != ',' && ch != '\n' && ch != ';')
            Vector[i][j] += ch;
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
    for (size_t i = 0; i < rows; ++i)
    {
        for (size_t j = 0; j < columns; ++j)
        {
            if (Vector[i][j][0] == '=' && !strpbrk(Vector[i][j].c_str(), ops))
            {
                fprintf(stderr, "\nError, no valid operator found in (row=%d,col=%d) cell.\n", i, j);
                system("pause");
                exit(EXIT_FAILURE);
            }
        }
    }

    cout << endl << "INPUT:" << endl << endl;
    for (size_t i = 0; i < rows; ++i)
    {
        for (size_t j = 0; j < columns; ++j)
        {
            if (j != columns-1)
                cout << Vector[i][j] << ",";
            else
                cout << Vector[i][j];
        }
        cout << endl;
    }

    // calculate the cells if needed
    for (size_t i = 0; i < rows; ++i)
    {
        for (size_t j = 0; j < columns; ++j)
        {
            if (Vector[i][j][0] == '=')
            {
                Vector[i][j] = Calculate(Vector[i][j], Vector);
            }
        }
    }

    cout << endl;
    cout << "OUTPUT:" << endl << endl;
    for (size_t i = 0; i < rows; ++i)
    {
        for (size_t j = 0; j < columns; ++j)
        {
            if (j != columns-1)
                cout << Vector[i][j] << ",";
            else
                cout << Vector[i][j];
        }
        cout << endl;
    }

    fclose(file);
    return 0;
}
//==============================================================================
string Calculate(const string& str, const vector<vector<string>>& Vector)
{
    char* op = nullptr;

    char cStr[str.size()+1];

    memcpy(cStr, str.c_str(), sizeof(cStr));

    // find operator
    op = strpbrk(cStr, ops);

    // divide the remaining line by ARG1 and ARG2
    size_t i = 1; // to ignore '='
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

    for (size_t i = 0; i < ARG1.size(); ++i)
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
    for (size_t i = 0; i < ARG2.size(); ++i)
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
    size_t ARG1_row_index = 0;
    size_t ARG1_col_index = 0;

    size_t ARG2_row_index = 0;
    size_t ARG2_col_index = 0;

    for (size_t i = 1; i < rows; ++i)
    {
        if (ARG1_row_name == Vector[i][0])
        {
            ARG1_row_index = i;
            break;
        }
    }
    for (size_t j = 0; j < columns; ++j)
    {
        if (ARG1_col_name == Vector[0][j])
        {
            ARG1_col_index = j;
            break;
        }
    }
    for (size_t i = 1; i < rows; ++i)
    {
        if (ARG2_row_name == Vector[i][0])
        {
            ARG2_row_index = i;
            break;
        }
    }
    for (size_t j = 0; j < columns; ++j)
    {
        if (ARG2_col_name == Vector[0][j])
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
        result = atoi(Vector[ARG1_row_index][ARG1_col_index].c_str()) +
                 atoi(Vector[ARG2_row_index][ARG2_col_index].c_str());
        break;
    case '-':
        result = atoi(Vector[ARG1_row_index][ARG1_col_index].c_str()) -
                 atoi(Vector[ARG2_row_index][ARG2_col_index].c_str());
        break;
    case '*':
        result = atoi(Vector[ARG1_row_index][ARG1_col_index].c_str()) *
                 atoi(Vector[ARG2_row_index][ARG2_col_index].c_str());
        break;
    case '/':
        result = atoi(Vector[ARG1_row_index][ARG1_col_index].c_str()) /
                 atoi(Vector[ARG2_row_index][ARG2_col_index].c_str());
        break;
    default:
        break;
    }

#ifdef DEBUG
    cout << endl << endl;
    cout << "input str = " <<str << endl << endl;
    cout << "result = "<< result << endl;
    cout << endl;
    cout << "ARG1_col_name =" << ARG1_col_name << endl;
    cout << "ARG1_row_name =" << ARG1_row_name << endl;
    cout << "ARG2_col_name =" << ARG2_col_name << endl;
    cout << "ARG2_row_name =" << ARG2_row_name << endl;
    cout << endl;
    cout << "str = " << ARG1 + *op + ARG2 << endl;
    static int call_count = 1;
    cout << endl;
    cout << call_count <<" Calculate call" << std::endl;
    ++call_count;
    cout << "operator "<< *op << endl << endl;
#endif

#ifdef DEBUG
    cout << "strResult = " << strResult << endl;
#endif

    return string(std::to_string(result));
}
