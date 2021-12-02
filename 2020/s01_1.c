#include "mystrings.c"

int icmp(const void *v0, const void *v1)
{
	const int *a,*b;
	a=v0; b=v1;
	return *a-*b;
}

/* res = result array, arr = source array
 * i0 = starting position, len = (source) array length
 * lvl = level (depth) of search (i.e. when we need a triplet start at lvl 2
 * s = sum that we search for */
int srch(int *res, int *arr, int i0, int len, int lvl, int s)
{
	int i,x,rx;
	for(i=i0;i<len-lvl;i++)
	{
		x=arr[i];
		if(x>s) return 1;
		if(lvl!=0) /* all but last level recurse one level down */
		{
			rx=srch(res,arr,i+1,len,lvl-1,s-x);
			if(rx==0)	/* success */
			{
				res[lvl]=x;
				return 0;
			}
			else if(rx<0)	/* error, shouldn't happen */
				return -1;
		}
		else if(x==s)	/* last level */
		{
			res[0]=x;
			return 0;
		}
	}
	return -1;
}

int main(int argc, char *argv[])
{
	char *buf;
	char **lines;
	int *arr;
	int *res;
	int lc,i,lvl,ds;
	if(argc!=4)
	{
		fprintf(stderr,"Usage: %s <filename> <values_cnt> <desired_sum>\n",argv[0]);
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

	lvl=atoi(argv[2]);
	ds=atoi(argv[3]);
	if(lvl<2 || ds<2)
	{
		fprintf(stderr,"values_cnt and desired_sum must be above 2\n");
		exit(1);
	}
	
	res=calloc(lvl,sizeof(int));
	arr=calloc(lc,sizeof(int));
	if(arr==NULL || res==NULL)
	{
		fprintf(stderr,"alloc error\n");
		exit(1);
	}
	for(i=0;i<lc;i++)
	{
		/* quick and dirty conversion, assume input is valid */
		arr[i]=atoi(lines[i]);
	}
	
	qsort(arr,lc,sizeof(int),icmp);
	if(srch(res,arr,0,lc,lvl-1,ds)==0)
	{
		printf("Found values. Printing...\n");
		for(i=0;i<lvl;i++)
			printf("Result %d = %d\n",i,res[i]);
	}
	else
		printf("Not found\n");
	
	free(res);
	free(arr);
	free(lines);
	free(buf);
	return 0;
}
