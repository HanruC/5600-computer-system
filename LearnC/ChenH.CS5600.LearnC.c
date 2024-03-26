/*
* ChenH.CS5600.LearnC.c / Conversion
*
* Hanru Chen / CS5600 / Northeastern University
* Spring 2024 / Jan 23th, 2024
*
*/
#include <stdio.h>
#include "mpg2km.h"

int main() {
    // Test cases for mpg2kml
    printf("25 MPG is %f KML\n", mpg2kml(25));
    printf("30 MPG is %f KML\n", mpg2kml(30));
    printf("-1 MPG is %f KML - This is the edge case.\n", mpg2kml(-1));

    // Test cases for mpg2lphm
    printf("25 MPG is %f LPHM\n", mpg2lphm(25));
    printf("30 MPG is %f LPHM\n", mpg2lphm(30));
    printf("0 MPG is %f LPHM - This is edge case.\n", mpg2lphm(0));

    // Test cases for lph2mpg
    printf("5 L/100KM is %f MPG\n", lph2mpg(5));
    printf("10 L/100KM is %f MPG\n", lph2mpg(10));
    printf("0 L/100KM is %f MPG - This is edge case.\n", lph2mpg(0));

    return 0;
}