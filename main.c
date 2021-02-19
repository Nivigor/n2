#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int minrow, maxrow, mincol, maxcol; } Frame;
typedef struct {
	int **p;
	int protect;
	int row, col;
} Array;

Array new_Array(int row, int col) {
	Array a; int i;
	a.protect = 0;
	a.row = row;
	a.col = col;
	a.p = malloc(row*sizeof(int *));
	for (i=0; i<row; i++) {
		a.p[i] = malloc(col*sizeof(int));
		memset(a.p[i], 0, col*sizeof(int));
	}
	return a;
}

void free_Array(Array a) {
	int i;
	if (a.protect) return;
	for (i=0; i<a.row; i++) free(a.p[i]);
	free(a.p);
}

Frame Frame_Array(Array a) {
	int col, row;
	Frame frame;
	frame.mincol = a.col;
	frame.maxcol = -1;
	frame.minrow = a.row;
	frame.maxrow = -1;
	for (row=0; row<a.row; row++) for (col=0; col<a.col; col++) if (a.p[row][col]) {
		if (col<frame.mincol) frame.mincol=col;
		if (col>frame.maxcol) frame.maxcol=col;
		if (row<frame.minrow) frame.minrow=row;
		if (row>frame.maxrow) frame.maxrow=row;
	}
	return frame;
}

char *htmlize(Array a) {
	char *s, *ss; int i, j;
	s=malloc(a.row*(a.col+1)+1);
	for (ss=s, i=0; i<a.row; i++) {
		for (j=0; j<a.col; j++, ss++) *ss = a.p[i][j] ? 178 : 176;
		*(ss++) = '\n';
	}
	*ss = 0;
	return s;
}

void out(Array a) {
	char *s;
	s = htmlize(a);
	printf(s); printf("\n");
	free(s);
}

Array read(char *s) {
	Array a; int col, row, i, j;
	col = strchr(s, '\n') - s;
	row = strlen(s)/(col+1);
	a = new_Array(row, col);
	for (i=0; i<row; i++) {
		for (j=0; j<col; j++) a.p[i][j] = *(s++) != ' ';
		s++;
	}
	return a;
}

int **get_generation(int **cells, int generations, int *rowptr, int *colptr) {
	int gen, col, row, dcol, drow;
	Array a, b;
	a.col = *colptr; a.row = *rowptr; a.p = cells; a.protect = 1;
	for (gen=0; gen<generations; gen++) {
		b = new_Array(a.row+2, a.col+2);
		for (row=0; row<a.row; row++) for (col=0; col<a.col; col++) if (a.p[row][col]) {
			for (drow=0; drow<3; drow++) for (dcol=0; dcol<3; dcol++)
				++b.p[row+drow][col+dcol];
			b.p[row+1][col+1] += 9;
		}
		for (row=0; row<b.row; row++) for (col=0; col<b.col; col++)
			b.p[row][col] = (b.p[row][col]==3) || (b.p[row][col]==12) || (b.p[row][col]==13);
		Frame bframe = Frame_Array(b);
		free_Array(a);
		if (bframe.mincol>bframe.maxcol) {
			*rowptr=*colptr=0;
			free_Array(b);
			return NULL;
		}
		else {
			a = new_Array(bframe.maxrow-bframe.minrow+1, bframe.maxcol-bframe.mincol+1);
			for (row=0; row<a.row; row++) for (col=0; col<a.col; col++)
				a.p[row][col] = b.p[row+bframe.minrow][col+bframe.mincol];
		}
	}
	*rowptr=a.row; *colptr=a.col; return a.p;
}

int main(int argc, char *argv[]) {
	char *test = " x \n"
				 "  x\n"
				 "xxx\n"; printf(test);
	Array a=read(test);
	out(a);
	int row = a.row; int col =a.col;
	Array b;
	b.p = get_generation(a.p, 1, &row, &col);
	b.col = col;
	b.row = row;
	out(b);
	free_Array(a);
	free_Array(b);
	return 0;
}
