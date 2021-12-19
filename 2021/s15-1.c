#include "mystrings.c"

typedef struct mypath mypath;
struct mypath
{
	int x;
	int y;
	int cost;
	mypath *pstep;
};

/* some globals used everywhere */
int mx,my;
mypath **omap;
char *cmap;

unsigned int steps=0;

mypath *occupy(int x, int y, mypath *mp)
{
	unsigned int moff;
	mypath *np;
	moff=y*mx+x;
	np=malloc(sizeof(mypath));
	if(!np)
	{
		fprintf(stderr,"malloc!\n");
		exit(1);
	}
//	printf("Occupy %d,%d\n",x,y);
	np->x=x;
	np->y=y;
	np->cost=cmap[moff];
	np->pstep=mp;

	/* since we are on what is effectively a tree structure, it's quite difficult
	 * to free the old branch, so we admit some leaks here */
	omap[moff]=np;
	return np;
}

int path_cost(mypath *mp)
{
	int rc=0;
	while(mp!=NULL)
	{
		rc+=mp->cost;
		mp=mp->pstep;
	}
	return rc;
}

int show_path(mypath *mp)
{
	int tc=0;
	while(mp!=NULL)
	{
		printf("At %d,%x, cost=%x\n",mp->x,mp->y,mp->cost);
		tc+=mp->cost;
		mp=mp->pstep;
	}
	printf("Total=%d\n",tc);
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
		ocost=path_cost(tp->pstep);
		mcost=path_cost(mp);
		if(mcost>=ocost) /* stop if we are not cheaper */
		{
			return;
		}
	}

	tp=occupy(x,y,mp);
	if(x<(mx-1)) step(x+1,y,tp);
	if(y<(my-1)) step(x,y+1,tp);
	if(x>0) step(x-1,y,tp);
	if(y>0) step(x,y-1,tp);
}


int main(int argc, char *argv[])
{
	char *buf;
	char *crtl;
	char **lines;
	int lc,i,j,cmoff;
	mypath *cp;
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
	step(0,0,NULL);
	show_path(omap[(my-1)*mx+mx-1]);

	printf("After %u steps\n",steps);

	free(lines);
	free(buf);
	return 0;
}
