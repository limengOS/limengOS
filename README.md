LimengOS 
==============

The LimengOS is based on L4Re snapshots (ver: 22.04)  OS framework

 LimengOS  is the L4Re microkernel that develop new Device Driver Environment(DDE)

 by linux-3.2.0 to build device drivers environment for Arm AM335x devices.



## Develope drivers for AM335x 

* Power and Clock Management

* Control module

* Ethernet subsystem

* The enhanced direct memory access (EDMA)

* General-purpose memory controller (GPMC)

* Mtd Nand driver

* Multimedia Card (MMC)

* I2c and Gpio

  

## Implement Virtual File Systems

* Ubifs filesystem

* Ext4 filesystem

## Implement  Tcp/Ip Stack 
* Lwip-2.1.2

*    http server

*    wget implement



# Using BeagleBone with an SD (microSD) card

Need SD card to run Pre-buit Image

1. Create the second partition of sd card  as Ext4 filesystem
2. Turn off `metadata_csum`

```
$ ls /dev/sdc*
/dev/sdc  /dev/sdc1  /dev/sdc2
$ sudo tune2fs -O ^metadata_csum  /dev/sdc2
```

3. It's better to mkdir /boot on /dev/sdc2 used for save Uboot  Environment later
4. Insert sd card to board

# Pre-built Image for BeagleBone  Black(BBB)

