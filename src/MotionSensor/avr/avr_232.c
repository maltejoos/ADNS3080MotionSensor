#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <avr/eeprom.h>

#include "ADNS3080.c"


//set desired baud rate
//#define BAUDRATE 115200UL
#define BAUDRATE 57600UL

//calculate UBRR value
#define UBRRVAL ((F_CPU/(BAUDRATE*16UL))-1)

volatile unsigned char id = 0;

volatile unsigned char stop_continuousMotion = 0;
volatile unsigned char stop_integrateMotion = 0;
volatile unsigned char send_integrateMotion = 0;

unsigned char motion_status=0;
signed char delta_x;
signed char delta_y;
unsigned char squal=0;
unsigned char shutter_upper=0;
unsigned char shutter_lower=0;
unsigned char max_pixel=0;

const unsigned char firmware[] PROGMEM =        {0xe6,0xff,0x40,0x22,0x86,0xaa,0x9f,0x73,0xdc,0x34,0x5f,0x1d,0x0d,0x05,0xe2,0xae,
                                                0x5c,0xe7,0x5e,0x67,0x2f,0xd2,0x45,0xaa,0xb7,0x2d,0xf9,0x71,0xc0,0xe2,0x89,0x8f,
                                                0x18,0xc0,0x50,0xb1,0x4c,0x5b,0x1e,0xb3,0x53,0x32,0x56,0x3a,0x14,0xe7,0x64,0x58,
                                                0x41,0x24,0x03,0x3c,0xc5,0x2a,0x9c,0x8c,0x06,0xa7,0xa2,0x1f,0x23,0xf7,0x64,0xe6,
                                                0x1b,0x50,0x97,0x3a,0x8a,0x86,0x16,0x4a,0xf5,0x77,0x5b,0x28,0x5d,0xa6,0xe0,0xcf,
                                                0xc1,0x4a,0x28,0x62,0xfb,0x85,0x09,0x80,0x07,0xce,0x11,0x59,0xd4,0x89,0xcf,0xc1,
                                                0x42,0x1d,0x05,0xea,0xa2,0x47,0x2e,0x0f,0xa1,0x10,0x0c,0xb4,0xe1,0x3f,0x95,0x70,
                                                0x6d,0x4d,0xc0,0xce,0x8b,0x49,0x46,0x6e,0xfd,0x77,0x84,0x7e,0x62,0x0a,0x5d,0xaa,
                                                0xc7,0x5d,0xa7,0x64,0x8d,0xc6,0x17,0x0d,0x44,0x6a,0xdc,0x56,0xae,0xa2,0x35,0xd0,
                                                0xd9,0xdb,0xa7,0x68,0xc1,0xa5,0xcb,0xdb,0xda,0xdf,0xe9,0x65,0xb9,0x55,0x49,0xf0,
                                                0x8f,0xe8,0x6e,0x4b,0xca,0xfc,0xe4,0x14,0x91,0x7a,0xaf,0x59,0x74,0x54,0x11,0x69,
                                                0xf1,0xce,0x80,0xc0,0xd1,0x99,0x40,0x81,0x36,0x20,0x48,0x69,0x92,0xb1,0x44,0xdc,
                                                0x97,0x4f,0x0e,0x95,0xc3,0x27,0x23,0xcf,0x7a,0xcd,0xe8,0x33,0x1b,0x12,0x9f,0xde,
                                                0x2f,0x60,0x04,0x05,0x43,0x80,0x6b,0x0c,0xa1,0x72,0x26,0x36,0x46,0x51,0xbd,0x6c,
                                                0xa2,0x4b,0x93,0x4b,0x3e,0x9b,0x5a,0x60,0xd8,0x23,0xa5,0x37,0x6b,0x04,0x37,0x11,
                                                0xa8,0x32,0xec,0x1b,0x0b,0x06,0x6e,0x59,0xe8,0xf7,0x6c,0x26,0x03,0xb7,0xf2,0xec,
                                                0x97,0x9d,0xc9,0x74,0x65,0xf3,0x90,0x58,0xa7,0x36,0x7b,0x6f,0xe8,0x31,0x05,0xfe,
                                                0x04,0x9a,0xa4,0x73,0xa7,0xcf,0x27,0x78,0x8f,0xdc,0xd3,0xc6,0x14,0x55,0x76,0xa1,
                                                0x24,0xcc,0xb0,0x38,0x53,0x7f,0x7b,0xf8,0x23,0xd7,0x09,0xd7,0xee,0xb8,0x19,0x1d,
                                                0x0a,0x9a,0x70,0xdd,0xd8,0x5e,0x70,0x6f,0xd4,0xe9,0x2c,0x23,0x44,0xe0,0x3e,0xa7,
                                                0x7d,0x31,0xf4,0x79,0xd0,0x40,0xfe,0x8e,0x8f,0x01,0xbc,0x4e,0x7f,0xbd,0x16,0xde,
                                                0x9b,0xd5,0xcb,0xf5,0x18,0xb0,0x2a,0x00,0x31,0x26,0x89,0x4b,0x62,0x14,0x95,0x21,
                                                0x45,0x2a,0x6a,0x99,0x38,0x8e,0xf3,0xad,0x25,0xfd,0xd0,0x1d,0x4a,0x91,0xf6,0xb7,
                                                0x71,0x01,0x5a,0xcd,0xc1,0x99,0x50,0xad,0x77,0x55,0x95,0xfd,0x88,0x0e,0xde,0xeb,
                                                0xb6,0x54,0xe1,0x8d,0xfd,0x6d,0x02,0x3d,0x00,0x15,0x75,0x43,0x88,0x17,0x04,0x15,
                                                0x39,0x95,0xaa,0x25,0x40,0x67,0x91,0x9b,0x7e,0x1d,0x6b,0xf2,0xaa,0x73,0x8f,0xf3,
                                                0x8f,0x75,0x0c,0x55,0x1a,0x41,0x8c,0xab,0xab,0x79,0xf8,0xc8,0x91,0xc2,0x70,0xe0,
                                                0xca,0x2a,0x8b,0xde,0x97,0x1c,0xcc,0xc7,0xa5,0x8f,0x55,0x9f,0x08,0x0d,0xe8,0x89,
                                                0x90,0xf9,0xe2,0xa1,0x87,0xb6,0x25,0xa0,0x5a,0x71,0xc5,0x28,0x6e,0x17,0xef,0xb2,
                                                0x58,0x8c,0xc6,0xe2,0xcf,0x26,0x93,0x29,0x63,0x59,0xbc,0x71,0x8e,0xe0,0x91,0x61,
                                                0x06,0xb0,0x65,0x07,0x51,0x45,0x2e,0xa3,0xa7,0x7c,0x75,0x6b,0x57,0x36,0x70,0xb2,
                                                0xbc,0xe4,0x08,0x6b,0xe2,0x8c,0x44,0x06,0x39,0x11,0xe0,0xd7,0x9c,0x13,0x4b,0x09,
                                                0xdc,0x16,0x9a,0xda,0x20,0x64,0xd5,0x8a,0xe5,0x4b,0x58,0x69,0x6c,0x96,0x05,0x07,
                                                0x03,0xd2,0x2d,0xce,0xbb,0x4c,0x88,0xe2,0x64,0x05,0xa4,0x48,0x41,0xeb,0x15,0xd8,
                                                0x42,0xaa,0xd5,0x83,0x52,0x79,0xcf,0xa5,0x3b,0x54,0x56,0xa2,0xe2,0xcb,0xf3,0xf2,
                                                0xcb,0xf2,0x17,0x02,0x6b,0xff,0x32,0x27,0x4d,0xbb,0xe7,0x65,0x7d,0x88,0x36,0xad,
                                                0x66,0x64,0x8b,0x1a,0x65,0xee,0xa2,0x0c,0xc3,0xde,0xce,0x7e,0xd7,0x4d,0xe2,0xd7,
                                                0x5c,0xc1,0xd6,0xc3,0xe2,0x91,0x31,0x5d,0xaf,0x9c,0x93,0xb8,0x4a,0x83,0x22,0x07,
                                                0x62,0x17,0xe8,0x73,0x06,0x8f,0xff,0x9e,0x24,0xd9,0xf7,0x11,0xdb,0x0f,0xbd,0x5c,
                                                0xda,0x93,0x7f,0x96,0xd1,0x98,0xf8,0x2b,0x8c,0x77,0xfc,0xc3,0xc4,0x97,0x5d,0x74,
                                                0x3a,0xa4,0x42,0xf5,0x97,0xe9,0x81,0x73,0x71,0xf3,0x81,0x94,0x47,0x96,0x91,0x60,
                                                0x6e,0x8f,0x0d,0xf2,0xa4,0x62,0x1e,0x58,0xcf,0x03,0x7c,0x31,0x5b,0xa8,0x24,0x1b,
                                                0xc9,0x31,0x30,0x07,0xdd,0xaa,0x31,0x86,0x55,0x93,0x42,0xec,0x17,0xca,0xdc,0x20,
                                                0x2a,0x60,0x2d,0x4e,0xce,0xae,0xa4,0x66,0x48,0x09,0x7a,0xa5,0x4d,0x4d,0xdd,0xc2,
                                                0xb3,0x85,0x1a,0x6f,0x0c,0x27,0xa8,0x5d,0xf8,0x2e,0xde,0xa3,0xde,0xf4,0xe8,0xaa,
                                                0x64,0xa7,0x69,0x9a,0xf8,0x78,0xba,0x73,0x8a,0xfd,0xee,0x06,0x72,0x5d,0xf2,0xe4,
                                                0x90,0xf7,0xa0,0x46,0xe5,0x06,0x44,0xe3,0xc1,0xee,0x92,0x23,0xc2,0x7c,0xd1,0xd2,
                                                0x31,0xba,0xeb,0x2f,0x16,0x4d,0xe2,0xf0,0xcf,0x98,0x38,0xbc,0x11,0x68,0xd7,0xc2,
                                                0xeb,0x91,0xe6,0x34,0x60,0xd1,0x48,0x95,0x54,0x30,0x49,0xb3,0x30,0x8a,0x5a,0xb2,
                                                0xec,0x14,0x41,0xe9,0xf4,0x84,0x46,0xea,0x31,0x5a,0xbc,0x29,0xb9,0xa1,0xc5,0x2c,
                                                0x07,0x16,0xfd,0x90,0x5f,0x95,0x41,0xdc,0xf2,0xc3,0x6e,0x57,0xf0,0xea,0x0f,0xdc,
                                                0xf0,0x58,0x5c,0x83,0x59,0x8a,0xff,0xe2,0xc9,0x33,0x5a,0xfc,0xde,0x4f,0x42,0x89,
                                                0xbe,0x66,0xfe,0x27,0x93,0x94,0x2a,0xfb,0xca,0xd5,0x78,0x79,0x4a,0x64,0x71,0xb0,
                                                0xb2,0xda,0x84,0x2c,0xd6,0x16,0xf3,0x79,0xfc,0x31,0x89,0x13,0xb8,0x39,0x2b,0x37,
                                                0x5c,0xd3,0x1e,0xb6,0xfa,0x4a,0x79,0x62,0x9c,0x86,0xb6,0x22,0x00,0xcc,0xc6,0x05,
                                                0x8c,0x86,0x98,0xd5,0x61,0x11,0xc2,0x67,0x06,0x8a,0x14,0x34,0xe9,0xd3,0x0a,0xae,
                                                0x45,0x50,0x52,0x23,0x43,0xd5,0x6d,0x1f,0x45,0x7c,0x91,0xe5,0x2b,0x2d,0x2c,0xef,
                                                0xbf,0x70,0x3a,0x0a,0x8c,0x50,0xc1,0xf4,0x21,0x0c,0x1e,0x34,0xc5,0xe3,0xad,0x7c,
                                                0x94,0x87,0x49,0x96,0x58,0x91,0x8c,0xf9,0x26,0xa6,0xb6,0x72,0x0e,0xad,0x36,0x75,
                                                0x38,0x0e,0x84,0x40,0xa1,0x60,0xc2,0xeb,0xd0,0xa8,0xbd,0x13,0x8d,0x3c,0x14,0x87,
                                                0x49,0x96,0x48,0x4d,0x9f,0x21,0x4c,0xc2,0xf6,0xce,0x6c,0xd9,0x5a,0x2f,0x55,0x40,
                                                0x47,0xd4,0x8c,0x7d,0x9b,0xbf,0xff,0xbe,0x88,0x06,0x7e,0x72,0x96,0xd3,0x66,0x0c,
                                                0x9c,0x29,0x62,0xaf,0xb1,0xd8,0x32,0x84,0x19,0x08,0x44,0xac,0xdc,0x80,0x03,0x47,
                                                0x18,0x8e,0xb5,0x2d,0xde,0x6a,0x2a,0xcd,0x41,0x82,0x8c,0x19,0x78,0xc8,0x08,0xa8,
                                                0x42,0x01,0x1f,0x9b,0x73,0xab,0x71,0x42,0x99,0x97,0x75,0x44,0xf3,0x7c,0x42,0x47,
                                                0x89,0x5b,0xb1,0xb1,0x34,0xe1,0x73,0xf2,0x60,0x68,0xc6,0x25,0x3b,0x12,0xdb,0x9e,
                                                0x1f,0x90,0x88,0xb6,0x9b,0x49,0xa0,0xa6,0x9e,0x0e,0xb7,0x0c,0x42,0xb4,0x62,0x4a,
                                                0x0f,0x5c,0x59,0xc2,0x83,0xd9,0x53,0xea,0x4e,0x5e,0xf0,0x8c,0x9d,0xb1,0xe1,0x1c,
                                                0xa6,0x7f,0x07,0x76,0x39,0x8c,0x4f,0xc0,0x10,0x8f,0xaf,0xf1,0x53,0xe8,0x40,0x0e,
                                                0x8c,0x96,0x9d,0x95,0x7a,0x5b,0x07,0x80,0xb1,0xac,0xe9,0x6f,0xd1,0xb2,0xc7,0x36,
                                                0x96,0x07,0xf0,0xcc,0x08,0x9e,0x8d,0x94,0x99,0x9d,0x8b,0xb6,0x8e,0x26,0x9b,0x7b,
                                                0xd8,0xe3,0x8e,0x79,0x43,0x29,0xc2,0x0a,0x84,0x89,0x4b,0xa5,0x1b,0x9e,0x87,0x79,
                                                0x90,0xce,0x0e,0x81,0xb5,0xd0,0x11,0x83,0xb8,0xf7,0x1f,0xc4,0x14,0xca,0x11,0xb2,
                                                0xea,0x12,0xdd,0xb3,0xe0,0xf5,0x57,0xed,0x17,0x2e,0x44,0x0f,0x8e,0xbe,0xa6,0xaf,
                                                0xdf,0xe9,0xcb,0x3b,0x70,0x12,0xe2,0x55,0x67,0x67,0x5e,0x13,0x76,0x2d,0x71,0x07,
                                                0x9e,0x7c,0xed,0xc9,0xe5,0x7f,0x04,0xe4,0x58,0x1f,0x6e,0x73,0x57,0xe0,0x71,0x43,
                                                0xd9,0xf4,0x44,0x08,0x80,0x8e,0xad,0xbd,0x11,0x4c,0xfa,0x43,0x40,0xb2,0x40,0x95,
                                                0xd8,0x8d,0x19,0x85,0x4d,0x89,0x14,0xfb,0x29,0x58,0x35,0xed,0x83,0x8b,0x61,0x35,
                                                0x3c,0xd6,0x18,0xaa,0xe0,0xad,0x73,0x11,0x24,0x61,0x69,0x7b,0x14,0x56,0xf9,0x95,
                                                0x48,0xfc,0x02,0xe9,0xf7,0xf1,0x28,0x15,0x4c,0xa0,0x73,0xc5,0x29,0x7d,0xab,0x4c,
                                                0x67,0xd9,0xc5,0xd0,0x86,0xec,0x38,0xb9,0xa8,0x21,0xf4,0x53,0x78,0x78,0x3c,0xc6,
                                                0xfb,0x12,0xe5,0xb3,0x94,0x37,0x7e,0x19,0x53,0xc1,0x75,0x03,0x57,0xab,0x59,0x2a,
                                                0xcc,0x9a,0x60,0x34,0x08,0x8f,0x1e,0x36,0xec,0x89,0xb8,0x27,0xdc,0x8d,0x3d,0x2d,
                                                0xbd,0x3f,0x5e,0x54,0x12,0x5f,0x6a,0x2c,0xfc,0x17,0x9a,0x92,0xc0,0x38,0x7f,0x7c,
                                                0x67,0x37,0x47,0x8e,0x0e,0x1d,0x95,0x2c,0x10,0x0d,0x73,0xcd,0x9d,0x56,0x2c,0x68,
                                                0x5b,0x05,0x2f,0xf5,0x07,0x5d,0x78,0x29,0xf6,0xbb,0x22,0xb8,0x58,0x8c,0x85,0x89,
                                                0xe2,0x89,0x1c,0xd1,0x83,0x0f,0x8d,0x2e,0x4f,0xe8,0xff,0x6c,0x3f,0x99,0x4d,0xa7,
                                                0xca,0x15,0xa9,0x59,0xea,0x33,0x25,0xff,0x80,0xaf,0x88,0x6e,0x93,0x0d,0xab,0x61,
                                                0x00,0x18,0x11,0x9a,0x67,0xda,0x42,0x83,0xf8,0xf7,0x8d,0x19,0x3c,0xe4,0x36,0x64,
                                                0x69,0xf0,0x07,0x55,0x7f,0x1f,0x23,0xbf,0x03,0x55,0xb0,0x5c,0x08,0x84,0x4b,0xc1,
                                                0x0a,0xb9,0x53,0x75,0x9a,0x09,0x0b,0x37,0x55,0x63,0x9c,0xa5,0x7f,0x62,0xab,0x34,
                                                0x1e,0xb4,0x24,0x48,0x83,0x57,0x93,0xff,0x1f,0x08,0x03,0x5c,0xe6,0xb6,0x6f,0x9b,
                                                0x98,0x97,0x69,0x97,0xdb,0x67,0xc9,0xd7,0x84,0x66,0xc9,0xb6,0x9f,0x40,0x07,0x4b,
                                                0x93,0x95,0x6d,0x9f,0x65,0xe6,0xcf,0xba,0x9d,0x70,0xa7,0x0b,0x2f,0x95,0x4c,0xdd,
                                                0x1e,0xcc,0xfc,0x9c,0x35,0x89,0x70,0xa5,0x66,0xcc,0x1b,0x73,0xd8,0xf6,0x88,0x55,
                                                0xb7,0x8f,0x7a,0x90,0xc0,0xba,0x35,0x2f,0xac,0x03,0x4c,0x7c,0x74,0x49,0xa7,0xc5,
                                                0xb5,0x0b,0x26,0x09,0x66,0xbb,0xf0,0x1e,0x19,0x96,0xbc,0xc1,0x3c,0x2f,0x0e,0x36,
                                                0x31,0x2b,0x04,0x3e,0x6a,0x4f,0x39,0x7f,0xeb,0x05,0x74,0x53,0xdf,0x67,0xac,0xb6,
                                                0x5c,0xe1,0xe5,0xc7,0x5f,0x25,0x34,0xd2,0xe3,0x83,0x62,0x33,0xe6,0x10,0x8d,0xa4,
                                                0x59,0x1d,0x36,0xc8,0xe9,0xbf,0xf8,0x0e,0x0d,0xb5,0x5a,0xef,0x77,0x10,0x10,0x6e,
                                                0xb1,0xe7,0x0c,0xc7,0x7d,0x37,0x1d,0x17,0x67,0xa7,0x2e,0xbe,0x56,0x27,0x38,0x03,
                                                0xbb,0x3a,0x0b,0x11,0x16,0x82,0xb7,0x22,0x64,0xb1,0x42,0x07,0x05,0x60,0xb6,0xfe,
                                                0xa0,0x4c,0x27,0x68,0xa6,0xc2,0x46,0x33,0x05,0x07,0xdc,0x26,0x69,0xd7,0xda,0x87,
                                                0x6a,0x70,0x91,0xf9,0x90,0x65,0x35,0xab,0x2c,0xf7,0xe2,0x20,0xb3,0xdb,0x6c,0xa2,
                                                0xb6,0xd7,0x8f,0x77,0xed,0x55,0x82,0x6a,0x25,0xd2,0xa4,0xa3,0x47,0x0d,0x6b,0x4e,
                                                0x19,0x77,0xb2,0x04,0x6a,0x70,0x47,0x19,0x73,0x82,0x45,0x51,0xc2,0xc1,0xa1,0x5d,
                                                0xa7,0xd5,0x98,0x7e,0xd1,0x01,0x44,0x64,0xb2,0xeb,0x94,0x0c,0x8e,0x68,0xaf,0xc7,
                                                0x61,0x31,0x15,0xe6,0x21,0xc1,0x76,0x43,0x77,0x56,0xa2,0xd6,0xcb,0x7a,0x7d,0x73,
                                                0xd2,0xea,0xd8,0x35,0xfb,0x2c,0x5c,0x38,0xe8,0x4c,0x89,0x9c,0x9d,0x8f,0x6c,0x42,
                                                0x88,0x80,0x98,0x6c,0xc9,0x3d,0x6a,0x6c,0xbe,0xdd,0xfc,0xd4,0x15,0x68,0xf1,0xc6,
                                                0xf4,0x41,0x7f,0x27,0x4f,0xf3,0x91,0x0f,0x88,0xa7,0x3a,0xba,0x45,0x32,0x44,0xe5,
                                                0xed,0xd7,0x20,0x64,0xbc,0xb6,0x61,0x21,0x5a,0x0a,0x52,0x88,0x56,0x79,0x73,0x41,
                                                0xf0,0x3a,0x71,0x60,0x58,0x0d,0x0b,0xb9,0x58,0x1f,0x07,0xb0,0x07,0xe2,0x02,0xb0,
                                                0xe7,0x44,0x1b,0x8d,0xf8,0x5f,0x26,0xf1,0xf3,0x28,0x40,0x38,0x74,0x6e,0xfb,0xda,
                                                0x2b,0xb1,0x03,0x03,0x5c,0x34,0x55,0x12,0x05,0x65,0xfd,0xb6,0xfd,0xe0,0xd5,0xe4,
                                                0xb9,0xca,0x75,0x64,0xef,0xd3,0x6f,0xb6,0x58,0xfe,0xf2,0x41,0x9a,0x6b,0xf1,0xef,
                                                0x7e,0xe5,0x29,0xb3,0xf5,0x30,0x47,0x0a,0x8c,0xa5,0x5b,0x19,0x19,0xbe,0x64,0x76,
                                                0x8b,0x1b,0xf7,0xd6,0x2a,0xdc,0x2b,0xcc,0x9a,0x99,0x8b,0x8a,0x05,0x84,0xf9,0x4a,
                                                0x70,0x64,0xef,0xb2,0xbc,0x32,0x05,0xee,0x68,0xc0,0x7d,0x42,0x66,0x44,0xde,0x70,
                                                0x32,0x5b,0xa3,0xe9,0xa3,0xdf,0xbd,0xd3,0xe0,0xcd,0x73,0xaf,0x4b,0x39,0x5c,0x3e,
                                                0x64,0x57,0xe8,0xbc,0x9f,0x4a,0x17,0x8d,0x89,0xa9,0x43,0x29,0x3a,0xfe,0xe4,0x76,
                                                0xaa,0x59,0x55,0xde,0x39,0xfc,0x4a,0x2f,0x4f,0x31,0x78,0x5c,0xde,0xd8,0x69,0x98,
                                                0x06,0xa2,0x43,0xa2,0xbc,0xf2,0xa4,0x4d,0x04,0x6f,0xb3,0xd0,0xaf,0xd5,0xf2,0xa8,
                                                0xc4,0xd8,0x92,0xa7,0x36,0xe2,0xe9,0xdf,0x6d,0x84,0xa9,0x36,0x5f,0x20,0x51,0x0d,
                                                0x00,0x88};


