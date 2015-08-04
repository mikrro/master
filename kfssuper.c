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
	struct page *pg = NULL;
	struct bio *bio = NULL;

	printk(KERN_INFO "Start module from here\n");

	if(dev == NULL) {
		printk(KERN_ERR "Device not specified");
		return 0;
	} 
	
	bdev = lookup_bdev(strim(dev));
	
	if(IS_ERR(bdev)) {	
		if(bdev == ERR_PTR(-EBUSY)) 
			printk(KERN_ERR "Device busyi\n");
	
		printk(KERN_ERR "Couldn't lock device <%ld>", PTR_ERR(bdev));
		return 0;
	}
	pg = alloc_page(GFP_KERNEL);
	
	//sb = kmalloc(sizeof(super_block), GFP_KERNEL);	
	
	if(!pg) {
		printk(KERN_ERR "Can't alloc page memory");
		return 0;
	}
	
	bio = bio_alloc(GFP_NOIO, 1);

	
	free_page(pg);	
	return 0;
}

static void __exit exit_fssuper(void)
{

}

module_init(init_fssuper);
module_exit(exit_fssuper);
