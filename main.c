#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "bmplib.h"
#include "mpi.h"

#define MASTER 0
#define MAX_FILE_NAME 100

int enlarge(PIXEL* original, int rows, int cols, int scale,
      PIXEL** new, int* newrows, int* newcols);
int rotate(PIXEL* original, int rows, int cols, int rotation,
     PIXEL** new, int* newrows, int* newcols);
int flip (PIXEL *original, PIXEL **new, int rows, int cols);

int main(int argc, char ** argv)
{
        /* MPI Variables*/
   int   numtasks, taskid, len;
   char hostname[MPI_MAX_PROCESSOR_NAME];
  /*  cmd option variables  */
  int o, err = 0;
  char* output;
  char *programs[10];

  FILE *fp;
  int count = 0;

  char line[50];
  int i=0;
  int j;
  char filename [MAX_FILE_NAME];
  int sflag=0, rflag=0,lflag = 0, fflag = 0, oflag=0;
  int scale, rotation;
  static char usage[] = "usage: %s [-s scale | -r degree | -f ] [-o output_file] [input_file]\n";
  char errormsg[100];
  /*  bitmap variables */
  int r, c;
  PIXEL *b = NULL, *nb = NULL;

MPI_Init(&argc, &argv);
MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
MPI_Get_processor_name(hostname, &len);

printf ("Hello from task %d on %s!\n", taskid, hostname);
if (taskid == MASTER)
   printf("MASTER: Number of MPI tasks is: %d\n",numtasks);


    /*  CECIS PART*/
    fp = fopen("imagelist.txt", "r");

    if (fp == NULL)
    {
        printf("Could not open file %s", filename);
        return 0;
    }
    while(fgets(line,sizeof line, fp)!=NULL){
        line[strlen(line)-1] = '\0';
        programs[i]=strdup(line);
        i++;
        count++;
    }
    fclose(fp);
    /*  CECIS PART END*/

  for(i = 0; i < 10; i++){
     printf("%s\n",programs[i]);
}

  rotation = 180;


  for(i = taskid ; i < 10 ; i += numtasks) {
     char prepath [50] = "images/";
     char destination [50] = "results/";
     strcat(prepath,programs[i]);
     printf("%s\n",prepath);
     readFile(prepath, &r,&c, &b);

    printf("Flipping\n");
    flip(b, &nb, r, c);


    //Write BMP file
    strcat(destination, programs[i]);
    printf("Writing file...\n");
    writeFile(destination, r, c, nb);

    if(b == nb) {
    free(b);
  } else {
    free(b);
    free(nb);
  }

  }

   printf("Process %d done. BYE.\n", taskid);
  MPI_Finalize();
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
