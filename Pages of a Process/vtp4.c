/*
===============================================================================
Driver Name		:		vtp
Author			:		YOGESH ROS
License			:		GPL
Description		:		LINUX DEVICE DRIVER PROJECT
===============================================================================
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/pid.h>
#include <linux/swap.h>

#include <linux/pid_namespace.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YOGESH ROS");

static int p_id = 1;
struct pid *pid_struct;
struct task_struct *task;
struct mm_struct *pid_mm;

struct vm_area_struct *pid_mmap;
char buf[200];
char *k;

struct vm_area_struct *vma = 0;
unsigned long vpage;
unsigned long phys;

module_param(p_id, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(myshort, "A PID of task");



unsigned long virt2phys(struct mm_struct *mm, unsigned long virt)
{

	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
	struct page *page;

	//printk(KERN_INFO "virt2phys");

	pgd = pgd_offset(mm, virt);
	if (pgd_none(*pgd) || pgd_bad(*pgd))
	    return 0;
	pud = pud_offset(pgd, virt);
	if (pud_none(*pud) || pud_bad(*pud))
	    return 0;
	pmd = pmd_offset(pud, virt);
	if (pmd_none(*pmd) || pmd_bad(*pmd))
	    return 0;
	if (!(pte = pte_offset_map(pmd, virt)))
	    return 0;
	if (!(page = pte_page(*pte)))
	    return 0;
	phys = page_to_phys(page);
	pte_unmap(pte);
	return phys;
}



static int __init vtp_init(void)
{

	int count = 0 ;
	pid_struct = find_get_pid(p_id);
	task = pid_task(pid_struct, PIDTYPE_PID);
	printk(KERN_INFO " task Name : %s\n",task->comm);




	if (task->mm && task->mm->mmap)
	    for (vma = task->mm->mmap; vma; vma = vma->vm_next)
	        for (vpage = vma->vm_start; vpage < vma->vm_end; vpage += PAGE_SIZE)
	            {
	        		//printk(KERN_INFO "before phys");
	        		phys = virt2phys(task->mm, vpage);
	        		//printk(KERN_INFO "after phys");
	        		if(phys)
	        			printk(KERN_INFO " page %d :%ld  \n",++count,phys);
	        		//printk(KERN_INFO "after print");
	            }




	return 0;
}

static void __exit vtp_exit(void)
{	




}

module_init(vtp_init);
module_exit(vtp_exit);

