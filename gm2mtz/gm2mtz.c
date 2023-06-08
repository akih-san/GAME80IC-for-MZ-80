//
// Convert GAME-80 program(Text) to MTZ format file
// Text program is converted to a running format for GAME-80 interpreter.
//
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>

//int fgetc(FILE * fp);
//char * fgets(char * buf, int size, FILE * fp);
//int fscanf(FILE * fp, const char * format, ...);

#define CR 0x0d

// define MTZ structure
typedef struct {
    char f_type;
    char f_name[17];
	unsigned short length;
	unsigned short ld_adr;
	unsigned short ex_adr;
	char btx_ul;
	char btx_as;
	char commnent[102];
	char f_dat[36864];		// 0x9000
} mzt;

void exit(int);

int main(int argc, char *argv[])
{

	FILE *fp;
	mzt mzt_f;

	int i, j;
	unsigned short ln;
	char a, out_fname[128];
	char buf[256];
	char *p, *o;
	char num[6];

	printf("\r\nSource File: %s\r\n\r\n", argv[1]);

	// copy file name and make output name
	p = basename( argv[1] );
	o = out_fname;
	while ( *p ) *o++ = *p++;	// copy base file name
	*o = 0;

	//* make file name in CMT infomation table
	p = out_fname;
	for (i = 0; i<16; i++) {
		if ( *p == 0 ) {
			mzt_f.f_name[i] = CR;
			break;
		}
		mzt_f.f_name[i] = *p & (*p>=0x60 ? 0xdf:0xff);
		p++;
	}
	if ( i == 16 ) mzt_f.f_name[16] = CR;

	o = out_fname;
	while ( *o ) o++;	// copy base file name
	*o++ = '.';
	*o++ = 'm';
	*o++ = 'z';
	*o++ = 't';
	*o = 0;
	
	p = argv[1];
	fp = fopen(p, "r");
	if (fp == NULL) {
		printf("cannot open %s\n\r\n\r",p);
		exit(1);
	}

	j = 0;
	while ( fgets(buf, 256, fp) != NULL ) {
//		printf("%s", buf);
		p = buf;
		i=0;
		while (*p>='0' && *p<='9'&& i<5) {
			num[i++] = *p++;
		}
		num[i]=0;
		ln = (unsigned short)atoi(num);
		if (ln >= 32768 ) {
			printf("%d : Line number too large\n\r\n\r", ln);
			exit(1);
		}

		mzt_f.f_dat[j++] = (char)(ln >> 8);
		mzt_f.f_dat[j++] = (char)(ln & 0x00ff);
		i = 0;
		while( *p >= 0x20 && *p <= 0x7e ) {
			if ( *p == 0x22 ) i ^= 1;
			else if ( *p >= 0x61 && *p <= 0x7a && i == 0 ) *p -= 0x20;
			mzt_f.f_dat[j++] = *p++;
		}
		mzt_f.f_dat[j++] = 0;
	}
	mzt_f.f_dat[j++] = 0xff;

	fclose(fp);
	
	printf("Make: %s (%dbytes)\n\r", out_fname, j);

	// Set MZT parameters
	mzt_f.f_type = 'G';					// set TYPE GAME
	mzt_f.ld_adr = 0x3700;				// fixed address
	mzt_f.ex_adr = 0;						// clear address
	mzt_f.length = (unsigned short)j;		// set program length
	
	p = (char *)&mzt_f;

	fp = fopen( out_fname, "wb");
	
	// adjust file length (add MZT header 128bytes)
	j = j + 128;

	while( j-- ) fputc( *p++, fp );
	fclose(fp);

	printf("Writing completed.\n\r\n\r", out_fname);
	return(0);
}
