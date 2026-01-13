#pragma once

#include <stdint.h>

uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint16_t getVendorID(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t getHeaderType(uint8_t bus, uint8_t slot, uint8_t func);
uint16_t pciCheckVendor(uint8_t bus, uint8_t slot);
void checkFunction(uint8_t bus, uint8_t device, uint8_t function);
void checkBus(uint8_t bus);
void checkAllBuses(void);
void checkDev(uint8_t bus, uint8_t dev);
int pcisub_init();