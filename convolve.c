#include "lodepng.h"
#include "wm.h"
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int weightMatrixDim = 3;

//restricts output to being between 0 and 255
unsigned char clamp_output(float input){
  if(input > 255.0){
    return 255;
  }
  else if(input < 0.0){
    return 0;
  }
  else{
    return (unsigned char)input;
  }
}

unsigned char reduction(float* subMatrix){
  float sum = 0;
  for(int i = 0; i < weightMatrixDim; i++){
    for(int j = 0; j < weightMatrixDim; j++){
          sum += subMatrix[3*i + j];
    }
  }
  free(subMatrix);
  return clamp_output(sum);
}



// color 0 = red, color 1 = green, color 2 = blue and color 3 = opacity.
// anything else gives you -1 (error)
int generateColorIndex(int i, int j, int color, int width, int height){
  if(color < 0 || color > 3){
    return -1;
  }
  return  4 * width * i + 4 * j + color;
}


// will generate sub matrix that will then be added to get index in array
float* generateSubConvolve(unsigned char *image, int i, int j, int color, int width, int height){
  float* subMatrix = (float*)malloc(weightMatrixDim * weightMatrixDim * sizeof(float));
  for(int ii = 0;ii <weightMatrixDim;ii++){
    for(int jj=0;jj<weightMatrixDim;jj++){
      int index = generateColorIndex(i+ii-1,j+jj-1,color,width,height);
      subMatrix[3*ii + jj] = image[index] * w[ii][jj];
    }
  }
  return subMatrix;
}


void convolve(unsigned char *image, unsigned char *new_image,unsigned width, unsigned height,long thread_count){
  //going through array doing a convolution on points between (1,height -1) and (1,width-1)
  for(int i = 1; i < height-1; i++){
    for(int j = 1; j < width-1; j++){
      // convolve rgb (do not convolve opacity)
      for(int color =0;color<3;color++){
        float* ptr = generateSubConvolve(image,i,j,color, width, height);
        // for each convolution that we generate, execute a reduction op to calculate the value
        // in the new image.
        new_image[generateColorIndex(i-1,j-1,color,width-2,height-2)] = reduction(ptr);
      }
      // setting opacity to be the same value
      int indexOpacity = generateColorIndex(i-1,j-1,3,width-2,height-2);
      new_image[indexOpacity] = image[indexOpacity];
    }
  }
}

void loadAndProcess(char* input_filename, char* output_filename,long thread_count)
{
  unsigned error;
  unsigned char *image, *new_image;
  unsigned width, height;

  // loading the image and recreating the image size
  error = lodepng_decode32_file(&image, &width, &height, input_filename);
  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
  new_image = malloc((width-2) * (height-2) * 4 * sizeof(unsigned char));

  //launch convolution
  convolve(image,new_image, width,height,thread_count);


  // save the file
  lodepng_encode32_file(output_filename, new_image, width-2, height-2);

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

  loadAndProcess(input_filename, output_filename,thread_count);

  return 0;
}
