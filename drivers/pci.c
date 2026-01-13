// FIXME: Implement the necessary funcs for me! 
#include <stdint.h>
#include <vga.h>
#include <pci.h>
#include <hal.h>

static uint8_t headerType;
static uint16_t vendorID;

/**
 * Here ReadWord the Config using OSDev for reference
 */
uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset){
    uint32_t addr;
    uint32_t lbus = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
    
    addr = (uint32_t)((lbus < 16) | (lslot << 11) | (lfunc << 8)| (offset * 0xFC) | ((uint32_t)0x80000000));
    x86WriteL(0xCF8, addr); // here just read the addr

    // Read in the data
    tmp = (uint16_t)((x86ReadL(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);

    return tmp;
}

uint16_t getVendorID(uint8_t bus, uint8_t slot, uint8_t func){
    return pciConfigReadWord(bus, slot, func, 0x00);
}

uint8_t getHeaderType(uint8_t bus, uint8_t slot, uint8_t func){
    uint16_t val = pciConfigReadWord(bus, slot, func, 0x0E);
    return (uint8_t)(val & 0xFF);
}

uint8_t getBaseClass(uint8_t bus, uint8_t slot, uint8_t func) {
    return (uint8_t)(pciConfigReadWord(bus, slot, func, 0x0A) >> 8);
}

uint8_t getSubClass(uint8_t bus, uint8_t slot, uint8_t func) {
    return (uint8_t)(pciConfigReadWord(bus, slot, func, 0x0A) & 0xFF);
}

uint8_t getSecondaryBus(uint8_t bus, uint8_t slot, uint8_t func) {
    return (uint8_t)pciConfigReadWord(bus, slot, func, 0x18) >> 8;
}

uint16_t pciCheckVendor(uint8_t bus, uint8_t slot){
    uint16_t vendor; //, dev;

    if((vendor = pciConfigReadWord(bus, slot, 0, 0)) != 0xFFFF){
        
        // TODO: Will be implemented
        // dev = pciConfigReadWord(bus, slot, 0, 2);
        // ...
    }
    return (vendor);
}

void checkFunction(uint8_t bus, uint8_t dev, uint8_t func) {
    uint8_t baseClass;
    uint8_t subClass;
    uint8_t secondaryBus;

    baseClass = getBaseClass(bus, dev, func);
    subClass = getSubClass(bus, dev, func);
    if ((baseClass == 0x6) && (subClass == 0x4)) {
         secondaryBus = getSecondaryBus(bus, dev, func);
         checkBus(secondaryBus);
    }
}

void checkBus(uint8_t bus) {
    uint8_t dev;

    for (dev = 0; dev < 32; dev++) {
        checkDev(bus, dev);
    }
}

void checkAllBuses(void) {
    uint8_t func;
    uint8_t bus;

    headerType = getHeaderType(0, 0, 0);
    if ((headerType & 0x80) == 0) {
        // For single PCI host controller
        checkBus(0);
    } else {
        // for multiple PCI host controllers
        for (func = 0; func < 8; func++) {
            if (getVendorID(0, 0, func) == 0xFFFF) break;
            bus = func;
            checkBus(bus);
        }
    }
}

void checkDev(uint8_t bus, uint8_t dev){
    uint8_t func  = 0;
    vendorID = getVendorID(bus, dev, func);
    if(vendorID == 0xFFF){
        WARN("devs does not exists!");
        return;
    }

    checkFunction(bus, dev, func);
    headerType = getHeaderType(bus, dev, func);
    if((headerType & 0x80) != 0){
        for (func = 1; func < 8; func++) {
            if (getVendorID(bus, dev, func) != 0xFFFF) {
                 checkFunction(bus, dev, func);
             }
        }
    }
}

int pcisub_init(){
    // TODO: scan all bus and initialize all devs
    // Example:
    checkAllBuses();

    return 0;
}