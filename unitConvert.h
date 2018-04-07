/********** UnitConvert.h ************** 
Name: Sarah Masimore (sm66498) and Zachary Susskind (zjs362)
Lab Number: 16000
Created Date: 02/09/2018
Last Updated Date: 02/09/2018
Description: Provides helper functions to convert between time units;
	namely, SI, clock cycles, and time slices
*/

#ifndef UNITCONVERT_H
#define UNITCONVERT_H

#include "stdint.h"

#define ms2cc(x)		80000*x
#define us2cc(x)		80*x
#define cc2us(x)		x/80
#define cc2ms(x)		x/80000

uint32_t ms2slices(uint32_t x);
uint32_t us2slices(uint32_t x);

#endif
