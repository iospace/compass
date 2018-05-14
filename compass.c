/* CODE BY Input/Output Space
 *
 * This file contains the code to run the portable compass.
 *
 * Author: Alex "iospace" Becker
 *
 * File name: compass.c
 *
 * Version: 0.1
 *
 * Purpose: To serve as the main file for the compass.  The main loop is here
 *
 */

/* #includes */
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/power.h>
#include <string.h>
#include <util/twi.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "compass.h"
#include "Lcd.h"
#include "Twi.h"
#include "boolean.h"

/* Global Variables */
boolean DataReady = FALSE; /* Data is ready to be read */

/* The DataReady pin is hooked up to INT0 on the 328p */
ISR(INT0_vect) {
    DataReady = TRUE;
}

/* Configure the pins on the ATmega328p */
void InitDevice() {
    //temp
    uint8_t test, status;
    char temp_str[16];
    /* Disable all possible devices on the board */
    power_all_disable();

    test = 0;
    status = 0;

    /* LCD setup */
    /* All PORTB pins are data pins for the LCD screen */
    //DDRB = (uint8_t)(-1);
    DDRB = PORT_ALL_OUTPUT;

    /* PDO, PD1, and PD3 are all used as control pins for the LCD screen.
     * PD0 is used as the R/S pin, which determines whether the LCD is getting a
     * command or a character
     * PD1 is the Enable pin, which signals the LCD to read the data pins
     * PD3 is the R/W Pin which is used to activate the Busy flag on the LCD and
     * is used for timing
     */
    DDRD |= (_BV(PD0) | _BV(PD1) | _BV(PD3));

    LcdInit();

    /* Display title screen */
    LcdWriteString(TITLE, LCD_LINE_ONE);
    LcdWriteString(NAME, LCD_LINE_TWO);
    _delay_ms(STARTUP_DELAY);

    LcdWriteString(BOOTUP, LCD_LINE_TWO);

    /* Calibration circuit */
    /* PD4 is used to determine if the calibration circuit is active or not */
    //DDRD &= ~_BV(PD4); /* Configure PD4 as an input */
    //PORTD |= _BV(PD4); /* Pullup PD4 */

    /* Configure the ADC */
    //Currently not implemented
    //power_adc_enable();

    /* Magnetometer set up */
    /* INT0 is attached to the DataReady pin on the magnometer, and will be used
     * to signal that data is ready to be read (obviously).  INT0 is set to go
     * off on a rising edge.
     */
    //EIMSK |= _BV(INT0);
    //EICRA |= _BV(ISC01) | _BV(ISC00);

    /* Configure TWI */
    power_twi_enable();
    TWCR = _BV(TWEN); /* Enable TWI */ //May not be needed here, but definitely elsewhere
    //Check to see if the magnetometer needs initialization from the ATmega
    //will need init
    /* Set the magnetometer in continuous mode */
    status = TwWriteByte(MAGNETOMETER_ADDR, MAG_MR_REG, 0x00);
    if(status != TW_SUCCESS) {
        sprintf(temp_str, "setup error: %x", status);
        LcdWriteString(temp_str, LCD_LINE_ONE);
    }

    /*status = TwReadByte(MAGNETOMETER_ADDR, MAG_MR_REG, &test);
    if(status != TW_SUCCESS) {
        sprintf(temp_str, "setup error: %x", status);
        LcdWriteString(temp_str, LCD_LINE_ONE);
    }
    if(test != 0x00)
        PORTC |= _BV(PC0);*/



    status = TwReadByte(MAGNETOMETER_ADDR, MAG_CRA_REG, &test);
    if(status != TW_SUCCESS) {
        sprintf(temp_str, "read error: %x", status);
        LcdWriteString(temp_str, LCD_LINE_TWO);
    }

    //Display "Waiting on data" on line 2
    if(test == 0x10) 
        LcdWriteString(WAITING, LCD_LINE_TWO);
    else {
        sprintf(temp_str, "MAG_CRA_REG: %x", test);
        LcdWriteString(temp_str, LCD_LINE_TWO);
    }
}

