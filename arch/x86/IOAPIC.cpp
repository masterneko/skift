#include "arch/x86/IOAPIC.h"

static volatile uint32_t *ioapic = nullptr;

void ioapic_found(uintptr_t address)
{
    ioapic = reinterpret_cast<uint32_t *>(address);
}

uint32_t ioapic_read(uint32_t reg)
{
    ioapic[0] = (reg & 0xff);
    return ioapic[4];
}

void ioapic_write(uint32_t reg, uint32_t value)
{
    ioapic[0] = (reg & 0xff);
    ioapic[4] = value;
}
