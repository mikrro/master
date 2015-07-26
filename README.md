fssuper - this is a module to load block device superblock 
compiled by make

to load module pass with root priviladges
	inmod fssuper dev='"/dev/devname"'
to unload module 
	rmmod fssuper
