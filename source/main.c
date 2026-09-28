#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include <unistd.h>
#include "common.h"
#include "decode.h"
int main(int argc, char *argv[]){
    if(argc < 2){
        printf("ERROR: Insufficient arguments\n");
        help_menu(argv);
        return 1;
    }
    if(argv[1][1] == 'h' || argv[1][1] == 'H'){
        help_menu(argv);
        return 1;
    }
    int operation_type = check_operation_type(argv[1][1]);

    if(operation_type == e_encode){
        EncodeInfo encInfo;
        printf("INFO: Operation Type is Encode\n");
        if(argc < 4){
            printf("ERROR: Insufficient arguments\n");
            encoding_help_menu(argv);
            return 1;
        }

        if (read_and_validate_encode_args(argv, &encInfo) == e_failure){
    	    printf("ERROR: Arguments validation failed\n" );
            printf("ERROR: Encoding failed\n");
    	    return 1;
        }
    	
        printf("SUCCESS: Arguments read and validated successfully\n");
         
        if(do_encoding(&encInfo) == e_failure){
            printf("ERROR: Encoding failed\n" );
            return 1;
        }
        printf("SUCCESS: Encoding completed successfully\n");


    }
    else if(operation_type == e_decode){
        DecodeInfo decInfo;
        printf("INFO: Operation Type is Decode\n");
        if(argc < 3){
            printf("ERROR: Insufficient arguments\n");
            decoding_help_menu(argv);
            return 1;
        }

        if (read_and_validate_decode_args(argv, &decInfo) == e_failure){
    	    printf("Error: Arguments validation failed\n");
            printf("ERROR: Decoding failed\n");
    	    return 1;
        }

        printf("SUCCESS: Arguments read and validated successfully\n");
        if(do_decoding(&decInfo) == e_failure){
            printf("ERROR: Decoding failed\n");
            return 1;
        }
        printf("SUCCESS: Decoding completed successfully\n");

    }
    else{
        printf("ERROR: Operation Type is Unsupported\n");
        help_menu(argv);
        return 1;
    }

    return 0;
}
