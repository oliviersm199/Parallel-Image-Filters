#include "lodepng.h"
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


void process(unsigned char *image, unsigned char *new_image,long thread_count,unsigned width,unsigned height)
{
  int index=0;

  #pragma omp parallel for num_threads(thread_count)
  for (int i = 0; i < height; i+=2) {
    for (int j = 0; j < width; j+=2) {
      int upLeftR = 4*width*i + 4*j;
      int upRightR = 4*width*i + 4*(j+1);
      int downLeftR = 4*width*(i+1) + 4*j;
      int downRightR = 4 * width*(i+1) + 4*(j+1);
      //Green
      int upLeftG = upLeftR+1;
      int upRightG = upRightR+1;
      int downLeftG = downLeftR+1;
      int downRightG = downRightR+1;
      //Blue
      int upLeftB = upLeftR+2;
      int upRightB = upRightR+2;
      int downLeftB = downLeftR+2;
      int downRightB = downRightR+2;
      //A
      int upLeftA = upLeftR+3;
      int upRightA = upRightR+3;
      int downLeftA = downLeftR+3;
      int downRightA = downRightR+3;


      if ((image[upLeftR] >= image[upRightR]) && (image[upLeftR] >= image[downLeftR]) && (image[upLeftR] >= image[downRightR]))
      {
        new_image[index]=image[upLeftR];
        index++;
      }
      else if ((image[upRightR]>=image[downLeftR])&&(image[upRightR]>=image[downRightR]))
      {
        new_image[index]=image[upRightR];
        index++;
      }
      else if (image[downLeftR]>=image[downRightR]){
        new_image[index]=image[downLeftR];
        index++;
      }
      else
      {
        new_image[index]=image[downRightR];
        index++;
      }
      //second block
      if ((image[upLeftG] >= image[upRightG]) && (image[upLeftG] >= image[downLeftG]) && (image[upLeftG] >= image[downRightG]))
      {
        new_image[index]=image[upLeftG];
        index++;
      }
      else if ((image[upRightG]>=image[downLeftG])&&(image[upRightG]>=image[downRightG]))
      {
        new_image[index]=image[upRightG];
        index++;
      }
      else if (image[downLeftG]>=image[downRightG]){
        new_image[index]=image[downLeftG];
        index++;
      }
      else
      {
        new_image[index]=image[downRightG];
        index++;
      }
      //third block
      if ((image[upLeftB] >= image[upRightB]) && (image[upLeftB] >= image[downLeftB]) && (image[upLeftB] >= image[downRightB]))
      {
        new_image[index]=image[upLeftB];
        index++;
      }
      else if ((image[upRightB]>=image[downLeftB])&&(image[upRightB]>=image[downRightB]))
      {
        new_image[index]=image[upRightB];
        index++;
      }
      else if (image[downLeftB]>=image[downRightB]){
        new_image[index]=image[downLeftB];
        index++;
      }
      else
      {
        new_image[index]=image[downRightB];
        index++;
      }
      //4 block
      if ((image[upLeftA] >= image[upRightA]) && (image[upLeftA] >= image[downLeftA]) && (image[upLeftA] >= image[downRightA]))
      {
        new_image[index]=image[upLeftA];
        index++;
      }
      else if ((image[upRightA]>=image[downLeftA])&&(image[upRightA]>=image[downRightA]))
      {
        new_image[index]=image[upRightA];
        index++;
      }
      else if (image[downLeftA]>=image[downRightA]){
        new_image[index]=image[downLeftA];
        index++;
      }
      else
      {
        new_image[index]=image[downRightA];
        index++;
      }
    }
  }
}

void loadImage(char* input_filename, char* output_filename,long thread_count){
  unsigned error;
  unsigned char *image, *new_image;
  unsigned width, height;
  error = lodepng_decode32_file(&image, &width, &height, input_filename);
  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
  new_image = malloc((width/2) * (height/2) * 4 * sizeof(unsigned char));

  for(int i = 0;i<100;i++){
    process(image,new_image,thread_count,width,height);
  }

  lodepng_encode32_file(output_filename, new_image, (width/2), (height/2));
  free(image);
  free(new_image);
}


int main(int argc, char *argv[])
{
  char* input_filename = argv[1];
  char* output_filename = argv[2];
  char *ptr;
  long thread_count;
  thread_count = strtol(argv[3],&ptr,10);
  loadImage(input_filename, output_filename,thread_count);
  return 0;
}
