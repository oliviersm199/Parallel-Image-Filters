#include "lodepng.h"
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// compare the each color and find the biggest one and return it.
int compare(int upLeft,int upRight,int downLeft,int downRight,unsigned char *image){	
		
	 if ((image[upLeft] >= image[upRight]) && (image[upLeft] >= image[downLeft]) && (image[upLeft] >= image[downRight]))
      {
	return upLeft;
	
      }
      else if ((image[upRight]>=image[downLeft])&&(image[upRight]>=image[downRight]))
      {
	return upRight;
       
      }
      else if (image[downLeft]>=image[downRight]){
	return downLeft;
	
      }
      else
      {
	return downRight;
      }	

}

//extract each color out of the image and take the max value of each color 
void process(unsigned char *image, unsigned char *new_image,long thread_count,unsigned width,unsigned height)
{
  #pragma omp parallel for num_threads(thread_count)
  for (int i = 0; i < height; i+=2) {
	for (int j = 0; j < width; j+=2) {
	//Red
      int upLeftR = 4*width*i + 4*j;
      int upRightR = 4*width*i + 4*(j+1);
      int downLeftR = 4*width*(i+1) + 4*j;
      int downRightR = 4 * width*(i+1) + 4*(j+1);
      int new_Red = compare(upLeftR,upRightR,downLeftR,downRightR,image);
	new_image[(4*(width/2)*(i/2))+4*(j/2)]=image[new_Red];
	
      //Green 
      int upLeftG = upLeftR+1;
      int upRightG = upRightR+1;
      int downLeftG = downLeftR+1;
      int downRightG = downRightR+1;
      int new_Green = compare(upLeftG,upRightG,downLeftG,downRightG,image);
	new_image[(4*(width/2)*(i/2))+4*(j/2)+1]=image[new_Green];
      //Blue
      int upLeftB = upLeftR+2;
      int upRightB = upRightR+2;
      int downLeftB = downLeftR+2;
      int downRightB = downRightR+2; 
      int new_Blue = compare(upLeftB,upRightB,downLeftB,downRightB,image);
	new_image[(4*(width/2)*(i/2))+4*(j/2)+2]=image[new_Blue];
      //A
      int upLeftA = upLeftR+3;
      int upRightA = upRightR+3;
      int downLeftA = downLeftR+3;
      int downRightA = downRightR+3;
      int new_Opacity = compare(upLeftA,upRightA,downLeftA,downRightA,image);
	new_image[(4*(width/2)*(i/2))+4*(j/2)+3]=image[new_Opacity];
	
    }
  }
}
// generate the new image 
void loadImage(char* input_filename, char* output_filename,long thread_count){
  unsigned error;
  unsigned char *image, *new_image;
  unsigned width, height;
  error = lodepng_decode32_file(&image, &width, &height, input_filename);
  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
  new_image = malloc((width/2) * (height/2) * 4 * sizeof(unsigned char));
	// to test how the number of thread change the process speed
	for(int i = 0;i<100;i++)
	{
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
