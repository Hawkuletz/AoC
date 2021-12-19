#include "mystrings.c"

#define PTYP_LIT	4

unsigned int version_sum=0;

void print_dword_bits(uint32_t x)
{
	int i;
	uint32_t cm=1<<31;
	printf("\n");
	for(i=0;i<32;i++)
	{
		if(x&cm) printf("1"); else printf("0");
		cm=cm>>1;
	}
	printf("\n");
	printf("       ^       ^       ^       ^\n");
}

/* count MSB to LSB (i.e. MSB is bit 0) */
int get_bits(uint8_t *msg, uint32_t pos, uint32_t cnt, uint32_t *res)
{
	uint32_t rv=0;
	uint8_t c;
	uint32_t bcnt,bpos,brm,mask;
	if(cnt>32)
		return -1;
	
//	printf("Requested %u bits at %u\n",cnt,pos);

	/* position in byte stream */
	bpos=pos>>3;
	/* position inside byte */
	brm=pos&7;
	/* available bits in byte */
	bcnt=8-brm;
	/* mask is 2^(8-brm)-1 (remove left bits) */
	mask=(1<<bcnt)-1;

//	printf("At %u, remainder %u, count=%u, mask= %x\n",bpos,brm,bcnt,mask);

	while(1)
	{
		c=msg[bpos];
		c=c&mask;
		if(cnt<bcnt)
		{
//			printf("shift right %u (%u-%u)\n",bcnt-cnt,bcnt,cnt);
			c>>=(bcnt-cnt);
//			printf("%02x\n",c);
			bcnt=cnt;
		}
		rv<<=bcnt;
		rv|=c;
		cnt-=bcnt;
		if(!cnt) break;
		bpos++;
		bcnt=8;
		mask=0xff;
	}
	*res=rv;
	return 0;
}


uint32_t proc_msg(uint8_t *msg, uint32_t moff, int msg_size)
{
	uint32_t a,pvers,ptype;
	uint32_t bli=0, pci=0;
	uint32_t lval=0;
	uint32_t rbits=0;
	int nibcnt=0;
	if((msg_size-moff)<6)
	{
		fprintf(stderr,"less than 6 bits remaining(%d)\n",msg_size-moff);
		exit(1);
	}
	get_bits(msg,moff+0,3,&pvers);
	get_bits(msg,moff+3,3,&ptype);
	version_sum+=pvers;
	printf("Packet version %u type %u\n",pvers,ptype);
	if(ptype!=PTYP_LIT)
	{
		get_bits(msg,moff+6,1,&a);
		if(a)
		{
			get_bits(msg,moff+7,11,&pci);
			printf("contains %u packets\n",pci);
			moff+=7+11;
			while(pci)
			{
				moff=proc_msg(msg,moff,msg_size);
				pci--;
			}

		}
		else
		{
			get_bits(msg,moff+7,15,&bli);
			printf("contains %u bits\n",bli);
			moff+=7+15;
			while(bli>6)
			{
				rbits=proc_msg(msg,moff,msg_size);
				bli-=(rbits-moff);
				moff=rbits;
				printf("read: %u, remaining: %u\n",rbits,bli);
			}
		}
	}
	else
	{
		moff+=6;
		do
		{
			get_bits(msg,moff,5,&a);
			moff+=5;
			lval|=(a&0xf);
//			printf("Got %x for literal, nibble %u\n",a,nibcnt);
			nibcnt++;
			if(nibcnt>8)
			{
				fprintf(stderr,"Value too large at %u\n",moff);
				exit(1);
			}
			if(a&0x10)
			{
				lval<<=4;
			}
		} while(a&0x10);
		printf("Value = %u with %u nibbles\n",lval,nibcnt);
		return moff;
	}
	return moff;
}


int main(int argc, char *argv[])
{
	char *buf;
	char **lines;
	uint8_t *msg;
	int lc,i;
	int msg_size;

/*	uint8_t ta[]={0b10010110,0b11100011,0b10101010};
	uint32_t t; */

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

	for(i=0;i<lc;i++)
	{
		msg_size=hex2bin((char **) &msg,lines[i],NULL);
		msg_size*=8; /* in bits */
		printf("Msg size is %u\n",msg_size);
		version_sum=0;
		proc_msg(msg,0,msg_size);
		printf("Version sum is %u\n",version_sum);
	}


/*	get_bits(ta,3,10,&t);
	print_dword_bits(t); */

	free(lines);
	free(buf);
	return 0;
}
