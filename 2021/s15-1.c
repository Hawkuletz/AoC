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
//	printf("Occupy %d,%d\n",x,y);
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
	printf("Total=%d\n",mp->cost);
	tc=mp->cost;
	while(mp!=NULL)
	{
		printf("At %d,%d, cost=%d\n",mp->x,mp->y,mp->cost);
		mp=mp->pstep;
	}
	return tc;
}

void step(int x, int y, mypath *mp)
{
	mypath *tp;
	int ocost,mcost;
	steps++;
//	printf("Step at %d,%d with cost %d\n",x,y,path_cost(mp));
	tp=omap[y*mx+x];
	if(tp!=NULL)
	{
		ocost=tp->cost;
		if(mp!=NULL)
			mcost=mp->cost+cmap[y*mx+x];
		else
			mcost=cmap[y*mx+x];

		/* stop if we are not cheaper */
		if(mcost>=ocost) return;
	}

	tp=occupy(x,y,mp);
	if(x>0) step(x-1,y,tp);
	if(x<(mx-1)) step(x+1,y,tp);
	if(y>0) step(x,y-1,tp);
	if(y<(my-1)) step(x,y+1,tp);
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

int main(int argc, char *argv[])
{
	char *buf;
	char *crtl;
	char **lines;
	int lc,i,j,cmoff;
	if(argc!=2)
	{
		fprintf(stderr,"Usage: %s <filename>\n",argv[0]);
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
	my=lc;
	/* occupy map */
	omap=calloc(mx*my,sizeof(mypath *));
	/* cost map */
	cmap=malloc(mx*my);

	/* copy data into cmap */
	cmoff=0;
	for (i=0;i<my;i++)
	{
		crtl=lines[i];
		for(j=0;j<mx;j++)
		{
			cmap[cmoff++]=crtl[j]-0x30;
		}
	}

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

/*	step(0,0,NULL,0); */
	show_path(omap[(my-1)*mx+mx-1]);

	printf("After %u steps\n",steps);

	free(lines);
	free(buf);
	return 0;
}
