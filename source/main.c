#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include <unistd.h>
#include "common.h"
#include "decode.h"
int main(int argc, char *argv[]){
    int operation_type = check_operation_type(argv[1][1]);

    if(operation_type == e_encode){
        EncodeInfo encInfo;
        printf("INFO: Operation Type is Encode\n");
        if(argc < 4){
            printf("ERROR: Insufficient arguments\n");
            return 1;
        }

        if (read_and_validate_encode_args(argv, &encInfo) == e_failure){
    	    printf("ERROR: %s function failed\n", "read_and_validate_encode_args" );
    	    return 1;
        }
    	
        printf("SUCCESS: Arguments read and validated successfully\n");
         
        if(do_encoding(&encInfo) == e_failure){
            printf("ERROR: %s function failed\n", "do_encoding" );
            return 1;
        }
        printf("SUCCESS: Encoding completed successfully\n");


    }
    else if(operation_type == e_decode){
        DecodeInfo decInfo;
        printf("INFO: Operation Type is Decode\n");
        if(argc < 3){
            printf("ERROR: Insufficient arguments\n");
            return 1;
        }

        if (read_and_validate_decode_args(argv, &decInfo) == e_failure){
    	    printf("ERROR: %s function failed\n", "read_and_validate_decode_args" );
    	    return 1;
        }

        printf("SUCCESS: Arguments read and validated successfully\n");
        if(do_decoding(&decInfo) == e_failure){
            printf("ERROR: %s function failed\n", "do_decoding" );
            return 1;
        }
        printf("SUCCESS: Decoding completed successfully\n");

    }
    else{
        printf("ERROR: Operation Type is Unsupported\n");
        return 1;
    }

    
    return 0;
}
