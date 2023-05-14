/*
 * HTF Generator -- Command Line Interface (software)
 * Copyright (C) 2023 - Rin "madeshiro" Baudelet
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this
 * program. If not, see <https://www.gnu.org/licenses/>.
 */
#include <iostream>
#include "code3c/3ccode.hh"

#define HTFGEN_CLI 202305100L
#define HTFGEN_CLI_VERSION "2023/05 v1.0.0-snapshot"
#define HTFGEN_CLI_MAJOR 1
#define HTFGEN_CLI_MINOR 0
#define HTFGEN_CLI_PATCH 0

using std::cout, std::endl;
using namespace code3c;

char inhtf[256];

// Input
char* inputhtf = nullptr;

char* inputbuf  = nullptr;
size_t inputlen = 0;

typedef int(*parsing)(char**argv, int avail, char*);
typedef bool(*checking)(const char*, const char*);
typedef bool(*action)();

int parse_null(char**, int avail, char*)
{
    return avail >= 1;
}

int parse_equal(char** argv, int avail, char* out)
{
    if (avail < 1)
        return 0;

    // printf("parse_equal '%s'\n", argv[0]);

    return sscanf(argv[0], "%*[^=]=%[^\n]", out) == 1;
}

int parse_composed(char** argv, int avail, char* out)
{
    if (avail < 2)
        return 0;

    // printf("parse_composed '%s %s'\n", argv[0], argv[1]);

    strcpy(out, argv[1]);
    return 2;
}

bool check_equal(const char* str, const char* id)
{
    size_t idlen = strlen(id);
    return strncmp(str, id, idlen) == 0 && str[idlen] == '=';
}

bool check_composed(const char* str, const char* id)
{
    return strcmp(str, id) == 0;
}

struct support_argument {
    const char* identifier[2];
    const char* args, *help;
    char* container;

    parsing parse;
    checking check;
    action exec = nullptr;

    void print_help()
    {
        char prefix[256];
        sprintf(prefix, "%s,%s%s", identifier[0], identifier[1], args);
        printf("%-40s ", prefix);

        size_t helplen = strlen(help);

        for (int i(0); i < helplen;)
        {
            printf("%.40s", &help[i]);
            i += 40;
            for (; i < helplen && help[i] != ' '; i++)
            {
                printf("%c", help[i]);
            }
            i++;

            if (i < helplen)
                printf("\n%*s ", 40, "");
            else printf("\n");
        }
    }
} registeredArguments[2] = {
        {
#define HTFGEN_CLI_ARG_HELP 0
                {"-h", "--help"},
                "",
                "Display this information",
                nullptr,
                parse_null,
                check_composed,
                []() -> bool
                {
                    printf("Usage: code3c [options]\n");
                    printf("Options:\n");
                    for (auto arg : registeredArguments) {
                        arg.print_help();
                    }
                    printf("\nCheck project repository at https://github"
                           ".com/madeshiro/3c-code or\nhttps://gitlab.isima"
                           ".fr/rinbaudelet/uca-l3_graphicalprot\n"
                           "\nFor bug reporting instructions, please contact:\n"
                           "<mailto:rin.baudelet@etu.uca.fr>.\n");
                    printf("-----\n");
                    printf("htfgen CLI " HTFGEN_CLI_VERSION "\n");
                    printf("3C-Code Library " CODE3C_LIB_VERSION "\n");
                    exit(EXIT_SUCCESS);
                }
        },
        {
#define HTFGEN_CLI_ARG_INHTF
                {"-t", "--table"},
                " <file>",
                "Read a .htf file and display the result",
                inhtf,
                parse_composed,
                check_composed,
                []() -> bool
                {
                    size_t inhtflen = strlen(inhtf);
                    if (inhtflen > 0)
                    {
                        inputhtf = strcpy(new char[inhtflen+1], inhtf);
                        return true;
                    }

                    return false;
                }
        }
};


bool parse_args(int argc, char** argv)
{
    for (int i(1); i < argc;)
    {
        int result;
        for (auto &argument : registeredArguments)
        {
            parsing pMethod = nullptr;
            for (auto id : argument.identifier)
            {
                if (argument.check(argv[i], id))
                {
                    pMethod = argument.parse;
                    break;
                }
            }

            if (pMethod)
            {
                if (!(result=pMethod(&argv[i],argc-i,argument.container)))
                    return false;
                if (argument.exec && !argument.exec())
                    return false;
                break;
            }
        }
        i+=result;
    }

    return true;
}

int main(int argc, char** argv)
{
    // Print help if no argument given
    if (argc <= 1)
    {
        registeredArguments[0].exec();
        return EXIT_SUCCESS;
    }
    else if (!parse_args(argc, argv))
        return EXIT_FAILURE;

    if (inputhtf)
    {
        HuffmanTable* table = HTFile::fromFile(inputhtf);
        if (table)
        {
            std::cout << *table << std::endl;
        } else printf("Unable to find %s\n", inputhtf);
    }

    return EXIT_SUCCESS;
}