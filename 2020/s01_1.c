#include "mystrings.c"

int icmp(const void *v0, const void *v1)
{
	const int *a,*b;
	a=v0; b=v1;
	return *a-*b;
}

/* search in array of length l, starting at position p, for pair that added yields s */
int srch_pair(int *arr, int l, int p, int s)
{
	int i,x;
	if(p>=l) return -1;
	s-=arr[p];
	for(i=l-1;i>p;i--)
	{
		x=arr[i];
		if(x<s) return 0;
		if(x==s) return i;
	}
	return 0;
}

int srch(int *arr, int l, int s, int *idx1, int *idx2)
{
	int i,x;
	for(i=0;i<l-1;i++)
	{
		if((x=srch_pair(arr,l,i,s))!=0)
		{
			*idx1=i;
			*idx2=x;
			return 0;
		}
	}
	return 1;
}

int srch(int *res, int *arr, int i0, int l, int lvl, int s)
{
	int i,x,rx;
	if(i0 >= (l-1))
		return -1;
	s-=arr[i0];
	for(i=i0+1;i<l;i++)
	{
		x=arr[i];
		if(x>s) return 1;
		if(lvl==0)
		{
			if(x==s)
			{
				res[lvl]=i;
				return 0;
			}
		}
		else
		{
			rx=srch(res,arr,i,l,lvl-1,s);
			if(rx==0)
			{
				res[lvl]=i;
				return 0;
			}
			else if(rx<0)
				return -1;
		}
	}
	return -1;
}

int main(int argc, char *argv[])
{
	char *buf;
	char **lines;
	int *arr;
	int lc,i;
	int idx1=0,idx2=0;
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
	arr=calloc(lc,sizeof(int));
	if(arr==NULL)
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
	if(srch(arr,lc,2020,&idx1,&idx2)==0)
		printf("Found values %d and %d\n",arr[idx1],arr[idx2]);
	else
		printf("Not found\n");

	return 0;
}
