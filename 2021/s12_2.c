#include "mystrdos.c"
/* this is meant for DOS! */
int main(int argc, char *argv[])
{
	int fd;
	char *buf;
	char *lines[];
	char *fn;
	struct stat finfos;

	if(argc!=2)
	{
		printf("Usage: %s <datafile>\n");
		return 1;
	}
	buf=file2buf(argv[1]);
	if(buf==NULL)
	{
		printf("Unable to read file\n");
		return 1;
	}

	return 0;


}
