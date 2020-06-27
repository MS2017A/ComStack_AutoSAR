#ifndef _PLATFORM_TYPES_H_
#define _PLATFORM_TYPES_H_



/*****************************************************************************/
/* AUTOSAR Release: 4.3.1                                                      */
/*****************************************************************************/

/*****************************************************************************/
/* GLOBAL DEFINITIONS                                                        */
/*****************************************************************************/
#define CPU_TYPE_8          8
#define CPU_TYPE_16         16
#define CPU_TYPE_32         32

#define MSB_FIRST           0
#define LSB_FIRST           1

#define HIGH_BYTE_FIRST     0
#define LOW_BYTE_FIRST      1

#define TRUE              1

#define FALSE             0

/*****************************************************************************/
/* REGISTER WIDTH                                                            */
/*****************************************************************************/
#define CPU_TYPE            CPU_TYPE_32

/*****************************************************************************/
/* ENDIANESS                                                                 */
/*****************************************************************************/
#define CPU_BIT_ORDER       LSB_FIRST

#if __little_endian__
  #define CPU_BYTE_ORDER    LOW_BYTE_FIRST
#else
  #define CPU_BYTE_ORDER    HIGH_BYTE_FIRST
#endif

#define CPU_BYTE_ORDER    LOW_BYTE_FIRST

/*****************************************************************************/
/* TYPE DEFINITIONS                                                          */
/*****************************************************************************/
typedef unsigned char       boolean;

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long long	uint64;

typedef signed char         sint8;
typedef signed short        sint16;
typedef signed int          sint32;
/**typedef signed long long	uint64;*/

typedef unsigned int        uint8_least;
typedef unsigned int        uint16_least;
typedef unsigned int        uint32_least;

typedef signed int          sint8_least;
typedef signed int          sint16_least;
typedef signed int          sint32_least;

typedef float               float32;
typedef double              float64;


#endif /* _PLATFORM_TYPES_H_ */
