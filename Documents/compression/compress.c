#include <stdio.h>
#include <string.h>

int convert(int);
int encodeNextCharacter(int,int *,FILE *,FILE *);
void compress(char *);
void compress2(char *, char *);
void helpScreen();
int main(int argc, char *argv[])
{
	if( argc == 2 )
	{
		if(strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"--help") == 0)
			helpScreen();
		else
			compress(argv[1]);	
  	}
  	else if(argc == 4)
  	{
  		if(strcmp(argv[1],"-o") == 0 || strcmp(argv[1],"--output") == 0)	
  			compress2(argv[3],argv[2]);
  		else if(strcmp(argv[2],"-o") == 0 || strcmp(argv[2],"--output") == 0)
  			compress2(argv[1],argv[3]);  			
  		else helpScreen();
  	}
  	else
  	{
  		helpScreen();
  	}
}

void compress(char *filename)
{
	FILE *fp;
	FILE *fh;
	int roundno=0;
	int ch2=0;
	fp = fopen(filename, "r");

	if(fp == NULL)
	{
		printf("cannot open file: %s\n", filename);
		helpScreen();
	}
	else
	{
		fh = fopen(strcat(filename,".rohit"),"w+");
		while ((roundno = encodeNextCharacter(roundno,&ch2,fp,fh)) >= 0);
		fclose(fp);
		fclose(fh);
	}
}

void compress2(char *filename, char *outputfile)
{
	FILE *fp;
	FILE *fh;
	int roundno=0;
	int ch2=0;
	fp = fopen(filename, "r");

	if(fp == NULL)
	{
		printf("cannot open file: %s\n", filename);
		helpScreen();
	}
	else
	{
		fh = fopen(strcat(outputfile,".rohit"),"w+");
		while ((roundno = encodeNextCharacter(roundno,&ch2,fp,fh)) >= 0);
		fclose(fp);
		fclose(fh);
	}
	//printf("%d",convert('A'));
	
}

int convert(int ch)
{
	if (ch >= 'a' && ch <= 'z') return (ch-33)%32;
	else if (ch >= 'A' && ch <= 'Z') return (ch-1)%32+31*32;
	else if (ch == ' ') return 26;
	else if (ch == '\n') return 27;
	else if (ch == '.') return 28;
	else if (ch == ',') return 29;
	else if (ch == '?') return 30;
	else return -1;
}

void helpScreen()
{
	printf("compress help screen:\n");
	printf("Usage: compress [option...] [file]\n");
	printf("-o, --output FILE\tRename output file to FILE\n");
	printf("-h  --help\t\tDisplay this help screen\n");
}

int encodeNextCharacter(int roundno,int *ch2,FILE *fp,FILE *fh)
{
	int c;
	c = fgetc(fp);
	c = convert(c);
	if (roundno == 0)
	{
		if(c < 0)
		{
			
			return -1;
		}
		else if (c <= 31)
		{
			*ch2 = c;
			return 5;
		}
		else
		{
			fputc(c/4,fh);
			*ch2 = c%4;
			return 2;
		}
	}
	if (roundno == 1)
	{
		if(c < 0)
		{
			fputc(*ch2*128+126,fh);
			
			return -1;
		}
		else if (c <=31)
		{
			*ch2 = *ch2*32+c;
			return 6;
		}
		else
		{
			fputc(*ch2*128+c/8,fh);
			*ch2 = c%8;
			return 3;
		}
	}
	if (roundno == 2)
	{
		if(c < 0)
		{
			fputc(*ch2*64+62,fh);
			
			return -1;
		}
		else if (c <=31)
		{
			*ch2 = *ch2*32+c;
			return 7;
		}
		else
		{
			fputc(*ch2*64+c/16,fh);
			*ch2 = c%16;
			return 4;
		}
	}
	if (roundno == 3)
	{
		if(c < 0)
		{
			fputc(*ch2*32+31,fh);
			
			return -1;
		}
		else if (c <=31)
		{
			fputc(*ch2*32+c,fh);
			*ch2 = 0;
			return 0;
		}
		else
		{
			fputc(*ch2*32+c/32,fh);
			*ch2 = c%32;
			return 5;
		}
	}
	if (roundno == 4)
	{
		if(c < 0)
		{
			fputc(*ch2*16,fh);
			
			return -1;
		}
		else if (c <=31)
		{
			fputc(*ch2*16+c/2,fh);
			*ch2 = c%2;
			return 1;
		}
		else
		{
			fputc(*ch2*16+c/64,fh);
			*ch2 = c%64;
			return 6;
		}
	}
	if (roundno == 5)
	{
		if(c < 0)
		{
			fputc(*ch2*8,fh);
			
			return -1;
		}
		else if (c <=31)
		{
			fputc(*ch2*8+c/4,fh);
			*ch2 = c%4;
			return 2;
		}
		else
		{
			fputc(*ch2*8+c/128,fh);
			*ch2 = c%128;
			printf("%d\n", *ch2);
			return 7;
		}
	}
	if (roundno == 6)
	{
		if(c < 0)
		{
			fputc(*ch2*4,fh);
			
			return -1;
		}
		else if (c <=31)
		{
			fputc(*ch2*4+c/8,fh);
			*ch2 = c%8;
			return 3;
		}
		else
		{
			fputc(*ch2*4+c/256,fh);
			fputc(c%256,fh);
			*ch2 = 0;
			return 0;
		}
	}
	if (roundno == 7)
	{
		if(c < 0)
		{
			fputc(*ch2*2,fh);
			
			return -1;
		}
		else if (c <=31)
		{
			fputc(*ch2*2+c/16,fh);
			*ch2 = c%16;
			return 4;
		}
		else
		{
			fputc(*ch2*2+c/512,fh);
			*ch2 = c%512;
			fputc(*ch2/2,fh);
			*ch2 = *ch2%2;
			return 1;
		}
	}
}