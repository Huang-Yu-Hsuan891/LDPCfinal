#include <stdio.h>
#include <math.h>
#include <stdlib.h>

/*#define N 816
#define RC 408
#define DV 3
#define DC 6*/
#define N 7
#define RC 7
#define DV 3
#define DC 3
/*#define N 8000
#define RC 4000
#define DV 3
#define DC 6*/

int main() {
    int i,j,k;
    int n,rc,dv,dc,a;
    int **L = NULL;          // check node connect 6 variable nodes
    int Llenrow = rc;        // rc = 408
    int Llencolumn = dc;     // dc = 6
    int **M = NULL;
    int Mlenrow = n;       // n = 816
    int Mlencolumn = dv;   //dv = 3
    int **H;
    int Hlenrow = rc;
    int Hlencolumn = n;
    
    
    FILE *fpr;
    fpr=fopen("parity816.txt","r");
    fscanf(fpr,"%d",&n);
    fscanf(fpr,"%d",&rc);
    printf("column = %d\n", n);
    printf("row = %d\n", rc);
    fscanf(fpr,"%d",&dv);
    fscanf(fpr,"%d",&dc);
    printf("dv = %d\n", dv);
    printf("dc = %d\n", dc);
    for (i = 0; i < n; i++) fscanf(fpr,"%d",&a);
    for (i = 0; i < rc; i++) fscanf(fpr,"%d",&a);
    // parity check matrix
    Mlenrow = n; 
    Mlencolumn = dv;
    Hlenrow = rc; 
    Hlencolumn = n;

    H = (int **)malloc(Hlenrow * sizeof(int *));
    for (i = 0; i < Hlenrow; i++) H[i] = (int *)malloc(Hlencolumn * sizeof(int));
    M = (int **)malloc(Mlenrow * sizeof(int *));
    for (i = 0; i < Mlenrow; i++) M[i] = (int *)malloc(Mlencolumn * sizeof(int));
    // parity check matrix
    for (i = 0; i < n; i++) {
        fscanf(fpr,"%d",&M[i][0]);
        fscanf(fpr,"%d",&M[i][1]);
        fscanf(fpr,"%d",&M[i][2]);
        M[i][0] = M[i][0] - 1;
        M[i][1] = M[i][1] - 1;
        M[i][2] = M[i][2] - 1;
        for (j = 0; j < rc; j++) {
            if (j == M[i][0] || j == M[i][1] || j == M[i][2]) H[j][i] = 1;
            else H[j][i] = 0;
        }
    }
    fclose(fpr);

    // GAUSS JORDAN METHOD
    int temprow = 0;    // for store pivot row 
    int stop = 0;
    int temptrans;
    int s = 0;
    printf("gauss jordan form!\n");

    for (i = 0; i < rc + s&& temprow < rc; i++) {                        // find pivot
        stop = 0;
        for (j = temprow; j < rc && stop == 0; j++) {      // where the pivot in the row
            if (H[j][i]  ==  1 && j == temprow) {
                temprow = j;
                stop = 1;
            }
            else if (H[j][i]  ==  1 && j != temprow) {
                stop = 1;
                for (k = 0; k < n; k++) {
                    temptrans = H[j][k];
                    H[j][k] = H[temprow][k];
                    H[temprow][k] = temptrans;
                }
            }
        }
        if(j == rc && stop == 0) {
            s++;
            printf("s = %d\n",s);
            continue;
        }
        // Eliminate the column have 1
        printf(" i = %d\n", i);
        for (j = 0; j < rc; j++) {
            if (j == temprow) continue;                    // skip the original row because the row no need Elimination
            else {
                if (H[j][i] == 1) {
                    for (k = 0; k < n; k++) {
                        H[j][k] = (H[j][k] + H[temprow][k]) % 2;
                    }
                }
            }
        }
        temprow++;
    }
    // bug is need to replace the position

    for (i = 0; i < rc; i++) {
        for (j = 0; j < n; j++) {
            //printf("%d ",H[i][j]); 
            if (H[i][j] == 1) printf("%d ",j + 1);//;printf("%d ",H[i][j]); 
        }
        printf("\n");
    }
    return 0;
}