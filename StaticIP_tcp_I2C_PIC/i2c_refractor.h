/*
 * i2c_refractor.h
 *
 *  Created on: 26/06/2019
 *      Author: hectorcruz
 */

#ifndef I2C_REFRACTOR_H_
#define I2C_REFRACTOR_H_

#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <ti/sysbios/BIOS.h>
#include <ti/drivers/I2C.h>
#include <ti/display/Display.h>


I2C_Handle i2c_create (I2C_Params *_i2cParams, I2C_BitRate _bitRate);

void i2c_refractor(I2C_Transaction *_i2cTransaction,
                     uint_least8_t slave_address, uint8_t *writeBuf,
                     uint8_t *readBuf);



#endif /* I2C_REFRACTOR_H_ */
