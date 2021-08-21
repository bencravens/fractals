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
    printf("diff is %f\n",diff);
    num = (int) (diff)/increment + 1;
    printf("num is %d\n",diff/increment);
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

void fractal_zoom(int max_iters, int x_size, int y_size) {
    double* x_range = arange(-2.5,1.0,0.1);
    double* y_range = arange(-1,1,0.1);
}

int main() {
    printf("hello world\n");
    fractal_zoom(1000,100,100);
    return 0;
}


