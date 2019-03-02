#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>

#include <assert.h>

#include "uf2format.h"

int main(int argc, char *argv[])
{
	UF2_Block block;

	FILE *fp;
	FILE *fpout;
	ssize_t n;
	size_t chunks;
	size_t filesize = 0;
	int invalid = 0;
	char outfname[UF2_FILENAME_MAX] = {0};
	char tmpname[32] = "/tmp/XXXXXX";
	int out;

	if (argc < 2) {
		return -1;
	}

	fp = fopen(argv[1], "r");
	if (!fp) {
		printf("file '%s' not found\n", argv[1]);
		return -1;
	}

	out = mkstemp(tmpname);
	if (out > -1) {
		fpout = fdopen(out, "w");
	}

	if (!fpout) {
		return -1;
	}

	do {
		n = fread(&block, sizeof(block), 1, fp);
		fwrite(block.data, 1, block.payloadSize, fpout);
		chunks += n;
		filesize += block.payloadSize;
		if (block.flags & UF2_FLAG_FILE) {
			if (outfname[0] == '\0') {
				if (strcmp(outfname, (const char *) &block.data[block.payloadSize])) {
					strncpy(outfname, (const char *) &block.data[block.payloadSize], sizeof(outfname)-1);
					outfname[sizeof(outfname)-1] = '\0';
					printf("file name is named as '%s'\n", outfname);
				}
			}
			else {
				if (strcmp(outfname, (const char *) &block.data[block.payloadSize])) {
					printf("file name is not persist\n");
					invalid = 1;
					break;
				}
			}
		}
		else {
			fprintf(stderr, "not supported flag %08x\n", block.flags);
		
		}

		if (!is_uf2_block(&block)) {
			fprintf(stderr, "invalid uf2 block\n");
			invalid = 1;
			break;
		}
		/*
		if (filesize > block.fileSize) {
			fprintf(stderr, "file size exceed %zd %d\n", filesize, block.fileSize);
			break;
		}
		*/
		if (block.blockNo >= block.numBlocks) {
			fprintf(stderr, "number of block exceed\n");
			invalid = 1;
			break;
		}
	} while (n > 0);

	fclose(fpout);

	if (!invalid) {
		fprintf(stderr, "%zd chunks, %zu bytes written\n", chunks, filesize);
		if (block.flags & UF2_FLAG_FILE) {
			if (rename(tmpname, outfname)) {
				fprintf(stderr, "cannot rename as '%s'\n", outfname);
				unlink(tmpname);
				invalid = 1;
			}
		}

		fprintf(stderr, "done!!\n");
	}

	fclose(fp);

	return invalid == 0;
}
