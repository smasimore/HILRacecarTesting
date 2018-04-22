/**	
 * File: USSensor.h
 * Author: Sarah Masimore
 * Last Updated Date: 04/19/2018
 * Description: Manages init'ing and updating Ultrasonic Ping sensor ports and 
 *              pins.
 */
 
#include "Simulator.h"
 
void USSensor_Init(struct sensor * sensor);

void USSensor_UpdateOutput(struct sensor * sensor);
