/*
 * i2c_refractor.c
 *
 *  Created on: 26/06/2019
 *      Author: hectorcruz
 */


#include "i2c_refractor.h"
#include "Board.h"
#include <ti/display/Display.h>

//extern Display_Handle display;

I2C_Handle i2c_create (I2C_Params *_i2cParams, I2C_BitRate _bitRate){

    I2C_Params_init(_i2cParams);
    _i2cParams ->bitRate    = _bitRate;
    I2C_Handle _i2c         = I2C_open(Board_I2C0,_i2cParams);
    if (_i2c == NULL) {
        //Display_printf(display, 0, 0, "Error Initializing I2C\n");
        while (1);
    }
    else {
        //Display_printf(display, 0, 0, "I2C Initialized!\n");
    }

    return _i2c;
}

void i2c_refractor(I2C_Transaction *_i2cTransaction,
                     uint_least8_t slave_address, uint8_t *writeBuf,
                     uint8_t *readBuf){

      _i2cTransaction->slaveAddress =   slave_address;
      _i2cTransaction->writeBuf     =   writeBuf;
      _i2cTransaction->writeCount   =   sizeof(writeBuf);
      _i2cTransaction->readBuf      =   readBuf;
      _i2cTransaction->readCount    =   sizeof(readBuf);

    //return I2C_transfer((void *)_i2c,_i2cTransaction);
    //
}
