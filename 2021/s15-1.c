#include "mystrings.c"

typedef struct mypath mypath;
struct mypath
{
	int x;
	int y;
	unsigned int cost;
	char *tries;
	mypath *pstep;
};

mypath **frontarr, **frontback;
uint32_t maxfrontsize=4;
uint32_t backfrontsize;
uint32_t crtfront;

/* some globals used everywhere */
int mx,my;
mypath **omap;
char *cmap;

unsigned int steps=0;

mypath *occupy(int x, int y, mypath *mp)
{
	unsigned int moff;
	unsigned int oldc=0;
	mypath *np;
	moff=y*mx+x;
	if(mp!=NULL) oldc=mp->cost;
	np=malloc(sizeof(mypath));
	if(!np)
	{
		fprintf(stderr,"malloc!\n");
		exit(1);
	}

	np->x=x;
	np->y=y;
	np->cost=oldc+cmap[moff];
	np->pstep=mp;

	omap[moff]=np;
	return np;
}

int show_path(mypath *mp)
{
	int tc=0;
	if(mp==NULL) return 0;
	tc=mp->cost;
	while(mp!=NULL)
	{
		printf("At %d,%d, cost=%d\n",mp->x,mp->y,mp->cost);
		mp=mp->pstep;
	}
	return tc;
}

void add2front(mypath *p)
{
	if(crtfront==maxfrontsize)
	{
		frontarr=realloc(frontarr,2*maxfrontsize*sizeof(mypath *));
		frontback=realloc(frontback,2*maxfrontsize*sizeof(mypath *));
		if(frontarr==NULL || frontback==NULL)
		{
			fprintf(stderr,"add2front: malloc\n");
			exit(1);
		}
		maxfrontsize*=2;
	}
	frontarr[crtfront++]=p;
}

int chk_front(int x, int y)
{
	int i;
	mypath *p;
	for(i=0;i<crtfront;i++)
	{
		p=frontarr[i];
		if(p->x==x && p->y==y)
			return i;
	}
	return -1;
}

void try_step(mypath *p, int x, int y)
{
	mypath *tp,*op;
	int fi;
	unsigned int crt_cost;
	unsigned int aoff=y*mx+x;
	tp=omap[aoff];
	steps++;
	crt_cost=p->cost+cmap[aoff];
	if(tp!=NULL && tp->cost<=crt_cost)
		return;
	fi=chk_front(x,y);
	if(fi==-1)
		add2front(occupy(x,y,p));
	else
	{
		op=frontarr[fi];
		if(op->cost>crt_cost)
			frontarr[fi]=occupy(x,y,p);
	}
}

void proc_step(mypath *p)
{
	if(p->x>0) try_step(p,p->x-1,p->y);
	if(p->x<(mx-1)) try_step(p,p->x+1,p->y);
	if(p->y>0) try_step(p,p->x,p->y-1);
	if(p->y<(my-1)) try_step(p,p->x,p->y+1);
}


void proc_front()
{
	int i;
	for (i=0;i<backfrontsize;i++)
		proc_step(frontback[i]);

	memcpy(frontback,frontarr,crtfront*sizeof(mypath *));
	backfrontsize=crtfront;
	crtfront=0;
}


/* quick&(very)dirty enlarge helper */
void line1plus(int soff, int doff, int l)
{
	int i;
	char c;
	for(i=0;i<l;i++)
	{
		c=1+cmap[soff+i];
		if(c==10) c=1;
		cmap[doff+i]=c;
	}
}

/* quick&(very)dirty enlarge for 2nd part of the challenge */
void embiggen(int ybsize)
{
	int b,y,soff,doff;
	for(b=1;b<5;b++)
		for(y=0;y<ybsize;y++)
		{
			soff=((b-1)*ybsize+y)*mx;
			doff=(b*ybsize+y)*mx;
			line1plus(soff,doff,mx);
		}
}




int main(int argc, char *argv[])
{
	char *buf;
	char *crtl;
	char **lines;
	char cc;
	int lc,i,j,k,l,cmoff,mxo;
	int c2=0;	/* challenge 2 */
	if(argc<2)
	{
		fprintf(stderr,"Usage: %s <filename> [2]\n",argv[0]);
		exit(1);
	}
	buf=file2buf(argv[1]);
	if(!buf)
	{
		fprintf(stderr,"Unable to read file %s\n",argv[1]);
		exit(1);
	}
	lc=buf2lines(buf,&lines);
	if(lc<=0)
	{
		fprintf(stderr,"Unable to read or no lines found\n");
		exit(1);
	}

	mx=strlen(lines[0]);	/* assume rectangular shape */
	mxo=mx;
	my=lc;
	if(argc==3 && atoi(argv[2])==2) /* make it bigger! */
	{
		c2=1;
		mx=mx*5;
		my=my*5;
	}
	/* occupy map */
	omap=calloc(mx*my,sizeof(mypath *));
	/* (original) cost map */
	cmap=malloc(mx*my);

	/* copy data into cmap */
	cmoff=0;
	for (i=0;i<lc;i++)
	{
		crtl=lines[i];
		cmoff=i*mx;
		for(j=0;j<mxo;j++)
		{
			cc=crtl[j]&0x0f;
			cmap[cmoff]=cc;
			if(c2)
			{
				for(k=1;k<5;k++)
				{
					cc++;
					if(cc>9) cc=1;
					cmap[cmoff+mxo*k]=cc;
				}
			}
			cmoff++;
		}
	}
	if(c2)
		embiggen(lc);

	/* show cost map, for debug */
/*
	for(i=0;i<my;i++)
	{
		for(j=0;j<mx;j++)
			printf("%d",cmap[i*mx+j]);
		printf("\n");
	}
*/

	printf("mx=%d, my=%d\n",mx,my);

	frontarr=calloc(maxfrontsize,sizeof(mypath *));
	frontback=calloc(maxfrontsize,sizeof(mypath *));

	backfrontsize=1;
	frontback[0]=occupy(0,0,NULL);

	while(backfrontsize)
	{
		printf("%d steps\n",backfrontsize);
		proc_front();
	}

	show_path(omap[(my-1)*mx+mx-1]);

	printf("After %u steps\n",steps);
	/* challenge demands cost without the first cave */
	printf("Cost is %d",omap[(my-1)*mx+mx-1]->cost-cmap[0]);

	free(lines);
	free(buf);
	return 0;
}
