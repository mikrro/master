#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/blkdev.h>
#include <linux/pagemap.h>
#include <linux/moduleparam.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hug");

static char* dev = NULL;
module_param(dev, charp, S_IRUGO | S_IWUSR);

static int __init init_fssuper(void) 
{

	struct super_block *sb;
	struct block_device *bdev = NULL;
	
	printk(KERN_INFO "Start module from here");

	if(dev == NULL) {
		printk(KERN_ERR "Device not specified");
		return 0;
	} 
	
	bdev = lookup_bdev(strim(dev));
	
	if(IS_ERR(bdev)) {	
		if(bdev == ERR_PTR(-EBUSY)) 
			printk(KERN_ERR "Device busy");
	
		printk(KERN_ERR "Couldn't lock device <%ld>", PTR_ERR(bdev));
		return 0;
	}
	
	sb = get_super(bdev);
	bdput(bdev);
	
	if(IS_ERR(sb)) {
		printk(KERN_ERR "Can't load sb <%ld>", PTR_ERR(sb));
		return 0;
	}
	
	printk(KERN_INFO "Sucessfuly loaded sb, uuid %x", *(sb->s_uuid));
	printk(KERN_INFO "\nFile system : %s", sb->s_type->name);
	return 0;
}

static void __exit exit_fssuper(void)
{

}

module_init(init_fssuper);
module_exit(exit_fssuper);
