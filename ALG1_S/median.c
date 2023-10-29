/* ====================================================== */
/*                                                        */
/*    INPUT 564 3 456 33 1 34 35 32 444 8 33 22           */
/*                                                        */
/*    ================================================    */
/*    HEAP (ROOT MAX) elements 12                         */
/*    ================================================    */
/*                                                        */
/*    0:                564                               */
/*    2:           444         456                        */
/*    4:        33    33    34      35                    */
/*    8:       3 32  1 8   22                             */
/*                                                        */
/*    ================================================    */
/*    HEAP (ROOT MIN) elements 12                         */
/*    ================================================    */
/*                                                        */
/*    0:                        1                         */
/*    2:               3              22                  */
/*    4:           32      8       34     35              */
/*    8:        564 444  33 33   456                      */
/*                                                        */
/* ====================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define TRUE 1
#define FALSE 0

#define ROOTMIN 0
#define ROOTMAX 1

#define MAXINT INT_MAX
#define DEBUG FALSE

#define HEAPMAX 20000

typedef struct {  /* HEAP STRUCTURE                         */
	int nh;       /* # elements in heap                     */
	int *hv;      /* heap array , starts at index 0         */
	int t;        /* heap type (0 - root min; 1 - root max) */
} heap;

typedef struct {  /* MEDIAN STRUCTURE                         */
	int nm;       /* # elements in median                     */
	heap *hl;     /* heap array low,  supports EXTRACT MAX    */
	heap *hh;     /* heap array high, supports EXTRACT MIN    */
	int *m;       /* median array according input stream      */
	int *in;      /* input stream itself (for debugging only) */
} median;

inith(heap *h, int type)
{
	h->hv = malloc((HEAPMAX+1)*sizeof(int));
	h->nh=0;
	h->t = type;
}

int geth(heap *h, int ix) /* public index 1..nh */
{
	if (h->nh <= 0) return (-1);
    return(h->hv[ix-1]);
}

int getnh(heap *h) /* public index 1..nh */
{
    return(h->nh);
}

int outord(heap *h, int ix, int iy)  /* public index 1..nh ix=child, iy=parent */
{
    if ((geth(h,ix) < geth(h,iy) && h->t == ROOTMIN) ||
        (geth(h,ix) > geth(h,iy) && h->t == ROOTMAX))
        return TRUE;
    else
        return FALSE;
}

bubbleup(heap *h, int ix) /* change bottom -> root */
{                         /* public index 1..nh   */
	int x,ixp;

    ixp = parent(h,ix);
    if (ixp == -1) return;  /* root */
    if (outord(h,ix,ixp) == TRUE)   {
        swap(h,ix,ixp);
        bubbleup(h,ixp);      /* recurse */
    }
}

swap(heap *h, int ix, int iy) /* public index 1..nh */
{
	int x;

    x = h->hv[ix-1];
    h->hv[ix-1] = h->hv[iy-1];
    h->hv[iy-1] = x;
}

int parent(heap *h, int ix) /* public index 1..nh */
{
	int i;
	i = (int) ix/2;
	if (ix == 1) i = -1;
	return(i);
}

int child(heap *h, int ix) /* public index 1..nh */
{
	int i;
	i = ix*2;
	if (i > h->nh) i = -1;
	return(i);
}

readh(heap *h, char *file)
{
    FILE *in;
    int count, hash, x;

    in = fopen(file, "r");
    count=0;
    while (fscanf(in,"%d",&x) != EOF)
        puth(h,x);
    fclose(in);
}

puth(heap *h, int x) /* put value at the end of heap */
{
	if (h->nh > HEAPMAX) {
		printf("heap insert overflow\n");
		return;
	}
    h->hv[h->nh++] = x;
    bubbleup(h,h->nh);  /* pass public index (internal+1) */
}

