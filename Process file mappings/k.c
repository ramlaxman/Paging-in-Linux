#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/pid.h>
#include <linux/swap.h>

#include <linux/pid_namespace.h>
static int p_id = 1;
struct pid *pid_struct;
struct task_struct *task;
struct mm_struct *pid_mm;
//pgd_t *pid_pgd;
struct vm_area_struct *pid_mmap;
char buf[200];
char *k;

/*struct rb_root *rb;
struct rb_node *rb_l,*rb1;*/


module_param(p_id, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(myshort, "A PID of task");


int proc_init(void) {
	int count=0;
	pid_struct = find_get_pid(p_id);
	task = pid_task(pid_struct, PIDTYPE_PID);


	printk(KERN_INFO " task Name : %s\n",task->comm);
	pid_mm=task->mm;
	pid_mmap=pid_mm->mmap;

	//pid_pgd=pid_mm->pgd->pgd;
	printk(KERN_INFO " total pages : %ld\n",pid_mm->total_vm);

/*	rb=&pid_mm->mm_rb;
	rb_l=rb_last(rb);
	while((struct rb_node*)rb1!=rb_l)
	{
		pid_mmap=(struct vm_area_struct*)rb;
		k=dentry_path_raw(pid_mmap->vm_file->f_path.dentry,buf,200);
		printk(KERN_INFO " file : %s\n",k);
		rb1=rb_next(rb1);


	}*/
	while(pid_mmap!=NULL)
	{
		if(pid_mmap->vm_file!=NULL)
		{
			if (pid_mmap->vm_file->f_path.dentry!=NULL)
				k=dentry_path_raw(pid_mmap->vm_file->f_path.dentry,buf,200);
			else break;
		}
		printk(KERN_INFO " file %d : %ld---%ld %s\n",++count,pid_mmap->vm_start,pid_mmap->vm_end,k);
		pid_mmap=pid_mmap->vm_next;
	}

	return 0;
}

void proc_cleanup(void) {
	printk(KERN_INFO " Inside cleanup_module\n");

}
MODULE_LICENSE("GPL");
module_init(proc_init);
module_exit(proc_cleanup);
