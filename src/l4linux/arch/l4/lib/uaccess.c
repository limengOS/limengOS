#include <linux/module.h>
#include <linux/sched.h>

#include <asm/linkage.h>
#include <asm/api/macros.h>
#include <asm/uaccess.h>
#include <asm/generic/memory.h>

#define LOG_EFAULT 1
#if LOG_EFAULT
#include <l4/sys/kdebug.h>
static void log_efault(const char *str, const void *address)
{
	unsigned int page_shift;
	pte_t *ptep = lookup_pte(current->mm, (unsigned long)address,
	                         &page_shift);

	printk("%s returning efault, address: %px, \n"
	       "  task: %s (%px), pdir: %px, ptep: %px, pte: %lx (%d)\n",
	       str, address, current->comm, current,
	       current->mm->pgd, ptep, ptep ? (unsigned long)pte_val(*ptep) : 0,
	       page_shift);
	L4XV_FN_v(enter_kdebug("log_efault"));
}
#else
#define log_efault(str, address)
#endif

long __get_user_1(u8 *val, const void __user *address)
{
	unsigned long page, offset, flags;

	page = parse_ptabs_read((unsigned long)address, &offset, &flags);
	if (page != -EFAULT) {
		*val = *(u8 *)(page + offset);
		local_irq_restore(flags);
		return 0;
	}
	log_efault(__func__, address);
	return -EFAULT;
}
EXPORT_SYMBOL(__get_user_1);

long __get_user_2(u16 *val, const void __user *address)
{
	unsigned long page, offset;

	if (((unsigned long)address & ~PAGE_MASK) != ~PAGE_MASK) {
		unsigned long flags;
		page = parse_ptabs_read((unsigned long)address, &offset, &flags);
		if (page != -EFAULT) {
			*val = *(u16 *)(page + offset);
			local_irq_restore(flags);
			return 0;
		}
	} else {
		u8 low, high;
		if ((__get_user_1(&low,  address)     != -EFAULT) &&
		    (__get_user_1(&high, address + 1) != -EFAULT)) {
			*val = (u16)low | ((u16)high << 8);
			return 0;
		}
	}
	log_efault(__func__, address);
	return -EFAULT;
}
EXPORT_SYMBOL(__get_user_2);

long __get_user_4(u32 *val, const void __user *address)
{
	unsigned long page, offset;

	if (((unsigned long)address & ~PAGE_MASK)
			<= (PAGE_SIZE - sizeof(*val))) {
		unsigned long flags;
		page = parse_ptabs_read((unsigned long)address, &offset, &flags);
		if (page != -EFAULT) {
			*val = *(u32 *)(page + offset);
			local_irq_restore(flags);
			return 0;
		}
	} else {
		switch ((unsigned long)address & 3) {
		case 1: fallthrough;
		case 3: {
			u8 first, third;
			u16 second;
			if ((__get_user_1(&first,  address)     != -EFAULT) &&
			    (__get_user_2(&second, address + 1) != -EFAULT) &&
			    (__get_user_1(&third,  address + 3) != -EFAULT)) {
				*val = first |
					((u32)second << 8) |
					((u32)third << 24);
				return 0;
			}
		}
			fallthrough;
		case 2: {
			u16 first, second;
			if ((__get_user_2(&first,  address)     != -EFAULT) &&
			    (__get_user_2(&second, address + 2) != -EFAULT)) {
				*val = first | ((u32)second << 16);
				return 0;
			}
		}
		}
	}
	log_efault(__func__, address);
	return -EFAULT;
}
EXPORT_SYMBOL(__get_user_4);

long __get_user_8(u64 *val, const void __user *address)
{
	unsigned long page, offset;

	if (((unsigned long)address & ~PAGE_MASK)
			<= (PAGE_SIZE - sizeof(*val))) {
		unsigned long flags;
		page = parse_ptabs_read((unsigned long)address, &offset, &flags);
		if (page != -EFAULT) {
			*val = *(u64 *)(page + offset);
			local_irq_restore(flags);
			return 0;
		}
	} else {
		u32 first, second;
		if ((__get_user_4(&first,  address)     != -EFAULT) &&
		    (__get_user_4(&second, address + 4) != -EFAULT)) {
			*val = first | ((u64)second << 32);
			return 0;
		}
	}
	log_efault(__func__, address);
	return -EFAULT;
}
EXPORT_SYMBOL(__get_user_8);

long __put_user_1(u8 val, const void __user *address)
{
	unsigned long page, offset, flags;

	page = parse_ptabs_write((unsigned long)address, &offset, &flags);
	if (page != -EFAULT) {
		*(u8*)(page + offset) = val;
		local_irq_restore(flags);
		return 0;
	}
	log_efault(__func__, address);
	return -EFAULT;
}
EXPORT_SYMBOL(__put_user_1);

long __put_user_2(u16 val, const void __user *address)
{
	unsigned long page, offset;

	if (((unsigned long)address & ~PAGE_MASK) != ~PAGE_MASK) {
		unsigned long flags;
		page = parse_ptabs_write((unsigned long)address, &offset, &flags);
		if (page != -EFAULT) {
			*(u16*)(page + offset) = val;
			local_irq_restore(flags);
			return 0;
		}
	} else {
		if ((__put_user_1((unsigned char)val, address) != -EFAULT)
		    &&
		    (__put_user_1((unsigned char)(val >> 8), address+1)
			!= -EFAULT)) {
			return 0;
		}
	}
	log_efault(__func__, address);
	return -EFAULT;
}
EXPORT_SYMBOL(__put_user_2);

long __put_user_4(u32 val, const void __user *address)
{
	unsigned long page, offset;

	if (((unsigned long)address & ~PAGE_MASK) <= (PAGE_SIZE - sizeof(val))) {
		unsigned long flags;
		page = parse_ptabs_write((unsigned long)address, &offset, &flags);
		if (page != -EFAULT) {
			*(u32*)(page + offset) = val;
			local_irq_restore(flags);
			return 0;
		}
	} else {
		switch ((unsigned long)address &3) {
		case 1: fallthrough;
		case 3:
			if ((__put_user_1((u8)val,
					  address)     != -EFAULT) &&
			    (__put_user_2((u16)(val >> 8),
					  address + 1) != -EFAULT) &&
			    (__put_user_1((u8)(val >> 24),
					  address + 3) != -EFAULT))
				return 0;
			fallthrough;
		case 2:
			if ((__put_user_2((unsigned short)val,
					  address)     != -EFAULT) &&
			    (__put_user_2((unsigned short)(val >> 16),
					  address + 2) != -EFAULT))
				return 0;
		}
	}
	log_efault(__func__, address);
	return -EFAULT;
}
EXPORT_SYMBOL(__put_user_4);

long __put_user_8(u64 val, const void __user *address)
{
	unsigned long page, offset;

	if (((unsigned long)address & ~PAGE_MASK) <= (PAGE_SIZE - sizeof(val))) {
		unsigned long flags;
		page = parse_ptabs_write((unsigned long)address, &offset, &flags);
		if (page != -EFAULT) {
			*(unsigned long long*)(page + offset) = val;
			local_irq_restore(flags);
			return 0;
		}
	} else {
		if (__put_user_4((u32)val,         address) != -EFAULT &&
		    __put_user_4((u32)(val >> 32), address + 4) != -EFAULT)
			return 0;
	}
	log_efault(__func__, address);
	return -EFAULT;
}
EXPORT_SYMBOL(__put_user_8);
