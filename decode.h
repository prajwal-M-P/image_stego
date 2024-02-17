#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user-defined typess
#define SECRET_BUF_SIZE 1
#define IMAGE_BUF_SIZE (SECRET_BUF_SIZE * 8)
#define FILE_SUFFIX 4
/* Structure to store information required for decoding */
typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_data_image_fname;
    FILE *fptr_stego_data_image;
   

    /* Secret File Info */
    char *secret_data_fname;
    FILE *fptr_secret_data_fname;

char magic_string_data[3];

    //etension
    char file_extn[4];
    int extn_size;
    int size_secret_file;
   


} DecodeInfo;

/* Decoding function prototype */
OperationType check_operation_type(char *argv[]);
/* Read and validate Decode args from argv */
Status_d read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status_d  do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status_d open_decode_files(DecodeInfo *decInfo);

/*Decode Magic String*/
Status_d decode_magic_string(DecodeInfo *decInfo);

/*Decode data from image*/
Status_d decode_data_from_image(char *buf, int size,FILE * fptr_stego_data_image);

/*Decode byte from lsb*/
Status_d decode_byte_from_lsb(char *ch,char *image_buffer);

/*Decode file extn size*/
Status_d decode_secret_file_extn_size(int *size,DecodeInfo *decInfo);

/*Decode size from lsb */
Status_d decode_size_from_lsb(char *size_buffer ,int *size);

/*decode secret file extn */
Status_d decode_secret_file_extn(int size,DecodeInfo *decInfo);


/*Decode secret file size */
Status_d decode_secret_file_size(int* file_size,DecodeInfo *decInfo);

/*Decode secret_file_data */
Status_d decode_secret_file_data(DecodeInfo *decInfo);

#endif
