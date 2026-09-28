#include "common.h"
#include <stdio.h>
#include <string.h>

void help_menu(char *argv[]){
    char *program_name = strrchr(argv[0], '/')?strrchr(argv[0], '/') : strrchr(argv[0], '\\');
    if (program_name != NULL) {
        program_name++;
    } else {
        program_name = argv[0];
    }
    printf("\nUsage:\n\t./%s -e <source_image.bmp> <secret_file> [<output_stego_image.bmp>]\n", program_name);
    printf("        ./%s -d <source_image.bmp> [<output_secret_file>]\n\n", program_name);
    printf("Options:\n");
    printf("  -e <source_image.bmp> <secret_file> [<output_stego_image.bmp>]\tSpecify the source BMP image file and the secret file to encode.\n");
    printf("\t\t\t\t\t\t\t\t\tOptional. Specify the name of the output stego image. If not provided, defaults to 'stego_image.bmp'.\n\n");
    
    printf("  -d <source_image.bmp>  [<output_secret_file>]\t\t\t\tSpecify the source BMP image file to decode.\n");
    printf("\t\t\t\t\t\t\t\t\tOptional. Specify the name of the output secret file. If not provided, defaults to 'output_secret_file'.\n");
    printf("\nExample:\n");
    printf("  ./%s -e source_image.bmp secret.txt stego_image.bmp\n", program_name);
    printf("  ./%s -d stego_image.bmp secret_output.txt\n\n", program_name);
}

OperationType check_operation_type(char c){
    if (c == 'e')
        return e_encode;
    else if (c == 'd')
        return e_decode;
    else
        return e_unsupported;
}