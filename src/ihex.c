/**
 *****************************************************************************
 * @file ihex.c
 * @brief C-Bibliothek um Intel Hex-Files zu manipulieren
 * @author Roman Buchert (roman.buchert@googlemail.com)
 *****************************************************************************/
/*****************************************************************************/

/*
 *****************************************************************************
 * INCLUDE-Dateien
 *****************************************************************************/
#include <ihex.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
/*****************************************************************************/

/**
 *****************************************************************************
 * @brief Wandelt einen Puffer mit Binärdaten in HEX-Records
 * @param inBuf Zeiger auf Puffer mit Binärdaten
 * @param inBufSize Größe des Puffers mit Binärdaten
 * @param outBuf Zeiger auf einen Puffer der die Hex-Records beinhaltet
 * @param outBufSize Größe des Puffers mit den Hex-Records
 * @return 0: Alles o.k. \n
 * 		   -ENOMEM		: konnte kein Speicher für outBuf allokieren.
 *****************************************************************************/
__s16 ihexBin2Ihex(const __s8* inBuf, __u32 inBufSize,
					__s8* outBuf, __u32 outBufSize)
{
	__u32 AnzahlRecords;

	//Berechne wieviele Records erstellt werden müssen
	AnzahlRecords = (inBufSize / 255) + 1;
	if ((inBufSize % 255) > 0)
		AnzahlRecords++;

	//Allokiere Speicher für outBuf
	outBuf = calloc(AnzahlRecords, (sizeof(THexRecord)));
	if (outBuf == NULL)
		return (-ENOMEM);

	//Erstelle HEX-Records

	return (0);
}
/*****************************************************************************/

/**
 *****************************************************************************
 * @brief Erstellt aus einer Hex-Record-Struktur einen Hex-String
 * @param record Datensatz mit dem Hex-Record
 * @param **string Zeiger auf String mit dem Hex-Record
 * @return 	0: Alles o.k.\n
 * 		   -ENOMEM		: konnte kein Speicher für Strings allokieren.
 *****************************************************************************/
__s16 ihexRecord2String(THexRecord record, __s8 **string)
{
	__s8 *tmpString;
	__u8 Cntr;
	//Speicher für String allokieren
	tmpString = calloc(14 + (record.RecLen<<1), sizeof(__u8));
	if (tmpString == 0)
		return (-ENOMEM);

	sprintf((char*)tmpString,":%2.2X%4.4X%2.2X",
			 (__u8)record.RecLen, (__u16)record.LoadOffset, (__u8)record.RecTyp);
	//Daten hinzufügen
	for(Cntr = 0; Cntr < record.RecLen; Cntr++)
	{
		sprintf((char*)(&tmpString[strlen((char*)tmpString)]),"%2.2X",
				(__u8)record.Data[Cntr]);
	}
	//CRC und Endekennung hinzufügen
	sprintf((char*)(&tmpString[strlen((char*)tmpString)]),"%2.2X\r\n",(__u8)record.ChkSum);
	*string = tmpString;
	return (0);
}
/*****************************************************************************/

/**
 *****************************************************************************
 * @brief Erstellt aus einem Hex-Record-String eine Hex-Record-Struktur
 * @param *string String mit dem Hex-Record
 * @param *record Zeiger auf Datensatz mit dem Hex-Record
 * @return 	0: Alles o.k.\n
 *****************************************************************************/
__s16 ihexString2Record(__s8* string, THexRecord *record)
{
	__u8 tmpString[5];
	__u8 Cntr;

	//Recordmark setzen
	record->RecordMark=(__u8) string[0];
	//Datenlänge setzen
	memset(tmpString, 0 ,sizeof(tmpString));
	strncpy((char*)tmpString, (char*) &string[1], 2);
	record->RecLen = (__u8) strtoul((char*)tmpString, NULL, 16);
	//Load Offset setzen
	memset(tmpString, 0 ,sizeof(tmpString));
	strncpy((char*)tmpString, (char*)&string[3],4);
	record->LoadOffset = (__u16) strtoul((char*)tmpString, NULL, 16);
	//Recordtype setzen
	memset(tmpString, 0 ,sizeof(tmpString));
	strncpy((char*)tmpString, (char*)&string[7],2);
	record->RecTyp = (__u8) strtoul((char*)tmpString, NULL, 16);
	//Daten füllen
	for (Cntr = 0; Cntr < record->RecLen; Cntr++)
	{
		memset(tmpString, 0 ,sizeof(tmpString));
		strncpy((char*)tmpString, (char*)&string[9+(Cntr<<1)], 2);
		record->Data[Cntr] = (__u8) strtoul((char*)tmpString, NULL, 16);
	}
	//Checksumme setzen
	memset(tmpString, 0 ,sizeof(tmpString));
	strncpy((char*)tmpString, (char*)&string[9+(Cntr<<1)],2);
	record->ChkSum = (__u8) strtoul((char*)tmpString, NULL, 16);

	return (0);
}
/*****************************************************************************/

/**
 *****************************************************************************
 * @brief Berechnet die Checksumme des HEX-Record
 * @param *record Zeiger auf Struktur des Hexrecord
 * @return 0: Alles o.k.
 *****************************************************************************/
__s16 ihexCalcChkSum(THexRecord *record)
{

	return (0);
}
/*****************************************************************************/

/**
 *****************************************************************************
 * @brief
 * @param
 * @return
 *****************************************************************************/
/*****************************************************************************/

