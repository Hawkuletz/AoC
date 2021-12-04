#ifndef __MYLIST_H__
#define __MYLIST_H__
typedef struct mylst mylst;
struct mylst
{
	void *data;
	mylst *n;
};

void myfreelst(mylst *f);
void myfreelstwd(mylst *f);
/* frees both list item and list data */
mylst *mystorelst(mylst *c, void *d);
/* store new list item after c. c can be NULL
 * returns new item or NULL if calloc fails */

mylst *myaddlst(mylst *c, void *d);
/* walks list starting at c, adds item d at the end
 * returns new item address or NULL in case of failure */

mylst *myaddlstu(mylst **c, void *d);
/* same as myaddlst but if c is NULL updates it to point to added element */

void myremovelsti(mylst **h, mylst *i);
/* remove list item identified as i */

void myremovelstd(mylst **h, void *d);
/* remove list item pointing to d */

void myproclst(mylst *l,void (*fp)());
/* executes fp(l->data) for each item */
/* WARNING: use myproclst_ds if fp removes item it operates on */

void myproclst_ds(mylst *l,void (*fp)());
/* executes fp(l->data) for each item */
/* delsafe - current list item can be removed by fp */

void myproclst_arg(mylst *l,void (*fp)(),void *arg);
/* similar to myproclst but call function with arg */

void mylstitohead(mylst **l, mylst *i);
/* move i to top of list */

int mylstdtohead(mylst **l, void *d);
/* search list for item pointing to d, move it to the top.
 * returns 0 on success, 1 if not found, -1 on invalid or empty list */

#endif
