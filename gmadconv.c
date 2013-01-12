/*
* Modifications by Ben 'Cue' Woodford
* With Credit to SiPlus (original author)
* Also with Credit to zQaX for fixing an earlier issue
*/

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	char *name;
	unsigned int size;
} addonfile;

char *ztstr(FILE *file)
{
	int size;
	char chr, *dest;
	for (size = 1;; size++)
	{
		fread(&chr, 1, 1, file);
		if (chr == '\0')
			break;
	}
	fseek(file, -size, SEEK_CUR);
	dest = malloc(size);
	fread(dest, size, 1, file);
	return dest;
}

FILE *fopenwb(char *path)
{
	int len = strlen(path) + 1, i;
	char *dirpath;
	if (len == 1)
	{
		fputs("File in addon has no name\n", stderr);
		exit(1);
	}
	dirpath = malloc(len);
	for (i = 0; i < len; i++)
	{
		if (path[i] == '/')
		{
			dirpath[i] = '\0';
			mkdir(dirpath, 0755);
		}
		dirpath[i] = path[i];
	}
	free(dirpath);
	return fopen(path, "wb");
}

int main(const int argc, const char *argv[])
{
	FILE *in, *out;
	unsigned int filecnt = 0, filenum, i, j;
	char *addonname, *addondesc, *addonauthor;
	int curbyte;
	addonfile *addonfiles = NULL, *curfile;

	puts("GMADConv v2 by SiPlus");
	if (argc < 2)
	{
		fputs("No file specified\n", stderr);
		return 1;
	}
	in = fopen(argv[1], "rb");
	if (in == NULL)
	{
		fprintf(stderr, "Error opening input file %s: %s\n", argv[1], strerror(errno));
		return errno;
	}
	fread(&i, 4, 1, in);
	if (i != 0x44414d47)
	{
		fputs("Input file is not addon\n", stderr);
		return 1;
	}
	fseek(in, 18, SEEK_CUR);
	addonname = ztstr(in);
	addondesc = ztstr(in);
	addonauthor = ztstr(in);
	fseek(in, 4, SEEK_CUR);
	puts("Extracting file names");
	for (;;)
	{
		fread(&filenum, 4, 1, in);
		if (filenum == 0)
			break;
		filecnt++;
		addonfiles = realloc(addonfiles, filecnt * sizeof(addonfile));
		curfile = &addonfiles[filecnt - 1];
		curfile->name = ztstr(in);
		fread(&curfile->size, 4, 1, in);
		fseek(in, 8, SEEK_CUR);
	}
	if (addonfiles == NULL)
	{
		puts("Addon is empty");
		return 0;
	}
	mkdir(addonname, 0755);
	chdir(addonname);
	for (i = 0; i < filecnt; i++)
	{
		curfile = &addonfiles[i];
		printf("Extracting %s (%uB)\n", curfile->name, curfile->size);
		out = fopenwb(curfile->name);
		if (out == NULL)
		{
			fprintf(stderr, "Error opening output file %s: %s\n", curfile->name, strerror(errno));
			return errno;
		}
		for (j = 0; j < curfile->size; j++)
		{
			fread(&curbyte, 1, 1, in);
			fputc(curbyte, out);
		}
		fflush(out);
		fclose(out);
	}
	puts("Writing info.txt");
	out = fopen("info.txt", "wb");
	if (out == NULL)
	{
		fprintf(stderr, "Error opening info.txt: %s\n", strerror(errno));
		return errno;
	}
	fprintf(out,
		"\"AddonInfo\"\r\n{\r\n\t\"name\" \"%s\"\r\n\t\"author_name\" \"%s\"\r\n\t\"info\" \"%s\"\r\n}", addonname, addonauthor, addondesc);
	printf("Finished extracting addon %s\n", addonname);
	chdir("..");
	return 0;
}