/* It's the main function for the program, what more do you need to know? */
int main() {
    int16_t  Degrees = 0;
    uint8_t  SregSave;
    char degree_str[16];
    //int16_t  Correction = 0;
    uint8_t test, test2;
	
	/* Disable interrupts during setup */
	cli();

    Degrees = Degrees; //Because it's a warning otherwise, and I don't think I can get rid of it
    DataReady = FALSE;

    DDRC = (uint8_t)(-1);
    //PORTC |= _BV(PC0);

    /* Setup the ATmega328p */
    InitDevice();

    //Debug
    PORTC &= ~(_BV(PC0));
    PORTC &= ~(_BV(PC1));

    /* Enable interrupts */
    sei();

    /* Wait till we get data */
    //while(!DataReady);
    //LcdWriteString("Degrees:", LCD_LINE_ONE);
    //PORTC |= _BV(PC1);


    SendByte(CLEAR_DISPLAY, TRUE);

    /* Main loop */
    while(1) {
        /* If there is pending data, process it */
        /*if(DataReady) {
            SregSave = SREG; // Preserve the status register //
            // We want the data retrevial completely performed without interruptions //
            cli();
            //Hand off to a function

            Degrees = ProcessData();

            DataReady = FALSE;

            // Restore the status register //
            SREG = SregSave;
        }*/

        // Polling time
        PORTC &= ~(_BV(PC0));
        PORTC &= ~(_BV(PC1));
        TwReadByte(MAGNETOMETER_ADDR, SR_REG_M, &test);
        if(test & SR_REG_M_DRDY) {
            Degrees = ProcessData();
            PORTC |= _BV(PC0);
            PORTC |= _BV(PC1);
        } else if(test & SR_REG_M_LOCK)
            PORTC |= _BV(PC0);
        else
            PORTC |= _BV(PC1);
        //LcdWriteString("Captured data   ", LCD_LINE_ONE);

        //Currently not used, add in later once supplies are received
        /* Check to see if the calibration circuit is active, and if so, adjust
         * change the mode to calibration mode
         */
        //if(bit_is_clear(PORTD, PD4)) {
            /* Check to so see if there is a low signal from the calibration circuit */
            //Correction = Calibrate(Correction, Degrees);
        //} else {
            /* If we aren't calibrating, display the direction and such */
            //LcdWriteString(HeadingString(Degrees), LCD_LINE_ONE);
        //}
        //Degrees += Correction;
        /*TwReadByte(MAGNETOMETER_ADDR, MAG_X_REG_L, &test);
        TwReadByte(MAGNETOMETER_ADDR, MAG_X_REG_H, &test2);
        sprintf(degree_str, "MAG_X hex %x%x", test2, test);
        LcdWriteString(degree_str, LCD_LINE_ONE);

        Degrees = (test2 << 8) | test;
        sprintf(degree_str, "MAG_X deg %d", Degrees);
        LcdWriteString(degree_str, LCD_LINE_TWO);*/

        //sprintf(degree_str, "Degrees: %3d�", Degrees);
        //LcdWriteString(degree_str, LCD_LINE_TWO);
    }
    /* If this is ever called, I don't even know anymore */
    return 0;
}

/* Convert the degrees into a string
 *
 * Parameters:
 *     Degrees - the degree value of the heading vector
 *
 * Returns: The string representation
 */
char* HeadingString(int16_t Degrees) {
// 338 <= N  <=  22
//  23 <= NE <=  67
//  68 <= E  <= 112
// 113 <= SE <= 157
// 158 <= S  <= 202
// 203 <= SW <= 247
// 248 <= W  <= 292
// 293 <= NW <= 337
    if((338 <= Degrees) || (Degrees <= 22)) {
        return NORTH_S;
    } else if((23 <= Degrees) && (Degrees <= 67)) {
        return NE_S;
    } else if((68 <= Degrees) && (Degrees <= 112)) {
        return EAST_S;
    } else if((113 <= Degrees) && (Degrees <= 157)) {
        return SE_S;
    } else if((158 <= Degrees) && (Degrees <= 202)) {
        return SOUTH_S;
    } else if((203 <= Degrees) && (Degrees <= 247)) {
        return SW_S;
    } else if((248 <= Degrees) && (Degrees <= 292)) {
        return WEST_S;
    } else if((293 <= Degrees) && (Degrees <= 337)) {
        return NW_S;
    } else {
        return "ERROR";
    }
}

