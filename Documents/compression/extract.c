#include <stdio.h>
#include <string.h>

int convert(int);
int decodeNextCharacter(int, int *, FILE *, FILE *);
void extract(char *);
void extract2(char *, char *);
void helpScreen();

int main(int argc, char *argv[])
{
	if( argc == 2 )
	{
		if(strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"--output") == 0)
			helpScreen();
		else
			extract(argv[1]);	
  	}
  	else if(argc == 4)
  	{
  		if(strcmp(argv[1],"-o") == 0 || strcmp(argv[1],"--output") == 0)
  			extract2(argv[3],argv[2]);
  		else if(strcmp(argv[2],"-o") == 0 || strcmp(argv[2],"--output") == 0)
  			extract2(argv[1],argv[3]);  			
  		else helpScreen();
  	}
  	else
  	{
  		helpScreen();
  	}
}
void extract(char *filename)
{
	FILE *fp;
	FILE *fh;
	char *outputfile;
	int ch2,roundno = 0;
	fp = fopen(filename,"r");
	strncpy(outputfile, filename, strlen(filename)-6);

	if(fp == NULL)
	{
		printf("cannot open file: %s\n", filename);
		helpScreen();
	}
	else
	{
		printf("%s\n", outputfile);
		fh = fopen(outputfile,"w+");
		while ((roundno = decodeNextCharacter(roundno,&ch2,fp,fh)) >= 0 );
		fclose(fp);
		fclose(fh);
	}
}

void extract2(char *filename, char *outputfile)
{
	FILE *fp;
	FILE *fh;
	int ch2,roundno = 0;
	fp = fopen(filename,"r");

	if(fp == NULL)
	{
		printf("cannot open file: %s\n", filename);
		helpScreen();
	}
	else
	{
		fh = fopen(outputfile,"w+");
		while ((roundno = decodeNextCharacter(roundno,&ch2,fp,fh)) >= 0 );
		fclose(fp);
		fclose(fh);
	}
}

int convert(int ch)
{
	if (ch >= 0 && ch <= 25) return 'a'+ch;
	else if (ch == 26) return ' ';
	else if (ch == 27) return '\n';
	else if (ch == 28) return '.';
	else if (ch == 29) return ',';
	else if (ch == 30) return '?';
}

void helpScreen()
{
	printf("extract help screen:\n");
	printf("Usage: compress [option...] [file]\n");
	printf("-o, --output FILE\tRename output file to FILE\n");
	printf("-h  --help\t\tDisplay this help screen\n");
}

int decodeNextCharacter(int roundno, int *ch2, FILE *fp, FILE *fh)
{
	int c,ch;
	c = fgetc(fp);if (c == -1) return -1;
	if (roundno == 0)
	{
		ch = c/8;
		*ch2 = c%8;
		if (ch == 31)
		{
			c = fgetc(fp);if (c == -1) return -1;
			ch = *ch2*4+c/64;
			fputc(convert(ch)-32,fh);
			*ch2 = c%64;
			ch = *ch2/2;
			*ch2 = *ch2%2;
			if (ch == 31)
			{
				c = fgetc(fp);if (c == -1) return -1;
				ch = *ch2*16+c/16;
				*ch2 = c%16;
				fputc(convert(ch)-32,fh);
				return 4;
			}
			else
			{
				fputc(convert(ch),fh);
				return 1;
			}
		}
		else
		{
			fputc(convert(ch),fh);
			return 3;
		}
	}
	else if (roundno == 1)
	{
		ch = *ch2*16+c/16;
		*ch2 = c%16;
		if (ch == 31)
		{
			c = fgetc(fp);if (c == -1) return -1;
			ch = *ch2*2+c/128;
			*ch2 = c%128;
			fputc(convert(ch)-32,fh);
			ch = *ch2/4;
			*ch2 = *ch2%4;
			if (ch == 31)
			{
				c = fgetc(fp);if (c == -1) return -1;
				ch = *ch2*8+c/32;
				*ch2 = c%32;
				fputc(convert(ch)-32,fh);
				ch = *ch2;
				if (ch == 31)
				{
					c = fgetc(fp);if (c == -1) return -1;
					ch = c/8;
					*ch2 = c%8;
					fputc(convert(ch)-32,fh);
					return 3;
				}
				else
				{
					fputc(convert(ch),fh);
					return 0;
				}
			}
			else
			{
				fputc(convert(ch),fh);
				return 2;
			}
		}
		else
		{
			fputc(convert(ch),fh);
			return 4;
		}
	}
	else if (roundno == 2)
	{
		ch = *ch2*8+c/32;
		*ch2 = c%32;
		if (ch == 31)
		{
			ch = *ch2;
			fputc(convert(ch)-32,fh);
			return 0;
		}
		else
		{
			fputc(convert(ch),fh);
			ch = *ch2;
			if (ch == 31)
			{
				c = fgetc(fp);if (c == -1) return -1;
				ch = c/8;
				*ch2 = c%8;
				fputc(convert(ch)-32,fh);
				return 3;
			}
			else
			{
				fputc(convert(ch),fh);
				return 0;
			}
		}
	}
	else if (roundno == 3)
	{
		ch = *ch2*4+c/64;
		*ch2 = c%64;
		if (ch == 31)
		{
			ch = *ch2/2;//hrer also
			*ch2 = *ch2%2;
								printf("%d\n", convert(ch)-32);

			fputc(convert(ch)-32,fh);
			return 1;
		}
		else
		{
			fputc(convert(ch),fh);
			ch = *ch2/2;
			*ch2 = *ch2%2;
			if (ch == 31)
			{
				c = fgetc(fp);if (c == -1) return -1;
				ch = *ch2*16+c/16;
				*ch2 = c%16;
				fputc(convert(ch)-32,fh);
				return 4;
			}
			else
			{
				fputc(convert(ch),fh);
				return 1;
			}
		}
	}
	else if (roundno == 4)
	{
		ch = *ch2*2+c/128;
		*ch2 = c%128;
		if (ch == 31)
		{
			ch = *ch2/4;//this might be wrong
			*ch2 = c%4;
			fputc(convert(ch)-32,fh);
			return 2;
		}
		else
		{
			fputc(convert(ch),fh);
			ch = *ch2/4;
			*ch2 = *ch2%4;
			if (ch == 31)
			{
				c = fgetc(fp);if (c == -1) return -1;
				ch = *ch2*8+c/32;
				*ch2 = c%32;
				fputc(convert(ch)-32,fh);
				ch = *ch2;
				if (ch == 31)
				{
					c = fgetc(fp);if (c == -1) return -1;
					ch = c/8;
					*ch2 = c%8;
					fputc(convert(ch)-32,fh);
					return 3;
				}
				else
				{
					fputc(convert(ch),fh);
					return 0;
				}
			}
			else
			{
				fputc(convert(ch),fh);
				return 2;
			}
		}
	}
}