printh(heap *h) /* traverse heap and print piramide */
{
    int i,j,k;
    char *a[] = {"ROOT MIN","ROOT MAX"};

    printf("================================================\n");
    printf("HEAP (%s) elements %d\n", a[h->t], h->nh);
    printf("================================================\n");
    j=0;
    k=0;
	printf("\n%d: ",j);
    for (i=0; i<h->nh; i++) {
		printf("%d ", h->hv[i]);
		if (k == j) {
		    j = j*2;
		    if (j == 0) j = 2;
		    k = 0;
			printf("\n%d: ",j);
		}
		k++;
	}
	printf("\n\n");
}



int extract(heap *h)     /* extract root element (min or max) */
{
	int x = -1;
	if (h->nh > 0) {
		x = h->hv[0];
		if (h->nh > 1)
		    h->hv[0] = h->hv[h->nh -1];
		h->nh--;
		bubbledown(h,1);    /* pass public index (internal+1) */
	}
	return x;
}

bubbledown(heap *h, int ix)    /* public index 1..nh */
{
	int ic, imin;

	ic = child(h,ix);
	if (ic == -1) return;
	imin = ix;

    if (ic < h->nh)           /* consider lower child */
       if (outord(h, ic+1, ic)) ic = ic+1;

    if (outord(h,ic,ix) == TRUE)   {
        swap(h,ic,ix);
        bubbledown(h,ic);      /* recurse */
    }
}

sortprint(heap *h)
{
	int i;
    char *a[] = {"ROOT MIN","ROOT MAX"};

    printf("================================================\n");
    printf("HEAP (%s) elements %d\n", a[h->t], h->nh);
    printf("================================================\n");

    while(h->nh > 0)
        printf("%d ", extract(h));
    printf("\n\n");
}

initmed(median *med)
{
	med->m  = (int *) malloc((HEAPMAX)*sizeof(int));
	med->in = (int *) malloc((HEAPMAX)*sizeof(int));
	med->hl = (heap *) malloc(sizeof(heap));
	med->hh = (heap *) malloc(sizeof(heap));
    inith(med->hl, ROOTMAX);
    inith(med->hh, ROOTMIN);
    med->nm = 0;
}

putmed(median *med, int x)
{
	int hlmax, hhmin;

	hlmax = geth(med->hl, 1);
	hhmin = geth(med->hh, 1);

    if (x <= hlmax || getnh(med->hl) == 0)
        puth(med->hl,x);
    else
        puth(med->hh,x);

    if (getnh(med->hh) > getnh(med->hl))
        puth(med->hl,extract(med->hh));
    else if (getnh(med->hl) > getnh(med->hh)+1)
        puth(med->hh,extract(med->hl));

    med->m[med->nm] = geth(med->hl, 1);
    med->in[med->nm] = x;
    med->nm++;
    if (DEBUG == TRUE) printmed(med,med->nm);
}

readmed(median *m, char *file)
{
    FILE *in;
    int x;

    in = fopen(file, "r");
    while (fscanf(in,"%d",&x) != EOF)
        putmed(m,x);
    fclose(in);
}

printmed(median *med, int start)
{
    int i,j;

    printf("================================================\n");
    printf("MEDIAN elements %d\n", med->nm);
    printf("================================================\n");

    for (i = start-1; i<med->nm; i++) {
        printf("-----------------------------------------\n");
        printf("median[%d]=%d\n", i, med->m[i]);
        if (start != 1) {
             printf("-----------------------------------------\n");
            for (j = 0; j<med->nm; j++)
                printf("%d ",med->in[j]);
            printf("\n");
	    }
    }
}

int summed(median *med)
{
    int i,sum;

    sum = 0;
    for (i = 0; i<med->nm; i++) {
        sum += med->m[i];
    }
    return (sum % med->nm);
}

main(int argc, char *argv[])
{
    median med;      /* double heap structure to store running median */

    if (argc != 2) { printf("enter heap input file name\n"); return; }

    printf("INITMED\n");
    initmed(&med);
    printf("READMED\n");
	readmed(&med, argv[1]);

    if (DEBUG == TRUE) {
       printf("PRINTMED\n");
	   printmed(&med,1);
    }
    printf("MEDIAN of %d elements SUM = %d \n", med.nm, summed(&med));
}
