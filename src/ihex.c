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
#include <arpa/inet.h>
/*****************************************************************************/

/**
 *****************************************************************************
 * @brief Wandelt einen Puffer mit Binärdaten in HEX-Records
 * @param *inBuf Zeiger auf Puffer mit Binärdaten
 * @param inBufSize Größe des Puffers mit Binärdaten
 * @param DataLen max. Länge der Daten.
 * @param **outBuf Zeiger auf einen Puffer der die Hex-Records beinhaltet
 * @param *outBufSize Größe des Puffers mit den Hex-Records
 * @return 0: Alles o.k. \n
 * 		   -ENOMEM		: konnte kein Speicher für outBuf allokieren.
 *****************************************************************************/
__s16 ihexBin2Ihex(const __s8 *inBuf, __u32 inBufSize, __u8 DataLen,
					__s8 **outBuf, __u32 *outBufSize)
{
	__u32 Cntr;
	THexRecord Record, AdrRecord;
	__s8 *RecordString = NULL;
	__u16 *AdrPtr = NULL;
	__s8 *tmpBuf = NULL;
	__s16 RetVal = 0;
	__u16 LoadAdresse;
	__u16 AddrCntr = 0;
	*outBufSize = 0;

	//Erstelle HEX-Records
	for (Cntr = 0; Cntr < inBufSize; Cntr++)
	{
		LoadAdresse = Cntr & 0xFFFF;
		//Erstelle den Segemt Adress Record
		if (((LoadAdresse) == 0))
		{
			AdrRecord.RecordMark = ':';
			AdrRecord.RecLen = 0x02;
			AdrRecord.LoadOffset = 0x0000;
			AdrRecord.RecTyp = rtXLA;
			AdrPtr = (__u16*) &AdrRecord.Data[0];
			*AdrPtr = (__u16) htons((Cntr >> 16));
			ihexCalcChksum(&AdrRecord);
			RecordString = NULL;
			if (ihexRecord2String(AdrRecord, &RecordString) != 0)
			{
				RetVal = (-ENOMEM);
				goto exitBin2Ihex;
			}
			if (tmpBuf == NULL)
			{

				if ((tmpBuf = malloc(1 + strlen((char*) RecordString))) == NULL)
				{
					RetVal = (-ENOMEM);
					goto exitBin2Ihex;
				}
				strcpy((char*) tmpBuf, (char*) RecordString);
			}
			else
			{

				if ((tmpBuf = realloc(tmpBuf,1 + strlen((char*) tmpBuf) + strlen((char*) RecordString))) == NULL)
				{
					RetVal = (-ENOMEM);
					goto exitBin2Ihex;
				}
				strcat((char*) tmpBuf, (char*) RecordString);
			}
			free(RecordString);
			RecordString = NULL;
			Record.LoadOffset = 0;
		}

		//Fülle Daten
		Record.Data[AddrCntr] = inBuf[Cntr];
		AddrCntr++;

		//Wenn Datensatz voll, speichern
		if (((AddrCntr) == (DataLen)) ||
			/*((Cntr + 1) == inBufSize) ||*/
			((LoadAdresse) == 0xFFFF))
		{

			if (LoadAdresse == 0xFFFF)
			{
				Record.RecordMark = ';';
			}
			Record.RecordMark = ':';

			Record.RecLen = AddrCntr;

			Record.LoadOffset = 1 + LoadAdresse - AddrCntr;

			Record.RecTyp=rtData;

			ihexCalcChksum(&Record);
			RecordString = NULL;
			if (ihexRecord2String(Record, &RecordString) != 0)
			{
				RetVal = (-ENOMEM);
				goto exitBin2Ihex;
			}

			if ((tmpBuf = realloc(tmpBuf, 1 + strlen((char*) tmpBuf) + strlen((char*) RecordString))) == NULL)
			{
				RetVal = (-ENOMEM);
				goto exitBin2Ihex;
			}

			strcat((char*) tmpBuf, (char*) RecordString);

			free(RecordString);
			RecordString = NULL;
			AddrCntr = 0;
		}

	}

	//Enderecord schreiben
	Record.RecordMark = ':';
	Record.RecLen = 0x00;
	Record.LoadOffset = 0x0000;
	Record.RecTyp = rtEOF;
	Record.ChkSum = 0xFF;
	RecordString = NULL;
	if(ihexRecord2String(Record, &RecordString) != 0)
	{
		RetVal = (-ENOMEM);
		goto exitBin2Ihex;
	}


	if ((tmpBuf = realloc(tmpBuf, 1 + strlen((char*) tmpBuf) + strlen((char*) RecordString))) == NULL)
	{
		free (tmpBuf);
		free (RecordString);
		return (-ENOMEM);
	}
	strcat((char*) tmpBuf, (char*) RecordString);

	*outBuf = tmpBuf;
	*outBufSize = strlen((char*) tmpBuf);

exitBin2Ihex:
	if (RetVal != 0)
		free (tmpBuf);

	free (RecordString);

	return (RetVal);
}
/*****************************************************************************/

/**
 *****************************************************************************
 * @brief Wandelt einen Hex-String in einen Binärpuffer um.
 * Es werden nur folgende Datensatztypen erkannt:\n
 * 00:	DATA	: Datenrecord\n
 * 01:	EOF		: Dateiende\n
 * 02:	XSA		: Segmentadresse für folgende Nutzdaten\n
 * 04:	LSA		: Lineare Startadresse für folgende Nutzdaten.
 *
 * @param *inBuf Zeiger mit Hex-String
 * @param **outBuf Zeiger auf einen Puffer für die Binärdaten
 * @param *outBufSize Zeiger auf eine Variable für die Größe der Binärdaten
 * @return 0: Alles o.k. \n
 * 		   -ENOMEM		: konnte kein Speicher für outBuf allokieren.
 *****************************************************************************/
