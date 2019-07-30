/*
 *    ======== tcpEcho.c ========
 *    Contains BSD sockets code.
 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
/* BSD support */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <ti/net/slnetutils.h>

#include <ti/display/Display.h>
#include <ti/drivers/I2C.h>


#define BYTES_FROM_PIC      (10)
#define NUMBER_OF_PICS      (3)
#define TCPPACKETSIZE_RCVD  (4)
/* The '+1' is to have one more space for the Board ID byte */
//#define TCPPACKETSIZE_SEND  ((NUMBER_OF_PICS * BYTES_FROM_PIC) + 1)
#define TCPPACKETSIZE_SEND  (31)

#define NUMTCPWORKERS       3

#define CAR_NUMBER          3
#define LEVEL               10
#define BOARD_ID         ((CAR_NUMBER << 4) | LEVEL)

/* Example/Board Header files */
#include "Board.h"

#include "i2c_refractor.h"

#define TMP006_DIE_TEMP     (0x0001)  /* TMP006 Die Temp Result Register */

#define TMP006_ADDR         (0x41)    /* TMP006 I2C Slave Address */

//#define Pic1_ADDR           (0x1B)    /*Address of PIC 1*/
//#define Pic2_ADDR           (0x1C)    /*Address of PIC 2*/
//#define Pic3_ADDR           (0x1D)    /*Address of PIC 3*/

#define Pic1_ADDR           0x20
#define Pic2_ADDR           0x35
#define Pic3_ADDR           0x48


extern Display_Handle display;

extern void *TaskCreate(void (*pFun)(), char *Name, int Priority,
        uint32_t StackSize, uintptr_t Arg1, uintptr_t Arg2, uintptr_t Arg3);

/*
 *  ======== tcpWorker ========
 *  Task to handle TCP connection. Can be multiple Tasks running
 *  this function.
 */
int bytesRcvd;
void tcpWorker(uint32_t arg0, uint32_t arg1)
{
    int  clientfd = (int)arg0;

    int  bytesSent;
    char buffer_rcvd[TCPPACKETSIZE_RCVD];
    char buffer_send[TCPPACKETSIZE_SEND];
    char error_buffer[BYTES_FROM_PIC]  = {2,2,2,2,2,2,2,2,2,2};


    I2C_Handle      i2c;
    I2C_Params      i2cParams;

    I2C_Transaction i2cTransaction1;
    I2C_Transaction i2cTransaction2;
    I2C_Transaction i2cTransaction3;

    uint8_t         txBuffer1[1]  = {0};
    uint8_t         txBuffer2[1]  = {0};
    uint8_t         txBuffer3[1]  = {0};
    uint8_t         rxBuffer1[BYTES_FROM_PIC];// ={0,0,0,0};
    uint8_t         rxBuffer2[BYTES_FROM_PIC];// = {0,0,0,0};
    uint8_t         rxBuffer3[BYTES_FROM_PIC];// = {0,0,0,0};
    rxBuffer1[0] = 0;
    rxBuffer2[0] = 0;
    rxBuffer3[0] = 0;

    buffer_send[0]  = BOARD_ID;
    /* Call I2C driver init function */
    I2C_init();

    /* Create I2C for usage */
    i2c = i2c_create(&i2cParams,I2C_400kHz);

    Display_printf(display, 0, 0, "tcp2Worker: start clientfd = 0x%x\n",
            clientfd);

/*
    i2c_refractor(&i2cTransaction1, Pic1_ADDR, txBuffer1, rxBuffer1);
    i2c_refractor(&i2cTransaction2, Pic2_ADDR, txBuffer2, rxBuffer2);
    i2c_refractor(&i2cTransaction3, Pic3_ADDR, txBuffer3, rxBuffer3);*/


    i2cTransaction1.slaveAddress = Pic1_ADDR;
    i2cTransaction1.writeBuf = txBuffer1;
    i2cTransaction1.writeCount = 1;
    i2cTransaction1.readBuf = rxBuffer1;
    i2cTransaction1.readCount = sizeof(rxBuffer1);


    i2cTransaction2.slaveAddress = Pic2_ADDR;
    i2cTransaction2.writeBuf = txBuffer2;
    i2cTransaction2.writeCount = 1;
    i2cTransaction2.readBuf = rxBuffer2;
    i2cTransaction2.readCount = sizeof(rxBuffer2);


    i2cTransaction3.slaveAddress = Pic3_ADDR;
    i2cTransaction3.writeBuf = txBuffer3;
    i2cTransaction3.writeCount = 1;
    i2cTransaction3.readBuf = rxBuffer3;
    i2cTransaction3.readCount = sizeof(rxBuffer3);


    while ((bytesRcvd = recv(clientfd, buffer_rcvd, TCPPACKETSIZE_RCVD, 0)) > 0) {
        if(buffer_rcvd[0] == 'T' && buffer_rcvd[3] == 'p'){
            Display_printf(display, 0, 0, "Temp request \n\r");
            /*Pic 1 Communication try*/
            if (I2C_transfer(i2c, &i2cTransaction1)) {

                memcpy(buffer_send +1, rxBuffer1, sizeof(rxBuffer1));
                /*Display_printf(display, 0, 0, "1PIC 1 Succes! rxBuffer1 1 and final values = %u , %u  \n\r",
                               rxBuffer1[0],rxBuffer1[9]);*/

            }
            else {
                memcpy(buffer_send +1, error_buffer, sizeof(error_buffer));
                Display_printf(display, 0, 0, "PIC 1 Failed!\n\r");
            }
            /*Pic 2 Communication try*/
            if (I2C_transfer(i2c, &i2cTransaction2)) {

                memcpy(buffer_send +1 +sizeof(rxBuffer1),
                       rxBuffer2, sizeof(rxBuffer2));
                /*Display_printf(display, 0, 0, "2PIC 2 Succes! rxBuffer2 1 and final values = %u , %u  \n\r",
                               rxBuffer2[0],rxBuffer2[9]);*/

            }
            else {
                memcpy(buffer_send +1 +sizeof(rxBuffer1),
                       error_buffer, sizeof(error_buffer));
                Display_printf(display, 0, 0, "PIC 2 Failed!\n\r");
            }
            /*Pic 3 Communication try*/
            if (I2C_transfer(i2c, &i2cTransaction3)) {

                memcpy(buffer_send +1 +sizeof(rxBuffer1)+sizeof(rxBuffer2),
                       rxBuffer3, sizeof(rxBuffer3));
                /*Display_printf(display, 0, 0, "3PIC 3 Succes! rxBuffer3 1 and final values = %u , %u  \n\r",
                               rxBuffer3[0],rxBuffer3[9]);*/
            }
            else {
                memcpy(buffer_send +1 +sizeof(rxBuffer1)+sizeof(rxBuffer2),
                       error_buffer, sizeof(error_buffer));

                Display_printf(display, 0, 0, "PIC 3 Failed!\n\r");
            }
            //Display_printf(display, 0, 0, "buffer_send first and final value : %u , %u \n\r",buffer_send[0],buffer_send[30]);
            bytesSent = send(clientfd, buffer_send, TCPPACKETSIZE_SEND, 0);
            if (bytesSent < 0 || bytesSent != TCPPACKETSIZE_SEND) {
                Display_printf(display, 0, 0, "Send failed!.\n");
                break;
            }

        }

    }
    Display_printf(display, 0, 0, "tcpWorker stop clientfd = 0x%x -- I2C closed! \n ", clientfd);

    /* Deinitialized I2C */
    I2C_close(i2c);
    close(clientfd);
}

