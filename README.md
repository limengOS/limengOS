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
....
Board: BeagleBone Black
BeagleBone Black:
BeagleBone Cape EEPROM: no EEPROM at address: 0x54
BeagleBone Cape EEPROM: no EEPROM at address: 0x55
BeagleBone Cape EEPROM: no EEPROM at address: 0x56
BeagleBone Cape EEPROM: no EEPROM at address: 0x57
Net:   eth2: ethernet@4a100000, eth3: usb_ether
Press SPACE to abort autoboot in 0 seconds
=> set ipaddr 192.168.3.100
=> set serverip 192.168.3.23
=> saveenv
Saving Environment to EXT4... File System is consistent
update journal finished
done
OK
=> tftp bootstrap.uimage;bootm
....
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
wget    | --1970-01-01 00:00:05--  http://mirrors.bupt.edu.cn/ubuntu-releases/20.04.6/ubuntu-20.04.6-live-server-amd64.iso.zsync
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