__s16 ihexIhex2Bin(__sc8 *inBuf,
					 __s8 **outBuf,
					 __u32 *outBufSize)
{
	__u32 BufSize;			//Größe des allokierten Speichers
	__u32 AdrOffset;		//Adressoffset
	__u32 LastAddr;
	__s8 *RecordString = NULL;		//Aktuell bearbeiteter Record;
	__s8 *RecordToken = NULL;		//Token ohne ":"
	__s8 *Buffer = NULL;	//interner Buffer um Binärdaten zusammenzusetzen
	__s16 RetVal = 0;
	__u16 *AdrPtr;
	THexRecord Record;

	//Ein Byte allokieren damit später nur mit realloc gearbeitet werden muss
	if((Buffer = malloc(1)) == NULL)
	{
		RetVal = -ENOMEM;
		goto exitIhex2Bin;
	}
	*Buffer = 0xFF;
	BufSize = 1;
	RecordToken = (__s8*) (strtok((char*) inBuf,":"));
	while(RecordToken != NULL)
	{
		__u32 Laenge = strlen((char*) RecordToken);
		RecordString = malloc(Laenge + 5);
		if (RecordString == NULL)
		{
			RetVal = -ENOMEM;
			goto exitIhex2Bin;
		}
		sprintf((char*) RecordString,":%s", (char*) RecordToken);
		ihexString2Record(RecordString, &Record);
		free(RecordString);
		RecordString = NULL;
		if (ihexCheckChksum(Record) != 0)
		{
			RetVal = -EILSEQ;
		}
		//Datensatztyp bearbeiten
		switch(Record.RecTyp)
		{
		case rtData:	//Datenrecord bearbeiten
			LastAddr = AdrOffset + Record.LoadOffset + Record.RecLen;
			if (BufSize < LastAddr)
			{
				Buffer = realloc(Buffer, LastAddr);
				if (Buffer == NULL)
				{
					RetVal = -ENOMEM;
					goto exitIhex2Bin;
				}
				BufSize = LastAddr;
				memcpy(Buffer + (AdrOffset + Record.LoadOffset),
					   &Record.Data[0],
					   Record.RecLen);

			}
			break;

		case rtEOF:		//EOF
			break;

		case rtSLA:		//Segmentladeadresse setzen
			AdrPtr = (__u16*)&Record.Data[0];
			AdrOffset = ntohs(*AdrPtr);
			break;

		case rtSSA:		//Start Segment Adress Record
			break;

		case rtXLA:		//Linear Startadresse setzen
			AdrPtr = (__u16*)&Record.Data[0];
			AdrOffset =(__u32) (ntohs(*AdrPtr)) << 16;
			break;

		case rtXSA:
			break;

		default:
			break;

		}

		RecordToken = (__s8*) strtok((char *)NULL, ":");
	}
	*outBuf = Buffer;
	*outBufSize = BufSize;

exitIhex2Bin:

	//Aufräumen im Fehlerfall
	if (RetVal)
	{
		if (Buffer)
			free(Buffer);
	}

	//Immer Aufräumen
	if(RecordString)
		free(RecordString);
	return (RetVal);
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
__s16 ihexRecord2String(THexRecord record, __s8 **String)
{
	__u8 Cntr;
	__u8 TmpString[10];
	//Speicher für String allokieren
	*String = malloc(20 + (record.RecLen<<1));
	if (*String == 0)
		return (-ENOMEM);
	sprintf((char*)*String, ":%2.2X%4.4X%2.2X",
			 (__u8)record.RecLen, (__u16)record.LoadOffset, (__u8)record.RecTyp);
	//Daten hinzufügen
	for(Cntr = 0; Cntr < record.RecLen; Cntr++)
	{
		sprintf((char*)(TmpString),"%2.2X",
		(__u8)record.Data[Cntr]);
		strcat((char*)*String,(char*)TmpString);
	}

	//CRC und Endekennung hinzufügen
	sprintf((char*)(TmpString),"%2.2X\r\n",(__u8)record.ChkSum);
	strcat((char*)*String,(char*)TmpString);
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
__s16 ihexCalcChksum(THexRecord *record)
{
	__u8 CheckSum = 0;
	__u8 *BytePtr;
	__u16 Cntr;

	BytePtr = (__u8*) record;

	BytePtr++;
	for (Cntr = 0; Cntr < (record->RecLen + 4); Cntr++ )
	{
		CheckSum += *BytePtr;
		BytePtr++;
	}

	record->ChkSum = -CheckSum;
	return (0);
}
/*****************************************************************************/

/**
 *****************************************************************************
 * @brief Prüft die Checksumme des HEX-Record
 * @param record Hexrecord
 * @return 0	: Prüfsumme stimmt. \n
 * 		   !=0	: Prüfsumme ist falsch.
 *****************************************************************************/
__s16 ihexCheckChksum(THexRecord record)
{
	__u8 TmpCheckSum;
	TmpCheckSum = record.ChkSum;

	ihexCalcChksum(&record);
	return (record.ChkSum - TmpCheckSum);
}
/*****************************************************************************/

/**
 *****************************************************************************
 * @brief
 * @param
 * @return
 *****************************************************************************/
/*****************************************************************************/

/**
 *****************************************************************************
 * @brief
 * @param
 * @return
 *****************************************************************************/
/*****************************************************************************/

