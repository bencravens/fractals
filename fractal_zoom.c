#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*malloc with error checking*/
void* emalloc(size_t n) {
    void* result = malloc(n);
    if (result == NULL) {
        printf("memory allocation failed!");
        exit(EXIT_FAILURE);
    }
    return result;
}

/*function adapted from cython github implementation*/
static int isclose(double a, double b, double rel_tol, double abs_tol) {
    double diff = 0.0;

    /* sanity check on the inputs */
    if (rel_tol < 0.0 || abs_tol < 0.0 ) {
        printf("tolerances must be non negative.");
        return -1;
    }

    if ( a == b ) {
        /* short circuit exact equality -- needed to catch two infinities of
           the same sign. And perhaps speeds things up a bit sometimes.
        */
        return 1;
    }

    /* now do the regular computation
       this is essentially the "weak" test from the Boost library
    */

    diff = fabs(b - a);

    return (((diff <= fabs(rel_tol * b)) ||
             (diff <= fabs(rel_tol * a))) ||
            (diff <= abs_tol));
}

/*my own rewrite of python's arange*/
double* arange(double start, double end, double increment) {
    int i;
    double* result;
    double diff;
    int num;

    /*sanity checks*/
    if (start > end) {
        printf("start must be less than end");
        exit(EXIT_FAILURE);
    } else if (increment > (end - start)) {
        printf("increment too large");
        exit(EXIT_FAILURE);
    }

    diff = end - start;
    num = (diff/increment) + 1;
    result = emalloc(num * sizeof result[0]);
    for (i=0;i<num;i++) {
        result[i] = start + increment * i;
    }

    return result;
}

/*generate the fractal*/
void fractal(char* filename, double x_min, double x_max, double y_min, double y_max, int max_iters, double increment) {
    double* x_range = arange(x_min,x_max,increment);
    int x_len = (int) ((fabs(x_max-x_min)/increment) + 1);
    double* y_range = arange(y_min,y_max,increment);
    int y_len = (int) ((fabs(y_max-y_min)/increment) + 1);
    double** result_matrix;
    int i;
    int j;
    double x_0;
    double y_0;
    double x;
    double y;
    double x_temp;
    double x2;
    double y2;
    double result_max = 0.0;
    int count;
    FILE* fp1;

    /*initialize result matrix*/
    /*results matrix is a pointer to an array of pointers, 
    each which point to a row in the matrix. So there are
    y_len (number of rows) double pointers we must allocate*/
    result_matrix = emalloc(y_len * sizeof result_matrix[0]);
    /*initialize the rows*/
    for(i=0;i<y_len;i++) {
        /*allocate memory for the row*/
        result_matrix[i] = emalloc(x_len * sizeof result_matrix[0][0]);
        for(j=0;j<x_len;j++) {
            result_matrix[i][j] = 0.0;
        }
    }

    printf("made it this far");
    /*make the result matrix*/
    for(i=0;i<y_len;i++){
        for(j=0;j<x_len;j++){
            x_0 = x_range[j];
            y_0 = y_range[i];
            x = x_range[j];
            y = y_range[i];
            count = 0;
            /*iterate until convergence for each pixel*/
            while ((pow(x,2.0) + pow(y,2.0) < 4) && (count < max_iters)) {
                x2 = pow(x,2.0);
                y2 = pow(y,2.0);
                x_temp = x2 - y2 + x_0;
                y = 2*x*y + y_0;
                x = x_temp;
                count++;
            }
            /*set max value*/
            if (count > result_max) {
                result_max = count;
            }
            result_matrix[i][j] = count;
        }
    }

    /*normalize result matrix to grayscale (0-255)*/
    printf("maximum value for %s is %f\n",filename,result_max);
    for(i=0;i<y_len;i++){
        for(j=0;j<x_len;j++){
            result_matrix[i][j] = (result_matrix[i][j] / result_max) * 255;
        }
    }
 
    /*write result to NetPBM format file so we can convert it with imagemagick*/
    fp1 = fopen(filename, "w");
    if (fp1 == NULL) {
        printf("could not open file\n");
        exit(EXIT_FAILURE);
    }

    fprintf(fp1,"P2\n");  // P5 filetype    
    fprintf(fp1,"%d %d\n",x_len, y_len); //dimensions
    fprintf(fp1,"255\n"); //Max pixel

    /*now write to file*/
    for (i=0; i<y_len; i++) {
        for (j=0; j<x_len; j++) {
            /*write to file, logarithmic intensity scale for visual effect*/
            fprintf(fp1, "%d", (int) result_matrix[i][j]);
            if (j<(x_len-1)){
                fprintf(fp1, "    ");
            } 
        }
        fprintf(fp1,"\n");
    }
    fclose(fp1);

    /*free memory*/
    free(x_range);
    free(y_range);

    /*free matrix rows*/
    for(i=0;i<y_len;i++) {
        free(result_matrix[i]);
    }

    /*free matrix data structure*/
    free(result_matrix);
}

