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

    printf("array is %d long\n",num);

    /*another error check. Last value of array should be "end"*/
    if (result[num-1] != end) {
        printf("last entry: %f. Should be %f\n",result[num-1],end);
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
    double x_old;
    double y_old;
    int period;
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

                /*check to see if we are trapped in a periodic iteration loop*/
                if (isclose(x,x_old,1e-3,0.0001) && isclose(y,y_old,1e-3,0.0001)) {
                    count = max_iters;
                    break;
                }
                period++;
                /* period checking code not working
                if (period > 20) {
                    period = 0;
                    x_old = x;
                    y_old = y;
                }
                */
            }
            result_matrix[i][j] = count;
        }
    }

    /*write result to csv file so we can plot it with python*/
    fp1 = fopen(filename, "w");
    if (fp1 == NULL) {
        printf("could not open file\n");
        exit(EXIT_FAILURE);
    }
    for (i=0; i<y_len; i++) {
        for (j=0; j<x_len; j++) {
            fprintf(fp1, "%lf", result_matrix[i][j]);
            if (j<(x_len-1)) {
                fprintf(fp1, ",");
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
        /*filename will be i.csv (i + 4 characters), and then we need another for null end char*/
        filename_len = snprintf(NULL,0,"%d",i) + 5;
        filename = emalloc(filename_len * sizeof filename[0]);
        sprintf(filename, "%d.csv",i);
        fractal(filename, x_min, x_max, y_min, y_max,1000,increment);
        free(filename);
    }
}

int main() {
    /*zoom in on https://en.wikipedia.org/wiki/Misiurewicz_point (-2, i)*/
    zoom(-2.0,1.1,-1.2,2.2,10);
    /*fractal("test.csv", -0.25, 0.25, -0.25, 0.25, 1000, 0.001);*/
    return 0;
}





