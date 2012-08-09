/**
 *****************************************************************************
 * @file ihex.h
 * @brief C-Bibliothek um Intel Hex-Files zu manipulieren
 * @author Roman Buchert (roman.buchert@googlemail.com)
 *****************************************************************************/
#ifndef __IHEX_H__
#define __IHEX_H__
/*****************************************************************************/

/*
 *****************************************************************************
 * INCLUDE-Dateien
 *****************************************************************************/
#include <ihex_types.h>
/*****************************************************************************/

/**
 ******************************************************************************
 * @name RecordType
 * @brief Verschiedene Typen der Hex-Records
 *@{***************************************************************************/
#define rtData (__u8) (0)	///<Nutzdaten
#define rtEOF  (__u8) (1)	///<Dateiende (End of File)
#define rtXSA  (__u8) (2)	///<Segmentadresse für folgende Nutzdaten (Extended Address Segment)
#define rtSSA  (__u8) (3)	///<Segmentierte Startadresse (Start Segment Address)
#define rtXLA  (__u8) (4)	///<Höherwertige 16Bit der Adresse für folgende Nutzdaten (Extended Linear Address)
#define rtSLA  (__u8) (5)	///<Lineare Startadresse (Start Linear  Address)
/**@} *************************************************************************/

/**
 ******************************************************************************
 * @struct THexRecord
 * @brief Struktur mit binärem HexRecord
 ******************************************************************************/
#pragma pack(1)
typedef struct
{
	__u8 RecordMark;	///<Satzbeginn (":")
	__u8 RecLen;		///<Datenlänge (Länge der Nutzdaten)
	__u16 LoadOffset;	///<Ladeadresse (16-Bit-Adresse / Big Endian)
	__u8 RecTyp;		///<Satztyp (Datensatztyp (0..5))
	__u8 Data[255];		///<Nutzdaten (RecLen / max 255 Byte)
	__u8 ChkSum;		///<Prüfsumme (Prüfsumme über Datensatz ohne RecordMark)
}THexRecord;
#pragma pack()
/******************************************************************************/

/**
 *****************************************************************************
 * @brief Wandelt einen Puffer mit Binärdaten in HEX-Records
 * @param inBuf Zeiger auf Puffer mit Binärdaten
 * @param inBufSize Größe des Puffers mit Binärdaten
 * @param DataLen max. Länge der Daten.
 * @param **outBuf Zeiger auf einen Puffer der die Hex-Records beinhaltet
 * @param *outBufSize Größe des Puffers mit den Hex-Records
 * @return 0: Alles o.k. \n
 * 		   -ENOMEM		: konnte kein Speicher für outBuf allokieren.
 *****************************************************************************/
__s16 ihexBin2Ihex(const __s8 *inBuf, __u32 inBufSize, __u8 DataLen,
					__s8 **outBuf, __u32 *outBufSize);
/*****************************************************************************/

/**
 *****************************************************************************
 * @brief Erstellt aus einer Hex-Record-Struktur einen Hex-String
 * @param record Datensatz mit dem Hex-Record
 * @param **string Zeiger auf String mit dem Hex-Record
 * @return 	0: Alles o.k.\n
 * 		   -ENOMEM		: konnte kein Speicher für Strings allokieren.
 *****************************************************************************/
__s16 ihexRecord2String(THexRecord record, __s8 **string);
/*****************************************************************************/

/**
 *****************************************************************************
 * @brief Erstellt aus einem Hex-Record-String eine Hex-Record-Struktur
 * @param *string String mit dem Hex-Record
 * @param *record Zeiger auf Datensatz mit dem Hex-Record
 * @return 	0: Alles o.k.\n
 * 		   -ENOMEM		: konnte kein Speicher für Strings allokieren.
 *****************************************************************************/
__s16 ihexString2Record(__s8* string, THexRecord *record);
/*****************************************************************************/

/**
 *****************************************************************************
 * @brief Berechnet die Checksumme des HEX-Record
 * @param *record Zeiger auf Struktur des Hexrecord
 * @return 0: Alles o.k.
 *****************************************************************************/
__s16 ihexCalcChksum(THexRecord *record);
/*****************************************************************************/

/**
 *****************************************************************************
 * @brief Prüft die Checksumme des HEX-Record
 * @param record Hexrecord
 * @return 0	: Prüfsumme stimmt. \n
 * 		   !=0	: Prüfsumme ist falsch.
 *****************************************************************************/
__s16 ihexCheckChksum(THexRecord record);
/*****************************************************************************/

#endif//__IHEX_H__
