#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kallsyms.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <asm/mmu_writeable.h>

#define DRIVER_AUTHOR "alexj"
#define DRIVER_DESCRIPTION "Sony RIC disabler"
#define DRIVER_VERSION "0.1"

#define OPCODE_SIZE 12 /* ARM hook size is 12 bytes */


struct hookdata_t
{
	void* paddr;
	unsigned char m_data[OPCODE_SIZE];
	unsigned char m_opcode[OPCODE_SIZE];
	struct list_head list;
};


LIST_HEAD(head_obj);

static unsigned long m_address;
static bool ric_enable=false; /* Current status - false if protection is enabled, true disabled. */


static int sony_ric_disabled(void)
{
	return 0;
}

inline void arm_poke ( void* p_addr, const char* p_data )
{
	unsigned long* m_target = (unsigned long*)p_addr;
	unsigned long* m_opcode = (unsigned long*)p_data;

	mem_text_write_kernel_word(m_target, *m_opcode);
	mem_text_write_kernel_word(m_target + 1, *(m_opcode + 1));
	mem_text_write_kernel_word(m_target + 2, *(m_opcode + 2));

}

/* Store the target function pointer, original code and new code to the linked list. */
static void ric_save ( void* p_addr,  const unsigned char* p_opcode, const unsigned char* p_data )
{
	struct hookdata_t* ps;

	if(p_addr!=NULL && p_opcode!=NULL && p_data!=NULL)
	{
		/* allocate the memory for the whole object */
		ps = kzalloc(sizeof(*ps), GFP_KERNEL);
		if ( !ps )
		{
			pr_err("unable to allocate memory\n");
			return;
		}
		ps->paddr = p_addr;
		memcpy(ps->m_data, p_data, OPCODE_SIZE);
		memcpy(ps->m_opcode, p_opcode, OPCODE_SIZE);
		list_add(&ps->list, &head_obj);
	}
}

/* Construct shellcode and rewrite target */
static void ric_rewrite ( void* p_addr, void* p_data )
{
	unsigned char m_opcode[OPCODE_SIZE]= {0}; /* local buffer for new code */
	unsigned char m_data[OPCODE_SIZE]= {0};   /* local buffer to save original code */

	/* ldr pc, [pc, #0]; .long addr; .long addr */
	memcpy(m_opcode, "\x00\xf0\x9f\xe5\x00\x00\x00\x00\x00\x00\x00\x00", OPCODE_SIZE);
	*(unsigned long*)&(m_opcode)[4] = (unsigned long)p_data;
	*(unsigned long*)&(m_opcode)[8] = (unsigned long)p_data;

	memcpy(m_data, p_addr, OPCODE_SIZE); /* save target pointer */

	/* At this point all function calls to sony_ric_enabled() will redirect to sony_ric_disabled(). */
	arm_poke(p_addr, m_opcode);

	ric_save(p_addr, m_opcode, m_data);

}

/* Restore original opcodes. */
static void ric_restore ( void* p_addr )
{
	struct hookdata_t* ps;

	list_for_each_entry ( ps, &head_obj, list )
	if ( p_addr == ps->paddr )
	{
		arm_poke(p_addr, ps->m_data);
		ric_enable=!ric_enable;
		list_del( &ps->list);
		kfree(ps);
		break;
	}
}

static int __init ric_mod_init(void)
{

	m_address=kallsyms_lookup_name("sony_ric_enabled");
	if(m_address!=0)
	{
		ric_rewrite((void*)m_address, &sony_ric_disabled);
		ric_enable=!ric_enable;
	}
	else
	{
		printk(KERN_ERR "[%s] Unable to loоckup sony_ric_enabled symbol\n", __this_module.name);
		return -1;
	}

	printk(KERN_INFO "[%s] module loaded\n", __this_module.name);
	printk(KERN_INFO "[%s] Sony RIC is now : %s\n", __this_module.name, ric_enable ?  "disabled" : "enabled");

	return 0;
}

static void __exit ric_mod_exit(void)
{

	if(ric_enable)
		ric_restore((void*)m_address);

	printk(KERN_INFO "[%s] module unloaded\n", __this_module.name);
	printk(KERN_INFO "[%s] Sony RIC is now : %s\n", __this_module.name, ric_enable ?  "disabled" : "enabled");

}

module_init(ric_mod_init);
module_exit(ric_mod_exit);

MODULE_DESCRIPTION(DRIVER_DESCRIPTION);
MODULE_VERSION(DRIVER_VERSION);
MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);

