#ifndef VEDIRECT_H
#define VEDIRECT_H

#include <HardwareSerial.h>

#define VE_SERIALPORT  1   /* Vilken serieport som skall användas */
#define VE_BAUDRATE    19200 /* Baudrate */
#define RXD_VE         18  /* Port nummer för att ta emot data på serieporten */
#define TXD_VE         17  /* Port nummer för att skicka data på serieporten */
#define SIZE_UTDATA    25  /* Buffer indata (keyword+data) */
#define NL1            0xd /* Ny rad 1: används som avslut utom för checksum */
#define NL2            0xa /* Ny rad 2: används som inledning, keyword kommer efter denna */
#define NXTDATA        0x9 /* Next data: tab, data kommer efter denna */

class VeDirect
{
    public:
        VeDirect();
        virtual ~VeDirect();
        bool IfMatchGetString(const char *testword, char *text, long rx_bit = 0);
        bool IfMatchGetVariable(const char *testword, long *variable, long rx_bit = 0);
        void GetKeyword(char *text);
        char* GetKeyword(void);
        void GetData(char *text);
        char* GetData(void);
        bool CompareKeyword(const char *testword);
        bool GetDataIsNumber(void);
        long GetDataAsLong(void);
        void Restart(void);
        void GetItems(void (*ThisItem)(void), void (*ChecksumOK)(bool));
        void GetItem(char x, void (*ThisItem)(void), void (*ChecksumOK)(bool));        
        long rx_bits;                  /* För att kontrollera om data är mottagen: sätt en bit för varje mottagen data */


    protected:
        char utdata[SIZE_UTDATA+1];           /* buffer för både keyword och data */

    private:
        bool ChkSum(void);
        char checksum;                      /* adderas medans man läser in data */
        int  textindex;                     /* räkne-index för inläst data */
        int  startpos_variable;             /* Var i bufferten data börjar (efter tab/0x9) */
};

#endif // VEDIRECT_H