/*
 *  ======== tcpHandler ========
 *  Creates new Task to handle new TCP connections.
 */
void tcpHandler(uint32_t arg0, uint32_t arg1)
{
    void *thread = NULL;
    int                status;
    int                clientfd;
    int                server;
    struct sockaddr_in localAddr;
    struct sockaddr_in clientAddr;
    int                optval;
    int                optlen = sizeof(optval);
    socklen_t          addrlen = sizeof(clientAddr);

    Display_printf(display, 0, 0, "TCP MCU Communication 2.0 example started\n");

    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1) {
        Display_printf(display, 0, 0, "tcpHandler: socket failed\n");
        goto shutdown;
    }


    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(arg0);

    status = bind(server, (struct sockaddr *)&localAddr, sizeof(localAddr));
    if (status == -1) {
        Display_printf(display, 0, 0, "tcpHandler: bind failed\n");
        goto shutdown;
    }

    status = listen(server, NUMTCPWORKERS);
    if (status == -1) {
        Display_printf(display, 0, 0, "tcpHandler: listen failed\n");
        goto shutdown;
    }

    optval = 1;
    status = setsockopt(server, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);
    if (status == -1) {
        Display_printf(display, 0, 0, "tcpHandler: setsockopt failed\n");
        goto shutdown;
    }

    while ((clientfd =
            accept(server, (struct sockaddr *)&clientAddr, &addrlen)) != -1) {

        Display_printf(display, 0, 0,
                "tcpHandler: Creating thread clientfd = %x\n", clientfd);

        thread = TaskCreate(tcpWorker, NULL, 3, 2048, (uintptr_t)clientfd,
            0, 0);

        if (!thread) {
            Display_printf(display, 0, 0,
                    "tcpHandler: Error - Failed to create new thread.\n");
            close(clientfd);
        }

        /* addrlen is a value-result param, must reset for next accept call */
        addrlen = sizeof(clientAddr);
    }

    Display_printf(display, 0, 0, "tcpHandler: accept failed.\n");

shutdown:
    if (server != -1) {
        close(server);
    }
}
