/********** UnitConvert.c ************** 
 * Authors: Sarah Masimore (sm66498) and Zachary Susskind (zjs362)
 * Created Date: 02/09/2018
 * Last Updated Date: 02/19/2018
 * Description: Provides helper functions to convert between time units;
 * namely, SI, clock cycles, and time slices
 */

#include "stdint.h"

extern uint32_t TimeSliceCycles;

uint32_t ms2slices(uint32_t x){
	x *= 80000;
	if((x % TimeSliceCycles) != 0) x += TimeSliceCycles;
	return (x / TimeSliceCycles);
}

uint32_t us2slices(uint32_t x){
	x *= 80;
	if((x % TimeSliceCycles) != 0) x += TimeSliceCycles;
	return (x / TimeSliceCycles);
}
