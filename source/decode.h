#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types
#include "common.h" // Contains common macros and constants
/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct DecodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;
    uint image_capacity;
    uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;
    int extn_size;

    char magic_string[sizeof(MAGIC_STRING)];


} DecodeInfo;


/* Encoding function prototype */



/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_decode_files(DecodeInfo *decInfo);

/* check capacity */
//Status check_capacity(EncodeInfo *encInfo);

/* Get image size */
//uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
//uint get_file_size(FILE *fptr);

/* Copy bmp image header */
Status skip_bmp_header(FILE *fptr_src_image);

/* Store Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);


Status decode_secret_file_extn_size(DecodeInfo *decInfo);

Status decode_secret_file_extn(const char *file_extn, DecodeInfo *decInfo);


Status decode_secret_file_size(DecodeInfo *decInfo);

Status decode_secret_file_data(DecodeInfo *decInfo);



Status decode_data_from_image(const char *data, int size, FILE *fptr_src_image);


Status decode_byte_from_lsb(char data, char *image_buffer);

/* Copy remaining image bytes from src to stego image after encoding */
//Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);
Status decode_int_data_from_image(char *data, int size, FILE *fptr_src_image);
#endif


Status create_secret_file(DecodeInfo *decInfo);