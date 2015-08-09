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

static inline sector_t calc_dev_sboffset(struct block_device *bdev)
{
        sector_t num_sectors = i_size_read(bdev->bd_inode) / 512;
        return MD_NEW_SIZE_SECTORS(num_sectors);
}

static int __init init_fssuper(void) 
{

	struct super_block *sb;
	struct block_device *bdev = NULL;
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
	
	pg = alloc_page(GFP_NOIO);
	bio = bio_alloc(GFP_NOIO, 1);
	
	if (!pg || !bio) {
		printk(KERN_ERR "Couldn't alloc page or bio");
		return 0;
	}
	
	bio->bi_bdev = blkdev_get_by_dev(bdev->bd_dev, FMODE_READ|FMODE_WRITE, NULL);
	bio->bi_sector = 0;
	bio->bi_vcnt = 1;
	bio->bi_idx = 0;
	bio->bi_size = sb_size;
	bio->bi_io_vec[0].bv_page = pg;
    	bio->bi_io_vec[0].bv_len = sb_size;
    	bio->bi_io_vec[0].bv_offset = 0;
	
	bio_get(bio);
	ret = submit_bio_wait(READ | REQ_SYNC, bio);
	printk(KERN_INFO "ret value %x, flags %x\n", ret, bio->bi_flags);
	
	ret = test_bit(BIO_UPTODATE, &bio->bi_flags);
	printk(KERN_INFO "ret value %x, flags %x\n", ret, bio->bi_flags);
	
	blkdev_put(bio->bi_bdev, FMODE_READ|FMODE_WRITE);	
	//sb = page_address(pg);
	bio_put(bio);
	free_page(pg);	
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