```
pre-built-image/bootstrap.uimage

$ cp pre-built-image/bootstrap.uimage  /tftpboot
note: have installed tftp-server and set TFTP_DIRECTORY="/tftpboot"
       
$ sudo minicom -D /dev/ttyUSB0

To power board:
U-Boot SPL 2021.10-00011-g6988a04622-dirty (Nov 28 2022 - 14:47:54 +0800)
Trying to boot from MMC2


U-Boot 2021.10-00011-g6988a04622-dirty (Nov 28 2022 - 14:47:54 +0800)

CPU  : AM335X-GP rev 2.1
Model: TI AM335x BeagleBone Black
DRAM:  512 MiB
Reset Source: Global external warm reset has occurred.
Reset Source: Global warm SW reset has occurred.
Reset Source: Power-on reset has occurred.
RTC 32KCLK Source: External.
WDT:   Started with servicing (60s timeout)
MMC:   OMAP SD/MMC: 0, OMAP SD/MMC: 1
Loading Environment from EXT4... OK
Board: BeagleBone Black
BeagleBone Black:
BeagleBone Cape EEPROM: no EEPROM at address: 0x54
BeagleBone Cape EEPROM: no EEPROM at address: 0x55
BeagleBone Cape EEPROM: no EEPROM at address: 0x56
BeagleBone Cape EEPROM: no EEPROM at address: 0x57
Net:   eth2: ethernet@4a100000, eth3: usb_ether
Press SPACE to abort autoboot in 1 seconds
ethernet@4a100000 Waiting for PHY auto negotiation to complete. done
link up on port 0, speed 100, full duplex
Using ethernet@4a100000 device
TFTP from server 192.168.3.23; our IP address is 192.168.3.100
Filename 'bootstrap.uimage'.
Load address: 0x82000000
Loading: ##################################################  7.6 MiB
         3.8 MiB/s
done
Bytes transferred = 7982708 (79ce74 hex)
## Booting kernel from Legacy Image at 82000000 ...
   Image Name:   L4 Image #159
   Created:      2023-04-10   7:53:51 UTC
   Image Type:   ARM Linux Kernel Image (uncompressed)
   Data Size:    7982644 Bytes = 7.6 MiB
   Load Address: 81000000
   Entry Point:  81000000
   Verifying Checksum ... OK
   Loading Kernel Image

Starting kernel ...


L4 Bootstrapper
  Build: #159 Mon Apr 10 15:53:49 CST 2023, 9.4.0
  Scanning up to 512 MB RAM, starting at offset 32MB
  Memory size is 512MB (80000000 - 9fffffff)
  RAM: 0000000080000000 - 000000009fffffff: 524288kB
  Total RAM: 512MB
  Scanning fiasco
  Scanning sigma0
  Scanning moe
  Moving up to 15 modules behind 81100000
  moving module 14 { 81798000-8179ce33 } -> { 81888000-8188ce33 } [20020]
  moving module 13 { 81735000-81797aa7 } -> { 81825000-81887aa7 } [404136]
  moving module 12 { 816a9000-81734b87 } -> { 81799000-81824b87 } [572296]
  moving module 11 { 8161f000-816a82d7 } -> { 8170f000-817982d7 } [561880]
  moving module 10 { 81595000-8161e2d7 } -> { 81685000-8170e2d7 } [561880]
  moving module 09 { 8132d000-81594edb } -> { 8141d000-81684edb } [2522844]
  moving module 08 { 812c3000-8132c98f } -> { 813b3000-8141c98f } [432528]
  moving module 07 { 810e7000-812c240b } -> { 811d7000-813b240b } [1946636]
  moving module 06 { 810d1000-810e661f } -> { 811c1000-811d661f } [87584]
  moving module 05 { 810d0000-810d0ea5 } -> { 811c0000-811c0ea5 } [3750]
  moving module 04 { 810ce000-810cf401 } -> { 811be000-811bf401 } [5122]
  moving module 03 { 810cd000-810cdfa9 } -> { 811bd000-811bdfa9 } [4010]
  moving module 02 { 8109b000-810cc717 } -> { 8118b000-811bc717 } [202520]
  moving module 01 { 8108f000-8109a3e3 } -> { 8117f000-8118a3e3 } [46052]
  moving module 00 { 81010000-8108e87b } -> { 81100000-8117e87b } [518268]
  Loading fiasco
  find kernel info page...
  found kernel info page (via ELF) at 80002000
  Loading sigma0
  Loading moe
Regions of list 'regions'
    [ 80000000,  80000fff] {     1000} Root   mbi_rt
    [ 80001000,  8008bfff] {    8b000} Kern   fiasco
    [ 80100000,  80110233] {    10234} Sigma0 sigma0
    [ 80140000,  8016fd83] {    2fd84} Root   moe
    [ 801713f0,  8017d577] {     c188} Root   moe
    [ 81000000,  8100e7ab] {     e7ac} Boot   bootstrap
    [ 810000d0,  81000143] {       74} Root   cpu_boot
    [ 8100f198,  8100fc5b] {      ac4} Boot   modinfo
    [ 811bd000,  8188ce33] {   6cfe34} Root   Module
  found kernel options (via ELF) at 80003000
  Sigma0 config    ip:801003ac sp:00000000
  Roottask config  ip:80141c0c sp:00000000
  Starting kernel fiasco at 80001234
Hello from Startup::stage2
FPU: Initialize
FPU0: Subarch: 3, Part: 30, Rev: 3, Var: c, Impl: 41
SERIAL ESC: allocated IRQ 72 for serial uart
Not using serial hack in slow timer handler.
Welcome to L4/Fiasco.OC!
L4/Fiasco.OC microkernel on arm
Rev: fd982fa80-dirty compiled with gcc 6.2.1 20161016 for AM33xx    []
Build: #4 Fri Mar 31 18:10:01 CST 2023

Calibrating timer loop... done.
MDB: use page size: 20
MDB: use page size: 12
SIGMA0: Hello!
  KIP @ 80002000
  allocated 4KB for maintenance structures
SIGMA0: Dump of all resource maps
RAM:------------------------
[4:RWX:80000000;80000fff]
[0:RWX:8008c000;800fffff]
[0:RWX:80111000;8013ffff]
[4:R-X:80140000;8016ffff]
[0:RWX:80170000;80170fff]
[4:RW-:80171000;8017dfff]
[0:RWX:8017e000;80ffffff]
[4:---:81000000;81000fff]
[0:RWX:81001000;811bcfff]
[4:RWX:811bd000;8188cfff]
[0:RWX:8188d000;9effffff]
IOMEM:----------------------
[0:RW-:0;7fffffff]
[0:RW-:a0000000;ffffffff]
MOE: Hello world
MOE: found 500060 KByte free memory
MOE: found RAM from 80000000 to 9f000000
MOE: allocated 496 KByte for the page array @0x8017e000
MOE: virtual user address space [0-bfffffff]
MOE: rom name space cap -> [C:103000]
MOE: rwfs name space cap -> [C:105000]
  BOOTFS: [811bd000-811bdfaa] [C:107000] ankh.cfg
  BOOTFS: [811be000-811bf402] [C:109000] ankh.io
  BOOTFS: [811c0000-811c0ea6] [C:10b000] Aw.lua
  BOOTFS: [811c1000-811d6620] [C:10d000] l4re
  BOOTFS: [811d7000-813b240c] [C:10f000] io
  BOOTFS: [813b3000-8141c990] [C:111000] ned
  BOOTFS: [8141d000-81684edc] [C:113000] ankh
  BOOTFS: [81685000-8170e2d8] [C:115000] morpong
  BOOTFS: [8170f000-817982d8] [C:117000] morping
  BOOTFS: [81799000-81824b88] [C:119000] wget_clnt
  BOOTFS: [81825000-81887aa8] [C:11b000] lwip_test
  BOOTFS: [81888000-8188ce34] [C:11d000] L4Re.html
MOE: cmdline: moe rom/ankh.cfg
MOE: Starting: rom/ned rom/ankh.cfg
MOE: loading 'rom/ned'
Ned says: Hi World!
Ned: loading file: 'rom/ankh.cfg'
io      | L4Re[ldr]:   STACK: 80000000 (8000)    KIP: affff000
io      | L4Re[ldr]:   PHDRs: type  offset      paddr   vaddr   filesz  memsz   rights
io      | L4Re[ldr]:    [    70000001] 0x115c18 0x1115c18       0x1115c18       0x40e0  0x40e0  r--
io      | L4Re[ldr]:    [PHDR        ] 0x34     0x1000034       0x1000034       0xc0    0xc0    r--
io      | L4Re[ldr]:    [LOAD        ] 0x0      0x1000000       0x1000000       0x119cfc        0x119cfc        r-x
io      | L4Re[ldr]:    [LOAD        ] 0x11a6a8 0x111b6a8       0x111b6a8       0xc089f 0xc5bb0 rw-
io      | L4Re[ldr]:    [TLS         ] 0x11a6a8 0x111b6a8       0x111b6a8       0x0     0x14    rw-
io      | L4Re[ldr]:    [L4_AUX      ] 0xf5968  0x10f5968       0x10f5968       0xc     0xc     r--
io      | L4Re[ldr]:  done...
io      | Io service
io      | Verboseness level: 4
io      | unused physical memory space:
io      |   [00000000000000-0000007fffffff]
io      |   [000000a0000000-000000ffffffff]
io      | no 'iommu' capability found, using CPU-phys for DMA
io      | Loading: config 'rom/ankh.io'
io      | Real Hardware -----------------------------------
io      | System Bus: 
io      |   Resources: ==== start ====
io      |   DMADOM  [00000000000000-00000000000000 1] (align=0 flags=6)
io      |   Resources: ===== end =====
io      |   BARDEVICE: hid=dev-bar,Example
io      |     compatible= { "dev-bar,mmio", "dev-bar" }
io      |     Resources: ==== start ====
io      |     IRQ     [00000000000014-00000000000014 1] none (align=0 flags=1)
io      |     IRQ     [00000000000013-00000000000013 1] none (align=0 flags=1)
io      |     IOMEM   [0000006f100000-0000006f100fff 1000] 32-bit non-pref (align=fff flags=2)
io      |     DMADOM  [00000000000000-00000000000000 1] (align=0 flags=6)
io      |     Resources: ===== end =====
io      |   FOODEVICE: hid=dev-cm,Example
io      |     compatible= { "dev-cm,mmio" }
io      |     Clients: ===== start ====
io      |       dev: [N2Vi9Proxy_devE]
io      |     Clients: ===== end ====
io      |     Resources: ==== start ====
io      |     IOMEM   [00000048080000-0000004808ffff 10000] 32-bit non-pref (align=ffff flags=2)
io      |     IRQ     [0000000000001e-0000000000001e 1] none (align=0 flags=1)
io      |     IRQ     [00000000000045-00000000000045 1] none (align=0 flags=1)
io      |     IRQ     [00000000000040-00000000000040 1] none (align=0 flags=1)
io      |     IRQ     [00000000000044-00000000000044 1] none (align=0 flags=1)
io      |     IOMEM   [00000048060000-00000048061fff 2000] 32-bit non-pref (align=1fff flags=2)
io      |     IOMEM   [000000481ae000-000000481aefff 1000] 32-bit non-pref (align=fff flags=2)
io      |     IOMEM   [00000048200000-00000048200fff 1000] 32-bit non-pref (align=fff flags=2)
io      |     IRQ     [0000000000005d-0000000000005d 1] none (align=0 flags=1)
io      |     IRQ     [00000000000014-00000000000014 1] none (align=0 flags=1)
io      |     IRQ     [00000000000021-00000000000021 1] none (align=0 flags=1)
io      |     IOMEM   [000000481d8000-000000481d9fff 2000] 32-bit non-pref (align=1fff flags=2)
io      |     IRQ     [0000000000001a-0000000000001a 1] none (align=0 flags=1)
io      |     IOMEM   [0000004819c000-000000481aefff 13000] 32-bit non-pref (align=12fff flags=2)
io      |     IOMEM   [0000004a300000-0000004a33ffff 40000] 32-bit non-pref (align=3ffff flags=2)
io      |     IRQ     [00000000000017-00000000000017 1] none (align=0 flags=1)
io      |     IOMEM   [00000050000000-0000005fffffff 10000000] 32-bit non-pref (align=fffffff flags=2)
io      |     IRQ     [0000000000001b-0000000000001b 1] none (align=0 flags=1)
io      |     IOMEM   [00000008000000-00000010000000 8000001] 32-bit non-pref (align=8000000 flags=2)
io      |     IOMEM   [00000049a00000-00000049a01fff 2000] 32-bit non-pref (align=1fff flags=2)
io      |     IOMEM   [00000044e00000-00000044e3ffff 40000] 32-bit non-pref (align=3ffff flags=2)
io      |     IRQ     [00000000000062-00000000000062 1] none (align=0 flags=1)
io      |     IRQ     [0000000000002b-0000000000002b 1] none (align=0 flags=1)
io      |     IOMEM   [00000047810000-00000047820fff 11000] 32-bit non-pref (align=10fff flags=2)
io      |     IRQ     [00000000000015-00000000000015 1] none (align=0 flags=1)
io      |     IRQ     [0000000000001d-0000000000001d 1] none (align=0 flags=1)
io      |     IRQ     [00000000000046-00000000000046 1] none (align=0 flags=1)
io      |     IRQ     [0000000000004e-0000000000004e 1] none (align=0 flags=1)
io      |     IRQ     [00000000000018-00000000000018 1] none (align=0 flags=1)
io      |     IRQ     [00000000000049-00000000000049 1] none (align=0 flags=1)
io      |     IOMEM   [000000480c8000-000000480c8fff 1000] 32-bit non-pref (align=fff flags=2)
io      |     IRQ     [0000000000005e-0000000000005e 1] none (align=0 flags=1)
io      |     IOMEM   [00000049000000-00000049007fff 8000] 32-bit non-pref (align=7fff flags=2)
io      |     IRQ     [0000000000000d-0000000000000d 1] none (align=0 flags=1)
io      |     IOMEM   [000000481ac000-000000481acfff 1000] 32-bit non-pref (align=fff flags=2)
io      |     IRQ     [00000000000061-00000000000061 1] none (align=0 flags=1)
io      |     IOMEM   [00000048022000-0000004802bfff a000] 32-bit non-pref (align=9fff flags=2)
io      |     IOMEM   [00000049800000-00000049801fff 2000] 32-bit non-pref (align=1fff flags=2)
io      |     IRQ     [00000000000019-00000000000019 1] none (align=0 flags=1)
io      |     IRQ     [00000000000028-00000000000028 1] none (align=0 flags=1)
io      |     IOMEM   [00000048030000-00000048030fff 1000] 32-bit non-pref (align=fff flags=2)
io      |     IRQ     [0000000000005b-0000000000005b 1] none (align=0 flags=1)
io      |     IOMEM   [00000048002000-00000048007fff 6000] 32-bit non-pref (align=5fff flags=2)
io      |     IOMEM   [0000004c000000-0000004c007fff 8000] 32-bit non-pref (align=7fff flags=2)
io      |     IRQ     [0000000000000e-0000000000000e 1] none (align=0 flags=1)
io      |     IOMEM   [00000049900000-00000049901fff 2000] 32-bit non-pref (align=1fff flags=2)
io      |     IRQ     [00000000000060-00000000000060 1] none (align=0 flags=1)
io      |     IRQ     [00000000000016-00000000000016 1] none (align=0 flags=1)
io      |     IRQ     [0000000000001c-0000000000001c 1] none (align=0 flags=1)
io      |     IOMEM   [0000004a100000-0000004a107fff 8000] 32-bit non-pref (align=7fff flags=2)
io      |     IOMEM   [00000048040000-0000004804cfff d000] 32-bit non-pref (align=cfff flags=2)
io      |     IRQ     [00000000000047-00000000000047 1] none (align=0 flags=1)
io      |     IRQ     [0000000000004a-0000000000004a 1] none (align=0 flags=1)
io      |     IRQ     [0000000000003f-0000000000003f 1] none (align=0 flags=1)
io      |     IRQ     [0000000000003e-0000000000003e 1] none (align=0 flags=1)
io      |     IRQ     [0000000000005c-0000000000005c 1] none (align=0 flags=1)
io      |     IRQ     [00000000000020-00000000000020 1] none (align=0 flags=1)
io      |     IRQ     [00000000000063-00000000000063 1] none (align=0 flags=1)
io      |     IRQ     [0000000000005f-0000000000005f 1] none (align=0 flags=1)
io      |     IRQ     [00000000000064-00000000000064 1] none (align=0 flags=1)
io      |     IRQ     [0000000000000c-0000000000000c 1] none (align=0 flags=1)
io      |     IRQ     [0000000000004d-0000000000004d 1] none (align=0 flags=1)
io      |     IRQ     [00000000000004-00000000000004 1] none (align=0 flags=1)
io      |     Resources: ===== end =====
io      | warning: could not register control interface at cap 'platform_ctl'
io      | Ready. Waiting for request.
ankh    | Initializing DDE page cache
ankh    | pcpu-alloc: s0 r0 d32768 u32768 alloc=1*32768
ankh    | pcpu-alloc: [0] 0 
ankh    | On node 0 totalpages: 51200
ankh    | free_area_init_node: node 0, pgdat 0123ab48, node_mem_map 00000000
ankh    |   Normal zone: 0 pages reserved
ankh    |   Normal zone: 51200 pages, LIFO batch:15
ankh    | Built 1 zonelists in Zone order, mobility grouping on.  Total pages: 51200
ankh    | Initialized DDELinux 
ankh    | Softirq daemon starting
ankh    | Dentry cache hash table entries: 1024 (order: 0, 4096 bytes)
ankh    | Inode-cache hash table entries: 1024 (order: 0, 4096 bytes)
ankh    | Mount-cache hash table entries: 512
ankh    | NR_IRQS:396
ankh    | Initialized Linux driver_init 
io      | new iomem region: p=00000044c00000 v=00000000400000 s=400000 (bmb=0x2a7d8)
ankh    | lim_request_mem_region: region pa=44e00000, va=94e00000, ln=40000, start:44e00000
io      | new iomem region: p=0000004a000000 v=00000000800000 s=400000 (bmb=0x2a880)
ankh    | lim_request_mem_region: region pa=4a100000, va=9a100000, ln=8000, start:4a100000
io      | new iomem region: p=0000004c000000 v=00000000c00000 s=400000 (bmb=0x2a928)
ankh    | lim_request_mem_region: region pa=4c000000, va=9c000000, ln=8000, start:4c000000
io      | new iomem region: p=00000048000000 v=00000001400000 s=400000 (bmb=0x2a9d0)
ankh    | lim_request_mem_region: region pa=48002000, va=98002000, ln=6000, start:48002000
ankh    | lim_request_mem_region: region pa=48022000, va=98022000, ln=a000, start:48022000
ankh    | lim_request_mem_region: region pa=48030000, va=98030000, ln=1000, start:48030000
ankh    | lim_request_mem_region: region pa=48040000, va=98040000, ln=d000, start:48040000
ankh    | lim_request_mem_region: region pa=48080000, va=98080000, ln=10000, start:48080000
ankh    | lim_request_mem_region: region pa=480c8000, va=980c8000, ln=1000, start:480c8000
ankh    | lim_request_mem_region: region pa=4819c000, va=9819c000, ln=13000, start:4819c000
ankh    | lim_request_mem_region: region pa=48200000, va=98200000, ln=1000, start:48200000
io      | new iomem region: p=00000049000000 v=00000001800000 s=400000 (bmb=0x2aa78)
ankh    | lim_request_mem_region: region pa=49000000, va=99000000, ln=8000, start:49000000
io      | new iomem region: p=00000049800000 v=00000001c00000 s=400000 (bmb=0x2ab20)
ankh    | lim_request_mem_region: region pa=49800000, va=99800000, ln=2000, start:49800000
ankh    | lim_request_mem_region: region pa=49900000, va=99900000, ln=2000, start:49900000
ankh    | lim_request_mem_region: region pa=49a00000, va=99a00000, ln=2000, start:49a00000
ankh    | lim_request_mem_region: region pa=4a300000, va=9a300000, ln=40000, start:4a300000
io      | new iomem region: p=00000050000000 v=00000010000000 s=10000000 (bmb=0x2abc8)
ankh    | lim_request_mem_region: region pa=50000000, va=a0000000, ln=1000000, start:50000000
io      | new iomem region: p=00000000000000 v=00000020000000 s=20000000 (bmb=0x2cbf0)
ankh    | lim_request_mem_region: region pa=8000000, va=58000000, ln=10000000, start:8000000
ankh    | lim_request_mem_region: region pa=48060000, va=98060000, ln=2000, start:48060000
ankh    | lim_request_mem_region: region pa=481d8000, va=981d8000, ln=2000, start:481d8000
io      | new iomem region: p=00000047800000 v=00000002000000 s=400000 (bmb=0x30c18)
ankh    | lim_request_mem_region: region pa=47810000, va=97810000, ln=11000, start:47810000
ankh    | MMM--lim_ini-
ankh    | AM335X ES1.0 (sgx neon )
ankh    | omap_hwmod: omap_hwmod_for_each_by_class: looking for modules of class wd_timer
ankh    | omap_hwmod: omap_hwmod_for_each_by_class: wd_timer2: calling callback fn
ankh    | <c>9961.47 BogoMIPS (lpj=4980736)
ankh    | print_constraints: dummy: 
ankh    | Total of 128 interrupts on 1 active controller
ankh    | omap_hwmod: gfx: failed to hardreset
ankh    | omap_hwmod: pruss: failed to hardreset
ankh    | omap_hwmod: omap_hwmod_for_each_by_class: looking for modules of class gpio
ankh    | omap_hwmod: omap_hwmod_for_each_by_class: gpio1: calling callback fn
ankh    | omap_hwmod: omap_hwmod_for_each_by_class: gpio2: calling callback fn
ankh    | omap_hwmod: omap_hwmod_for_each_by_class: gpio3: calling callback fn
ankh    | omap_hwmod: omap_hwmod_for_each_by_class: gpio4: calling callback fn
ankh    | Thread for IRQ 96
ankh    | OMAP GPIO hardware version 0.1
ankh    | Thread for IRQ 98
ankh    | Thread for IRQ 32
ankh    | Thread for IRQ 62
ankh    | omap_mux_init: Add partition: #1: core, flags: 0
ankh    |  omap_i2c.1: alias fck already exists
ankh    | omap_hwmod: omap_hwmod_for_each_by_class: looking for modules of class timer
ankh    | omap_hwmod: omap_hwmod_for_each_by_class: timer0: calling callback fn
ankh    | omap_hwmod: omap_hwmod_for_each_by_class: timer2: calling callback fn
ankh    | omap_hwmod: omap_hwmod_for_each_by_class: timer3: calling callback fn
ankh    | omap_hwmod: omap_hwmod_for_each_by_class: timer4: calling callback fn
ankh    | omap_hwmod: omap_hwmod_for_each_by_class: timer5: calling callback fn
ankh    | omap_hwmod: omap_hwmod_for_each_by_class: timer6: calling callback fn
ankh    | omap_hwmod: omap_hwmod_for_each_by_class: timer7: calling callback fn
ankh    | omap_hwmod: omap_hwmod_for_each_by_class: looking for modules of class mcspi
ankh    | omap_hwmod: omap_hwmod_for_each_by_class: spi0: calling callback fn
ankh    |  omap2_mcspi.1: alias fck already exists
ankh    | omap_hwmod: omap_hwmod_for_each_by_class: spi1: calling callback fn
ankh    |  omap2_mcspi.2: alias fck already exists
ankh    |  edma.0: alias fck already exists
ankh    |  edma.0: alias fck already exists
ankh    |  edma.0: alias fck already exists
ankh    | l4dde26_register_rx_callback: New rx callback @ 0x100295c.
ankh    | io scheduler noop registered
ankh    | bio: create slab <bio-0> at 0
ankh    | Thread for IRQ 70
ankh    | omap_i2c omap_i2c.1: bus 1 rev2.4.0 at 100 kHz
ankh    | Thread for IRQ 12
ankh    | Thread for IRQ 14
ankh    | io scheduler cfq registered (default)
ankh    | i2c-core: driver [tsl2550] using legacy suspend method
ankh    | i2c-core: driver [tsl2550] using legacy resume method
ankh    | at24 1-0050: 32768 byte 24c256 EEPROM, writable, 64 bytes/write
ankh    | Board name: A335BNLT
ankh    | Board version: 00C0
ankh    | The board is a AM335x Beaglebone.
ankh    | tps65217 1-0024: TPS65217 ID 0xe version 1.2
ankh    | print_constraints: DCDC1: 900 <--> 1800 mV at 1500 mV 
ankh    | print_constraints: DCDC2: 900 <--> 3300 mV at 1325 mV 
ankh    | print_constraints: DCDC3: 900 <--> 1500 mV at 1100 mV 
ankh    | print_constraints: LDO1: 1000 <--> 3300 mV at 1800 mV 
ankh    | print_constraints: LDO2: 900 <--> 3300 mV at 3300 mV 
ankh    | print_constraints: LDO3: 1800 <--> 3300 mV at 1800 mV 
ankh    | print_constraints: LDO4: 1800 <--> 3300 mV at 3300 mV 
ankh    |  omap_hsmmc.0: alias fck already exists
ankh    |  omap_hsmmc.1: alias fck already exists
ankh    | in master_led_pin_mux_init(), .....................
ankh    | in master_other_pin_mux_init(), .....................
ankh    | _omap_mux_get_by_name: Could not find signal gpmc_ben1.gpio1_30
ankh    | enable sim7600_power_on pin...
ankh    | in rtc_init(), , add rtc_i2c_boardinfo into am335x_i2c_boardinfo................
ankh    | in am33xx_cpsw_init, cpsw_slaves[0] = 84:AC:A4:FF:9F:1D
ankh    | in am33xx_cpsw_init, cpsw_slaves[1] = 00:00:00:00:00:00
ankh    | at24 1-0051: 32768 byte 24c256 EEPROM, writable, 64 bytes/write
ankh    | No daughter card found
ankh    | init_mtdblock..............
ankh    | mtdoops: mtd device (mtddev=name/number) must be supplied
ankh    | Thread for IRQ 64
ankh    | Thread for IRQ 28
ankh    | VFS: Disk quotas dquot_6.5.2
ankh    | Dquot-cache hash table entries: 1024 (order 0, 4096 bytes)
ankh    | omap2-nand driver initializing
ankh    | OneNAND driver initializing
ankh    | UBI error: ubi_init: UBI error: cannot initialize UBI, error -19
ankh    | Detected MACID=84:ac:a4:ff:9f:1d
ankh    | Thread for IRQ 40
ankh    | Thread for IRQ 93
ankh    | Thread for IRQ 94
ankh    | Thread for IRQ 43
ankh    | Enter mmc_attach sd++++++++++++++++
ankh    | in mmc_sd_attack_bus_ops,rm_ret=0
ankh    | in mmc_sd_attack_bus_ops,attach mmc_sd_ops_unsafe
ankh    | in mmc_attach sd,call mmc_sd_init_card
ankh    | davinci_mdio davinci_mdio.0: davinci mdio revision 1.6
ankh    | davinci_mdio davinci_mdio.0: detected phy mask fffffffe
ankh    | davinci_mdio.0: probed
ankh    | davinci_mdio davinci_mdio.0: phy[0]: device 0:00, driver SMSC LAN8710/LAN8720
ankh    | Power Management for AM33XX family
ankh    | Thread for IRQ 77
ankh    | Thread for IRQ 78
ankh    | omap_hwmod: wkup_m3: wkup_m3: hwmod data error: OMAP4 does not support st_shift
ankh    | open_network_devices() 
ankh    | opening lo
ankh    | device lo entered promiscuous mode
ankh    | set interface to promiscuous mode.
ankh    | opening eth0
ankh    | device eth0 entered promiscuous mode
ankh    | set interface to promiscuous mode.
ankh    | 
ankh    | CPSW phy found : id is : 0x7c0f1
ankh    | PHY 0:01 not found
ankh    | Opened 2 network devices.
ankh    | lo    IRQ: 0x00  MAC: 00:00:00:00:00:00  MTU: 16436
ankh    | eth0  IRQ: 0x28  MAC: 84:AC:A4:FF:9F:1D  MTU:  1500
ankh    | Registered @ registry.
ankh    | Gooood Morning Ankh-Morpoooooork! TID = 0xffffffff
ankh    | Sysfs reserve_area buf_addr=0x3c0000, pages=16(16)
ankh    | Created Sysfs handler thread
ankh    | mmc0: host does not support reading read-only switch. assuming write-enable.
ankh    | in mmc_attach sd,call mmc_release_host
ankh    | mmc0: new high speed SDHC card at address aaaa
ankh    | mmcblk0: mmc0:aaaa SL08G 7.40 GiB 
ankh    |  mmcblk0: p1 p2
ankh    | End mmc_attach sd-------------------
ankh    | unimplemented: async_synchronize_full
ankh    | Enter mmc_attach sd++++++++++++++++
ankh    | mmc1: new high speed MMC card at address 0001
ankh    | mmcblk1: mmc1:0001 MK2704 3.53 GiB 
ankh    | mmcblk1boot0: mmc1:0001 MK2704 partition 1 2.00 MiB
ankh    | mmcblk1boot1: mmc1:0001 MK2704 partition 2 2.00 MiB
ankh    |  mmcblk1: p1
ankh    |  mmcblk1boot1: unknown partition table
ankh    |  mmcblk1boot0: unknown partition table
ankh    | EXT4-fs (mmcblk0p2): 0x80007be8V
ankh    | EXT4-fs (mmcblk0p2): 0x80007be8V
ankh    | VFS: Mounted root (ext4 filesystem) on device 179:2.
ankh    | Creating session factory.
ankh    | Configuration: debug,device=eth0,shm=shm_lwip,bufsize=16384
ankh    |   Debug mode ON.
ankh    |   Device 'eth0' requested.
ankh    |   SHM area 'shm_lwip' requested.
ankh    |   Buffer size: 16384
ankh    | shmc_create: 0
ankh    | Created shmc area 'shm_lwip'
ankh    | Assigning MAC address: 04:EA:90:03:A2:1B
ankh    | Configuration: debug,promisc,device=eth0,shm=shm_morpork,bufsize=16384,phys_mac
lwip    | L4Re: adding regions from remote region mapper
lwip    | L4Re: load binary 'rom/lwip_test'
lwip    | L4Re: Start server loop
lwip    | getopt: 1
lwip    | shm name: shm
lwip    | getopt: 0
lwip    | buf size: 16384
lwip    | getopt: -1
lwip    | add_chunk: area 0x106549c, name 'tx_ring', size 16420
lwip    | SND: signal RX ffffffff, TX ffffffff
lwip    | buf @ 0x1065434, area 0x106549c (shm), size 16420
lwip    |    owner 0, name tx_ring, signal tx_signal
lwip    |    head @ 0x20b030, lock state 'unlocked', data size 16384
lwip    |    next_rd 0 next_wr 0 filled 0, data @ 0x20b051
lwip    | add_chunk: area 0x106549c, name 'rx_ring', size 16420
lwip    | SND: signal RX ffffffff, TX ffffffff
lwip    | buf @ 0x1065464, area 0x106549c (shm), size 16420
lwip    |    owner 0, name rx_ring, signal rx_signal
lwip    |    head @ 0x20f07c, lock state 'unlocked', data size 16384
lwip    |    next_rd 0 next_wr 0 filled 0, data @ 0x20f09d
ankh    |   Debug mode ON.
ankh    |   Using promiscuous mode.
ankh    |   Device 'eth0' requested.
ankh    |   SHM area 'shm_morpork' requested.
ankh    |   Buffer size: 16384
ankh    |   Physical MAC requested.
ankh    | shmc_create: 0
ankh    | Created shmc area 'shm_morpork'
ankh    | Assigning MAC address: 84:AC:A4:FF:9F:1D
wget    | add_chunk: area 0x10950f4, name 'tx_ring', size 16420
wget    | SND: signal RX ffffffff, TX ffffffff
wget    | buf @ 0x109508c, area 0x10950f4 (shm_morpork), size 16420
wget    |    owner 0, name tx_ring, signal tx_signal
wget    |    head @ 0x40b030, lock state 'unlocked', data size 16384
wget    |    next_rd 0 next_wr 0 filled 0, data @ 0x40b051
wget    | add_chunk: area 0x10950f4, name 'rx_ring', size 16420
wget    | SND: signal RX ffffffff, TX ffffffff
wget    | buf @ 0x10950bc, area 0x10950f4 (shm_morpork), size 16420
wget    |    owner 0, name rx_ring, signal rx_signal
wget    |    head @ 0x40f07c, lock state 'unlocked', data size 16384
wget    |    next_rd 0 next_wr 0 filled 0, data @ 0x40f09d
ankh    | l4shmc_attach("shm_lwip") = 0
ankh    | Shm_chunk::get(0x2b3e4, "info", 36)
ankh    |  ... = 0x5f88
ankh    | RCV: signal RX ffffffff, TX ffffffff
ankh    | RCV: buf size 16420
ankh    | buf @ 0x178fc4, area 0x2b3e4 (shm_lwip), size 16420
ankh    |    owner 73667562, name rx_ring, signal rx_signal
ankh    |    head @ 0x53807c, lock state 'unlocked', data size 16384
ankh    |    next_rd 0 next_wr 0 filled 0, data @ 0x53809d
ankh    | SND: attaching to signal ffffffff (59f000)
ankh    |      buf @ 0x53807c
ankh    | xmit thread started, data @ 0x2b380, TID: 0xffffffff
ankh    | RCV: signal RX ffffffff, TX ffffffff
ankh    | RCV: buf size 16420
ankh    | buf @ 0x19f94, area 0x2b3e4 (shm_lwip), size 16420
ankh    |    owner 6d6873, name tx_ring, signal tx_signal
ankh    |    head @ 0x534030, lock state 'unlocked', data size 16384
ankh    |    next_rd 0 next_wr 0 filled 0, data @ 0x534051
ankh    | RCV: attaching to signal ffffffff (5a4000)
ankh    | 16384 ... 16384
lwip    | activated Ankh connection.
lwip    | SND: attaching to signal ffffffff (426000)
lwip    |      buf @ 0x20b030
lwip    | --> 04:EA:90:03:A2:1B <--
lwip    | RCV: attaching to signal ffffffff (427000)
lwip    | ANKHIF::Recv rb @ 0x1065464
lwip    | netif_add: 0x10635f4 (0x10635f4)
lwip    | dhcp_start()
lwip    | dhcp started
ankh    | l4shmc_attach("shm_morpork") = 0
ankh    | Shm_chunk::get(0x2b47c, "info", 36)
ankh    |  ... = 0x5fd0
ankh    | RCV: signal RX ffffffff, TX ffffffff
ankh    | RCV: buf size 16420
ankh    | buf @ 0x23fb4, area 0x2b47c (shm_morpork), size 16420
ankh    |    owner 75622c6b, name rx_ring, signal rx_signal
ankh    |    head @ 0x55407c, lock state 'unlocked', data size 16384
ankh    |    next_rd 0 next_wr 0 filled 0, data @ 0x55409d
ankh    | SND: attaching to signal ffffffff (5a9000)
ankh    |      buf @ 0x55407c
ankh    | xmit thread started, data @ 0x2b418, TID: 0xffffffff
ankh    | RCV: signal RX ffffffff, TX ffffffff
ankh    | RCV: buf size 16420
ankh    | buf @ 0x1ffac, area 0x2b47c (shm_morpork), size 16420
ankh    |    owner 0, name tx_ring, signal tx_signal
ankh    |    head @ 0x550030, lock state 'unlocked', data size 16384
ankh    |    next_rd 0 next_wr 0 filled 0, data @ 0x550051
ankh    | RCV: attaching to signal ffffffff (5ae000)
ankh    | 16384 ... 16384
wget    | activated Ankh connection.
wget    | SND: attaching to signal ffffffff (429000)
wget    |      buf @ 0x40b030
wget    | --> 84:AC:A4:FF:9F:1D <--
wget    | RCV: attaching to signal ffffffff (42a000)
wget    | ANKHIF::Recv rb @ 0x10950bc
wget    | dhcp start...
ankh    | PHY: 0:00 - Link is Up<c> - 100/Full<c>
lwip    | Network interface is up.
lwip    | IP: 192.168.3.26
lwip    | GW: 192.168.3.1
lwip    | to read file :rom/L4Re.html
lwip    | read file ret len = 20020
lwip    | to read file :/sys/test_new_dir/new.html
lwip    | read file ret len = 20020
lwip    | md5sum = c5067d0a4cb3513312918c078af34828
lwip    | socket created: 3
lwip    | bound to addr: -1
lwip    | listen(): 0
wget    | Network interface is up.
wget    | IP: 192.168.3.25
wget    | MASK: 255.255.255.0
wget    | GW: 192.168.3.1
wget    | --1970-01-01 00:00:06--  http://mirrors.bupt.edu.cn/ubuntu-releases/20.04.6/ubuntu-20.04.6-live-server-amd64.iso.zsync
wget    | Resolving mirrors.bupt.edu.cn... 211.68.71.120
wget    | Connecting to mirrors.bupt.edu.cn|211.68.71.120|:80... connected.
wget    | HTTP request sent, awaiting response... 200 OK
wget    | Length: 2905205 (2.8M) [application/octet-stream]
wget    | Saving to: `/sys/ubuntu-server-amd64.iso.zsync'
wget    | 
100%[======================================>] 2,905,205    640K/s   in 4.5s    
wget    | 
wget    | 1970-01-01 00:00:10 (637 KB/s) - `/sys/ubuntu-server-amd64.iso.zsync' saved [2905205/2905205]
wget    | 



```