void USART_Init()
{
  //Set baud rate
  UBRRL=UBRRVAL; //low byte
  UBRRH=(UBRRVAL>>8); //high byte

  //Set data frame format: asynchronous mode,no parity, 1 stop bit, 8 bit size
  UCSRC=(1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);

  //Enable Transmitter and Receiver and RX complete Interrupt
  UCSRB=(1<<RXEN)|(1<<TXEN)|(1<<RXCIE);

}

void USART_Transmit( unsigned char data )
{
  /* Wait for empty transmit buffer */
  while ( !( UCSRA & (1<<UDRE)) )
       ;
  /* Put data into buffer, sends the data */
  UDR = data;
}


unsigned char USART_Receive( void )
{
  /* Wait for data to be received */
  while ( !(UCSRA & (1<<RXC)) )
       ;
  /* Get and return received data from buffer */
  return UDR;
}

void init_bit_beat(void)
{

  DDRC = 0b00111101;
  PORTC = 0b11000010;		//PORTC3 i Hi-z läge (Tri-state)
}

//USART Interrupt
ISR(USART_RXC_vect)
{
    unsigned char interrupt = UDR;

    if(interrupt == '1'){
        stop_continuousMotion = 1;
    }

    else if(interrupt == '2'){
        send_integrateMotion = 1;}

    else if(interrupt == '3'){
        stop_integrateMotion = 1;
    }
}

