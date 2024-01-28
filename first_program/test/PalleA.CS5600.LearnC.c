/*
 * PalleA.CS5600.LearnC.c / Program in C
 *
 * Adithya Palle / CS5600 / Northeastern University
 * Spring 2024 / Jan 20, 2024
 *
 */
#include <stdio.h>
#include "mpg2km.h"
#include <float.h>
#include <assert.h>

int main(int argc, char *argv[])
{
    // Test cases for mpg2kml

    assert(mpg2kml(0.5) == 0.212572);
    assert(mpg2kml(1) == 0.425144);
    assert(mpg2kml(2) == 0.850288);
    assert(mpg2kml(0) == 0);

    // for "bad" argumetns
    assert(mpg2kml(-1) == -0.425144);
    assert(mpg2kml(-2) == -0.850288);
    assert(mpg2kml(-0) == 0);

    // for overflow
    assert(mpg2kml(10000000000) == 4251440000);
    assert(mpg2kml(DBL_MAX) == DBL_MAX); // just return the max value if overflow
    assert(mpg2kml(DBL_MIN) == DBL_MIN); // just return the min value if overflow

    // Test cases for mpg2lphm

    assert(approxEqual(mpg2lphm(0.5), 470.428843)); 
    assert(approxEqual(mpg2lphm(1), 235.214421));
    assert(approxEqual(mpg2lphm(2), 117.607210));
    assert(approxEqual(mpg2lphm(0), DBL_MAX));

    
    // for "bad" argumetns
    assert(approxEqual(mpg2lphm(-1), -235.214421));
    assert(approxEqual(mpg2lphm(-2), -117.607210));
    assert(approxEqual(mpg2lphm(0), DBL_MAX));

    // for overflow
    assert(mpg2lphm(DBL_MAX) == DBL_MIN); // just return the min value if overflow
    assert(mpg2lphm(DBL_MIN) == DBL_MAX); // just return the max value if underflow  
    assert(mpg2lphm(-DBL_MAX) == -1*DBL_MIN); 
    assert(mpg2lphm(-DBL_MIN) == -1*DBL_MAX); 

    // Test cases for lph2mpg

    assert(approxEqual(lph2mpg(0.5), 470.4288));
    assert(approxEqual(lph2mpg(1), 235.2144));
    assert(approxEqual(lph2mpg(2), 117.6072));
    assert(approxEqual(lph2mpg(0), DBL_MAX));

    
    // for "bad" argumetns
    assert(approxEqual(lph2mpg(-1), -235.2144));
    assert(approxEqual(lph2mpg(-2), -117.6072));
    assert(approxEqual(lph2mpg(-0), DBL_MAX));

    // for overflow
    assert(lph2mpg(DBL_MAX) == DBL_MIN); // just return the min value if overflow
    assert(lph2mpg(DBL_MIN) == DBL_MAX); // just return the max value if underflow 
    assert(lph2mpg(-DBL_MAX) == -DBL_MIN); 
    assert(lph2mpg(-DBL_MIN) == -DBL_MAX);




    printf("All tests passed!\n");
    return 0;
}