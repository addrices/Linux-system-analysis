#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/init_task.h>
#include <linux/list.h>

MODULE_LICENSE("GPL");
#define __NR_ptree 325
#define MAX_BUFFER 256

struct prinfo {
	pid_t parent_pid;
	pid_t pid;
	pid_t first_child_pid;
	pid_t next_sibling_pid;
	long state;
	long uid;
	char comm[64];
	int num;
};

static int (*oldcall)(void);

static unsigned long **find_sys_call_table(void) {
	unsigned long int offset = PAGE_OFFSET;
	unsigned long **sct;

	while (offset < ULLONG_MAX) {
		sct = (unsigned long **)offset;

		if (sct[__NR_close] == (unsigned long *) sys_close) {
			printk(KERN_INFO "Found syscall table at address: 0x%02lX",
					(unsigned long) sct);
			return sct;
		}

	offset += sizeof(void *);
	}

	return NULL;
}

static int sys_ptree_reduce(struct task_struct *root,struct prinfo *buf, int *nr, int num){
	struct prinfo a;
	struct list_head *point = root->children.next;
	struct task_struct *process;
	a.pid = root->pid;
	a.parent_pid = root->real_parent->pid;
	a.state = root->state;
	a.uid = root->cred->uid.val;
	a.num = num;
	if(root->children.next == &root->children){
                a.first_child_pid = -1;
        }
        else
                a.first_child_pid = list_entry(root->children.next,struct task_struct, sibling)->pid;
        if(init_task.sibling.next == &root->sibling){
                a.next_sibling_pid = -1;
        }
	else{
		a.next_sibling_pid = list_entry(root->sibling.next,struct task_struct, sibling)->pid;
	}
	strcpy(a.comm, root->comm);
	if(*nr < MAX_BUFFER)
		copy_to_user(&buf[*nr],&a,sizeof(struct prinfo));
	*nr = *nr + 1;
	printk(KERN_INFO "%d", *nr);
	while(point != &root->children){
		process = list_entry(point,struct task_struct, sibling);
		sys_ptree_reduce(process, buf, nr, num+1);
		point = point->next;
	}
	return 0;
}

static int sys_ptree(struct prinfo *buf, int *nr){	
	struct prinfo a;
	struct list_head *point = init_task.children.next;
	struct task_struct *process;
	if(buf == NULL)
		return -EINVAL;
	a.pid = init_task.pid;
	a.parent_pid = -1; // a.parent_pid = init_task.parent->pid;
	a.state = init_task.state;
	a.uid = init_task.cred->uid.val;
	a.num = 0;
	if(init_task.children.next == &init_task.children){
		a.first_child_pid = -1;
	}
	else
		a.first_child_pid = list_entry(init_task.children.next,struct task_struct, sibling)->pid;
	if(init_task.sibling.next == &init_task.sibling){
		a.next_sibling_pid = -1;
	}
	else
		a.next_sibling_pid = list_entry(init_task.sibling.next,struct task_struct, sibling)->pid;
	strcpy(a.comm, init_task.comm);
	copy_to_user(buf,&a,sizeof(struct prinfo));
	*nr = 1;
	printk(KERN_INFO "sys_ptree\n");
	printk(KERN_INFO "%d",*nr);
	while(point != &init_task.children){
		process = list_entry(point,struct task_struct, sibling);
		sys_ptree_reduce(process, buf, nr, 1);
		point = point->next;
	}
	return 0;
}

static int addsyscall_init(void) {
	long *syscall = (long *)find_sys_call_table();
	oldcall = (int(*)(void))(syscall[__NR_ptree]);	    
	write_cr0 (read_cr0 () & (~ 0x10000));	        
	syscall[__NR_ptree] = (unsigned long) sys_ptree;
	write_cr0 (read_cr0 () | 0x10000);
	printk(KERN_INFO "module load\n");	    
	return 0;
}

static void addsyscall_exit(void) {
	long *syscall = (long *) find_sys_call_table();        
	write_cr0 (read_cr0 () & (~ 0x10000));	    
	syscall[__NR_ptree] = (unsigned long) oldcall;	        
	write_cr0 (read_cr0 () | 0x10000);		    
	printk(KERN_INFO "module exit\n");
}

module_init(addsyscall_init);
module_exit(addsyscall_exit);

