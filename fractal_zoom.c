#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void* emalloc(size_t n) {
    void* result = malloc(n);
    if (result == NULL) {
        fprintf(stderr, "memory allocation failed!");
        exit(EXIT_FAILURE);
    }
    return result;
}

/*my own rewrite of python's arange*/
double* arange(double start, double end, double increment) {
    int i;
    double* result;
    double diff;
    int num;

    /*sanity checks*/
    if (start > end) {
        fprintf(stderr, "start must be less than end");
    } else if (increment > (end - start)) {
        fprintf(stderr, "increment too large");
    }

    diff = end - start;
    num = (diff/increment) + 1;
    result = emalloc(num * sizeof result[0]);
    for (i=0;i<num;i++) {
        result[i] = start + increment * i;
    }

    for (i=0;i<num;i++) {
        printf("%f\n",result[i]);
    }

    /*another error check. Last value of array should be "end"*/
    if (result[num-1] != end) {
        fprintf(stderr,"last entry: %f. Should be %f\n",result[num-1],end);
    }

    return result;
}

void fractal_zoom(int max_iters, double increment) {
    double* x_range = arange(-2.5,1.0,increment);
    int x_len = (int) ((3.5)/increment) + 1;
    double* y_range = arange(-1,1,increment);
    int y_len = (int) ((2)/increment) + 1;
    double result[y_len][x_len];
    int i;
    int j;
    double x_0;
    double y_0;
    double x;
    double y;
    double x_temp;
    double x2;
    double y2;
    int count;
    FILE* fp1;

    /*make the result matrix*/
    for(i=0;i<y_len;i++){
        for(j=0;j<x_len;j++){
            printf("%f, %f\n",x_range[j],y_range[i]);
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
            result[i][j] = count;
        }
    }

    /*write result to csv file so we can plot it with python*/
    fp1 = fopen("fractal.csv", "w");
    if (fp1 == NULL) {
        fprintf(stderr, "could not open file\n");
        exit(EXIT_FAILURE);
    }
    for (i=0; i<y_len; i++) {
        for (j=0; j<x_len; j++) {
            fprintf(fp1, "%lf", result[i][j]);
            if (j<(x_len-1)) {
                fprintf(fp1, ",");
            }
        }
        fprintf(fp1,"\n");
    }
    fclose(fp1);

    free(x_range);
    free(y_range);
}

int main() {
    printf("hello world\n");
    fractal_zoom(1000,0.005);
    return 0;
}
