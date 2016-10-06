#include "lodepng.h"
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void process(char* input_filename, char* output_filename,long thread_count)
{
  unsigned error;
  unsigned char *image, *new_image;
  unsigned width, height;

  error = lodepng_decode32_file(&image, &width, &height, input_filename);
  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
  new_image = malloc(width * height * 4 * sizeof(unsigned char));
  for (int i = 0; i < height; i++) {
    #pragma omp parallel for num_threads(thread_count)
    for (int j = 0; j < width; j++) {
      int indexRed = 4*width*i + 4*j + 0;
      int indexGreen = 4*width*i + 4*j + 1;
      int indexBlue = 4*width*i + 4*j + 2;
      int indexOpacity = 4 * width*i + 4*j + 3;

      // Red
      if(image[indexRed]< 127){
        new_image[indexRed] = 127;
      }else{
        new_image[indexRed] = image[indexRed];
      }

      // Green
      if(image[indexGreen]< 127){
        new_image[indexGreen] = 127;
      }else{
        new_image[indexGreen] = image[indexGreen];
      }

      // Blue
      if(image[indexBlue]< 127){
        new_image[indexBlue] = 127;
      }else{
        new_image[indexBlue] = image[indexBlue];
      }

      // Opacity
      new_image[indexOpacity] = image[indexOpacity];
    }
  }

  lodepng_encode32_file(output_filename, new_image, width, height);

  free(image);
  free(new_image);
}

int main(int argc, char *argv[])
{
  char* input_filename = argv[1];
  char* output_filename = argv[2];
  char *ptr;
  long thread_count;
  thread_count = strtol(argv[3], &ptr, 10);

  if(thread_count <=0){
    printf("Invalid input, please input a positive integer\n");
    exit(-1);
  }

  process(input_filename, output_filename,thread_count);

  return 0;
}
