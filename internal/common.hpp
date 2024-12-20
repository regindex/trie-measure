#ifndef COMMON_HPP_
#define COMMON_HPP_

/* INCLUDE SECTION */
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
/* TYPES DEFINITION SECTION */
typedef uint32_t ulint;
typedef int32_t lint;
typedef bool boolt;
/* use following definitions for larger datasets
typedef uint32_t ulint;
typedef int32_t lint;
typedef bool boolt;
*/
/* NAMESPACES SECTION */
using namespace std;
/* STRUCT STORING INPUT PARAMETERS */
struct Args
{
    bool grammar = false;
    bool optimalOrdered = false;
    bool optimalOrderedShifted = false;
    bool getEncoding = false;
    bool all = false;
};

#endif