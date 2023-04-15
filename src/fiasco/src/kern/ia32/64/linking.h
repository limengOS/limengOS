#pragma once

#include "globalconfig.h"

#define FIASCO_MP_TRAMP_PAGE     0x1000   // must be below 1MB
#define FIASCO_IMAGE_PHYS_START  0x400000
//#define FIASCO_IMAGE_PHYS_START  0x2000
#define FIASCO_IMAGE_VIRT_START  0xfffffffff0000000
#ifndef CONFIG_KERNEL_NX
#define FIASCO_IMAGE_VIRT_SIZE   0x400000 // must be superpage-aligned
#else
#define FIASCO_IMAGE_VIRT_SIZE   0x600000 // must be superpage-aligned
#endif
#define FIASCO_KENTRY_SYSCALL_PAGE 0xffff817fffff8000
#define FIASCO_IMAGE_PHYS_OFFSET (FIASCO_IMAGE_VIRT_START - (FIASCO_IMAGE_PHYS_START & 0xffffffffffc00000))
