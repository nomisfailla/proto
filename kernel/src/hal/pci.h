#ifndef _PCI_H
#define _PCI_H

#include <stdint.h>
#include <stddef.h>

#include "../cpu/ports.h"

typedef enum
{
    PCI_UNCLASSIFIED = 0x00,
    PCI_MASS_STORAGE_CONTROLLER = 0x01,
    PCI_NETWORK_CONTROLLER = 0x02,
    PCI_DISPLAY_CONTROLLER = 0x03,
    PCI_MULTIMEDIA_CONTROLLER = 0x04,
    PCI_MEMORY_CONTROLLER = 0x05,
    PCI_BRIDGE_DEVICE = 0x06,
    PCI_SIMPLE_COMMUNICATION_CONTROLLER = 0x07,
    PCI_BASE_SYSTEM_PERIPHERAL = 0x08,
    PCI_INPUT_DEVICE_CONTROLLER = 0x09,
    PCI_DOCKING_STATION = 0x0A,
    PCI_PROCESSOR = 0x0B,
    PCI_SERIAL_BUS_CONTROLLER = 0x0C,
    PCI_WIRELESS_CONTROLLER = 0x0D,
    PCI_INTELLIGENT_CONTROLLER = 0x0E,
    PCI_SATELLITE_COMMUNICATION_CONTROLLER = 0x0F,
    PCI_ENCRYPTION_CONTROLLER = 0x10,
    PCI_SIGNAL_PROCESSING_CONTROLLER = 0x11,
    PCI_PROCESSING_ACCELERATOR = 0x12,
    PCI_NONESSENTIAL_INSTRUMENTATION = 0x13,
    PCI_COPROCESSOR = 0x40,
    PCI_UNASSIGNED = 0xFF
} pci_class_t;

typedef struct
{
    uint16_t device_id;
    uint16_t vendor_id;

    pci_class_t class;
    uint8_t subclass;
} pci_device_t;

typedef void(*pci_enumerator_t)(pci_device_t);

void enumerate_pci(pci_enumerator_t enumerator);

#endif
