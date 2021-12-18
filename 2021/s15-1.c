#include "mystrings.c"

typedef struct mypath mypath;
struct mypath
{
	int x;
	int y;
	int cost;
	mypath *pstep;
}

/* some globals used everywhere */
int mx,my;
mypath *omap;
char *cmap;

mypath *occupy(int x, int y, mypath *mp)
{
	unsigned int mv;
	mypath *np;
	mv=y*mx+x;
	np=malloc(sizeof(mypath));
	if(!np)
	{
		fprintf(stderr,"malloc!\n");
		exit(1);
	}
	np->x=x;
	np->y=y;
	np->cost=cmap[mv];
	np->pstep=mp;

	/* since we are on what is effectively a tree structure, it's quite difficult
	 * to free the old branch, so we admit some leaks here */
	omap[y*mx+x]=np;
	return np;
}

int is_occupied(int x, int y)
{
	return omap[y*mx+x];
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

int step(int x, int y, mypath *mp)
{
	mypath *tp;
	if((tp=omap[y*mx+x])!=NULL)
	{
		if(cost(tp)>cost(mp)) /* replace path if we are cheaper */
		{
			occupy(x,y,mp);
		}
		/* but anyway, stop here */
		return 1;
	}
	else
	{
		tp=occupy(x,y,mp);
		if(x>0) step(x-1,y,tp);
		if(x<mx) step(x+1,y,tp);
		if(y>0) step(x,y-1,tp);
		if(y<my) step(x,y+1,tp);
	}
	return 0;
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

	


	free(lines);
	free(buf);
	return 0;
}
