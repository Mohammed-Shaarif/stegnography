#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"

/* Function Definitions */

OperationType check_operation_type(char c){
    if (c == 'e')
        return e_encode;
    else if (c == 'd')
        return e_decode;
    else
        return e_unsupported;
}


Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo){
    // step 1: validate arguments
    encInfo->src_image_fname = argv[2];
    if(encInfo->src_image_fname == NULL)
        return e_failure;
    if(strlen(encInfo->src_image_fname) < 5 || strcmp(&encInfo->src_image_fname[strlen(encInfo->src_image_fname)-4], ".bmp") != 0){
        fprintf(stderr, "ERROR: Source image must be a BMP file\n");
        return e_failure;
    }

    encInfo->secret_fname = argv[3];
    if(argv[4] == NULL){
        encInfo->stego_image_fname = "output.bmp";
        printf("INFO: Stego Image File Name not provided, using default: %s\n", encInfo->stego_image_fname);
    }
    else
        encInfo->stego_image_fname = argv[4];

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
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
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
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
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


    return e_success;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image){
    char header[54];
    fread(header, sizeof(char), 54, fptr_src_image);
    fwrite(header, sizeof(char), 54, fptr_dest_image);

    return e_success;
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image){
    // step 1: read size bytes from src image to buffer
    char image_buffer[size];
    fread(image_buffer, sizeof(char), size, fptr_src_image);

    // step 2: encode data to image buffer
    for (int i = 0; i < size; i++)
    {
        if (encode_byte_to_lsb(data[i], &image_buffer[i]) == e_failure)
        {
            fprintf(stderr, "ERROR: Encoding byte %d failed\n", i);
            return e_failure;
        }
    }

    // step 3: write modified buffer to stego image
    fwrite(image_buffer, sizeof(char), size, fptr_stego_image);

    return e_success;
}



Status encode_byte_to_lsb(char data, char *image_buffer){

    for (int i = 0; i < 8; i++)
    {
        // step 1: clear the LSB of image buffer
        *image_buffer &= 0xFE;

        // step 2: set the LSB of image buffer to data
        *image_buffer |= ((data >> (7 - i)) & 0x01);

        // step 3: move to next byte in image buffer
        image_buffer++;
    }
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

    return e_success;
}