/* Calculate the heading based on the data passed from the magnetometer
 *
 * Parameters:
 *     X - The X value of the heading vector
 *     Y - The Y value of the heading vector
 *
 * Returns: The number of degrees from the X axis the vector is from
 */
int16_t CalculateDegHeading(int16_t X, int16_t Y) {
    double TempResult;
    //May not needed due to how atan is handled
    /*if(X == 0 && Y >= 0)
        return 0;
    else if (X == 0 && Y < 0)
        return 180;*/

    TempResult = atan2(Y, X);

    /* Convert the result to degrees */
    TempResult = -TempResult * RAD_TO_DEG;

    /* Correct the result */
    if(TempResult >= 360)
        TempResult -= 360; /* Adjust, shouldn't be an issue */
    else if(TempResult < 0)
        TempResult += 360; /* Convert to a positive degree */
    return floor(TempResult);
}

//Currently not implemented
/* Return the calibration value */
//int16_t Calibrate(int16_t CalibrationOffset, int16_t Degrees) {
    //int16_t RawValue = 0;

    //The following should get moved to its own file!
    //ADCSRA |= (_BV(ADEN) | _BV(ADIE)); /* Enable the ADC */ //May need to be offset
    //ADCSRA |= _BV(ADSC); /* Start the ADC conversion */

    //loop_until_bit_is_set(ADCSRA, ADIF); /* Wait for the ADC to finish */

    //RawValue = (ADCH << 8) | (ADCL); /* Read the value off the ADC */

    //Print out RawValue to LCD
    /* A result of 0x100 refers to the pot pointing straight up and down */ //TODO VERIFY THIS
    //RawValue -= 0x100;
    //5k (middle) = offset of 0
    //lower line is the offset
    //return CalibrationOffset;
//}

/* Process the data from the magnetometer
 *
 * Parameters: None
 *
 * Returns: The heading in degrees
 */
//Currently uses only the x and y data, but see if it's possible to use the z
//value to correct for it being held at an angle
int16_t ProcessData() {
    int16_t  MagX = 0;
    int16_t  MagY = 0;
    int16_t  heading;
    //int16_t  MagZ = 0; //may not need
    uint8_t  Buffer[6];
    uint8_t x_low, x_high, y_low, y_high;
    char temp_str[17];

    //MagZ = MagZ; //Warning killer hack YOU BETTER REMOVE THIS LATER ON FOOLE

    //read I2C data
    // Might have to shift right 4
    //TwReadMultiple(MAGNETOMETER_ADDR, MAG_X_REG_L, Buffer, 2);
    //MagX = (int16_t)((uint16_t)Buffer[0] | ((uint16_t)Buffer[1] << 8)) >> 4;
    TwReadByte(MAGNETOMETER_ADDR, MAG_X_REG_L, &x_low);
    TwReadByte(MAGNETOMETER_ADDR, MAG_X_REG_H, &x_high);
    MagX = (int16_t)((uint16_t)x_low | ((uint16_t)x_high << 8)) >> 4;

    sprintf(temp_str, "%x,%x,%d", x_low, x_high, MagX);
    LcdWriteString(temp_str, LCD_LINE_ONE);

    //TwReadMultiple(MAGNETOMETER_ADDR, MAG_Y_REG_L, Buffer, 2);
    //MagY = (Buffer[0] | (Buffer[1] << 8));
    TwReadByte(MAGNETOMETER_ADDR, MAG_Y_REG_L, &y_low);
    TwReadByte(MAGNETOMETER_ADDR, MAG_Y_REG_H, &y_high);
    MagY = (int16_t)((uint16_t)y_low | ((uint16_t)y_high << 8)) >> 4;

    heading = CalculateDegHeading(MagX, MagY);

    //sprintf(temp_str, "%x,%x,%d", Buffer[0], Buffer[1], MagY);
    sprintf(temp_str, "%x,%x,%d,%d", y_low, y_high, MagY, heading);
    LcdWriteString(temp_str, LCD_LINE_TWO);

    //calculate heading
    return CalculateDegHeading(MagX, MagY);
}
