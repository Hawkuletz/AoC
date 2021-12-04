#include "mystrings.c"

int val2draw(char *vals, int **draw)
{
	char *crt;
	int *d;
	int cnt=1,i=0;
	/* count (start from 1 because we have one more values than commas */
	crt=vals;
	while((crt=strchr(crt,','))!=NULL)
	{
		cnt++;
		crt++;
	}
	if(!cnt || (d=calloc(cnt,sizeof(int)))==NULL) return -1;

	/* tokenize and load into draw */
	crt=strtok(vals,",");
	while(crt!=NULL)
	{
		d[i++]=atoi(crt);
		crt=strtok(NULL,",");
	}
	*draw=d;
	return cnt;
}

int fill_card_line(char *src,int *dst)
{
	char *v;
	int i;
	v=strtok(src," ");
	for(i=0;i<5;i++)
	{
		if(v==NULL)
		{
			fprintf(stderr,"something went wrong, NULL encountered when looking for value\n");
			exit(1);
		}
		dst[i]=atoi(v);
		v=strtok(NULL," ");
	}
	return 0;
}

int fill_cards(char *src[], int *dst[], int cnt)
{
	int *card;
	int i;
	for(i=0;i<cnt;i++)
	{
		card=calloc(50,sizeof(int));
		if(card==NULL)
		{
			fprintf(stderr,"malloc failed\n");
			exit(1);
		}
		dst[i]=card;
		fill_card_line(src[2+i*6],card);
		fill_card_line(src[3+i*6],card+5);
		fill_card_line(src[4+i*6],card+10);
		fill_card_line(src[5+i*6],card+15);
		fill_card_line(src[6+i*6],card+20);
	}


	return 0;
}

void print_card(int *card)
{
	int i;
	for (i=0;i<25;i++)
	{
		if(i && !(i%5))
			printf("\n");
		printf("%2d ",card[i]);
	}
	printf("\n---------------\n");
}

void print_marked_card(int *card)
{
	int i;
	int *mark=card+25;
	char c;
	for (i=0;i<25;i++)
	{
		if(i && !(i%5))
			printf("\n");
		if(mark[i]) c='*'; else c=' ';
		printf("%2d%c ",card[i],c);
	}
	printf("\n---------------\n");
}



void mark_card(int d, int *card)
{
	int i;
	for(i=0;i<25;i++)
		if(card[i]==d)
			card[i+25]=1;
}

void print_sum(int *card)
{
	int *mark=card+25;
	int i,sum=0;
	for(i=0;i<25;i++)
		if(!mark[i]) sum+=card[i];
	printf("Sum is %d\n",sum);
}

int chk_card(int *card)
{
	int i,j,h,v;
	int *mark=card+25;
	for(i=0;i<5;i++)
	{
		h=1;
		v=1;
		for(j=0;j<5;j++)
		{
			/* horizontal */
			if(mark[i*5+j]!=1)
				h=0;
			/* vertical */
			if(mark[i+j*5]!=1)
				v=0;
		}
		if(h || v)
		{
			printf("BINGO!\n");
			print_marked_card(card);
			print_sum(card);
			return 1;
		}
	}
	return 0;
}

void empty_card(int *dst)
{
	int i;
	int *mrk=dst+25;

	for(i=0;i<25;i++)
	{
		dst[i]=-1;
		mrk[i]=0;
	}
}

int drw_num(int d, int *cards[], int ccnt, int *win, int *dwin)
{
	int i;
	for(i=0;i<ccnt;i++)
	{
		mark_card(d,cards[i]);
		if(chk_card(cards[i]))
		{
			memcpy(win,cards[i],50*sizeof(int));
			*dwin=d;
			empty_card(cards[i]);
		}
	}
	return -1;
}



int main(int argc, char *argv[])
{
	char *buf;
	char **lines;
	int *draw;
	int **cards;
	int *crt;
	int ccnt,dcnt;
	int lastwin[50];
	int ldrwin=-1;
	int x;

	int lc,i;
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
	dcnt=val2draw(lines[0],&draw);
	ccnt=(lc-1)/6;
	if((lc-1)%6)
	{
		fprintf(stderr,"Implausible number of lines: %d\n",lc);
		exit(1);
	}
	cards=calloc(ccnt,sizeof(int *));
	fill_cards(lines,cards,ccnt);

	for(i=0;i<dcnt;i++)
	{
		printf("Draw %d\n",draw[i]);
		drw_num(draw[i],cards,ccnt,lastwin,&ldrwin);
	}

	printf("Last winning draw was: %d\n",ldrwin);
/*	print_card(lastwin); */
	print_sum(lastwin);

	for(i=0;i<ccnt;i++)
		free(cards[i]);
	free(cards);
	free(draw);
	free(lines);
	free(buf);
	return 0;
}
