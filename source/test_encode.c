#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"

int main(int argc, char *argv[]){
    // step 1: check for sufficient arguments
    if(argc < 4){
        printf("ERROR: Insufficient arguments\n");
        return 1;
    }

    // step 2: check operation type
    int operation_type = check_operation_type(argv[1][1]);
    if(operation_type == e_encode)
        printf("INFO: Operation Type is Encode\n");
    else if(operation_type == e_decode)
        printf("INFO: Operation Type is Decode\n");
    else{
        printf("ERROR: Operation Type is Unsupported\n");
        return 1;
    }


    EncodeInfo encInfo;
    //uint img_size;

    // step 3: read and validate encode arguments
    if (read_and_validate_encode_args(argv, &encInfo) == e_failure){
    	printf("ERROR: %s function failed\n", "read_and_validate_encode_args" );
    	return 1;
    }else
    	printf("SUCCESS: %s function completed\n", "read_and_validate_encode_args" );


    // step 4: perform encoding
    if(do_encoding(&encInfo) == e_failure){
        printf("ERROR: %s function failed\n", "do_encoding" );
        return 1;
    }else
        printf("SUCCESS: %s function completed\n", "do_encoding" );

 /*   // Fill with sample filenames
    encInfo.src_image_fname = "beautiful.bmp";
    encInfo.secret_fname = "secret.txt";
    encInfo.stego_image_fname = "stego_img.bmp";

    // Test open_files
    if (open_files(&encInfo) == e_failure)
    {
    	printf("ERROR: %s function failed\n", "open_files" );
    	return 1;
    }
    else
    {
    	printf("SUCCESS: %s function completed\n", "open_files" );
    }

    // Test get_image_size_for_bmp
    img_size = get_image_size_for_bmp(encInfo.fptr_src_image);
    printf("INFO: Image size = %u\n", img_size);
*/

    
    return 0;
}
