#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include <string.h>
#include "decode.h"
int main(int argc, char *argv[]) {
    EncodeInfo E1;
    DecodeInfo D1;

    if (argc > 3)
        printf("INFO: cla is validated\n");
    else
        printf("INFO: cla is not validating\n ");

    OperationType res = check_operation_type(argv);
    if (res == e_encode) {
        printf("INFO: Encoding is selected\n");

        if (read_and_validate_encode_args(argv, &E1) == e_success) {
            printf("INFO: read and validation of encoding is successful\n");

            if (open_files(&E1) == e_success) {
                printf("INFO: Opening files is successful\n");

                if (check_capacity(&E1) == e_success) {
                    printf("INFO: Check capacity of file is successful\n");

                    if (do_encoding(&E1) == e_success) {
                        printf("INFO: Encoding is successful\n");
                    } else {
                        printf("ERROR: Encoding is failed\n");
                    }
                } else {
                    printf("INFO: Check capacity of file is failed\n");
                }
            } else {
                printf("ERROR: Opening is failed\n");
                return 1;
            }
        } else {
            printf("INFO: read and validation is failed\n");
            return 1;
        }
    } else if (res == e_decode) {
        printf("INFO: Decoding is selected\n");
        // Add decoding logic here
        if(read_and_validate_decode_args(argv,&D1)== d_success)
                {
                    printf("INFO :read and validate is successfull\n");
                    if(open_decode_files(&D1)==d_success)
                    {
                         printf("INFO : open files is succsesful\n");
                         if(do_decoding(&D1)== d_success)
                                 {
                                     printf("INFO : DECODING IS SUCCESSFUL\n");

                                 }
                         else
                         {
                             printf("ERROR:decoding is failed\n ");
                         }
                    }
                    else
                    {
                        printf("error: open filles are failed\n");
                        return 1;
                    }

                }
        else
        {
            printf("ERROR : read and validation is failed\n");
            return 1;
        }

    } else {
        printf("ERROR: Unsupported format\n");
        printf("Usage:\nEncoding:\n./a.out -e beautiful.bmp secret.txt [stego.bmp]\nDecoding:\n./a.out -d stego.bmp [data.txt]\n");
        return 1;
    }

    return 0;
}

// Function to check operation type
OperationType check_operation_type(char *argv[]) {
    if (strcmp(argv[1], "-e") == 0)
        return e_encode;
    else if (strcmp(argv[1], "-d") == 0)
        return e_decode;
    else
        return e_unsupported;
}
