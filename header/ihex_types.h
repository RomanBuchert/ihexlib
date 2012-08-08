/**
 ******************************************************************************
 * @file ihex_types.h
 * @brief Typdefinitionen der IHEX-Lib
 * @author Roman Buchert (roman.buchert@googlemail.com)
 * Hier stehen die Typdefinitionen, die von der IHEX-Bibliothek verwendet
 * werden.
 ******************************************************************************/
#ifndef __IHEX_TYPES_H__
#define __IHEX_TYPES_H__
/******************************************************************************/

/**
 ******************************************************************************
 * @name Typdefinitionen unsigned
 *@{***************************************************************************/
typedef unsigned char   __u8;
typedef unsigned short  __u16;
typedef unsigned long   __u32;
typedef unsigned long long __u64;
/**@}**************************************************************************/

/**
 ******************************************************************************
 * @name Typdefinitionen unsigned, nur lesend
 *@{***************************************************************************/
typedef unsigned char  const __uc8;
typedef unsigned short const __uc16;
typedef unsigned long  const __uc32;
typedef unsigned long long const __uc64;
/**@}**************************************************************************/

/**
 ******************************************************************************
 * @name Typdefinitionen signed
 *@{***************************************************************************/
typedef signed char   __s8;
typedef signed short  __s16;
typedef signed long   __s32;
typedef signed long long __s64;
/**@}**************************************************************************/

/**
 ******************************************************************************
 * @name Typdefinitionen signed, nur lesend
 *@{***************************************************************************/
typedef signed char  const __sc8;
typedef signed short const __sc16;
typedef signed long  const __sc32;
typedef signed long long const __sc64;
/**@}**************************************************************************/

/**
 ******************************************************************************
 * @name Typdefinitonen unsigned volatile
 *@{***************************************************************************/
typedef volatile unsigned char   __vu8;
typedef volatile unsigned short  __vu16;
typedef volatile unsigned long   __vu32;
typedef volatile unsigned long long __vu64;
/**@}**************************************************************************/

/**
 ******************************************************************************
 * @name Typdefinitionen unsigned volatile, nur lesend
 *@{***************************************************************************/
typedef volatile unsigned char  const __vuc8;
typedef volatile unsigned short const __vuc16;
typedef volatile unsigned long  const __vuc32;
typedef volatile unsigned long long const __vuc64;
/**@}**************************************************************************/

/**
 ******************************************************************************
 * @name Typdefinitionen signed volatile
 *@{***************************************************************************/
typedef volatile signed char   __vs8;
typedef volatile signed short  __vs16;
typedef volatile signed long   __vs32;
typedef volatile signed long long __vs64;
/**@}**************************************************************************/

/**
 ******************************************************************************
 * @name Typdefinitionen signed volatile, nur lesend
 *@{***************************************************************************/
typedef volatile signed char  const __vsc8;
typedef volatile signed short const __vsc16;
typedef volatile signed long  const __vsc32;
typedef volatile signed long long const __vsc64;
/**@}**************************************************************************/

/**
 ******************************************************************************
 * @name bool
 * @enum bool
 * @brief Typdefinitionen TRUE / FALSE
 ******************************************************************************/
#ifndef bool
typedef enum {FALSE = 0, TRUE = 1} bool;
#endif
/******************************************************************************/

#endif //__IHEX_TYPES_H__
