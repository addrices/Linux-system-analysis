#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
 
MODULE_LICENSE("GPL");
#define __NR_sys_orientevt_destroy 321
#define __NR_sys_orientevt_wait 322
#define __NR_hello 323
#define __NR_set_orientation 325
#define __NR_create_orientation 324

struct dev_orientation {
	int azimuth; /* rotation around
		      * the X axis (-180<=azimuth<180)
		      */
	int pitch;   /* rotation around the Y-axis: -90<=pitch<=90 */
	int roll;    /* rotation around Z-axis: +Y == -roll, -180<=roll<=180 */
};

struct orientation_range {
	struct dev_orientation orient; /* device orientation */
	unsigned int azimuth_range; /* +/- degrees around X-axis */
	unsigned int pitch_range; /* +/- degrees around Y-axis */
	unsigned int roll_range; /* +/- degrees around Z-axis */
};

struct dev_orientation orientation_KER;			//储存当前orientation信息

struct orientation_event {
	struct orientation_range range;
	wait_queue_head_t range_event_head;
	bool flag;
};

struct orientation_event* events[2] = {NULL , NULL};

static int (*oldcall_321)(void);
static int (*oldcall_322)(void);
static int (*oldcall_323)(void);
static int (*oldcall_324)(void);
static int (*oldcall_325)(void);

static long sys_hello(void);
static int sys_set_orientation(struct dev_orientation *orient);
static int sys_orientevt_create(struct orientation_range *orient);
static int sys_orientevt_destroy(int event_id);
static int sys_orientevt_wait(int event_id);
static __always_inline bool orient_within_range(struct dev_orientation *orient, struct orientation_range *range);

static unsigned long **find_sys_call_table(void);

static long sys_hello(void){
	printk(KERN_INFO "hello, world!\n");
	return 0;
}

static int sys_set_orientation(struct dev_orientation *orient){
	int i = 0;
	orientation_KER.azimuth = orient->azimuth;
	orientation_KER.pitch = orient->pitch;
	orientation_KER.roll = orient->roll;
	printk(KERN_INFO "sys_set_orientation\n");
	while(i < 2){
		if(events[i]!= NULL && events[i]->flag){
			if(orient_within_range(&orientation_KER,&events[i]->range)){
				wake_up(&events[i]->range_event_head);
			}
		}
		i++;
	}
	return 0;
}

static int sys_orientevt_create(struct orientation_range *orient){
	int i = 0;
	while(i < 2){															//选择空闲的事件表
		if(events[i] == NULL){
			events[i] = (struct orientation_event*)kmalloc(sizeof(struct orientation_event), GFP_KERNEL);
			if(!events[i]){
				return -ENOMEM;
			}
			events[i]->range.orient = orient->orient;
			events[i]->range.azimuth_range = orient->azimuth_range;
			events[i]->range.pitch_range = orient->pitch_range;
			events[i]->range.roll_range = orient->roll_range;
			events[i]->flag = true;											//初始化空闲事件
			init_waitqueue_head(&events[i]->range_event_head);				//初始化等待队列头
			printk(KERN_INFO "create\n");		
			return i;														//返回event_id
		}
		i++;
	}
	return -EFAULT;
}

static int sys_orientevt_wait(int event_id){
	if(events[event_id] != NULL && events[event_id]->flag){
		wait_queue_t data;
		init_waitqueue_entry(&data,current);								//将自己加入等待队列项中						
		add_wait_queue(&events[event_id]->range_event_head,&data);			//将这一项加入等待队列中
		printk(KERN_INFO "wait\n");		
		while(orient_within_range(&orientation_KER,&events[event_id]->range) == false){		//如果wakeup时候，不满足条件
			prepare_to_wait(&events[event_id]->range_event_head, &data, TASK_INTERRUPTIBLE);	//重新加入等待队列中
			schedule();																		//调度其他进程
		}
		remove_wait_queue(&events[event_id]->range_event_head, &data);						//跳出等待队列
		return 0;
	}
	else
		return -EFAULT;
}

static int sys_orientevt_destroy(int event_id){
	events[event_id]->flag = false;
	wake_up(&events[event_id]->range_event_head);		//在删除这个等待队列之前需要将等待队列上的进程全部唤醒，这些进程会自己退出
	kfree(events[event_id]);
	events[event_id] = NULL;
	return 0;
}

static __always_inline bool orient_within_range(struct dev_orientation *orient, struct orientation_range *range){
	struct dev_orientation *target = &range->orient;
	unsigned int azimuth_diff = abs(target->azimuth - orient->azimuth);
	unsigned int pitch_diff = abs(target->pitch - orient->pitch);
	unsigned int roll_diff = abs(target->roll - orient->roll);
	return (!range->azimuth_range || azimuth_diff <= range->azimuth_range\
	|| 360 - azimuth_diff <= range->azimuth_range)\
	&& (!range->pitch_range || pitch_diff <= range->pitch_range)\
	&& (!range->roll_range || roll_diff <= range->roll_range\
	|| 360 - roll_diff <= range->roll_range);
}

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

static int addsyscall_init(void) {
	long *syscall = (long *)find_sys_call_table();
	oldcall_321 = (int(*)(void))(syscall[__NR_sys_orientevt_destroy]);
	oldcall_322 = (int(*)(void))(syscall[__NR_sys_orientevt_wait]);
	oldcall_323 = (int(*)(void))(syscall[__NR_create_orientation]);
	oldcall_324 = (int(*)(void))(syscall[__NR_hello]);
	oldcall_325 = (int(*)(void))(syscall[__NR_set_orientation]);	    
	write_cr0 (read_cr0 () & (~ 0x10000));
	syscall[__NR_hello] = (unsigned long) sys_hello;
	//printk(KERN_INFO "sys_hello in\n");
	syscall[__NR_sys_orientevt_destroy] = (unsigned long)sys_orientevt_destroy;
	//printk(KERN_INFO "sys_orientevt_destroy in\n");
	syscall[__NR_sys_orientevt_wait] = (unsigned long) sys_orientevt_wait;
	//printk(KERN_INFO "sys_orientevt_wait in\n");
	syscall[__NR_set_orientation] = (unsigned long) sys_set_orientation;
	//printk(KERN_INFO "sys_set_orientation in\n");
	syscall[__NR_create_orientation] = (unsigned long) sys_orientevt_create;
	//printk(KERN_INFO "sys_orientevt_create in\n");
	write_cr0 (read_cr0 () | 0x10000);
	printk(KERN_INFO "module load\n");	    
	return 0;
}

static void addsyscall_exit(void) {
	long *syscall = (long *) find_sys_call_table();        
	write_cr0 (read_cr0 () & (~ 0x10000));	    
	syscall[__NR_hello] = (unsigned long) oldcall_324;
	syscall[__NR_set_orientation] = (unsigned long) oldcall_325;
	syscall[__NR_create_orientation] = (unsigned long) oldcall_323;
	syscall[__NR_sys_orientevt_wait] = (unsigned long) oldcall_322;
	syscall[__NR_sys_orientevt_destroy] = (unsigned long) oldcall_321;
	write_cr0 (read_cr0 () | 0x10000);		    
	printk(KERN_INFO "module exit\n");
}

module_init(addsyscall_init);
module_exit(addsyscall_exit);