#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/blkdev.h>
#include <linux/blk_types.h>
#include <linux/pagemap.h>
#include <linux/moduleparam.h>
#include <asm/uaccess.h>
#include <linux/raid/md_p.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hug");

static char* dev = NULL;
module_param(dev, charp, S_IRUGO | S_IWUSR);

static void calc_dev_sboffset(struct block_device *bdev)
{
        sector_t num_sectors = i_size_read(bdev->bd_inode) / 512;
        printk(KERN_INFO, "Sektor nums %ld", num_sectors);
}

static void io_end(struct bio* bio, int error) 
{
	printk(KERN_INFO "ret value %x, flags %x\n", error, bio->bi_flags);
}

static void test_flags(long unsigned int flags, int max)
{
	int i = 0;
	for(; i < max+1; i++)
	{ 	
		if(test_bit(i, &flags)) 
			printk(KERN_INFO "FLAG %d is set\n", i);
	}
}

static int __init init_fssuper(void) 
{

	struct super_block *sb;
	struct block_device *bdev = NULL;
	struct request_queue *q = NULL;
	struct page *pg = NULL;
	struct bio *bio = NULL;
	int sb_size = 4096;
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
	
	bio = bio_alloc(GFP_NOIO, 1);
	if (ret || !bio) {
		printk(KERN_ERR "Couldn't alloc page or bio");
		return 0;
	}
	
	ret = bio_alloc_pages(bio,GFP_KERNEL);
	bio->bi_bdev = blkdev_get_by_dev(bdev->bd_dev, FMODE_READ, NULL);
	bio->bi_iter.bi_sector = 98304;
	bio->bi_end_io = io_end;	

//	bio_add_page(bio,pg,sb_size,0);
	
	if(!bio->bi_bdev) {
		printk("Couldn't open device");
		return 0;
	}
	
	q = bdev_get_queue(bio->bi_bdev);
	if(!q)
		return 0;
	test_flags(q->queue_flags,22);
	test_flags(bio->bi_flags,13);
	
	ret = submit_bio_wait(READ, bio);
	printk(KERN_INFO "ret value %x\n", ret);
	
	test_flags(bio->bi_flags,13);
	blkdev_put(bio->bi_bdev, FMODE_READ|FMODE_WRITE);	
	//sb = page_address(pg);
	bio_put(bio);
	//if(!sb) {
	//	printk(KERN_ERR "Couldn't load sb");
	//	return 0;
	//}
	//printk(KERN_INFO " UUID %x \n",sb->s_uuid);
	//printk(KERN_INFO " MAGIC %x \n",sb->s_magic);
	return 0;
}

static void __exit exit_fssuper(void)
{

}

module_init(init_fssuper);
module_exit(exit_fssuper);
