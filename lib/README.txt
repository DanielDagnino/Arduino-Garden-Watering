This directory is intended for the project specific library SRS (SunSet/Rise).
Platform will compile it to static library and link to executable file.

The source code of the library and main code should be organiced like:
|--path_arduino
|  |--automatic_watering
|  |  |--main.cpp
|  |--libraries
|  |  |- SRS.c
|  |  |- SRS.h

In the main.cpp code you should see:
#include <SRS.h>
that includes the library.
