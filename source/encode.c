#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include <unistd.h>


void encoding_help_menu(char *argv[]){
    char *program_name = strrchr(argv[0], '/')?strrchr(argv[0], '/') : strrchr(argv[0], '\\');
    if (program_name != NULL) {
        program_name++;
    } else {
        program_name = argv[0];
    }
    printf("Usage:\n\t%s -e <source_image.bmp> <secret_file> [<output_stego_image.bmp>]\n\n", program_name);
    printf("Options:\n");
    printf("\t-e <source_image.bmp> <secret_file> [<output_stego_image.bmp>]\t\tSpecify the source BMP image file and the secret file to encode.\n");
    printf("\t\t\t\t\t\t\t\t\t\tOptional. Specify the name of the output stego image. If not provided, defaults to 'stego_image.bmp'.\n\n");
    printf("Example:\n");
    printf("\t%s -e source_image.bmp secret.txt stego_image.bmp\n\n", program_name);
}
/* Function Definitions */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo){
    // step 1: validate arguments
    encInfo->src_image_fname = argv[2];
    if(encInfo->src_image_fname == NULL)
        return e_failure;
    if(strlen(encInfo->src_image_fname) < 5 || strcmp(&encInfo->src_image_fname[strlen(encInfo->src_image_fname)-4], ".bmp") != 0){
        fprintf(stderr, "ERROR: Source image must be a BMP file\n");
        return e_failure;
    }
    memset(encInfo->extn_secret_file, 0, MAX_FILE_SUFFIX);

    encInfo->secret_fname = argv[3];
    strcpy(encInfo->extn_secret_file,strrchr(encInfo->secret_fname, '.'));
    if(strlen(encInfo->extn_secret_file) == 0){
        fprintf(stderr, "ERROR: Secret file must have an extension\n");
        return e_failure;
    }
    printf("INFO: Secret file extension is %s\n", encInfo->extn_secret_file);
    if(argv[4] == NULL){
        encInfo->stego_image_fname = "output.bmp";
        printf("INFO: Stego Image File Name not provided, using default: %s\n", encInfo->stego_image_fname);
    }
    else{
        char *stego_image_fname = argv[4];
        stego_image_fname[strrchr(stego_image_fname, '.') - stego_image_fname] = '\0';
        strcat(stego_image_fname, ".bmp");
        encInfo->stego_image_fname = stego_image_fname;
    }

    if(strlen(encInfo->stego_image_fname) < 5 || strcmp(&encInfo->stego_image_fname[strlen(encInfo->stego_image_fname)-4], ".bmp") != 0){
        fprintf(stderr, "ERROR: output image must be a BMP file\n");
        return e_failure;
    }

    if(encInfo->secret_fname == NULL || encInfo->stego_image_fname == NULL)
        return e_failure;
    
    printf("INFO: Source Image File Name: %s\n", encInfo->src_image_fname);
    printf("INFO: Secret File Name: %s\n", encInfo->secret_fname);
    printf("INFO: Stego Image File Name: %s\n", encInfo->stego_image_fname);


    // step 2: validate files exists
    if(open_files(encInfo) == e_failure){
        return e_failure;
    }

    return e_success;
}


