#ifndef __COMPASS_H
#define __COMPASS_H

/* Includes */
#include "boolean.h"
#define F_CPU 1000000UL /* CPU speed = 1 MHz */
#include <util/delay.h>

/* Pin defines */
#define PORT_ALL_HIGH   0xff
#define PORT_ALL_OUTPUT 0xff
#define PORT_ALL_PULLUP 0xff
//may not need

/* Direction Strings */
#define NORTH_S   "     North      "
#define NE_S      "   Northeast    "
#define EAST_S    "     East       "
#define SE_S      "   Southeast    "
#define SOUTH_S   "     South      "
#define SW_S      "   Southwest    "
#define WEST_S    "     West       "
#define NW_S      "   Northwest    "
#define DIRECTION "Degrees:        "

/* Bootup strings */
#define TITLE     "Compass Ver X.Y "
#define NAME      " -Alex Becker-  "
#define BOOTUP    "   Booting up   "
#define WAITING   "Waiting on data "

/* Calibration strings */
#define CALIB     "  Calibrating   "

/* Directional defines */
//Define the ranges here, figure out how to make this work.
// 338 <= N  <=  22
//  23 <= NE <=  67
//  68 <= E  <= 112
// 113 <= SE <= 157
// 158 <= S  <= 202
// 203 <= SW <= 247
// 248 <= W  <= 292
// 293 <= NW <= 337

/* I2C Address defines */
#define LINEAR_ACC_ADDR   0x32
#define MAGNETOMETER_ADDR 0x3C

/* Registers */
#define CRA_REG_M   0x00
#define CRB_REG_M   0x01
#define MR_REG_M    0x02
/* Coordinate registers */
#define OUT_X_H_M   0x03
#define OUT_X_L_M   0x04
#define OUT_Z_H_M   0x05
#define OUT_Z_L_M   0x06
#define OUT_Y_H_M   0x07
#define OUT_Y_L_M   0x08
/* other regs */
#define SR_REG_M    0x09

/* Other defines */
#define STARTUP_DELAY 2000    /* Delay between startup screens */
#define RAD_TO_DEG (180/M_PI) /* Convert radians to degrees */
#define DEG_TO_RAD (M_PI/180) /* Convert degrees to radians */

/* Function prototypes */
double CalculateDegHeading(int16_t X, int16_t Y);
double ProcessData();
boolean InitDevice();
char* HeadingString(int16_t Degrees);

#endif /* __COMPASS_H */
