#ifndef __MYLIST_C__
#define __MYLIST_C__

#ifndef NULL	/* missing stdlib? */
# include <stdlib.h>
#endif

#include "mylist.h"

void myfreelst(mylst *f)
{
	mylst *c,*t;
	if(f==NULL) return;
	c=f;
	while(c!=NULL)
	{
		t=c->n;
		free(c);
		c=t;
	}
}

void myfreelstwd(mylst *f)
	/* frees both list item and list data */
{
	mylst *c,*t;
	if(f==NULL) return;
	c=f;
	while(c!=NULL)
	{
		t=c->n;
		free(c->data);
		free(c);
		c=t;
	}
}


mylst *mystorelst(mylst *c, void *d)
/* store new list item after c. c can be NULL
 * returns new item or NULL if calloc fails */
{
	mylst *n=calloc(1,sizeof(mylst));
	if(n==NULL) return NULL;
	n->data=d;
	if(c!=NULL) c->n=n;
	return n;
}

mylst *myaddlst(mylst *c, void *d)
/* walks list starting at c, adds item d at the end
 * returns new item address or NULL in case of failure */
{
	if(c==NULL) return mystorelst(NULL,d);
	while(c->n!=NULL) c=c->n;
	return mystorelst(c,d);
}

mylst *myaddlstu(mylst **c, void *d)
/* same as myaddlst but if c is NULL updates it to point to added element */
{
	mylst *l=myaddlst(*c,d);
	if(*c==NULL) *c=l;
	return l;
}

void myremovelsti(mylst **h, mylst *i)
/* remove list item identified as i */
{
	mylst *th,*tp,*c;
	if(h==NULL || *h==NULL) return;
	th=*h;
	c=*h;
	while(c!=NULL)
	{
		if(c==i)
		{
			if(c==th)
				*h=c->n;
			else
				tp->n=c->n;
			free(c);
			return ;
		}
		tp=c;
		c=c->n;
	}
}

void myremovelstd(mylst **h, void *d)
/* remove list item pointing to d */
{
	mylst *c,*tp;
	if(h==NULL || *h==NULL) return;
	c=*h;
	tp=*h; /* initialized to valid pointer just to silence warnings with -O3 */
	for(;c!=NULL;c=c->n)
	{
		if(c->data==d)
		{
			if(c==*h)
				*h=c->n;
			else
				tp->n=c->n;
			free(c);
			return;
		}
		tp=c;
	}
}

void myproclst(mylst *l,void (*fp)())
/* executes fp(l->data) for each item */
/* WARNING: use myproclst_ds if fp removes item it operates on */
{
	for(;l!=NULL;l=l->n)
		fp(l->data);
}

void myproclst_ds(mylst *l,void (*fp)())
/* executes fp(l->data) for each item */
/* delsafe - current list item can be removed by fp */
{
	mylst *t=l;
	while(l!=NULL)
	{
		t=l->n;
		fp(l->data);
		l=t;
	}
}

void myproclst_arg(mylst *l,void (*fp)(),void *arg)
/* similar to myproclst but call function with arg */
{
	for(;l!=NULL;l=l->n)
		fp(l->data,arg);
	
}

void mylstitohead(mylst **l, mylst *i)
/* move i to top of list */
{
	mylst *c;
	if(l==NULL || *l==NULL || i==NULL || i==*l) return;
	c=*l;
	for(;c->n!=i;c=c->n)
		if(c->n==NULL) return;
	c->n=i->n;
	i->n=*l;
	*l=i;
}

int mylstdtohead(mylst **l, void *d)
/* search list for item pointing to d, move it to the top.
 * returns 0 on success, 1 if not found, -1 on invalid or empty list */
{
	mylst *c,*i;
	if(l==NULL || *l==NULL) return -1;
	c=*l;
	for(;c->n!=NULL;c=c->n)
	{
		if(c->n->data==d)
		{
			i=c->n;
			c->n=i->n;
			i->n=*l;
			*l=i;
			return 0;
		}
	}
	return 1;
}

#endif
