-- vim:set ft=lua:

--Example configuration for io

--Configure two platform devices to be known to io

Io.Dt.add_children(Io.system_bus(),function()

--create a new hardware device called"FOODEVICE"

FOODEVICE=Io.Hw.Device(function()

--set the compatibility IDs for this device

--a client tries to match against these IDs and configures

--itself accordingly

--the list should be sorted from specific to less specific IDs

    compatible={"dev-cm,mmio"};

--set the‘hid’property of the device,the hid can also be used

--as a compatible ID when matching clients

    Property.hid="dev-cm,Example";

--note:names for resources are truncated to 4 letters and a client

--can determine the name from the ID field of a l4vbus_resource_t

--add two resources‘irq0’and‘reg0’to the device

    Resource.irq0=Io.Res.irq(30);
    Resource.irq1=Io.Res.irq(40);
    Resource.irq2=Io.Res.irq(43);
    Resource.irq3=Io.Res.irq(68);
    Resource.irq4=Io.Res.irq(69);
    Resource.irq5=Io.Res.irq(70);
    Resource.irq6=Io.Res.irq(71);
    Resource.irq7=Io.Res.irq(73);
    Resource.irq8=Io.Res.irq(74);
    Resource.irq9=Io.Res.irq(77);
    Resource.irq10=Io.Res.irq(91);
    Resource.irq11=Io.Res.irq(92);
    Resource.irq12=Io.Res.irq(93);
    Resource.irq13=Io.Res.irq(94);
    Resource.irq14=Io.Res.irq(95);
    Resource.irq15=Io.Res.irq(100);
    Resource.irq16=Io.Res.irq(20);
    Resource.irq17=Io.Res.irq(21);
    Resource.irq18=Io.Res.irq(22);
    Resource.irq19=Io.Res.irq(23);
    Resource.irq20=Io.Res.irq(24);
    Resource.irq21=Io.Res.irq(25);
    Resource.irq22=Io.Res.irq(26);
    Resource.irq23=Io.Res.irq(27);
    Resource.irq24=Io.Res.irq(78);
    Resource.irq25=Io.Res.irq(4);
    Resource.irq26=Io.Res.irq(28);
    Resource.irq27=Io.Res.irq(29);
    Resource.irq28=Io.Res.irq(64);      --mmc0 MMCHS0 
    Resource.irq29=Io.Res.irq(96);      --gpio0
    Resource.irq30=Io.Res.irq(97);      --gpio0
    Resource.irq31=Io.Res.irq(98);      --gpio1
    Resource.irq32=Io.Res.irq(99);      --gpio1
    Resource.irq33=Io.Res.irq(32);      --gpio2
    Resource.irq34=Io.Res.irq(33);      --gpio2
    Resource.irq35=Io.Res.irq(62);      --gpio3
    Resource.irq36=Io.Res.irq(63);      --gpio3
    Resource.irq37=Io.Res.irq(12);      --tpcc_edma
    Resource.irq38=Io.Res.irq(13);      --tpcc_edma
    Resource.irq39=Io.Res.irq(14);      --tpcc_edma

    Resource.reg0=Io.Res.mmio(0x44E00000,0x44e3ffff);
    Resource.reg1=Io.Res.mmio(0x4A100000,0x4A107fff);
    Resource.reg2=Io.Res.mmio(0x4C000000,0x4C007fff);
    Resource.reg3=Io.Res.mmio(0x48002000,0x48007fff);
    Resource.reg4=Io.Res.mmio(0x48022000,0x4802bfff);
    Resource.reg5=Io.Res.mmio(0x48030000,0x48030fff);
    Resource.reg6=Io.Res.mmio(0x48040000,0x4804cfff);       --dmtimer2~7 gpio1
    Resource.reg7=Io.Res.mmio(0x48080000,0x4808ffff);
    Resource.reg8=Io.Res.mmio(0x480C8000,0x480C8fff);
    Resource.reg9=Io.Res.mmio(0x4819C000,0x481Aefff);
    Resource.reg10=Io.Res.mmio(0x48200000,0x48200fff);
    Resource.reg11=Io.Res.mmio(0x49000000,0x49007fff);
    Resource.reg12=Io.Res.mmio(0x49800000,0x49801fff);
    Resource.reg13=Io.Res.mmio(0x49900000,0x49901fff);
    Resource.reg14=Io.Res.mmio(0x49A00000,0x49A01fff);
    Resource.reg15=Io.Res.mmio(0x4A300000,0x4A33ffff);
    Resource.reg16=Io.Res.mmio(0x50000000,0x5fffffff);
    Resource.reg17=Io.Res.mmio(0x8000000,0x10000000);
    Resource.reg18=Io.Res.mmio(0x48060000,0x48061fff);      --mmc0 MMCHS0 8K
    Resource.reg19=Io.Res.mmio(0x47810000,0x47820fff);
    Resource.reg20=Io.Res.mmio(0x481d8000,0x481d9fff);
    Resource.reg21=Io.Res.mmio(0x481AC000,0x481ACfff);      --gpio2
    Resource.reg22=Io.Res.mmio(0x481AE000,0x481AEfff);      --gpio3

end);

--create a new hardware device called "BARDEVICE"

BARDEVICE=Io.Hw.Device(function()

--set the compatibility IDs for this device

--a client tries to match against these IDs and configures

--itself accordingly

--the list should be sorted from specific to lesss pecific IDs

compatible={"dev-bar,mmio","dev-bar"};

--set the‘hid’property of the device,the hid can also be used

--as a compatible ID when matching clients

Property.hid="dev-bar,Example";

--Specify that this device is able to use direct memory access(DMA).

--This is needed to allow clients to gain access to DMA addresses

--used by this device to directly access memory.

Property.flags=Io.Hw_device_DF_dma_supported;

--note:names for resources are truncated to 4 letters and a client

--can determine the name from the ID field of a l4vbus_resource_t

--add three resources’irq0’,’irq1’,and’reg0’to the device

Resource.irq0=Io.Res.irq(19);

Resource.irq1=Io.Res.irq(20);

Resource.reg0=Io.Res.mmio(0x6f100000,0x6f100fff);

end);

end);

Io.add_vbusses

{

--Create a virtual bus for a client and give access to FOODEVICE

vbus=Io.Vi.System_bus(function()

dev=wrap(Io.system_bus():match("BARDEVICE"));
dev=wrap(Io.system_bus():match("dev-cm,mmio"));

end);

--Create a virtual bus for another client and give it access to BARDEVICE

-- bus=Io.Vi.System_bus(function()

-- dev=wrap(Io.system_bus():match("BARDEVICE"));

-- end);

}
