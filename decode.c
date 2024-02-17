#include<stdio.h>
#include "decode.h"
#include "types.h"
#include <string.h>
#include "common.h"


/*function Definitions to read and validate*/
Status_d read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo)
{
    if(! strcmp(strstr(argv[2],".") ,".bmp") )
    {
	decInfo->stego_data_image_fname=argv[2];
    }
    else
	return d_failure;

    if (argv[3] != NULL )
    {
	decInfo->secret_data_fname = argv[3];
    }
    else
    {
	decInfo ->secret_data_fname = "decoded.txt";
    }
    return d_success;

}

/*open files*/
Status_d  open_decode_files(DecodeInfo *decInfo)
{
   
    //stego image file
    decInfo->fptr_stego_data_image = fopen(decInfo->stego_data_image_fname,"r");
    printf("INFO: Opened %s\n",decInfo->stego_data_image_fname);

    //Doerror handling
    if(decInfo->fptr_stego_data_image == NULL)
    {
	perror("fopen");
	fprintf(stderr,"Error: Unable to open file %s\n",decInfo->stego_data_image_fname);
	return d_failure;
    }
    //no failure return d_success
    return d_success;
}
//function definition to decode magic string
Status_d decode_magic_string(DecodeInfo *decInfo)
{   
    printf("***decode magic string started***\n");
    fseek(decInfo->fptr_stego_data_image,54,SEEK_SET);
    decode_data_from_image(decInfo -> magic_string_data, strlen(MAGIC_STRING),decInfo->fptr_stego_data_image);
    decInfo->magic_string_data[2] = 0;
    if (!strcmp(decInfo->magic_string_data,MAGIC_STRING))
	return d_success;
    else
	return d_failure;
}
//function definition to decode data from image
Status_d decode_data_from_image(char *buf, int size, FILE *fptr_stego_data_image)
{

    char str[8];
    for (int i = 0; i < size; i++)
    {
	fread(str, 8, sizeof(char),fptr_stego_data_image);
	decode_byte_from_lsb(&buf[i], str);
    }
}
//function definition to decode byte from lsb
Status_d decode_byte_from_lsb(char *ch, char *image_buffer)
{
    *ch = 0x00;
    for (int i = 0; i < 8; i++)
    {
	*ch = ((image_buffer[i] & 0x01) << (7-i)) | *ch ;
    }
}
//function definition to decode secret file extn size
Status_d decode_secret_file_extn_size(int *size,DecodeInfo *decInfo)
{
    *size = 0;
    char str[32];
    fread(str,32,1,decInfo->fptr_stego_data_image);
    decode_size_from_lsb(str,size);

    return d_success;
}
//function to decode size/int from lsb
Status_d decode_size_from_lsb( char *size_buffer,int *size)
{
    for (int i=0;i<32;i++)
    {
	*size= ((size_buffer[i] & 0x01) <<(31-i)) | *size; 
    }

}
//function definition to decode secret file extn
Status_d decode_secret_file_extn(int size,DecodeInfo * decInfo)
{
    printf("***decoding output file extension data is started***\n");
    char str[decInfo->extn_size];
    decode_data_from_image(decInfo->file_extn, decInfo->extn_size,decInfo->fptr_stego_data_image);
    printf("INFO: Done\n");
    if( ! strcmp(decInfo->file_extn, ".txt"))
    {
	decInfo->fptr_secret_data_fname = fopen(decInfo->secret_data_fname, "w");
	if(decInfo->fptr_secret_data_fname == NULL)
	    return d_failure;
	if(! strcmp(decInfo->secret_data_fname,"decoded.txt"))
	{
	    printf("INFO: Output File not mentioned. Creating decoded.txt as default\n");
	    printf("INFO: Opened %s\n",decInfo->secret_data_fname);
	}
	else
	    printf("INFO: Opened %s\n",decInfo->secret_data_fname);


    }

    return d_success;
}
//function todecode secret file size
Status_d decode_secret_file_size(int *file_size,DecodeInfo *decInfo)
{
    printf("INFO: Decoding %s File Size\n",decInfo->secret_data_fname);
    char str[32];
    fread(str,32,sizeof(char),decInfo->fptr_stego_data_image);
    *file_size = 0;
    decode_size_from_lsb(str,file_size);
    return d_success;
}
//function to decode secret file data
Status_d decode_secret_file_data(DecodeInfo *decInfo)
{
    printf("INFO: Decoding %s File Data\n",decInfo->secret_data_fname);
    char str[decInfo->size_secret_file + 1];
    decode_data_from_image(str,decInfo->size_secret_file,decInfo->fptr_stego_data_image);
    str[decInfo->size_secret_file] = 0;
    fwrite(str,decInfo->size_secret_file,1,decInfo->fptr_secret_data_fname);
    return d_success;
}
//function defintion to do decoding process
Status_d do_decoding(DecodeInfo *decInfo)
{
    printf("***decoding is started***\n");
    if(open_decode_files(decInfo) == d_success)
    {
	if(decode_magic_string(decInfo) == d_success)
	{
	    printf("INFO :magic string matching is succsessful\n");
	    if( decode_secret_file_extn_size (&(decInfo->extn_size),decInfo) == d_success)
	    {
            printf("INFO:decoding secret file size is succsessful\n");
		if( decode_secret_file_extn(decInfo->extn_size,decInfo) == d_success)
		{
		    printf("INFO:decoding secret file exention data is succsessful\n");
		    if (decode_secret_file_size(&(decInfo->size_secret_file),decInfo) == d_success)
		    {
			printf("INFO: decoding secret file size is succsessful\n");
			if( decode_secret_file_data(decInfo) ==d_success)
			{
			    printf("INFO: decoding secret file data is succsessful\n");
			    printf("**data transfored to  %s is done\n",decInfo->secret_data_fname);
                return d_success;
			}
			else
			{ 
			    printf("INFO: Decoding Secret File Data  failed\n");
			    return d_failure;
			}
		    }
		    else
		    {
			printf("INFO: Decoding Secret File Size failed\n");
			return d_failure;
		    }
		}
		else
		{
		    printf("INFO: Decoding Secret File Extension failed\n");
		    return d_failure;
		}
	    }
	    else
	    {
		printf("INFO: Decoding Secret File Extension Size failed\n");
		return d_failure;
	    }
	}
	else
	{
	    printf("INFO: Decoding Magic String failed\n");
	    return d_failure;
	}
    }
    else
    {
	printf("INFO: Opening required files failed\n");
	return d_failure;
    }
    return d_success;
}

