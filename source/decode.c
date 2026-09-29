#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include <unistd.h>

void decoding_help_menu(char *argv[]){
    char *program_name = strrchr(argv[0], '/')?strrchr(argv[0], '/') : strrchr(argv[0], '\\');
    if (program_name != NULL) {
        program_name++;
    } else {
        program_name = argv[0];
    }
    printf("\nUsage:\n\t./%s -d <source_image.bmp> [<output_secret_file>]\n\n", program_name);
    printf("Options:\n");
    printf("\t-d <source_image.bmp> [<output_secret_file>]\t\tSpecify the source BMP image file to decode.\n");
    printf("\t\t\t\t\t\t\t\tOptional. Specify the name of the output secret file. If not provided, defaults to 'output_secret_file'.\n\n");
    printf("Example:\n");
    printf("\t./%s -d stego_image.bmp secret_output.txt\n", program_name);
}

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo){
    // step 1: validate arguments
    decInfo->src_image_fname = argv[2];
    if(decInfo->src_image_fname == NULL)
        return e_failure;
    if(strlen(decInfo->src_image_fname) < 5 || strcmp(&decInfo->src_image_fname[strlen(decInfo->src_image_fname)-4], ".bmp") != 0){
        fprintf(stderr, "ERROR: Source image must be a BMP file\n");
        return e_failure;
    }

    if(argv[3] == NULL){
        printf("INFO: Secret File Name not provided\n");
        decInfo->secret_fname = "output_secret_file";
    }
    else{
        // remove the file extension from the secret file name if provided
        char *secret_fname = argv[3];
        char *dot = strrchr(secret_fname, '.');
        if (dot != NULL) {
            *dot = '\0';
            decInfo->secret_fname = secret_fname;
        }
        else{   
            decInfo->secret_fname = secret_fname;
        }
    }

    if(decInfo->src_image_fname == NULL)
        return e_failure;
    
    printf("INFO: Source Image File Name: %s\n", decInfo->src_image_fname);

    // step 2: validate files exists
    if(open_decode_files(decInfo) == e_failure){  
        return e_failure;
    }

    return e_success;
}

Status open_decode_files(DecodeInfo *decInfo){
    decInfo->fptr_src_image = fopen(decInfo->src_image_fname, "rb");
    if (decInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->src_image_fname);

    	return e_failure;
    }
    return e_success;
}

Status do_decoding(DecodeInfo *decInfo){

    if(fgetc(decInfo->fptr_src_image) != 'B' || fgetc(decInfo->fptr_src_image) != 'M'){
        fprintf(stderr, "ERROR: Source image is not a valid BMP file\n");
        return e_failure;
    }
    fseek(decInfo->fptr_src_image, 0, SEEK_SET);
    // step 1: open files
    printf("INFO: Files opened successfully\n");
      
    // step 2: skip bmp header
    if(skip_bmp_header(decInfo->fptr_src_image) == e_failure){
        fprintf(stderr, "ERROR: Skipping BMP header failed\n");
        return e_failure;
    }
      
    printf("INFO: BMP header skipped successfully\n");

    // step 3: decode magic string
    if(decode_magic_string(MAGIC_STRING, decInfo) == e_failure){
        fprintf(stderr, "ERROR: Decoding magic string failed\n");
        return e_failure;
    }
    if(strcmp(decInfo->magic_string, MAGIC_STRING) != 0){
        printf("%s,%s\n", decInfo->magic_string, MAGIC_STRING);
        fprintf(stderr, "ERROR: Magic string does not match, not a valid stego image\n");
        return e_failure;
    }
      
    printf("INFO: Magic string decoded successfully\n");
    printf("INFO: Magic string matches, valid stego image\n");

    if(decode_secret_file_extn_size(decInfo) == e_failure){
        fprintf(stderr, "ERROR: Decoding secret file extension size failed\n");
        return e_failure;
    }
      
    printf("INFO: Secret file extension size decoded successfully\n");

    if(decode_secret_file_extn(decInfo->extn_secret_file, decInfo) == e_failure){
        fprintf(stderr, "ERROR: Decoding secret file extension failed\n");
        return e_failure;
    }
      
    printf("INFO: Secret file extension decoded successfully\n");

    if(create_secret_file(decInfo) == e_failure){
        fprintf(stderr, "ERROR: Creating secret file failed\n");
        return e_failure;
    }
      
    printf("INFO: Secret file created successfully\n");

    if(decode_secret_file_size(decInfo) == e_failure){
        fprintf(stderr, "ERROR: Decoding secret file extension failed\n");
        return e_failure;
    }
      
    printf("INFO: Secret file extension decoded successfully\n");

    if(decode_secret_file_data(decInfo) == e_failure){
        fprintf(stderr, "ERROR: Decoding secret file data failed\n");
        return e_failure;
    }
      
    printf("INFO: Secret file data decoded successfully\n");


    return e_success;

}

