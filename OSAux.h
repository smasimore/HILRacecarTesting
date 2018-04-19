/********** OSAux.h ************** 
 * Authors: Sarah Masimore (sm66498) and Zachary Susskind (zjs362)
 * Created Date: 02/15/2018
 * Last Updated Date: 02/28/2018
 Description: Handles suppemental OS functions
	These should not be called by a process
*/
#ifndef OSAUX_H
#define OSAUX_H

/**************OSAux_Wake***************
 Removes threads that are ready to be woken from the sleep list
 Inputs : none
 Outputs: none
*/
void OSAux_Wake(void);

#endif
