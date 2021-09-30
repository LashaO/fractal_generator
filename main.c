#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <complex.h>
#define ESCAPE_RADIUS 2.0

void set_color(unsigned char *color, int iteration, int max_iteration){
 /* compute color per pixel depending on the number of iterations to escape */
    if (iteration==max_iteration){
    /*  convergent part of the set */
        color[0]=0;  /* Red */
        color[1]=0;  /* Green */
        color[2]=0;/* Blue */

    }
    else{
        /* divergent part of the set */
        /* scale factors are choosen arbitrarily, to ensure appealing visuals */
        double scale = (double)iteration / max_iteration;
        color[0]=255 * (1-scale) * scale;  /* Red */
        color[1]=255 * 0.5 *  scale;  /* Green */
        color[2]=255 *  scale; /* Blue */
    }
}


double complex polynomial(const double complex z0, const double complex *const parameters) {
    /* compute the value of a polynomial z^n + c, given the parameters */
    const double complex c = parameters[0];
    const double complex n = parameters[1];
    return cpow(z0, n) + c;
}


int julia(const double complex z0, const double complex *const parameters, int max_iterations) {
    /* compute number of iterations to escape, julia-style*/
    /* notice, c is constant and z0 varies*/
    double complex z1 = z0;
    double complex z2 = polynomial(z1, parameters);

    /* if it diverges, return the number of iterations*/
    for (int i = 1; i < max_iterations; i++) {
        z1 = z2;
        z2 = polynomial(z1, parameters);

        if (cabs(z2) > ESCAPE_RADIUS) {
            return i;
        }
    }
    /* if it does not diverge, return max_iterations which is equal to number of iterations checked*/
    return max_iterations;
}

int mandelbrot(const double complex z0, double complex *const parameters, int max_iterations) {
    /* mandelbrot set is the same as julia set, but z0 starts from 0, and c varies instead */
    /* hence, we can use the formula for julia if we swap c for z0 and set z0 = 0*/
    parameters[0] = z0;

    return julia(0, parameters, max_iterations);
}


int main() {

    char MODE[20];
    double n;
    double c_re;
    double c_im;

    /* user prompt*/
    printf("Select fractal mode [julia/mandelbrot]: \n");
    scanf("%s", MODE);

    if (strcmp(MODE, "julia")==0){
        printf("Select parameters n, c_real, c_imaginary for function z^n + (c_real + c_imaginary): \n");
        printf("n = ");
        scanf("%lf", &n);
        printf("\n");
        printf("c_real = ");
        scanf("%lf", &c_re);
        printf("\n");
        printf("c_imaginary = ");
        scanf("%lf", &c_im);
        printf("\n");
        }

    else if(strcmp(MODE, "mandelbrot")==0){
        printf("Select parameter n for function z^n + c: \n");
        printf("n = ");
        scanf("%lf", &n);
        printf("\n");
        c_re = 0;
        c_im = 0;
    }

    const int width = 1600;
    const int height = 1600;
    const int max_iterations = 250;

    /* complex numbers consist of the real and imaginary part, which the user provides*/
    /* we can combine them and perform operations easily with complex.h standard library*/
    const double complex c = c_re + c_im * I;
    double complex parameters[2] = {c, n};

    double xi, yi;
    double complex zi;

    /* range of coordinates to plot */
    const double x_min = -2;
    const double x_max = 2;
    const double y_min = -2;
    const double y_max = 2;

    /* lenght traversed on the coordinate system, corresponding to each pixel on the rendered image */
    const double x_step = (x_max - x_min) / width;
    const double y_step = (y_max - y_min) / height;

    int num_iterations;
    static unsigned char color[3];

    /* opening the file to write the resulting fractal to */
    char *filename= "fractal.ppm";
    FILE *fp;
    fp = fopen(filename,"wb");
    /*ASCII header of the file*/
    fprintf(fp,"P6\n # \n %d\n %d\n 255\n",width,height);


    printf("Generating the fractal...\n");

    /* loop which iterates over a grid of pixels, computes iteration value and plots the corresponding color */
    for (int hi = 0; hi < height; hi++) {
            for (int wi = 0; wi < width; wi++) {
                /* find the appropriate coordinates x,y given the pixel */
                xi = x_min + wi * x_step;
                yi = y_max - hi * y_step;
                zi = xi + yi * I;

                /* calculate iterations to escape */
                if (strcmp(MODE, "julia") == 0) {
                    num_iterations = julia(zi, parameters, max_iterations);
                }
                else if (strcmp(MODE, "mandelbrot") == 0) {
                    num_iterations = mandelbrot(zi, parameters, max_iterations);
                }

                /* find the color, array is passed by pointer and modified in-place */
                set_color(color, num_iterations, max_iterations);
                /* write the resulting color to the file */
                fwrite(color, 1, 3,fp);

            }

        }
        fclose(fp);

        printf("Done!\n");
        return 0;
    }

