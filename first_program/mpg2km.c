/*
 * PalleA.CS5600.LearnC.c / Program in C
 *
 * Adithya Palle / CS5600 / Northeastern University
 * Spring 2024 / Jan 20, 2024
 *
 */
#include <stdio.h>
#include "mpg2km.h"
#include <assert.h>
#include <math.h>
#include <float.h>
double mpg2kml(double mpg)
{
    /**
     * Takes in miles per gallon and returns kilometers per lfiter
     * args:
     * mpg : double = miles per gallon
     * returns:
     * kml : double = kilometers per liter
     *
     */
    //printf("mpg2kml, mpg: %f , abs(mpg): %d\n", mpg,abs(mpg));
    if (fabs(mpg) >= DBL_MAX || fabs(mpg) <= DBL_MIN)
    {
        return mpg;
    }

    return mpg * 0.425144;
}

double mpg2lphm(double mpg)
{
    /**
     * Takes in miles per gallon and returns liters per hundred kilometers
     * args:
     *      mpg : double = miles per gallon
     * returns:
     *      lphm : double = liters per hundred kilometers
     */
    int sign = (mpg >= 0) ? 1 : -1;
    double lphm;
    if (fabs(mpg) >= DBL_MAX)
    {
        lphm = sign*DBL_MIN;
    } else if (fabs(mpg) <= DBL_MIN)
    {
        lphm = sign*DBL_MAX;
    }
    else{
        lphm = 100/mpg2kml(mpg);
    }
    return lphm;
}

double lph2mpg(double lphm)
{
    /**
     * Takes in liters per hundred kilometers and returns miles per gallon
     * args:
     *      lphm : double = liters per hundred kilometers
     * returns:
     *      mpg : double = miles per gallon
     */

    return mpg2lphm(lphm); // its the same conversion as before
}

int approxEqual(double a, double b)
{
    /**
     * Takes in two doubles and returns true if they are within the tolerance
     * args:
     *      a : double = first double
     *      b : double = second double
     *      tolerance : double = tolerance
     * returns:
     *      bool : true if a and b are within tolerance
     */
    return fabs(a - b) <= 0.0001;
}