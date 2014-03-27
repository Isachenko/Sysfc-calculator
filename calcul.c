#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>

#define MAX_PROC_SIZE 100
static int op1;
static int op2;
static char operator;
static int ans;

static struct proc_dir_entry *proc_op1_entry;
static struct proc_dir_entry *proc_op2_entry;
static struct proc_dir_entry *proc_operator_entry;
static struct proc_dir_entry *proc_result_entry;

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
    char tmpVal[30];
    if(count > MAX_PROC_SIZE)
        count = MAX_PROC_SIZE;
    if(copy_from_user(tmpVal, buf, count))
        return -EFAULT;
    operator = tmpVal[0];

    return count;
}

int read_proc_result(char *buf, char **start, off_t offset, int count, int *eof, void *data )
{
    int len=0;
    countAns();
    len = sprintf(buf, "\n %d\n ", ans);

    return len;
}

void create_new_proc_entry()
{
    proc_op1_entry = create_proc_entry("proc_op1", 0666, NULL);
    proc_op2_entry = create_proc_entry("proc_op2", 0666, NULL);
    proc_operator_entry = create_proc_entry("proc_operator", 0666, NULL);
    proc_result_entry = create_proc_entry("proc_result", 0666, NULL);
    if (!(proc_op1_entry && proc_op2_entry && proc_operator_entry && proc_result_entry)) {
        printk(KERN_INFO "Error creating proc entries");
        return -ENOMEM;
    }
    proc_op1_entry->read_proc = read_proc_op1;
    proc_op1_entry->write_proc = write_proc_op1;
    proc_op2_entry->read_proc = read_proc_op2;
    proc_op2_entry->write_proc = write_proc_op2;
    proc_operator_entry->read_proc = read_proc_operator;
    proc_operator_entry->write_proc = write_proc_operator;
    proc_result_entry->read_proc = read_proc_result;
    printk(KERN_INFO "proc initialized");
}

int proc_init (void)
{
    create_new_proc_entry();
    return 0;
}

void proc_cleanup(void)
{
    printk(KERN_INFO "Inside cleanup_module\n");
    remove_proc_entry("proc_op1", NULL);
    remove_proc_entry("proc_op2", NULL);
    remove_proc_entry("proc_operator", NULL);
    remove_proc_entry("proc_result", NULL);
}
MODULE_LICENSE("GPL");   
module_init(proc_init);
module_exit(proc_cleanup);
 