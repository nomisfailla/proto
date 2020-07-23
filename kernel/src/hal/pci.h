#ifndef _PCI_H
#define _PCI_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "../cpu/ports.h"

#define PCI_CONFIG_ADDR 0xCF8
#define PCI_CONFIG_DATA 0xCFC

#define PCI_TYPE_MULTIFUNC 0x80

#define PCI_CONFIG_VENDOR_ID      0x00
#define PCI_CONFIG_DEVICE_ID      0x02
#define PCI_CONFIG_COMMAND        0x04
#define PCI_CONFIG_PROG_IF        0x09
#define PCI_CONFIG_SUBCLASS       0x0A
#define PCI_CONFIG_CLASS_CODE     0x0B
#define PCI_CONFIG_HEADER_TYPE    0x0E
#define PCI_CONFIG_INTERRUPT_LINE 0x3C

#define PCI_CONFIG_BAR0 0x10
#define PCI_CONFIG_BAR1 0x14
#define PCI_CONFIG_BAR2 0x18
#define PCI_CONFIG_BAR3 0x1C
#define PCI_CONFIG_BAR4 0x20
#define PCI_CONFIG_BAR5 0x24

#define PCI_BAR_IO     0x01
#define PCI_BAR_LOWMEM 0x02
#define PCI_BAR_64     0x04

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
    uint8_t header_type;

    uint8_t interrupt_line;

    uint32_t bar0;
    uint32_t bar1;
    uint32_t bar2;
    uint32_t bar3;
    uint32_t bar4;
    uint32_t bar5;
} pci_device_t;

typedef void(*pci_enumerator_t)(pci_device_t);

void enumerate_pci(pci_enumerator_t enumerator);

#endif
