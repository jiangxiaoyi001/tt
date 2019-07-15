#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <asm/uaccess.h> 
#include <linux/miscdevice.h>
#include <linux/time.h>
#include <linux/timer.h>
//#define TEST_MAGIC 'x'
//#define TEST_MAX_NR 2
//#define TEST_SET_TIME _IO(TEST_MAGIC,1)
#define TEST_SET_TIME 0X1


struct globalmem_dev{
	struct miscdevice mdev;
	unsigned int timest[2];
};
struct globalmem_dev *globalmem_devp;
//////////////////////////////////////////
int globalmem_open(struct inode *inode,struct file *filp)
{
	printk(KERN_INFO "globalmem open!\n");
	filp->private_data=globalmem_devp;
	return 0;
}
int globalmem_release(struct inode *inode,struct file *filp)
{
	printk(KERN_INFO "globalmem release!\n");
	return 0;
}

static long int globalmem_ioctl(struct file *filp,unsigned int cmd,
				unsigned long arg)
{
	struct globalmem_dev *dev=filp->private_data;
	switch(cmd){
		case TEST_SET_TIME:
			raw_copy_from_user(&(dev->timest),(unsigned int*) arg,2*sizeof(unsigned int));
			printk(KERN_INFO "SET TIME！\n");
			break;
		default:
			return -EINVAL;
	}
	return 0;
}


static ssize_t globalmem_read(struct file *filp,char __user *buf,size_t size,
				loff_t *ppos)
{
	unsigned int count=size;
	int ret=0;
	struct globalmem_dev *dev=filp->private_data;
	if(raw_copy_to_user(buf,&(dev->timest),2*sizeof(unsigned int)))
		ret=-EFAULT;
	return ret;
}


//////////////////////////////////////////////////////////
static const struct file_operations globalmem_fops={
	.owner=THIS_MODULE,
	.open=globalmem_open,
	.release=globalmem_release,
	.unlocked_ioctl=globalmem_ioctl,
	.read=globalmem_read,
};
////////////////////////////////////////////////
static struct miscdevice mdev_struct={
	.minor=MISC_DYNAMIC_MINOR,
	.name="globalmem",
	.fops=&globalmem_fops,
};
/////////////////////////////////////////////////
int globalmem_init(void)
{
	int result;
	globalmem_devp=kmalloc(sizeof(struct globalmem_dev),GFP_KERNEL);
	if(!globalmem_devp){
		result=-ENOMEM;
		goto fail_malloc;
	}
	memset(globalmem_devp,0,sizeof(struct globalmem_dev));
	
	globalmem_devp->mdev=mdev_struct;
	globalmem_devp->timest[0]=jiffies;
	globalmem_devp->timest[1]=0;
	result=misc_register(&(globalmem_devp->mdev));
	if(result<0)
		return result;
	else
		return 0;
fail_malloc:
	return result;
}

void globalmem_exit(void)
{
	misc_deregister(&(globalmem_devp->mdev));
	if(globalmem_devp)
		kfree(globalmem_devp);
}

module_init(globalmem_init);
module_exit(globalmem_exit);














