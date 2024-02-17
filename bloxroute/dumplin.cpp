#include <stdio.h>


// dump a hexadecimal file in one line, so it is easier to look for patterns in vim.

int main(int argc, char *argv[])
{
    if (argc < 1) {
        fprintf(stderr, "no input file\n");
    }
    FILE *fp = fopen(argv[1],"r");
    int ch;

    if (fp != nullptr) {
        while ( (ch = fgetc(fp)) != -1)  {
            fprintf(stdout,"%02x", ch );
        }
    }
    return 0;
}
