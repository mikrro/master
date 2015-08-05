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

//	struct super_block *sb;
	struct block_device *bdev = NULL;
	struct page *pg = NULL;
	struct bio *bio = NULL;
	int ret = 0;

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
	bio = bio_alloc(GFP_NOIO, 1);
	if (!pg || !bio) {
		printk(KERN_ERR "Couldn't alloc page or bio");
		return 0;
	}
	
	bio->bi_bdev = bdev;
	printk(KERN_INFO "page %x, bio %x, bdev %x, bio dev %x", pg, bio, bdev, bio->bi_bdev);
	printk(KERN_INFO ", bio flags  %x, bio rw %x, bio vec size %x", pg, bio->bi_flags, bio->bi_rw, bio->bi_vcnt);
	ret = blkdev_get(bdev, FMODE_READ, init_fssuper);
	printk(KERN_INFO "ret value %x",ret);
//	bio_add_page(bio, pg, 4096, 0);
//	submit_bio(READ | REQ_SYNC, bio);
	
	return 0;
}

static void __exit exit_fssuper(void)
{

}

module_init(init_fssuper);
module_exit(exit_fssuper);
