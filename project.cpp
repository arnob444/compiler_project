#include <iostream>
#include <vector>
#include <cctype>
#include <unordered_set>
#include <sstream>
using namespace std;

unordered_set<string> keywords = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if", "int",
    "long", "register", "return", "short", "signed", "sizeof", "static", "struct",
    "switch", "typedef", "union", "unsigned", "void", "volatile", "while"};

unordered_set<char> symbols = {'(', ')', '{', '}', '[', ']', ';', ',', '.'};
unordered_set<string> operators = {
    "+", "-", "*", "/", "%", "++", "--", "=", "+=", "-=", "*=", "/=", "%=",
    "==", "!=", ">", "<", ">=", "<=", "&&", "||", "!", "&", "|", "^", "~",
    "<<", ">>"};

struct Token
{
    string type;
    string value;
    int line;

    void print() const
    {
        cout << "Token(" << type << ", " << value << ", line=" << line << ")\n";
    }
};

bool isKeyword(const string &word)
{
    return keywords.find(word) != keywords.end();
}

bool isSymbol(char ch)
{
    return symbols.find(ch) != symbols.end();
}

bool isOperator(const string &op)
{
    return operators.find(op) != operators.end();
}

vector<Token> tokenize(const string &code, vector<string> &errors)
{
    vector<Token> tokens;
    string word;
    int n = code.length(), line = 1;
    int openBraces = 0, openParens = 0;

    for (int i = 0; i < n; i++)
    {
        char ch = code[i];

        if (ch == '\n')
        {
            line++;
            continue;
        }

        if (isspace(ch))
            continue;

        if (ch == '#')
        {
            word.clear();
            while (i < n && code[i] != '\n')
                word += code[i++];
            tokens.push_back({"PREPROCESSOR", word, line});
            line++;
            continue;
        }

        if (ch == '/' && i + 1 < n)
        {
            if (code[i + 1] == '/')
            {
                word.clear();
                while (i < n && code[i] != '\n')
                    word += code[i++];
                tokens.push_back({"COMMENT", word, line});
                line++;
                continue;
            }
            if (code[i + 1] == '*')
            {
                word.clear();
                // i += 2;
                while (i + 1 < n && !(code[i] == '*' && code[i + 1] == '/'))
                {
                    if (code[i] == '\n')
                        line++;
                    word += code[i++];
                }
                if (i + 1 >= n)
                {
                    errors.push_back("Error: Unclosed multi-line comment.");
                    continue;
                }
                word += "*/";
                i++;
                tokens.push_back({"MULTILINE COMMENT", word, line});
                continue;
            }
        }

        if (isalpha(ch) || ch == '_')   
        {
            word.clear();
            while (i < n && (isalnum(code[i]) || code[i] == '_'))
                word += code[i++];
            i--;

            if (isKeyword(word))
            {
                tokens.push_back({"KEYWORD", word, line});
            }
            else
            {
                tokens.push_back({"IDENTIFIER", word, line});
            }
        }

        else if (isdigit(ch))
        {
            word.clear();
            while (i < n && (isdigit(code[i]) || code[i] == '.'))
                word += code[i++];
            i--;
            tokens.push_back({"NUMBER", word, line});
        }

        else if (ch == '"')
        {
            word.clear();
            word += ch;
            i++;
            while (i < n && code[i] != '"')
                word += code[i++];
            if (i >= n)
            {
                errors.push_back("Error: Unclosed string literal on line " + to_string(line));
                continue;
            }
            word += '"';
            tokens.push_back({"STRING", word, line});
        }

        else if (ch == '\'')
        {
            word.clear();
            word += ch;
            i++;
            while (i < n && code[i] != '\'')
                word += code[i++];
            if (i >= n)
            {
                errors.push_back("Error: Unclosed character literal on line " + to_string(line));
                continue;
            }
            word += '\'';
            tokens.push_back({"CHARACTER", word, line});
        }

        else if (isSymbol(ch))
        {
            tokens.push_back({"PUNCTUATION", string(1, ch), line});
            if (ch == '{')
                openBraces++;
            if (ch == '}')
                openBraces--;
            if (ch == '(')
                openParens++;
            if (ch == ')')
                openParens--;
        }

        else
        {
            string op;
            op += ch;   // প্রথম অক্ষর সংগ্রহ করা (যেমন '+', '-', '=' ইত্যাদি)
            if (i + 1 < n)
            {
                op += code[i + 1]; // পরবর্তী অক্ষর আছে কি না চেক করা
                if (isOperator(op))  // দ্বিতীয় অক্ষর সংগ্রহ করা (যেমন '==', '>=', '<=' ইত্যাদি)
                {
                    tokens.push_back({"OPERATOR", op, line});
                    i++;
                    continue;
                }
            }
            // যদি এটি একক অক্ষরের অপারেটর হয় (যেমন '+', '-', '=', '!')
            // ch কে একটি string-এ রূপান্তর করা হয়।
            // উদাহরণ: যদি ch = '+' হয়, তাহলে string(1, ch) হবে "+"।
            if (isOperator(string(1, ch)))
            {
                tokens.push_back({"OPERATOR", string(1, ch), line});
            }
        }
    }

    if (openBraces > 0)
        errors.push_back("Error: Unmatched '{'.");
    if (openBraces < 0)
        errors.push_back("Error: Unmatched '}'.");
    if (openParens > 0)
        errors.push_back("Error: Unmatched '('.");
    if (openParens < 0)
        errors.push_back("Error: Unmatched ')'.");

    return tokens;
}

vector<string> parseFunctions(const vector<Token> &tokens)
{
    vector<string> functions;
    for (int i = 0; i < tokens.size(); i++)
    {
        if (tokens[i].type == "KEYWORD" && (tokens[i].value == "int" || tokens[i].value == "void"))
        {
            if (i + 1 < tokens.size() && tokens[i + 1].type == "IDENTIFIER")
            {
                if (i + 2 < tokens.size() && tokens[i + 2].type == "PUNCTUATION" && tokens[i + 2].value == "(")
                {
                    functions.push_back(tokens[i].value + " " + tokens[i + 1].value + "()");
                }
            }
        }
    }
    return functions;
}

int main()
{
    string code, line;

    cout << "C Lexical Analyzer\n=================\n";
    cout << "Enter your C code (press Ctrl+Z then Enter to finish):\n";

    while (getline(cin, line))
    {
        code += line + "\n";
    }

    vector<string> errors;
    vector<Token> tokens = tokenize(code, errors);

    cout << "\nTokens:\n-------\n";
    for (const auto &token : tokens)
    {
        token.print();
    }

    for (const auto &error : errors)
    {
        cout << error << endl;
    }

    vector<string> functions = parseFunctions(tokens);
    cout << "\nParsed Functions:\n";
    for (const auto &func : functions)
    {
        cout << func << endl;
    }

    return 0;
}