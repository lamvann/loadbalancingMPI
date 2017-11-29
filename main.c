#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "bmplib.h"



int enlarge(PIXEL* original, int rows, int cols, int scale, 
      PIXEL** new, int* newrows, int* newcols);
int rotate(PIXEL* original, int rows, int cols, int rotation,
     PIXEL** new, int* newrows, int* newcols);
int flip (PIXEL *original, PIXEL **new, int rows, int cols);

int main(int argc, char ** argv)
{
  /*  cmd option variables  */
  extern char* optarg;
  extern int optind;
  int o, err = 0; 
  char* output;
  int sflag=0, rflag=0, fflag = 0, oflag=0;
  int scale, rotation;
  static char usage[] = "usage: %s [-s scale | -r degree | -f ] [-o output_file] [input_file]\n";
  char errormsg[100];
  /*  bitmap variables */
  int r, c;
  PIXEL *b = NULL, *nb = NULL;

  while((o = getopt(argc, argv,"s:r:fo:")) != -1){
    switch(o){
    case 's':
      sflag++;
      scale = atoi(optarg);
      if(scale < 1){
        err=1;
        strcpy(errormsg,"Error: Scale factor must be a positive number > 1");
      }
      break;
    case 'r':
      rflag++;
      rotation = atoi(optarg);
      if(rotation % 90) {
        err=1;
        strcpy(errormsg,"Error: Rotatation degree must be multiple of 90");
      }
      break;
    case 'f':
      fflag++;
      break;
    case 'o':
      oflag++;
      output = optarg;
      break;
    case '?':
      err = 1;
      break;
    }
  }
  if (err) {
    fprintf(stderr, usage, argv[0]);
    printf("%s\n", errormsg);
    exit(1);
  }
  if (optind < argc) {  
    readFile(argv[optind], &r, &c, &b);
  } else {
    readFile(output, &r, &c, &b);
  }

  if (sflag) {      
    printf("Scaling by a factor of %d\n", scale);
    enlarge(b,r,c,scale,&nb,&r,&c);
    if (rflag || fflag)
    { 
      b=nb;
      nb=NULL;
    }
  }  
  if(rflag) {
    printf("Rotating by %d degrees\n", rotation);
    rotate(b,r,c,rotation,&nb,&r,&c);
    if (fflag)
    { 
      b=nb;
      nb=NULL;
    }
  }
  if(fflag) {
    printf("Flipping\n");
    flip(b, &nb, r, c);  
  }
  if(oflag) {
    printf("Writing file...\n");
    writeFile(output, r, c, nb);
  }

  //printf("/** main **/\nb :%p\nnb:%p\n", b,nb);

  if(b == nb) {
    free(b);
  } else {
    free(b);
    free(nb); 
  }
  printf("Done. BYE.\n");
  return 0;
}
/*
 * This method enlarges a 24-bit, uncompressed .bmp file
 * that has been read in using readFile()
 *
 * original - an array containing the original PIXELs, 3 bytes per each
 * rows     - the original number of rows
 * cols     - the original number of columns
 *
 * scale    - the multiplier applied to EACH OF the rows and columns, e.g.
 *           if scale=2, then 2* rows and 2*cols
 *
 * new      - the new array containing the PIXELs, allocated within
 * newrows  - the new number of rows (scale*rows)
 * newcols  - the new number of cols (scale*cols)
 */
int enlarge(PIXEL* original, int rows, int cols, int scale, 
      PIXEL** new, int* newrows, int* newcols) 
{
  int row,col;
  *newrows = rows*scale;
  *newcols = cols*scale;

  if ((rows <= 0) || (cols <= 0)) return -1;
 
  *new = (PIXEL*)malloc((*newrows) * (*newcols) * sizeof(PIXEL));
  
  for (row=0; row < (*newrows); row++)
    for (col=0; col < (*newcols); col++) {
      //PIXEL* o = original + row*cols + col;
      //PIXEL* n = (*new) + row*cols + (cols-1-col);
      PIXEL* o = original + (row/scale)*cols + (col/scale);
      PIXEL* n = (*new) + row * (*newcols) + col;
      *n = *o;
    }
  return 0;
}
/*
 * This method rotates a 24-bit, uncompressed .bmp file that has been read 
 * in using readFile(). The rotation is expressed in degrees and can be
 * positive, negative, or 0 -- but it must be a multiple of 90 degrees
 * 
 * original - an array containing the original PIXELs, 3 bytes per each
 * rows     - the number of rows
 * cols     - the number of columns
 * rotation - a positive or negative rotation, 
 *
 * new      - the new array containing the PIXELs, allocated within
 * newrows  - the new number of rows
 * newcols  - the new number of cols
 */
int rotate(PIXEL* original, int rows, int cols, int rotation,
     PIXEL** new, int* newrows, int* newcols)
{
  
  int row,col,flag;
  PIXEL* temp;

  if(rotation  < 0) {
    flag = 1;
  } else {
    flag = 0;
  }
  rotation = (rotation/90)%4;
  if (rotation==0)
  {
    *new=original;
    return 0;
  }

  if ((rows <= 0) || (cols <= 0)) return -1;

  *new = (PIXEL*)malloc(rows*cols*sizeof(PIXEL));

  if(flag) { /* left rotation*/
    rotation *= -1;
    while(rotation){
      for(col = 0 ; col < cols; col++){
        for(row = rows; row > 0; row-- ){
          PIXEL* o = original + col + (cols*(row-1));
          PIXEL* n = (*new) + (col*rows) + (rows-row);
          *n = *o;
        }
      }
      /*  Switch pointer locations */
      if(rotation > 1) {
        temp = original;
        original = *new;
        *new = temp;
      }
      rotation--;
    }
  } else { /* right rotation*/
    while(rotation){
      for(col = 0 ; col < cols; col++){
        for(row = rows; row > 0; row-- ){
          PIXEL* o = original + col + (cols*(row-1));
          PIXEL* n = (*new) + (row-1) + (rows*(cols-col-1));
          *n = *o;
        }
      }
      /*  Switch pointer locations  */
      if(rotation > 1) {
        temp = original;
        original = *new;
        *new = temp;
      }
      rotation--;
    }
  }
  return 0;
}
/*
 * This method horizontally flips a 24-bit, uncompressed bmp file
 * that has been read in using readFile(). 
 * 
 * THIS IS GIVEN TO YOU SOLELY TO LOOK AT AS AN EXAMPLE
 * TRY TO UNDERSTAND HOW IT WORKS
 *
 * original - an array containing the original PIXELs, 3 bytes per each
 * rows     - the number of rows
 * cols     - the number of columns
 *
 * new      - the new array containing the PIXELs, allocated within
 */
int flip (PIXEL *original, PIXEL **new, int rows, int cols) 
{
  int row, col;

  if ((rows <= 0) || (cols <= 0)) return -1;

  *new = (PIXEL*)malloc(rows*cols*sizeof(PIXEL));

  for (row=0; row < rows; row++)
    for (col=0; col < cols; col++) {
      PIXEL* o = original + row*cols /* y */ + col /* x */;
      PIXEL* n = (*new) + row*cols + (cols-1-col);
      *n = *o;
    }

  return 0;
}
