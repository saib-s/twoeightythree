/*  
 *  cmpe283-1-amd.c - Kernel module for CMPE283 assignment 1
 *  (AMD version)
 */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */

#define MAX_MSG 80

/*
 * struct capability_info
 *
 * Represents a single capability (bit number and description).
 * Used by report_capability to output SVM capabilities.
 */
struct capability_info {
	uint8_t bit;
	const char *name;
};

/*
 * See AMD Programmer Reference Guide Vol 3 Appendix E
 */
 struct capability_info svm0a_eax[1] =
{
	{ 0, "SVM Nested Paging" },
	
};
 
struct capability_info svm0a_edx[18] =
{
	{ 0, "SVM Nested Paging" },
	{ 1, "LBR Virtualization" },
	{ 2, "SVM Lock" },
	{ 3, "NRIP save support" },
	{ 4, "MSR based TSC rate control" },
	{ 5, "VMCB clean bits" },
	{ 6, "Flush by ASID and support for extended VMCB TLB_Control" }, 
	{ 7, "Decode assists" },
	{ 10, "Pause intercept filter" },
	{ 12, "PAUSE filter threshold" },
	{ 13, "AMD advanced virtual interrupt controller" },
	{ 15, "VMSAVE and VMLOAD virtualization" },
	{ 16, "Virtualize the Global Interrupt Flag" },
	{ 17, "Guest Mode Execution Trap" },
	{ 19, "SVM supervisor shadow stack restrictions"},
	{ 20, "SPEC_CTRL virtualization" },
	{ 23, "Host MCE Override" },
	{ 24, "Support for INVLPGB/TLBSYNC hypervisor enable in VMCB and TLBSYNC intercept" }
};

 struct capability_info svm1a_eax[3] =
{
	{ 0, "128-bit SSE (multimedia) instruction execution with full-width internal operations" },
	{ 1, "MOVU SSE instructions are more efficient than SSE MOVL/MOVH. Same with adjacent instructions" },
	{ 2, "256-bit AVX instruction execution with full-width internal operations" }
	
};

 struct capability_info svm1f_eax[13] =
{
	{ 0, "Secure Memory Encryption" },
	{ 1, "Secure Encrypted Virtualization" },
	{ 2, "Page Flush MSR" },
	{ 3, "SEV Encrypted State"},
	{ 4, "SEC Secure Nested Paging"},
	{ 5, "VM Permission Levels"},
	{ 10, "Hardware cache coherency across encryption domains enforced"},
	{ 11, "SEV guest execution only allowed from 64-bit host"},
	{ 12, "Restricted Injection" },
	{ 13, "Alternate Injection" },
	{ 14, "Full debug state swap for SEV-ES guests"},
	{ 15, "Disallowing IBS use by the host"},
	{ 16, "Virtual Transparent Encryption"}
};

/*
 * report_capability
 *
 * Reports capabilities present in 'cap'.
 *
 * Parameters:
 *  cap: capability_info structure for this feature
 *  len: number of entries in 'cap'
 *  function: CPUID function number
 *  reg: register to use (0 = eax, 1 = ebx, 2 = ecx, 3 = edx)
 */
void
report_capability(struct capability_info *cap, uint8_t len,
    uint32_t function, uint8_t reg)
{
	uint8_t i;
	uint32_t eax, ebx, ecx, edx, r;
	struct capability_info *c;
	char msg[MAX_MSG];

	memset(msg, 0, sizeof(msg));

	cpuid(function, &eax, &ebx, &ecx, &edx);

	switch (reg) {
	case 0: r = eax; break;
	case 1: r = ebx; break;
	case 2: r = ecx; break;
	case 3: r = edx; break;
	default: return;
	}

	for (i = 0; i < len; i++) {
		c = &cap[i];
		snprintf(msg, 79, "  %s: Can set=%s\n",
		    c->name,
		    (r & (1) << (c->bit)) ? "Yes" : "No");
		printk(msg);
	}
}

void
revision_num_printer(uint32_t function)
{
	uint32_t eax, ebx, ecx, edx;

	cpuid(function, &eax, &ebx, &ecx, &edx);

	printk("SVM revision number: %u", eax);
}

/*
 * detect_svm_features
 *
 * Detects and prints SVM capabilities of this host's CPU.
 */
void
detect_svm_features(void)
{
	revision_num_printer(0x8000000A);
	report_capability(svm0a_edx,
	    18, 
	    0x8000000A,
	    3);
	report_capability(svm1a_eax,
	    3, 
	    0x8000001A,
	    0);
	report_capability(svm1f_eax,
	    13, 
	    0x8000001F,
	    0);
}

/*
 * init_module
 *
 * Module entry point
 *
 * Return Values:
 *  Always 0
 */
int
init_module(void)
{
	printk(KERN_INFO "CMPE 283 Assignment 1 Module Start\n");

	detect_svm_features();

	/* 
	 * A non 0 return means init_module failed; module can't be loaded. 
	 */
	return 0;
}

/*
 * cleanup_module
 *
 * Function called on module unload
 */
void
cleanup_module(void)
{
	printk(KERN_INFO "CMPE 283 Assignment 1 Module Exits\n");
}