Open the  browser.

Type lwip IP addesss : 192.168.3.26

 ![](https://github.com/limengOS/limengOS/blob/main/doc/pic/lwip_http.png)

BeagleBone Black :

  [https://beagleboard.org/getting-started](https://beagleboard.org/getting-started)



# Structural framework

![](https://github.com/limengOS/limengOS/blob/main/doc/pic/whole.jpg)





![](https://github.com/limengOS/limengOS/blob/main/doc/pic/vfs.jpg)



![](https://github.com/limengOS/limengOS/blob/main/doc/pic/lwip.jpg)



Host system requirements
========================

We are confident that the works on the following distributions:

* Debian 10 or later
* Ubuntu 20.04 or later



Cross Compiling for ARM
-----------------------

On Ubuntu

```
sudo apt install gcc-arm-linux-gnueabihf
sudo apt install g++-arm-linux-gnueabihf

#for make config & others
sudo apt install device-tree-compiler
sudo apt install flex
sudo apt install bison
sudo apt install gawk
sudo apt install u-boot-tools 
```



Configuring yourself
====================

you can do this yourself for your specific targets.

Generally, the microkernel is built for a very specific target, i.e. it is
build for a SoC, such as ARM's AM335x .


Configure the L4Re microkernel aka Fiasco 
-----------------------------------------

Within the layout build directories for Fiasco are created under
`obj/fiasco`. To create a build directory, go to `src/kernel/fiasco` and do:

    $ cd src/fiasco
    $ make B=../../obj/fiasco/builddir
    Creating build directory "../../obj/fiasco/builddir"...
    done.

This will have created a build directory, go there and configure it
according to your requirements:

    $ cd ../../obj/fiasco/builddir
    $ make config

Select 

​            Architecture (ARM processor family)  --->

​            Platform (TI OMAP)  --->

​            OMAP Platform (TI AM33xx)  --->



`make config` will open up a configuration menu. Configure Fiasco as
required. Finally save the configuration and build:

    $ make -j4

When successful, this will create a file `fiasco` in the build directory.


Configure L4Re User-Level Infrastructure
----------------------------------------

Within the layout build directories for the L4Re user-level
infrastructure are under `obj/l4`. To create a build directory, go to
`src/l4` and do:

    $ cd src/l4
    $ make B=../../obj/l4/builddir

This will have created a build directory, go there and configure
it according to your requirements:

    $ cd ../../obj/l4/builddir
    $ make config

Select  

​             Target Architecture (ARM architecture)   --->

​             CPU variant  (ARMv7A type CPU)  --->

​             Platform Selection (Beagleboard)  --->



`make config` will open up a configuration menu. Configure as
required. Finally save the configuration build:

    $ make -j4

Building will compile all the components of L4Re, however, it will not build
an image that you can load on the target.


Pulling it together
-------------------

For creating images to load on the target, the image building step
needs to know where all the files can be found to include in the image.
The image contains all the executable program files of the setup to build,
including the Fiasco kernel, but also other files that are necessary
to run the setup, such as configuration files,  or data files.

The most relevant variable in that file is `MODULE_SEARCH_PATH` which
defines where the image building process shall look for files. This variable
has absolute paths separated with either spaces or colons (':').
For the examples to work, we need to add the path to the Fiasco
build directory as you have chosen in the above building step.
Change the line accordingly.

When done, you can proceed to build an image. Go to the l4 build directory
and create an image. You can create ELF, uimage and raw images, chose
whichever one you need for your target's boot loader. For example:

    $ obj/l4/builddir
    
    $ make uimage E=ankh MODULES_LIST=${PWD}/../../../src/l4/conf/ankh.list MODULE_SEARCH_PATH=${PWD}/../../../obj/fiasco/builddir:${PWD}/../../../src/l4/conf
    $ cp images/bootstrap.uimage /tftpboot/


This will finally build the image. 



