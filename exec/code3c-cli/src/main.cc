/*
 * 3C-CODE CLI -- Command Line Interface (software)
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

#define CODE3C_CLI 202305100L
#define CODE3C_CLI_VERSION "2023/05 v1.0.0-snapshot-22w14a"
#define CODE3C_CLI_MAJOR 1
#define CODE3C_CLI_MINOR 0
#define CODE3C_CLI_PATCH 0

using std::cout, std::endl;
using namespace code3c;

char infile[256],
    intext[1024],
    outfile[256],
    inlogo[256];
char model[20],
    errmodel[20],
    huffmodel[20];

char * inbuf = nullptr;
size_t inlen = 0;

typedef int(*parsing)(char**argv, int avail, char*);
typedef bool(*checking)(const char*, const char*);
typedef bool(*action)();

struct {
    uint8_t model     = CODE3C_MODEL_WB2C;
    uint8_t errmodel  = CODE3C_ERRLVL_A;
    uint8_t huffmodel = CODE3C_HUFFMAN_NO;

    char * logo = nullptr;
    char* outfile = nullptr;
} code3c_args = {};

int parse_null(char**, int avail, char*)
{
    return avail >= 1;
}

int parse_equal(char** argv, int avail, char* out)
{
    if (avail < 1)
        return 0;

    return sscanf(argv[0], "%*[^=]=%[^\n]", out) == 1;
}

int parse_composed(char** argv, int avail, char* out)
{
    if (avail < 2)
        return 0;

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
} registeredArguments[8] = {
        {
#define CODE3C_CLI_ARG_HELP 0
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
                    printf("3C-Code CLI " CODE3C_CLI_VERSION "\n");
                    printf("3C-Code Library " CODE3C_LIB_VERSION "\n");
                    exit(EXIT_SUCCESS);
                }
        },
        {
#define CODE3C_CLI_ARG_OUTPUT 1
                {"-o", "--output"},
                " <file.png>",
                "Specify the output file. If null, no file will be saved or default"
                " name will be \"code3c.png\"",
                outfile,
                parse_composed,
                check_composed,
                []() -> bool
                {
                    if (strlen(outfile) > 1)
                    {
                        code3c_args.outfile = strcpy(new char[strlen(outfile+1)],
                                                     outfile);
                    }
                    return true;
                }
        },
        {
#define CODE3C_CLI_ARG_HUFFMODEL 2
                {"-hf", "--huffman"},
                "=<{ASCII, LATIN1}",
                "Set up the Huffman compressing method. Per default, no compression "
                "method is set",
                huffmodel,
                parse_equal,
                check_equal,
                []() -> bool
                {
                    if (strcmp(huffmodel, "NO") == 0)
                        code3c_args.huffmodel = CODE3C_HUFFMAN_NO;
                    else if (strcmp(huffmodel, "ASCII") == 0)
                        code3c_args.huffmodel = CODE3C_HUFFMAN_ASCII;
                    else if (strcmp(huffmodel, "LATIN1") == 0)
                        code3c_args.huffmodel = CODE3C_HUFFMAN_LATIN;
                    else
                    {
                        printf("Invalid argument. Expected 'NO', 'ASCII' or 'LATIN1'\n");
                        return false;
                    }

                    return true;
                }
        },
        {
#define CODE3C_CLI_ARG_INFILE 3
                {"-f", "--file"},
                " <file>",
                "Specify an input file to generate 3C-Code",
                infile,
                parse_composed,
                check_composed,
                []() -> bool
                {
                    FILE* file = fopen(infile, "rb");
                    if (file)
                    {
                        fseek(file, 0, SEEK_END);
                        inlen = (size_t) ftell(file);
                        fseek(file, 0, SEEK_SET);
                        if (inlen < 0) {
                            printf("File empty, cannot generate 3C-Code\n");
                            return false;
                        }

                        inbuf = new char[inlen+1];
                        inbuf[inlen] = '\0';
                        fread(inbuf, sizeof(char), inlen, file);

                        return true;
                    }
                    else
                    {
                        printf("Unable to find input file \"%s\"\n", infile);
                        return false;
                    }
                }
        },
        {
#define CODE3C_CLI_ARG_MODEL 4
                {"-m", "--model"},
                "=<{WB, WB2C, WB6C}",
                "Specify the 3C-Code model. Per default, WB2C is set",
                model,
                parse_equal,
                check_equal,
                []() -> bool
                {
                    if (strcmp(model, "WB") == 0)
                        code3c_args.model = CODE3C_MODEL_WB;
                    else if (strcmp(model, "WB2C") == 0)
                        code3c_args.model = CODE3C_MODEL_WB2C;
                    else if (strcmp(model, "WB6C") == 0)
                        code3c_args.model = CODE3C_MODEL_WB6C;
                    else
                    {
                        printf("Invalid argument. Expected 'WB', 'WB2C' or 'WB6C'\n");
                        return false;
                    }

                    return true;
                }
        },
        {
#define CODE3C_CLI_ARG_ERRMODEL 5
                {"-e", "--err"},
                "=<{Hamming743, Hamming313}>",
                "Specify the error model. Per default, Hamming743 is set "
                "(14% error coverage)",
                errmodel,
                parse_equal,
                check_equal,
                []() -> bool
                {
                    if (strcmp(errmodel, "Hamming743") == 0)
                        code3c_args.errmodel = CODE3C_ERRLVL_A;
                    else if (strcmp(errmodel, "Hamming313") == 0)
                        code3c_args.errmodel = CODE3C_ERRLVL_B;
                    else
                    {
                        printf("Invalid argument. Expected 'Hamming743' or "
                               "'Hamming313'\n");
                        return false;
                    }

                    return true;
                }
        },
        {
#define CODE3C_CLI_ARG_INTEXT 6
                {"-t", "--text"},
                " \"input text\"",
                "Specify an input text to generate 3C-Code",
                intext,
                parse_composed,
                check_composed,
                []() -> bool
                {
                    inlen = strlen(intext);
                    if (inlen > 0)
                    {
                        inbuf = strcpy(new char[inlen+1], intext);
                        return inbuf != nullptr;
                    }

                    printf("Input text cannot be empty\n");
                    return false;
                }
        },
        {
#define CODE3C_CLI_ARG_LOGO 7
                {"", "--logo"},
                " <file>",
                "Specify the logo to put in the 3C-Code",
                inlogo,
                parse_composed,
                check_composed,
                []() -> bool
                {
                    size_t inlogolen = strlen(inlogo);
                    if (inlogolen > 0)
                    {
                        code3c_args.logo = strcpy(new char[inlogolen+1],
                                                  inlogo);
                    }
                    return true;
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

void setup_userprompt()
{
    auto input = [](char* rvar, int arg) {
        do
        {
            scanf("%s", rvar);
        }
        while (!registeredArguments[arg].exec());
    };

    printf("=== code3c generator ===\n");

    // Ask model
    printf("-- setup model\n(WB, WB2C, WB6C)? ");
    input(model, CODE3C_CLI_ARG_MODEL);

    // Ask error model
    printf("-- setup err model\n(Hamming743, Hamming313)? ");
    input(errmodel, CODE3C_CLI_ARG_ERRMODEL);

    // Ask huffman model
    printf("-- setup compression algorithm\n (NO, ASCII, LATIN1)? ");
    input(huffmodel, CODE3C_CLI_ARG_HUFFMODEL);

    // Ask input text
    printf("-- input text\n(up to 1024B)? ");
    scanf(" %[^\n]", intext);
    registeredArguments[CODE3C_CLI_ARG_INTEXT].exec();
}

int main(int argc, char** argv)
{
    // Full user prompt
    if (argc <= 1)
    {
        setup_userprompt();
    }
    else if (!parse_args(argc, argv))
        return EXIT_FAILURE;

    // Set-up using CLI
    Code3C code3C(inbuf, inlen);
    code3C.setModel(code3c_args.model);
    code3C.setErrorModel(code3c_args.errmodel);
    code3C.setHuffmanTable(code3c_args.huffmodel);
    if (code3c_args.logo)
        code3C.setLogo(code3c_args.logo);
    if (code3c_args.outfile)
        code3C.set_output(code3c_args.outfile);

    if (!code3C.generate())
        return EXIT_FAILURE;

    printf("=== code3c specs ===\n"
           "-- dataSegSize: %zu\n"
           "-- errSegSize: %zu\n"
           "-- dimension: %d x %d\n"
           "-- capacity: %dB\n"
           "-- bitl: %d bit\n",
           code3C.dataSegSize(),
           code3C.errSegSize(),
           code3C.dimension().axis_t, code3C.dimension().axis_r,
           code3C.dimension().capacity * code3C.model().bitl / 8,
           code3C.model().bitl
           );

    // Display result
    code3C.display();

    // Free memory
    delete[] inbuf;
    delete[] code3c_args.logo;

    return EXIT_SUCCESS;
}