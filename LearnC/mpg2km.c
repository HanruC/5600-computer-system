#include "mpg2km.h"
#include <stdio.h>

// Convert miles per gallon to kilometers per liter
double mpg2kml(double mpg) {
    if (mpg < 0) {
        printf("Error: mpg2kml: mpg cannot be negative.\n");
        return -1; 
    }
    return mpg * 1.60934 / 3.78541;
}

// Convert miles per gallon to liters per 100 kilometers
double mpg2lphm(double mpg) {
    // Check for zero or negative input
    if (mpg <= 0) {
        printf("Error: mpg cannot be negative or zero.\n");
        return -1;
    }
    return 235.215 / mpg;
}

// Convert liters per 100 kilometers to miles per gallon
double lph2mpg(double lph) {
    if (lph <= 0) {
        printf("Error: lph cannot be negative or zero.\n");
        return -1;
    }
    return 235.215 / lph;
}

