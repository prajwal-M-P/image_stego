#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
#include "common.h"
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}
uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);

}


Status open_files(EncodeInfo *encInfo)
{
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
Status copy_bmp_header(EncodeInfo *encInfo)
{
    char str[55];
    fseek(encInfo->fptr_src_image,0,SEEK_SET);
    fread(str,54,1,encInfo->fptr_src_image);
    fwrite(str,54,1,encInfo->fptr_stego_image);
    return e_success;
}
Status encode_magic_string(char *ch,EncodeInfo *encInfo)
{
   encode_data_to_image(ch,strlen(ch),encInfo);

return e_success;
}
Status encode_data_to_image(char *ch,int size,  EncodeInfo *encInfo)
    {

        for  (int i=0;i<size;i++)
        {
            fread( encInfo->image_data,8,1,encInfo->fptr_src_image);
                encode_byte_to_lsb(ch[i],encInfo->image_data);
            fwrite(encInfo->image_data,8,1,encInfo->fptr_stego_image);

        }
        return e_success;
    }

Status encode_byte_to_lsb(char ch,char *image_buffer)
{
    for (int i=0;i<8;i++){

        image_buffer[i]=(image_buffer[i] & 0XFE)| ((ch &(1<<(7-i)))>>(7-i));

    }

}
Status encode_secret_file_extn_size(int size,EncodeInfo *encInfo)
{
    encode_size_to_lsb(size,encInfo);
    return e_success;

}
Status encode_size_to_lsb(int size,EncodeInfo * encInfo)
{
    char str[32];
    fread(str,32,1,encInfo->fptr_src_image);
    
    for (int i=0;i<32;i++){
     str[i]=(str[i] & 0XFE)| ( (size &(1<<(31-i)))>>(31-i));

    }
    fwrite(str,32,1,encInfo->fptr_stego_image);
    return e_success;
}
Status encode_secret_file_extn(char *file_extn,EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn,strlen(file_extn),encInfo);
    return e_success;
}
Status encode_secret_file_size(int size,EncodeInfo *encInfo)
{
 
   encode_size_to_lsb(size,encInfo);
   return e_success;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char str[encInfo->size_secret_file];
    fseek(encInfo->fptr_secret,0,SEEK_SET);
    fread(str,encInfo->size_secret_file,1,encInfo->fptr_secret);

    encode_data_to_image(str,strlen(str),encInfo);
    return e_success;


}

Status copy_remaining_img_data(EncodeInfo *encInfo)
{
    int len=encInfo->image_capacity+54-ftell(encInfo->fptr_stego_image);
    char str[len];
    fread(str,len,1,encInfo->fptr_src_image);
    fwrite(str,len,1,encInfo->fptr_stego_image);
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);
    return e_success;
    

}


Status read_and_validate_encode_args(char*argv[],EncodeInfo *encInfo)
{
    if (strcmp((strstr(argv[2],".")),".bmp")==0)

    {
        encInfo->src_image_fname=argv[2];

        printf("%s\n",encInfo->src_image_fname);

    }
    else
    {
        return e_failure;
    }

    if (strcmp((strstr(argv[3],".")),".txt")==0)
    {
        encInfo->secret_fname= argv[3];
        printf("%s\n",encInfo->secret_fname);

        
    }
    else
    {
        return e_failure;
    }

    if (argv[4]!=NULL){


            printf("INFO : out file name is passed  passed \n");
            encInfo->stego_image_fname=argv[4];
            printf("%s\n", (char*)encInfo->stego_image_fname);

    }

        else
        { 
            printf("INFO : Output file name not passed so create default.bmp \n");            
                encInfo->stego_image_fname="default.bmp";
                 }
       
               

    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
   encInfo->image_capacity= get_image_size_for_bmp(encInfo->fptr_src_image);

   encInfo->size_secret_file =get_file_size(encInfo->fptr_secret);

  if(encInfo->image_capacity>(16+32+32+32+(encInfo->size_secret_file*8)))
      return e_success;
  else
      return e_failure;
}
Status do_encoding(EncodeInfo *encInfo)
{
    if (copy_bmp_header(encInfo)==e_success)
    {
        printf("INFO : copied bmp header is successfully\n");
        if(encode_magic_string(MAGIC_STRING,encInfo)==e_success)
        {
            printf("INFO :Encoding magic string is succsessful\n");
            strcpy(encInfo->extn_secret_file, strstr(encInfo->secret_fname,"."));

                  printf("%s\n",encInfo->extn_secret_file);
           if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo)==e_success)
            {
                printf("INFO :  extension secret file size is encoded successful\n");
                if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success)
                        
                        {
                               printf("INFO : encoding secret extention data is succeseeful\n" );                                                             
                               if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success)
                                       {
                                           printf("Info :encoding secret file is successful\n");
                                           if(encode_secret_file_data(encInfo)==e_success)
                                           {
                                               printf("info :encodede secret  data is succsess\n");
                                               if(copy_remaining_img_data(encInfo)==e_success)
                                               {
                                                   printf("INFO : copyeid remaining data is succesful\n");
                                               }
                                               else
                                               {
                                                   printf("error : copied is failure\n");
                                                   return e_failure;
                                               }

                                           }
                                           else
                                           {
                                               printf(" error : encoded secret data is failed\n ");
                                               return e_failure;
                                           }
                                       }
                                       else
                                       {
                                           printf("Error : encoding of secret file size is failed \n");
                                       }
                        }
                        else
                        {
                            printf("ERROR :encoding secret file data is failed\n");
                            return e_failure;
                        }
            }
            else
            {
                printf("ERROR : extention secret file size encoded is failed\n");
            }

        } 
        else
        {
            printf("ERROR: Encode magic string is failure\n");
            return e_failure;
        }

   }
    else
    {
        printf("ERROR: failed to copy\n");
          return e_failure;
           
    }
return e_success;
}






        

