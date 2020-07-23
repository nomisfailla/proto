#include "pci.h"

#include "../term/terminal.h"

static uint16_t read_word(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset)
{
    uint64_t lbus = bus;
    uint64_t lslot = slot;
    uint64_t lfunc = func;
    uint16_t tmp = 0;
    uint64_t address = (uint64_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
    out32(0xCF8, address);
    tmp = (uint16_t)((in32(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
}

static uint16_t get_vendor_id(uint16_t bus, uint16_t device, uint16_t function)
{
    return read_word(bus, device, function, 0);
}

static uint16_t get_device_id(uint16_t bus, uint16_t device, uint16_t function)
{
    return read_word(bus, device, function, 2);
}

static uint8_t get_class(uint16_t bus, uint16_t device, uint16_t function)
{
    return (read_word(bus, device, function, 0xA) & ~0x00FF) >> 8;
}

static uint8_t get_subclass(uint16_t bus, uint16_t device, uint16_t function)
{
    return (read_word(bus, device, function, 0xA) & ~0xFF00);
}

void enumerate_pci(pci_enumerator_t enumerator)
{
    for(uint16_t bus = 0; bus < 256; bus++)
    {
        for(uint16_t slot = 0; slot < 32; slot++)
        {
            for(uint16_t function = 0; function < 8; function++)
            {
                pci_device_t device;
                device.vendor_id = get_vendor_id(bus, slot, function);
                if(device.vendor_id == 0xFFFF) { continue; }
                device.device_id = get_device_id(bus, slot, function);
                device.subclass = get_subclass(bus, slot, function);

                uint8_t class = get_class(bus, slot, function);
                switch(class)
                {
                case 0x00: device.class = PCI_UNCLASSIFIED; break;
                case 0x01: device.class = PCI_MASS_STORAGE_CONTROLLER; break;
                case 0x02: device.class = PCI_NETWORK_CONTROLLER; break;
                case 0x03: device.class = PCI_DISPLAY_CONTROLLER; break;
                case 0x04: device.class = PCI_MULTIMEDIA_CONTROLLER; break;
                case 0x05: device.class = PCI_MEMORY_CONTROLLER; break;
                case 0x06: device.class = PCI_BRIDGE_DEVICE; break;
                case 0x07: device.class = PCI_SIMPLE_COMMUNICATION_CONTROLLER; break;
                case 0x08: device.class = PCI_BASE_SYSTEM_PERIPHERAL; break;
                case 0x09: device.class = PCI_INPUT_DEVICE_CONTROLLER; break;
                case 0x0A: device.class = PCI_DOCKING_STATION; break;
                case 0x0B: device.class = PCI_PROCESSOR; break;
                case 0x0C: device.class = PCI_SERIAL_BUS_CONTROLLER; break;
                case 0x0D: device.class = PCI_WIRELESS_CONTROLLER; break;
                case 0x0E: device.class = PCI_INTELLIGENT_CONTROLLER; break;
                case 0x0F: device.class = PCI_SATELLITE_COMMUNICATION_CONTROLLER; break;
                case 0x10: device.class = PCI_ENCRYPTION_CONTROLLER; break;
                case 0x11: device.class = PCI_SIGNAL_PROCESSING_CONTROLLER; break;
                case 0x12: device.class = PCI_PROCESSING_ACCELERATOR; break;
                case 0x13: device.class = PCI_NONESSENTIAL_INSTRUMENTATION; break;
                case 0x40: device.class = PCI_COPROCESSOR; break;
                case 0xFF: device.class = PCI_UNASSIGNED; break;
                default:   device.class = PCI_UNCLASSIFIED; break;
                }

                enumerator(device);
            }
        }
    }
}
