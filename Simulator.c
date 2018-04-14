/*	
 * File: Simulator.h
 * Author: Sarah Masimore
 * Last Updated Date: 03/14/2018
 * Description: Manages simulation, including environment, car, sensors, and 
 * 							actuators. Units in millimeters.
*/

#include "Simulator.h"
#include "TrigLookup.h"
#include "isqrt.h"

uint8_t getSegmentIntersection(int32_t p0_x, int32_t p0_y, int32_t p1_x, 
	                             int32_t p1_y, int32_t p2_x, int32_t p2_y, 
                               int32_t p3_x, int32_t p3_y, int32_t *i_x, 
                               int32_t *i_y);
int32_t getDistanceBetweenPoints(int32_t x0, int32_t y0, int32_t x1, int32_t y1);

/**
 * Based on velocity, direction, and sim_freq update car's position.
 * Don't need to worry about walls since that will be checked in 
 * Simulator_HitWall.
 */
void Simulator_MoveCar(struct car * car, uint32_t simFreq) {
	uint32_t vel = car->vel;
	uint32_t dir = car->dir;
  uint32_t x = car->x;
	uint32_t y = car->y;
	uint32_t hyp = vel / SIM_FREQ; // get distance traveled (hypoteneuse)
	int32_t deltaX = CosLookup[dir]*(int32_t)hyp/TRIG_SCALE;
	int32_t deltaY = SinLookup[dir]*(int32_t)hyp/TRIG_SCALE;

	// Prevent car's x position from overflowing.
	if (deltaX < 0 && (-1*deltaX > x)) {
		car->x = 0;
	} else {	
		car->x = car->x + deltaX;
	}
	
	if (deltaY < 0 && (-1*deltaY > y)) {
		car->y = 0;
	} else {
		car->y = car->y + deltaY;
	}
}

/**
 * Based on previous and next location, determine if hit wall.
 */
uint8_t Simulator_HitWall(uint32_t prevX, uint32_t prevY, 
													uint32_t nextX, uint32_t nextY) {
	

	return 0;
}

/**
 * Update sensor values relative to environment. For each sensor, based on 
 * sensor's direction, car's direction, and car's position determine distance
 * to closest wall. 
 *
 * To determine if sensor line of sight intersects with a wall, loops through
 * each wall per sensor. Assumes max line of sight of MAX_SENSOR_LINE_OF_SIGHT.
 */
void Simulator_UpdateSensors(struct car * car, struct environment * env) {
	// Loop through sensors. Based on their type and distance from nearest
	// wall in its path, update value in struct sensor.
	uint8_t i, j;
	struct sensor * sensor;
	uint16_t absDir;
	int32_t endX, endY, intrsX, intrsY;
	struct wall * wall;
	uint32_t distance;
	uint32_t minDistance;
	uint8_t wallInSight;
	
	for (i = 0; i < car->numSensors; i++) {
		sensor = &car->sensors[i];
		absDir = car->dir + sensor->dir;
		if (absDir >= 360) {
			absDir -= 360;
		}
		
		// This can result in negative values, but this is ok since any negative
		// points on the line of sight will not intersect with walls.
	  endY = car->y + SinLookup[absDir]*MAX_SENSOR_LINE_OF_SIGHT/TRIG_SCALE;
	  endX = car->x + CosLookup[absDir]*MAX_SENSOR_LINE_OF_SIGHT/TRIG_SCALE;

		// Set minDistance to max int32
		minDistance = ~MAX_U32INT;
		wallInSight = 0;
		for (j = 0; j < env->numWalls; j++) {
			wall = &env->walls[j];
			
			// If segments intersect, calculate distance and maybe update minDistance.
			if (getSegmentIntersection(car->x, car->y, endX, endY, wall->startX, 
			                           wall->startY, wall->endX, wall->endY, 
			                           &intrsX, &intrsY)) {
				distance =  getDistanceBetweenPoints(car->x, car->y, intrsX, intrsY);
			  if (distance < minDistance) {
				  minDistance = distance;
				}
				wallInSight = 1;
			}
		}
		
		if (wallInSight) {
			sensor->val = minDistance;
		} else {
			sensor->val = MAX_U32INT;
		}
	}
}

/**
 * Determine if 2 segments intersect and store the intersection point if they
 * do. Uses fixed point.
 * 
 * Returns 1 if the lines intersect, otherwise 0. If lines intersect, the 
 * intersection point is stored in i_x and i_y.
 */
uint8_t getSegmentIntersection(int32_t s0_x, int32_t s0_y, int32_t s1_x, 
	                             int32_t s1_y, int32_t w0_x, int32_t w0_y, 
                               int32_t w1_x, int32_t w1_y, int32_t *i_x, 
                               int32_t *i_y)
{
	int32_t x_intrs, y_intrs;
	// Scaled up 1000x for fixed point math
	int32_t sensorSlope = (s1_y - s0_y) * 1000 / (s1_x - s0_x);
	int32_t sensorYIntersect = s0_y - sensorSlope * s0_x / 1000;
	
	
	
	// Horizontal wall
	if (w0_y == w1_y) {
		// Check if wall y is within sensor's y's
		if ((w0_y >= s0_y && w0_y <= s1_y) || (w0_y >= s1_y && w0_y <= s0_y)) {
			// Get sensor's x when at w0_y
			x_intrs = (w0_y - sensorYIntersect) * 1000 / sensorSlope;
			
			// Check if x in wall
			if ((x_intrs >= w0_x && x_intrs <= w1_x) || 
				  (x_intrs >= w1_x && x_intrs <= w0_x)) {
				*i_x = x_intrs;
				*i_y = w0_y;
				return 1;
			}
		}
		
	// Vertical wall
	} else {
		if ((w0_x >= s0_x && w0_x <= s1_x) || (w0_x >= s1_x && w0_x <= s0_x)) {
			y_intrs = sensorSlope * w0_x / 1000 + sensorYIntersect;
			
			// Check if y in wall
			if ((y_intrs >= w0_y && y_intrs <= w1_y) || 
				  (y_intrs >= w1_y && y_intrs <= w0_y)) {
				*i_x = w0_x;
				*i_y = y_intrs;
				return 1;
			}
		}
	}
	
	// No collision
	return 0; 
}

/**
 * Calculates approximate distance between points.
 */
int32_t getDistanceBetweenPoints(int32_t x0, int32_t y0, int32_t x1, int32_t y1) {
	int32_t xDiff = x1 - x0; 
	int32_t yDiff = y1 - y0; 
  
	return isqrt(xDiff * xDiff + yDiff * yDiff);
}
