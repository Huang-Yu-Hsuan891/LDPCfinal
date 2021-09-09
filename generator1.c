#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int main() {
    int i,j,k;
    int n,rc,dv,dc;
    int **H;        // from row reduced form from parity matrix
    int Hlenrow = rc;
    int Hlencolumn = n;
    int a = 0;
    int a1 = 0;
    int c[4000];
    int c1[4000];
    int m = 0;
    int **Hsyst;
    int Hsystrow = 4000;
    int Hsystcolumn = 4000;
    int **Hsyst1;
    int Hsyst1row = 4000;
    int Hsyst1column = 4000;
    int **Gsys;
    int Gsysrow = 4000;
    int Gsyscolumn = 8000;
    int **G;
    int Grow = 4000;
    int Gcolumn = 8000;

    Hsyst = (int **)malloc(Hsystrow * sizeof(int *));
    for (i = 0; i < Hsystrow; i++) Hsyst[i] = (int *)malloc(Hsystcolumn * sizeof(int));
    Hsyst1 = (int **)malloc(Hsyst1row * sizeof(int *));
    for (i = 0; i < Hsyst1row; i++) Hsyst1[i] = (int *)malloc(Hsyst1column * sizeof(int));
    Gsys = (int **) malloc (Gsysrow * sizeof(int *));
    for (i = 0; i < Gsysrow; i++) Gsys[i] = (int *) malloc(Gsyscolumn *sizeof(int));
    G = (int **)malloc(Grow * sizeof(int *));
    for (i = 0; i < Grow; i++) G[i] = (int *)malloc(Gcolumn * sizeof(int));
    FILE *fpr;
    fpr=fopen("rref80001.txt","r");
    fscanf(fpr,"%d",&rc);
    fscanf(fpr,"%d",&n);
    printf("column = %d\n", n);
    printf("row = %d\n", rc);
    Hlenrow = rc;
    Hlencolumn = n;
    H = (int **)malloc(Hlenrow * sizeof(int *));
    for (i = 0; i < Hlenrow; i++) H[i] = (int *)malloc(Hlencolumn * sizeof(int));
    
    for(i = 0; i < Hlenrow; i++) {
        for (j = 0; j < Hlencolumn; j++) {
            fscanf(fpr, "%d", &H[i][j]);
            //if (H[i][j] == 1)printf("%d ", j);
        }
        //printf("\n");
    }
    fclose(fpr);
    for (j = 0; j < n; j++) {
        m = 0;
        for (i = 0; i < rc; i++) {
            if (H[i][j] == 0) m = m + 1;
        }
        if (m == rc - 1) {
            for (i = 0; i < rc; i++) {
                Hsyst[i][a] = H[i][j]; 
            }
            c[a] = j;
            a++;
        }
        else {
            for (i = 0; i < rc; i++) {
                Hsyst1[i][a1] = H[i][j];
            }
            c1[a1] = j;
            a1++;
        }
    }
    for (i = 0; i < 4000; i++) printf("%d ", c[i]);
    printf("\n");
    for (i = 0; i < 4000; i++) printf("%d ",c1[i]);
    printf("\n");
    m = 0;
    for (j = 0; j < n; j++) {
        //m = 0;
        if (j < 4000) {
            for (i = 0; i < rc; i++) {
                Gsys[i][j] = Hsyst[i][j];
            }
        } else {
            /*for (i = 0; i < rc; i++) {*/
                for (k = rc; k < n; k++) {
                    Gsys[m][k] = Hsyst1[k-4000][j-4000];
                }  
                m++;
            //}
            
        }
        
    }
    for (i = 0; i < rc; i++) {
        for (j = 0; j < n; j++) {
            printf("%d ", Gsys[i][j]);
        }
    }
    //printf("nn\n");
    a = 0;
    a1 = 0;
    for (j = 0; j < n; j++) {
        if (c[a] == j) {
            printf("nna\n");
            for (i = 0; i < rc; i++) {
                G[i][j] = Gsys[i][4000 + a];
            }
            a++;
        }
        if (c1[a1] == j) {
            printf("nna1\n");
            for (i = 0; i < rc; i++) {
                G[i][j] = Gsys[i][a1];
            }
            a1++;
        }

    }
    printf("y\n");
    for (i = 0; i < rc; i++) {
        for (j = 0; j < n; j++) {
            if (G[i][j] == 1) printf("%d ", j + 1);
        }
        printf("\n");
    }

    /*FILE *outfp1;
    outfp1 = fopen("generator8000.txt","w");
    for (int i = 0; i < rc; i++) {
        for (int j = 0; j < n; j++) {
            if (G[i][j] == 1) fprintf(outfp1,"%d ",j+1);
        }
        fprintf(outfp1,"\n");
    }
    fclose(outfp1);*/

    FILE *outfp;
    outfp = fopen("generator80001.txt","w");
    for (int i = 0; i < rc; i++) {
        for (int j = 0; j < n; j++) {
            fprintf(outfp,"%d ",G[i][j]);
        }
        fprintf(outfp,"\n");
    }
    fclose(outfp);

    for (i = 0; i < Hlenrow; i++) free(H[i]);
    free(H);
    for (i = 0; i < Hsystrow; i++) free(Hsyst[i]);
    free(Hsyst);
    for (i = 0; i < Hsyst1row; i++) free(Hsyst1[i]);
    free(Hsyst1);
    for (i = 0; i < Gsysrow; i++) free(Gsys[i]);
    free(Gsys);
    for (i = 0; i < Grow; i++) free(G[i]);
    free(G);

    return 0;
}