Status skip_bmp_header(FILE *fptr_src_image){
    // step 1: skip bmp header
    fseek(fptr_src_image, 54, SEEK_SET);
    return e_success;
}

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo){
    // step 1: decode the magic string from the image
    char decoded_magic_string[sizeof(MAGIC_STRING)*8];
    if (decode_data_from_image(decoded_magic_string, strlen(MAGIC_STRING), decInfo->fptr_src_image) == e_failure){
        fprintf(stderr, "ERROR: Decoding magic string failed\n");
        return e_failure;
    }
    strcpy(decInfo->magic_string, decoded_magic_string);
    //printf("INFO: Magic string decoded successfully\n");
    return e_success;
}

Status decode_data_from_image(const char *data, int size, FILE *fptr_src_image){
    // step 1: read the image data
    char image_buffer[size * 8];
    fread(image_buffer, sizeof(char), size * 8, fptr_src_image);

    // step 2: decode the data from the image
    for (int i = 0; i < size; i++){
        char decoded_byte = 0;
        for (int j = 0; j < 8; j++){
            decoded_byte = decoded_byte << 1;
            if (image_buffer[i * 8 + j] & 1){
                decoded_byte |= 1;
            }
        }
        ((char *)data)[i] = decoded_byte;
    }

    return e_success;
}

Status decode_byte_from_lsb(char data, char *image_buffer){
    // step 1: extract the least significant bit from the image buffer
    char lsb = image_buffer[0] & 1;

    // step 2: set the least significant bit of the data
    data = (data & ~1) | lsb;


    return e_success;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo){
    // step 1: decode the secret file extension size from the image
    if (decode_int_data_from_image((char *)&decInfo->extn_size, sizeof(int), decInfo->fptr_src_image) == e_failure){
        fprintf(stderr, "ERROR: Decoding secret file extension size failed\n");
        return e_failure;
    }

    return e_success;
}

Status decode_int_data_from_image(char *data, int size, FILE *fptr_src_image) {
    // step 1: read the image data (size*8 bytes)
    char image_buffer[size * 8];
    size_t bytes_read = fread(image_buffer, sizeof(char), size * 8, fptr_src_image);
    if (bytes_read < size * 8) {
        fprintf(stderr, "ERROR: Not enough image data to decode integer\n");
        return e_failure;
    }

    // step 2: decode each byte from 8 image bytes
    for (int i = 0; i < size; i++) {
        unsigned char decoded_byte = 0;
        for (int j = 0; j < 8; j++) {
            decoded_byte <<= 1;
            decoded_byte |= (image_buffer[i * 8 + j] & 0x01);
        }
        ((unsigned char *)data)[i] = decoded_byte;
    }
    return e_success;
}

Status decode_secret_file_extn(const char *file_extn, DecodeInfo *decInfo){
    // step 1: decode the secret file extension from the image
    if (decode_data_from_image(file_extn, decInfo->extn_size, decInfo->fptr_src_image) == e_failure){
        fprintf(stderr, "ERROR: Decoding secret file extension failed\n");
        return e_failure;
    }
    strcpy(decInfo->extn_secret_file, file_extn);
    decInfo->extn_secret_file[decInfo->extn_size] = '\0'; // Null-terminate the string

    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo){
    // step 1: decode the secret file size from the image
    if (decode_int_data_from_image((char *)&decInfo->size_secret_file, sizeof(long), decInfo->fptr_src_image) == e_failure){
        fprintf(stderr, "ERROR: Decoding secret file size failed\n");
        return e_failure;
    }


    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo){
    // step 1: decode the secret file data from the image}
    for (long i = 0; i < decInfo->size_secret_file; i++){
        char decoded_byte = 0;
        if (decode_data_from_image(&decoded_byte, 1, decInfo->fptr_src_image) == e_failure){
            fprintf(stderr, "ERROR: Decoding secret file data failed\n");
            return e_failure;
        }
        fwrite(&decoded_byte, sizeof(char), 1, decInfo->fptr_secret);
    }

    return e_success;
}

Status create_secret_file(DecodeInfo *decInfo){
    char secret_file_name[100];
    strcpy(secret_file_name, decInfo->secret_fname);
    strcat(secret_file_name, decInfo->extn_secret_file);
    decInfo->fptr_secret = fopen(secret_file_name, "w");
    if (decInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to create file %s\n", decInfo->secret_fname);

    	return e_failure;
    }
    return e_success;
}


Status close_decode_files(DecodeInfo *decInfo){
    if (decInfo->fptr_src_image != NULL) {
        fclose(decInfo->fptr_src_image);
        decInfo->fptr_src_image = NULL;
    }
    if (decInfo->fptr_secret != NULL) {
        fclose(decInfo->fptr_secret);
        decInfo->fptr_secret = NULL;
    }

    return e_success;
}