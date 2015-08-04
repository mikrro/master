#include <linux/raid/md_p.h>
#include <linux/fs.h>

// struct super_type  {
// 	char *name;
// 	int (*load_super)(struct md_rdev *rdev, struct md_rdev *refdev,int minor_version);
// 	int	(*read_super)(struct mddev *mddev, struct md_rdev *rdev);
// };
//
// static struct super_type super_types[] = {
// 	[0] = {
// 		.name	= "0.90.0",
// 		.load_super	    = super_90_load,
// 		.read_super	    = super_90_read
// 	},
// 	[1] = {
// 		.name	= "md-1",
// 		.load_super	    = super_1_load,
// 		.read_super	    = super_1_read
// 	},
// };

static inline sector_t calc_dev_sboffset(struct block_device *bdev)
{
     sector_t num_sectors = i_size_read(bdev->bd_inode) / 512;
     return MD_NEW_SIZE_SECTORS(num_sectors);
}

static int super_90_load( struct block_device *bdev, struct mddev *mddev, struct page *sb_page)
{
  char b[BDEVNAME_SIZE];
  int ret;
  sector_t sb_start;
  struct mdp_super_t *sb;

	ret = 0;
  sb_start = calc_dev_sboffset(bdev);
	ret = sync_page_io(bdev,mddev,sb_start,sb_page);
	if(ret)
		return ret;

	bdevname(bdev, b);
	sb = page_address(sb_page);
	return 0;
}

int sync_page_io(struct block_device *bdev, struct mddev *mddev, sector_t sb_start, struct page *page)
{
	struct bio *bio;
	bio = bio_alloc_mddev(GFP_NOIO, 1, mddev);
	bio->bi_bdev = bdev;
	bio->bi_iter.bi_sector = sb_start;
	bio_add_page(bio, page, MD_SB_BYTES, 0);
	submit_bio_wait(READ, bio);
	ret = test_bit(BIO_UPTODATE, &bio->bi_flags);
	bio_put(bio);
	return ret;
}

EXPORT_SYMBOL_GPL(sync_page_io);

int main(int argc, char **argv)
{
  if(argc < 2)
    err(EXIT_FAILURE, _("not enough input values"));

  char* dev = argv[1];

	int fd = open(dev, O_RDONLY);

	if (fd < 0)
		err(EXIT_FAILURE,"error opening %s: %s\n", dev, strerror(errno));

	char nm[32], *nmp;

	nmp = fd2devnm(fd);
	if (!nmp) {
		pr_err("Cannot find %s in sysfs!!\n", devname);
		return 1;
	}
	strcpy(nm, nmp);

	super_90_load();

  return EXIT_SUCCES;
}