/*generate zoom_num csv files zooming in on x_midpoint, y_midpoint*/
void zoom(double x_min, double x_max, double y_min, double y_max, int num_files) {
    int i;
    double increment = 0.005;
    char* filename;
    int filename_len;
    double x_span = (x_max - x_min);
    double x_center = x_min + (x_span/2);
    double y_span = (y_max - y_min);
    double y_center = y_min + (y_span/2);
    double zoom_factor = 1.208;
    for (i=0;i<num_files;i++) {
        /*shrink bounding box by zoom_factor X. Increase resolution by zoom_factor X.*/
        increment = increment / zoom_factor;
        x_span = x_span / zoom_factor;
        y_span = y_span / zoom_factor;
        x_min = x_center - (x_span / zoom_factor);
        x_max = x_center + (x_span / zoom_factor);
        y_min = y_center - (y_span / zoom_factor);
        y_max = y_center + (y_span / zoom_factor);
        /*generate filename*/
        /*filename will be i.pgm (i + 4 characters), and then we need another for null terminator*/
        filename_len = snprintf(NULL,0,"%d",i) + 5;
        filename = emalloc(filename_len * sizeof filename[0]);
        sprintf(filename, "%d.pgm",i);
        fractal(filename, x_min, x_max, y_min, y_max,1000,increment);
        free(filename);
    }
}

int main() {
    double x_min,x_max,y_min,y_max;
    double x_mid,y_mid;
    double x_width,y_width;
    int zooms;
    int result_code;   
 
    /*get user input*/
    printf("Welcome to my fractal zoom program.\n");
    printf("Enter the x co-ordinate you want to zoom in on:\n");
    result_code = scanf("%lg",&x_mid);
    if (result_code!=1) {
        printf("scanf failed.\n");
        exit(EXIT_FAILURE);
    }
    printf("Enter the y co-ordinate you want to zoom in on:\n");
    result_code = scanf("%lg",&y_mid);
    if (result_code!=1) {
        printf("scanf failed.\n");
        exit(EXIT_FAILURE);
    }
    printf("Enter the initial x domain range:\n");
    result_code = scanf("%lg",&x_width);
    if (result_code!=1) {
        printf("scanf failed.\n");
        exit(EXIT_FAILURE);
    }   
    printf("Enter the initial y domain range:\n");
    result_code = scanf("%lg",&y_width);
    if (result_code!=1) {
        printf("scanf failed.\n");
        exit(EXIT_FAILURE);
    }
    printf("Enter the number of zooms you would like to do:\n");
    result_code = scanf("%d",&zooms);
    if (result_code!=1) {
        printf("scanf failed.\n");
        exit(EXIT_FAILURE);
    }  

    printf("input was %f %f %f %f %d\n",x_mid,x_width,y_mid,y_width,zooms);    

    /*calculate x_min,x_max,y_min,y_max*/
    x_min = x_mid - (x_width / 2);
    x_max = x_mid + (x_width / 2);
    y_min = y_mid - (y_width / 2);
    y_max = y_mid + (y_width / 2);

    /*call zoom function*/ 
    zoom(x_min,x_max,y_min,y_max,zooms);
    return 0;
}





