#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

#define MAX_PROC_SIZE 100

static int op1;
static int op2;
static char operator;
static int ans;

#ifdef PROC
static struct proc_dir_entry *proc_op1_entry;
static struct proc_dir_entry *proc_op2_entry;
static struct proc_dir_entry *proc_operator_entry;
static struct proc_dir_entry *proc_ans_entry;
#endif

void countAns()
{
    if (operator == '+'){
        ans = op1 + op2;
    }
    if (operator == '-'){
        ans = op1 - op2;
    }
    if (operator == '*'){
        ans = op1 * op2;
    }
    if (operator == '/'){
        ans = op1 / op2;
    }
}

#ifdef PROC

int read_proc_op1(char *buf, char **start, off_t offset, int count, int *eof, void *data )
{
    int len=0;
    len = sprintf(buf, "\n %d\n ", op1);
    return len;
}

int write_proc_op1(struct file *file, const char *buf, int count, void *data )
{
    char tmpVal[30];
    if (count > MAX_PROC_SIZE)
        count = MAX_PROC_SIZE;
    if (copy_from_user(tmpVal, buf, count))
        return -EFAULT;
    sscanf(tmpVal, "%d", &op1);    

    return count;
}

int read_proc_op2(char *buf, char **start, off_t offset, int count, int *eof, void *data )
{
    int len=0;
    len = sprintf(buf, "\n %d\n ", op2);
    return len;
}

int write_proc_op2(struct file *file, const char *buf, int count, void *data )
{
    char tmpVal[30];
    if(count > MAX_PROC_SIZE)
        count = MAX_PROC_SIZE;
    if(copy_from_user(tmpVal, buf, count))
        return -EFAULT;
    sscanf(tmpVal, "%d", &op2);    

    return count;
}

int read_proc_operator(char *buf, char **start, off_t offset, int count, int *eof, void *data )
{
    int len=0;
    len = sprintf(buf, "\n %c\n ", operator);

    return len;
}

int write_proc_operator(struct file *file, const char *buf, int count, void *data )
{
    sscanf(buf, "%c", &operator);
    return count;
}

int read_proc_ans(char *buf, char **start, off_t offset, int count, int *eof, void *data )
{
    int len=0;
    countAns();
    len = sprintf(buf, "\n %d\n ", ans);

    return len;
}

#else // sysfs

static ssize_t read_sysfs_op1 (struct kobject *kobj, struct kobj_attribute *attr, char *buf) 
{
    return sprintf(buf, "%d\n", op1);
}
 
static ssize_t write_sysfs_op1 (struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) 
{
    sscanf(buf, "%du", &op1);
    return count;
}

static struct kobj_attribute op1_attribute = 
    __ATTR(op1, 0666, read_sysfs_op1, write_sysfs_op1);

static ssize_t read_sysfs_op2 (struct kobject *kobj, struct kobj_attribute *attr, char *buf) 
{
    return sprintf(buf, "%d\n", op2);
}
 
static ssize_t write_sysfs_op2 (struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count) 
{
        sscanf(buf, "%du", &op2);
        return count;
}

static struct kobj_attribute op2_attribute = 
    __ATTR(op2, 0666, read_sysfs_op2, write_sysfs_op2);

static ssize_t read_sysfs_oper (struct kobject *kobj, struct kobj_attribute *attr, char *buf) 
{
    int len=0;
    len = sprintf(buf, "\n %c\n ", operator);
    return len;
}
 
static ssize_t write_sysfs_oper (struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) 
{
    sscanf(buf, "%c", &operator);
    return count;
}

static struct kobj_attribute operator_attribute = 
    __ATTR(operator, 0666, read_sysfs_oper, write_sysfs_oper);

static ssize_t read_sysfs_ans (struct kobject *kobj, struct kobj_attribute *attr, char *buf) 
{
    countAns();
    return sprintf(buf, "%d\n", ans);
}
 
static ssize_t write_sysfs_ans (struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    sscanf(buf, "%du", &ans);
    return count;
}

static struct kobj_attribute res_attribute = 
    __ATTR(ans, 0666, read_sysfs_ans, write_sysfs_ans);


static struct attribute *attrs[] = {
        &op1_attribute.attr,
        &op2_attribute.attr,
        &operator_attribute.attr,
        &res_attribute.attr,
        NULL,
};

static struct attribute_group attr_group = {
       .attrs = attrs,
};

static struct kobject *example_kobj;

#endif // PROC

#ifdef PROC
void create_new_proc_entry()
{
    proc_op1_entry = create_proc_entry("calc_op1", 0666, NULL);
    proc_op2_entry = create_proc_entry("calc_op2", 0666, NULL);
    proc_operator_entry = create_proc_entry("calc_operator", 0666, NULL);
    proc_ans_entry = create_proc_entry("calc_ans", 0666, NULL);
    if (!(proc_op1_entry && proc_op2_entry && proc_operator_entry && proc_ans_entry)) {
        printk(KERN_INFO "Error creating proc entries");
        return -ENOMEM;
    }
    proc_op1_entry->read_proc = read_proc_op1;
    proc_op1_entry->write_proc = write_proc_op1;
    proc_op2_entry->read_proc = read_proc_op2;
    proc_op2_entry->write_proc = write_proc_op2;
    proc_operator_entry->read_proc = read_proc_operator;
    proc_operator_entry->write_proc = write_proc_operator;
    proc_ans_entry->read_proc = read_proc_ans;
    printk(KERN_INFO "proc initialized");
}

#else // sysfs

int create_new_sysfs_entry()
{   
    int retval;
    printk("in create_new_sysfs_entry\n");
    example_kobj = kobject_create_and_add("calcul", kernel_kobj);
    if (!example_kobj) {
        printk("exeption\n");
        return -ENOMEM;
    }

    /* Create the files associated with this kobject */
    retval = sysfs_create_group(example_kobj, &attr_group);
    printk("retval: %d\n", retval);
    if (retval) {
        kobject_put(example_kobj);
    }
    return 0;
}

#endif // PROC

int init (void)
{
    printk(KERN_INFO "Inside init_module\n");
#ifdef PROC
    create_new_proc_entry();
#else 
    create_new_sysfs_entry();
#endif // PROC
    return 0;
}

void cleanup(void)
{
    printk(KERN_INFO "Inside cleanup_module\n");
#ifdef PROC
    remove_proc_entry("calc_op1", NULL);
    remove_proc_entry("calc_op2", NULL);
    remove_proc_entry("calc_operator", NULL);
    remove_proc_entry("calc_ans", NULL);
#else
    //kobject_del(example_kobj);
#endif
}

MODULE_LICENSE("GPL");   
module_init(init);
module_exit(cleanup);
 