#include "VeDirect.h"
#include <string.h>
#include <ctype.h>

HardwareSerial VeSerial(VE_SERIALPORT);   /* serieport */

VeDirect::VeDirect()
{
    //ctor
    VeSerial.begin(VE_BAUDRATE, SERIAL_8N1, RXD_VE, TXD_VE);
    startpos_variable=0;
    textindex=0;
    checksum = 0;
}

VeDirect::~VeDirect()
{
    //dtor
}

/* Kopiera keyword som text */
bool VeDirect::IfMatchGetString(const char *testword, char *text, long rx_bit)
{
    if (CompareKeyword(testword))
    {
        GetData(text);
        {
         rx_bits |= rx_bit; /* Markera som mottagen */
         return true; /* Keyword hittat */
       }
    }
    return false; /* Keyword hittatdes inte */
}

/* Kopiera keyword som variabel OBS: den sätter inte variabel om indata inte är ett tal, men returnerar ändå sant */
bool VeDirect::IfMatchGetVariable(const char *testword, long *variable, long rx_bit)
{
    if (CompareKeyword(testword))
    {
        if (GetDataIsNumber()) /* sätt om det är en siffra */
        {
            *variable = GetDataAsLong();
            rx_bits |= rx_bit; /* Markera som mottagen */
        }
        return true; /* Keyword hittat */
    }
    return false; /* Keyword hittatdes inte */
}


/* Kopiera keyword som text */
void VeDirect::GetKeyword(char *text)
{
    if (text)
        strcpy(text, utdata);
}

/* Returnera keyword som text */
char* VeDirect::GetKeyword(void)
{
    return utdata;
}

/* Kopiera data som text */
void VeDirect::GetData(char *text)
{
    if (text)
        strcpy(text, utdata+startpos_variable);
}

/* Returnera data som text */
char* VeDirect::GetData(void)
{
    return utdata+startpos_variable;
}

/* Testa om keyword matchar: returnera sant i så fall */
bool VeDirect::CompareKeyword(const char *testword)
{
    return strcmp (utdata,testword)==0;
}

/* Testa om variabel är siffra: sant om variabel, falskt om text */
bool VeDirect::GetDataIsNumber(void)
{
    int i=startpos_variable;
    if (utdata[i]=='-') /* Räkna in minus */
        i++;
    for (; utdata[i]; i++)
    {
        if (isdigit((int)utdata[i])==0)
            return false;
    }
    return true;
}

/* Hämta data som variabel */
long VeDirect::GetDataAsLong(void)
{
    return strtol(utdata+startpos_variable, NULL, 10);
}

/* Init: från början */
void VeDirect::Restart(void)
{
    startpos_variable=0;
    textindex=0;
    checksum = 0;
}

/* Läs in från serieporten, ta med allt som är i bufferten */
void VeDirect::GetItems(void (*thisitem)(void), void (*ChecksumOK)(bool))
{
    while (VeSerial.available()>0)
    {
        GetItem(VeSerial.read(), thisitem, ChecksumOK);
    }
}

/* Intern funktion: testa om det är checkum */
//void VeDirect::GetItem(char indata, void (*thisitem)(char*, char*), void (*ChecksumOK)(bool))
bool VeDirect::ChkSum(void)
{
    if ((textindex==9) && (startpos_variable==9))/* om längd ordet Checksum + tab */
    {
        if (!strcmp (utdata,"Checksum")) /* testa om ordet checksum */
            return true;
    }
    return false;
}

/* Läs in ett tecken */
void VeDirect::GetItem(char indata, void (*thisitem)(void), void (*ChecksumOK)(bool))
{
    checksum += indata;
    if (ChkSum()) /* Testa om det är checksum */
    {
        if (ChecksumOK)
            ChecksumOK(checksum==0); /* Callback: meddela status på checksum */
        checksum = 0;  /* nollstäl checksum för nästa omgång */
        textindex = 0; /* börja om */
    }
    else if (indata == NL2) /* Före meddelande */
    {
        textindex = 0;
    }
    else if (indata == NL1) /* Efter meddelande */
    {
        utdata[textindex++] = 0; /* Lägg till 0 på slutet */
        if (thisitem)
            thisitem(); /* Callback: meddela när vi fått in ny data */
    }
    else /* Spara data från serieport */
    {
        /* Skriv till buffer */
        if (textindex<(SIZE_UTDATA))
        {
            if (indata==NXTDATA) /* Om "tab" (som delar av keyword och data/variabel ) */
            {
                utdata[textindex++] = 0; /* ändra tab -> 0 */
                startpos_variable = textindex; /* spara position: data-index börjar här */
            }
            else
                utdata[textindex++] = indata; /* spara en byte från serieport */
        }
        else
            utdata[SIZE_UTDATA]=0; /* end of buffer: om vi föreöker lagra mer än det finns plats för */
    }
}
