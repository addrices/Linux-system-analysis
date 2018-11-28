# lab03

## 数据结构
这个实验中使用的是内核中的等待队列，wait_queue_head_t是这个队列的头部，也负责控制整个队列，其上有一个自旋锁负责管理边界区的访问。使用task_list来和之后的wait_queue_t连接起来，每一个wait_queue_t负责管理这个等待队列上的一个进程的等待行为。

下列是wait_queue_head_t和wait_queue_t的数据结构
```
struct __wait_queue_head {
    spinlock_t lock;
    struct list_head task_list;
};
typedef struct __wait_queue_head wait_queue_head_t;
```

```{.bash bgcolor = "bg"}
struct __wait_queue {
    unsigned int flags;
#define WQ_FLAG_EXCLUSIVE   0x01
    void *private;
    wait_queue_func_t func;
    struct list_head task_list;
};
```
|字段|说明|
|:---|:---|
|flags|为WQ_FLAG_EXCUSIVE或为0,WQ_FLAG_EXCUSIVE表示等待进程想要被独占地唤醒|
|private|是一个指针，指向等待进程的task_struct实例，这个变量本质上可以指向任意的私有数据|
|func|等待唤醒进程|
|task_list|用作一个链表元素，用于将wait_queue_t实例放置到等待队列中|

这2个数据结构的初始化函数如下：
```
void init_waitqueue_head(wait_queue_head_t *q)
{
    spin_lock_init(&q->lock);
    INIT_LIST_HEAD(&q->task_list);
}
```
```

static inline void init_waitqueue_entry(wait_queue_t *q, struct task_struct *p)
{
	q->flags	= 0;
	q->private	= p;        //current代表当前进程。
	q->func		= default_wake_function;
}
```
等待函数中可以使用prepare_to_wait和add_wait_queue这2个函数在执行时会讲自旋锁锁上，操作完后再打开，防止同时访问。
```
prepare_to_wait(wait_queue_head_t *q, wait_queue_t *wait, int state)
{
	unsigned long flags;
	wait->flags &= ~WQ_FLAG_EXCLUSIVE;
	spin_lock_irqsave(&q->lock, flags); 
//判断当前wait->task_list 为null，如果为null，说明没有进程需要唤醒，也就没有
//必要添加到wait_queue_head_t *q,
	if (list_empty(&wait->task_list))
		__add_wait_queue(q, wait);
	set_current_state(state);
	spin_unlock_irqrestore(&q->lock, flags);
}
```
```
void add_wait_queue(wait_queue_head_t *q, wait_queue_t *wait)
{
	unsigned long flags;
#可见先清除WQ_FLAG_EXCLUSIVE 标志后，然后调用__add_wait_queue 将wait添加到q中
	wait->flags &= ~WQ_FLAG_EXCLUSIVE;
	spin_lock_irqsave(&q->lock, flags);
	__add_wait_queue(q, wait);
	spin_unlock_irqrestore(&q->lock, flags);
}
```
我的描述等待时间的数据结构如下，内存中保存着对应的指针列表，当需要使用时，发现当前指针为NULL则能够使用kmalloc函数申请空间，创建事件。
```
struct orientation_event {
	struct orientation_range range;
	wait_queue_head_t range_event_head;
	bool flag;
};
```
等待事件的代码如下：
```
init_waitqueue_entry(&data,current);  
//将自己加入等待队列项中			
add_wait_queue(&events[event_id]->range_event_head,&data); 
//将这一项加入等待队列中
printk(KERN_INFO "wait\n");		
while(orient_within_range(&orientation_KER,&events[event_id]->range) == false){		
//如果wakeup时候，不满足条件
prepare_to_wait(&events[event_id]->range_event_head, &data,TASK_INTERRUPTIBLE)；
//重新加入等待队列中
schedule();     
//调度其他进程
}
remove_wait_queue(&events[event_id]->range_event_head, &data);
```
而在结束事件的时候需要先关闭事件上的flag，然后wake_up所有的进程，这时如果进程还调用等待函数的话则检查到错误，将自己退出，之后内核才能将该事件对应的内存空间释放。
## 实验结果
![实验结果截图：](/lab3/实验结果.png)