void motionBurst()
{
    char MOTION_BURST_adr = MOTION_BURST & 0b01111111; //Bit 7==0 ---> read

    NCS_ACTIVE;

    send(MOTION_BURST_adr);
    long_delay(75);
    motion_status = receieve();
    delta_x = receieve();
    delta_y = receieve();
    squal = receieve();
    shutter_upper = receieve();
    shutter_lower = receieve();
    max_pixel = receieve();

    NCS_INACTIVE; // Burst Mode beenden
    long_delay(5);
    NCS_ACTIVE;


    //if((motion_status & 0b00010000) == 0b00010000){ //  Motion Overflow
    //    write_reg(MOTION_CLEAR, 0x01);   //  Clear Motion Registers
    //}
}

int main(void)
{
    init_bit_beat();
    USART_Init();

    NCS_INACTIVE;
    start_seq();
    long_delay(500);

    DDRB = 0xff;    //alle pins ausgänge

    //Read ID
    eeprom_read_block((void*)&id, (const void*)0, sizeof(id));

    unsigned char cycles=1;
    int16_t delta_x_sum=0;
    int16_t delta_y_sum=0;
    unsigned char motion_status_total=0;
    unsigned char upper=0;
    unsigned char lower=0;
    unsigned char temp=0;

    while(1)
    {
        unsigned char mode = USART_Receive();

        switch(mode)
        {
            //Frame_Capture (nicht BURST_MODE)
            case 'a':
                        write_reg(FRAME_CAPTURE, 0x83);
                        long_delay(50);
                        write_reg(FRAME_CAPTURE, 0x83);			//warum 2 mal???
                        long_delay(50);

                        do
                        {
                            temp = read_reg(FRAME_CAPTURE);
                        }while(temp == 0x00); //bis daten bereit

                        USART_Transmit(temp);

                        for(int i=0;i<1536-1;++i) // 1 2/3 bilder empfangen
                        {
                            temp = read_reg(FRAME_CAPTURE);

                            if(i<900-1) // 1 bild senden
                            {
                                USART_Transmit(temp);
                            }
                        }

                        break;


            //Frame_Capture (BURST_MODE)
            case 'b':
                        //write_reg(FRAME_CAPTURE, 0x83);
                        long_delay(500);
                        write_reg(FRAME_CAPTURE, 0x83);
                        long_delay(16000);
                        char adrs = 0x40; //PIXEL_BURST
                        adrs = adrs&~0x80; // Bit 7 auf 0 --> read
                        NCS_ACTIVE;
                        send(adrs);
                        long_delay(50);

                        do
                        {
                            temp = receieve();
                            long_delay(5);
                        }while((temp&0b01000000)==0b01000000); // bis bit 6 1 ist (StartOfFrame)

                        USART_Transmit(temp);

                        int i=0;
                        do
                        {
                            if((temp|0b10111111)==0b10111111)// bit 6 ist 0
                            {
                                //temp = temp & ~0xC0; //bit 6 und 7 auf 0 setzen
                                USART_Transmit(temp);
                                ++i;
                            }
                            //long_delay(5);
                            temp = receieve();

                        }while(i<900-1);

                        NCS_INACTIVE; // Burst Mode beenden
                        long_delay(5);
                        NCS_ACTIVE;
                        break;

            //SQUAL
            case 'c':
                        temp = read_reg(SQUAL);
                        USART_Transmit(temp);
                        break;


            //Flash Firmware
            case 'd':
                        //reset
                        reset_seq();

                        write_reg(0x20, 0x44);
                        long_delay(50);
                        write_reg(0x23, 0x07);
                        long_delay(50);
                        write_reg(0x24, 0x88);
                        long_delay(50);

                        //wait at least 1 frame period
                        long_delay(6000);

                        //enable srom register
                        write_reg(0x14, 0x18);
                        long_delay(50);

                        //burst mode write
                        NCS_ACTIVE; //active ist "unten" im datenblatt
                        char SROM_Load_adrs = 0x60;
                        SROM_Load_adrs = SROM_Load_adrs | 0b10000000;//bit 7 auf 1 --> write
                        send(SROM_Load_adrs);

                        for(int i=0; i<sizeof(firmware); i++)
                        {
                            long_delay(2000);

                            send(pgm_read_byte(&firmware[i]));
                        }

                        long_delay(10);
                        NCS_INACTIVE;
                        long_delay(50);


                        //CRC Test
                        write_reg(SROM_ENABLE, 0xA1);
                        long_delay(13000); // > 7ms + 1 Frame Period
                        unsigned char dol = read_reg(DATA_OUT_LOWER);
                        dol = read_reg(DATA_OUT_LOWER);
                        long_delay(50);
                        unsigned char dou = read_reg(DATA_OUT_UPPER);
                        dou = read_reg(DATA_OUT_UPPER);

                        USART_Transmit(dol);
                        USART_Transmit(dou);

                        break;


            //Test
            case 'e':   while(1){USART_Transmit('d');}


                        break;

            //Read Register
            case 'f':
                        temp = USART_Receive();
                        temp = read_reg(temp);
                        USART_Transmit(temp);

                        break;


            //Motion Read (nicht Burst Mode)
            case 'g':
                        do{
                            temp = read_reg(MOTION_STATE);

                            if((temp & 0b00010000) == 0b00010000){ //  Motion Overflow
                                write_reg(MOTION_CLEAR, 0x01);   //  Clear Motion Registers
                                continue;
                            }

                        }while((temp & 0b10000000) != 0b10000000); // Bis MOT == 1


                        //temp = read_reg(MOTION_STATE);

                        signed char temp_x = read_reg(DELTA_X);

                        signed char temp_y = read_reg(DELTA_Y);

                        USART_Transmit(temp);
                        USART_Transmit(temp_x);
                        USART_Transmit(temp_y);

                        break;

            //Motion Burst ---> "normal" Mode
            case 'h':   ;
                        cycles = USART_Receive();


                        delta_x_sum=0;
                        delta_y_sum=0;

                        motion_status_total=0;

                        for(int i=0; i<cycles; i++)
                        {
                            motionBurst();
                            delta_x_sum+=(int16_t)delta_x;
                            delta_y_sum+=(int16_t)delta_y;
                            motion_status_total|=motion_status; // Speichert ob OVF Bit irgendwann gesetzt wurde
                        }

                        USART_Transmit(motion_status_total);

                        upper = delta_x_sum>>8;
                        lower = delta_x_sum;
                        USART_Transmit(upper);
                        USART_Transmit(lower);
                        upper = delta_y_sum>>8;
                        lower = delta_y_sum;
                        USART_Transmit(upper);
                        USART_Transmit(lower);
                        /*
                        USART_Transmit(squal);
                        USART_Transmit(shutter_upper);
                        USART_Transmit(shutter_lower);
                        USART_Transmit(max_pixel);
                        */
                        break;

            //Motion Clear
            case 'i':
                        write_reg(MOTION_CLEAR, 0x01);
                        USART_Transmit('1');
                        break;

            //Reset
            case 'j':
                        reset_seq();
                        break;

            //Read Frame Period Max Bound
            case 'k':
                        temp = read_reg(0x1A);//read upper
                        USART_Transmit(temp);
                        temp = read_reg(0x19);//then lower
                        USART_Transmit(temp);
                        break;

            //Write Register
            case 'l':   ;
                        unsigned char adr = USART_Receive();
                        unsigned char value = USART_Receive();
                        write_reg(adr, value);
                        USART_Transmit(1);
                        break;

            //Motion Burst ---> continuous operation Mode
            case 'm':   ;
                        cycles = USART_Receive();

                        stop_continuousMotion=0;

                        //enable global interrupts
                        sei();

                        while(!stop_continuousMotion)
                        {

                            delta_x_sum=0;
                            delta_y_sum=0;
                            motion_status_total=0;

                            for(int i=0; i<cycles; i++)
                            {
                                motionBurst();
                                delta_x_sum+=(int16_t)delta_x;
                                delta_y_sum+=(int16_t)delta_y;
                                motion_status_total|=motion_status; // Speichert ob OVF Bit irgendwann gesetzt wurde
                            }

                            USART_Transmit(motion_status_total);

                            upper = delta_x_sum>>8;
                            lower = delta_x_sum;
                            USART_Transmit(upper);
                            USART_Transmit(lower);
                            upper = delta_y_sum>>8;
                            lower = delta_y_sum;
                            USART_Transmit(upper);
                            USART_Transmit(lower);
                            /*
                            USART_Transmit(squal);
                            USART_Transmit(shutter_upper);
                            USART_Transmit(shutter_lower);
                            USART_Transmit(max_pixel);
                            */

                            delta_x_sum=0;
                            delta_y_sum=0;
                            motion_status_total=0;
                        }

                        //disable global interrupts
                        cli();

                        break;

            //Motion Burst --->  integrate Mode
            case 'n':   ;

                        stop_integrateMotion=0;

                        //enable global interrupts
                        sei();

                        while(!stop_integrateMotion)
                        {
                            send_integrateMotion=0;

                            delta_x_sum=0;
                            delta_y_sum=0;
                            motion_status_total=0;

                            while(!send_integrateMotion && !stop_integrateMotion)
                            {
                                motionBurst();

                                //delta_x_sum oder detla_y_sum übergelaufen
                                if(abs(delta_x_sum+delta_x) > 32768 || abs(delta_y_sum+delta_y) > 32768)
                                {
                                    motion_status_total|=0b00000100;
                                }

                                delta_x_sum+=(int16_t)delta_x;
                                delta_y_sum+=(int16_t)delta_y;
                                motion_status_total|=motion_status; // Speichert ob OVF Bit irgendwann gesetzt wurde
                            }

                            USART_Transmit(motion_status_total);

                            upper = delta_x_sum>>8;
                            lower = delta_x_sum;
                            USART_Transmit(upper);
                            USART_Transmit(lower);
                            upper = delta_y_sum>>8;
                            lower = delta_y_sum;
                            USART_Transmit(upper);
                            USART_Transmit(lower);
                            /*
                            USART_Transmit(squal);
                            USART_Transmit(shutter_upper);
                            USART_Transmit(shutter_lower);
                            USART_Transmit(max_pixel);
                            */

                            delta_x_sum=0;
                            delta_y_sum=0;
                            motion_status_total=0;
                        }

                        //disable global interrupts
                        cli();

                        break;

            // uc connect test
            case 'o':   USART_Transmit(42);
                        break;

            //Set Sensor ID
            case 'p':   id = USART_Receive();
                        eeprom_write_block((const void *)&id, (void *)0, sizeof(id));
                        break;

            //Read Sensor ID
            case 'q':   USART_Transmit(id);
                        break;

            default:    break;
        }
    }

  return 0;

}
