#include <linux/kernal.h>
#include <linux/module.h>
#include <linux/interrupt.h>

static int __init my_init(void)
{
	printf(KERN_INFO, "my_init\n");
	return 0;
}

void my_exit(void)
{
	printf(KERN_INFO, "my_exit\n");
}

module_init(my_init);
module_exit(my_exit);
MODULE_AUTHOR("Heejin Park <park0122@gmail.com>");
MODULE_LICENSE("GPL");
