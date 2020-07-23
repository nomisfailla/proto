#include "pci.h"

#define PCI_ID(bus, dev, func) ((bus) << 16) | ((dev) << 11) | ((func) << 8)

static pci_class_t classify(uint8_t class)
{
    switch(class)
    {
    case 0x00: return PCI_UNCLASSIFIED;
    case 0x01: return PCI_MASS_STORAGE_CONTROLLER;
    case 0x02: return PCI_NETWORK_CONTROLLER;
    case 0x03: return PCI_DISPLAY_CONTROLLER;
    case 0x04: return PCI_MULTIMEDIA_CONTROLLER;
    case 0x05: return PCI_MEMORY_CONTROLLER;
    case 0x06: return PCI_BRIDGE_DEVICE;
    case 0x07: return PCI_SIMPLE_COMMUNICATION_CONTROLLER;
    case 0x08: return PCI_BASE_SYSTEM_PERIPHERAL;
    case 0x09: return PCI_INPUT_DEVICE_CONTROLLER;
    case 0x0A: return PCI_DOCKING_STATION;
    case 0x0B: return PCI_PROCESSOR;
    case 0x0C: return PCI_SERIAL_BUS_CONTROLLER;
    case 0x0D: return PCI_WIRELESS_CONTROLLER;
    case 0x0E: return PCI_INTELLIGENT_CONTROLLER;
    case 0x0F: return PCI_SATELLITE_COMMUNICATION_CONTROLLER;
    case 0x10: return PCI_ENCRYPTION_CONTROLLER;
    case 0x11: return PCI_SIGNAL_PROCESSING_CONTROLLER;
    case 0x12: return PCI_PROCESSING_ACCELERATOR;
    case 0x13: return PCI_NONESSENTIAL_INSTRUMENTATION;
    case 0x40: return PCI_COPROCESSOR;
    case 0xFF: return PCI_UNASSIGNED;
    }
    return PCI_UNCLASSIFIED;
}

static uint8_t pci_read8(const uint32_t id, const uint32_t reg)
{
    const uint32_t addr = 0x80000000 | id | (reg & 0xFC);
    out32(PCI_CONFIG_ADDR, addr);
    return in8(PCI_CONFIG_DATA + (reg & 0x03));
}

static uint16_t pci_read16(const uint32_t id, const uint32_t reg)
{
    const uint32_t addr = 0x80000000 | id | (reg & 0xFC);
    out32(PCI_CONFIG_ADDR, addr);
    return in16(PCI_CONFIG_DATA + (reg & 0x02));
}

static uint32_t pci_read32(const uint32_t id, const uint32_t reg)
{
    const uint32_t addr = 0x80000000 | id | (reg & 0xFC);
    out32(PCI_CONFIG_ADDR, addr);
    return in32(PCI_CONFIG_DATA);
}

static pci_device_t pci_read_device(const uint32_t bus, const uint32_t dev, const uint32_t func)
{
    const uint32_t id = PCI_ID(bus, dev, func);

    pci_device_t device;
    device.vendor_id = pci_read16(id, PCI_CONFIG_VENDOR_ID);
    device.device_id = pci_read16(id, PCI_CONFIG_DEVICE_ID);
    const uint8_t class = pci_read8(id, PCI_CONFIG_CLASS_CODE);
    device.class = classify(class);
    device.subclass = pci_read8(id, PCI_CONFIG_SUBCLASS);
    device.header_type = pci_read8(id, PCI_CONFIG_HEADER_TYPE);
    device.interrupt_line = pci_read8(id, PCI_CONFIG_INTERRUPT_LINE);
    device.bar0 = pci_read32(id, PCI_CONFIG_BAR0);
    device.bar1 = pci_read32(id, PCI_CONFIG_BAR1);
    device.bar2 = pci_read32(id, PCI_CONFIG_BAR2);
    device.bar3 = pci_read32(id, PCI_CONFIG_BAR3);
    device.bar4 = pci_read32(id, PCI_CONFIG_BAR4);
    device.bar5 = pci_read32(id, PCI_CONFIG_BAR5);

    return device;
}

void enumerate_pci(pci_enumerator_t enumerator)
{
    for(uint32_t bus = 0; bus < 256; bus++)
    {
        for(uint32_t device = 0; device < 32; device++)
        {
            uint8_t header_type = pci_read8(PCI_ID(bus, device, 0), PCI_CONFIG_HEADER_TYPE);
            uint32_t function_count = header_type & PCI_TYPE_MULTIFUNC ? 8 : 1;

            for(uint16_t function = 0; function < function_count; function++)
            {
                pci_device_t pci_device = pci_read_device(bus, device, function);
                if(pci_device.vendor_id != 0xFFFF)
                {
                    enumerator(pci_device);
                }
            }
        }
    }
}
