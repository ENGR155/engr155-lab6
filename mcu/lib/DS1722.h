// DS1722.h
// TODO: <YOUR NAME>
// TODO: <YOUR EMAIL>
// TODO: <DATE>
// TODO: <SHORT DESCRIPTION OF WHAT THIS FILE DOES>

#ifndef DS1722_H
#define DS1722_H
#include <stdint.h>

// Wait 150ms
void initDS1722(void);
// Read temp
int16_t ds1722ReadRaw(void);
#endif