/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image){
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("INFO: Width = %u, ", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("Height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr){
    fseek(fptr, 0L, SEEK_END);
    long size = ftell(fptr);
    fseek(fptr, 0L, SEEK_SET);
    return (uint)size;
}

Status check_capacity(EncodeInfo *encInfo){
    // step 1: check if image has enough capacity in bits to hold secret data
    // magic string + secret file size + extension size + extension + secret data
    int addons = strlen(MAGIC_STRING) + sizeof(long) + MAX_FILE_SUFFIX + encInfo->size_secret_file;
    addons *= 8; // convert to bits
    printf("INFO: Required capacity = %d bits, Image capacity = %u bits\n", addons, encInfo->image_capacity);
    if (encInfo->image_capacity < (uint)(addons))
    {
        fprintf(stderr, "ERROR: Image capacity is less than secret file size\n");
        return e_failure;
    }
    return e_success;
}

/*  * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo){
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo){
    // check meta data for bmp for BM tag
    if (fgetc(encInfo->fptr_src_image) != 'B' || fgetc(encInfo->fptr_src_image) != 'M'){
        fprintf(stderr, "ERROR: Source image is not a valid BMP file\n");
        return e_failure;
    }
    fseek(encInfo->fptr_src_image, 0, SEEK_SET);
    // step 1: check capacity
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    if (check_capacity(encInfo) == e_failure){
        fprintf(stderr, "ERROR: Capacity check failed\n");
        return e_failure;
    }
      
    printf("INFO: Image Capacity = %u bytes, Secret File Size = %ld bytes\n", encInfo->image_capacity, encInfo->size_secret_file);

    // step 2: copy bmp image header
    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure){
        fprintf(stderr, "ERROR: Copying BMP header failed\n");
        return e_failure;
    }
      
    printf("INFO: BMP header copied successfully\n");

    // step 3: encode magic string
    if(encode_magic_string(MAGIC_STRING, encInfo) == e_failure){
        fprintf(stderr, "ERROR: Encoding magic string failed\n");
        return e_failure;
    }
      
    printf("INFO: Magic string encoded successfully\n");

    // step 4: encode secret file extension size
    if(encode_secret_file_extn_size(encInfo->extn_secret_file, encInfo) == e_failure){
        fprintf(stderr, "ERROR: Encoding secret file extension size failed\n");
        return e_failure;
    }
      
    printf("INFO: Secret file extension size encoded successfully\n");

    // step 5: encode secret file extension
    if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure){
        fprintf(stderr, "ERROR: Encoding secret file extension failed\n");
        return e_failure;
    }
      
    printf("INFO: Secret file extension encoded successfully\n");


    // step 6: encode secret file size
    if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure){
        fprintf(stderr, "ERROR: Encoding secret file size failed\n");
        return e_failure;
    }
      
    printf("INFO: Secret file size encoded successfully\n");

    // step 7: encode secret file data
    if(encode_secret_file_data(encInfo) == e_failure){
        fprintf(stderr, "ERROR: Encoding secret file data failed\n");
        return e_failure;
    }
      
    printf("INFO: Secret file data encoded successfully\n");

    // step 8: copy remaining image data
    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure){
        fprintf(stderr, "ERROR: Copying remaining image data failed\n");
        return e_failure;
    }
      
    printf("INFO: Remaining image data copied successfully\n");
    return e_success;
}

Status encode_data_to_image(const char *data, int size,FILE *fptr_src_image, FILE *fptr_stego_image){
    // step 1: read size*8 bytes from src image
    char image_buffer[size * 8];
    size_t bytes_read = fread(image_buffer, sizeof(char), size * 8, fptr_src_image);
    if (bytes_read < size * 8) {
        fprintf(stderr, "ERROR: Not enough image data to encode\n");
        return e_failure;
    }

    // step 2: encode each byte into 8 image bytes
    for (int i = 0; i < size; i++) {
        encode_byte_to_lsb(data[i], &image_buffer[i * 8]);
    }

    // step 3: write modified buffer back (size*8 bytes)
    fwrite(image_buffer, sizeof(char), size * 8, fptr_stego_image);
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer){
    for (int i = 0; i < 8; i++)
    {
        // step 1: clear the LSB of image buffer
        *image_buffer &= 0xFE;

        // step 2: set the LSB of image buffer to data
        *image_buffer |= ((data >> (7 - i)) & 0x01);
        //printf("INFO: Encoding bit %d of byte %c into image byte %d\n", 7 - i, data, *image_buffer);

        // step 3: move to next byte in image buffer
        image_buffer++;
    }
    return e_success;

}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image){
    char header[54];
    rewind(fptr_src_image);
    rewind(fptr_dest_image);
    fread(header, sizeof(char), 54, fptr_src_image);
    fwrite(header, sizeof(char), 54, fptr_dest_image);

    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo){
    // step 1: get the size of the magic string
    int magic_size = strlen(magic_string);

    // step 2: encode the magic string to the image
    if (encode_data_to_image(magic_string, magic_size, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure){
        fprintf(stderr, "ERROR: Encoding magic string failed\n");
        return e_failure;
    }
    //printf("INFO: Magic string encoded successfully\n");

    return e_success;
}

Status encode_secret_file_extn_size(const char *file_extn, EncodeInfo *encInfo){
    // step 1: get the size of the file extension
    int extn_size = strlen(file_extn);
    if (extn_size > MAX_FILE_SUFFIX) {
        extn_size = MAX_FILE_SUFFIX;
        fprintf(stderr, "WARNING: Secret file extension size exceeds maximum limit, truncating to %d characters\n", MAX_FILE_SUFFIX);
    }

    // step 2: encode the file extension size to the image
    if (encode_data_int_to_image(extn_size, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure){
        fprintf(stderr, "ERROR: Encoding secret file extension size failed\n");
        return e_failure;
    }
    return e_success;
}

Status encode_data_int_to_image(int num, FILE *fptr_src_image, FILE *fptr_stego_image) {
    // step 1: read 4*8 bytes from src image
    char image_buffer[sizeof(int) * 8];
    size_t bytes_read = fread(image_buffer, sizeof(char), sizeof(int) * 8, fptr_src_image);
    if (bytes_read < sizeof(int) * 8) {
        fprintf(stderr, "ERROR: Not enough image data to encode integer\n");
        return e_failure;
    }

    // step 2: encode each byte of the integer into 8 image bytes (little-endian order)
    for (int i = 0; i < sizeof(int); i++) {
        unsigned char byte = (num >> (i * 8)) & 0xFF;   // little-endian: lowest byte first
        encode_byte_to_lsb(byte, &image_buffer[i * 8]);
    }

    // step 3: write modified buffer back (4*8 bytes)
    fwrite(image_buffer, sizeof(char), sizeof(int) * 8, fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo){
    int extn_size = strlen(file_extn);
    if(extn_size > MAX_FILE_SUFFIX) {
        extn_size = MAX_FILE_SUFFIX;
        fprintf(stderr, "WARNING: Secret file extension size exceeds maximum limit, truncating to %d characters\n", MAX_FILE_SUFFIX);
    }
    if(encode_data_to_image(file_extn, extn_size, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure){
        fprintf(stderr, "ERROR: Encoding secret file extension failed\n");
        return e_failure;
    }
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo){
    if (encode_data_to_image((const char *)&file_size, sizeof(long), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure){
        fprintf(stderr, "ERROR: Encoding secret file size failed\n");
        return e_failure;
    }

    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo){
    // step 1: get the size of the secret file
    long secret_size = encInfo->size_secret_file;

    // step 2: encode the secret file data to the image
    for (long i = 0; i < secret_size; i++) {
        char byte;
        fread(&byte, sizeof(char), 1, encInfo->fptr_secret);
        if (encode_data_to_image(&byte, 1, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure){
            fprintf(stderr, "ERROR: Encoding secret file data failed at byte %ld\n", i);
            return e_failure;
        }
    }
    //printf("INFO: Secret file data encoded successfully\n");

    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest){
    char buffer[1024];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, sizeof(char), sizeof(buffer), fptr_src)) > 0)
    {
        fwrite(buffer, sizeof(char), bytes_read, fptr_dest);
    }

    return e_success;   
}

Status close_files(EncodeInfo *encInfo){
    if (encInfo->fptr_src_image != NULL) {
        fclose(encInfo->fptr_src_image);
        encInfo->fptr_src_image = NULL;
    }
    if (encInfo->fptr_secret != NULL) {
        fclose(encInfo->fptr_secret);
        encInfo->fptr_secret = NULL;
    }
    if (encInfo->fptr_stego_image != NULL) {
        fclose(encInfo->fptr_stego_image);
        encInfo->fptr_stego_image = NULL;
    }
    return e_success;
}