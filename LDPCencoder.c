#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define N 8000
#define RC 4000
#define DV 3
#define DC 6 
#define ERROR_BLOCK 50

int n, rc, dv, dc;
int M[N][DV];
int L[RC][DC];
double q[RC][DC], u[RC][DC];
double y[N], result[N], LLR[N];
int x[N];

int A_col[N];
int A_col_index = 0;

//double log_table[8] = {0.65, 0.55, 0.45, 0.35, 0.25, 0.15, 0.05, 0};

double Ranq1();
void normal(double* n1, double* n2, double sigma);
int **matrix_rref(int **matrix, int rc, int n);

unsigned long long SEED = 17;
//SEED must be an unsigned integer smaller than 4101842887655102017
unsigned long long RANV;
int RANI = 0;

double *n1, *n2;

int main(void)
{
    int skip;
    int round = 0;
    //int not_eq = 0;
    int all_not_equal = 0;
    int error_times = 0;
    int row_sum = 0;
    long long all_times = 0;
    int first_codeword = 1;
    for(int i=0; i<N; i++) A_col[i] = -1;

    FILE *fin;
    FILE* outfile;
    outfile = fopen("out_decoding_8000.txt", "w");

    double v1, v2;
    v1 = 0;
    v2 = 0;
    n1 = &v1;
    n2 = &v2;

    //fin
    fin = fopen("parity8000.txt", "rt");
    if(fin == NULL){
        printf("Fail To Open File in1.txt!!\n");
        return 1;
    }
    fscanf(fin, "%d", &n);
    fscanf(fin, "%d", &rc);
    fscanf(fin, "%d", &dv);
    fscanf(fin, "%d", &dc);

    for(int i=0; i<n+rc; i++) fscanf(fin, "%d", &skip);

    for(int i=0; i<n; i++){
        for(int j=0; j<dv;j++){
            fscanf(fin, "%d", &M[i][j]);
            M[i][j] = M[i][j] -1;
        }
    }

    for(int i=0; i<rc; i++){
        for(int j=0; j<dc;j++){
            fscanf(fin, "%d", &L[i][j]);
            L[i][j] = L[i][j] -1;
        }
    }

    //初始化mar
    int **mar = (int**) malloc(RC * sizeof(int*));
    for(int i=0; i<RC; i++){
        mar[i] = (int*) malloc(N * sizeof(int));
    }

    for(int i=0; i<rc; i++){
        for(int j=0; j<n;j++){
            mar[i][j] = 0;
        }
    }
    for(int i=0; i<rc; i++){
        for(int j=0; j<dc;j++){
            mar[i][L[i][j]] = 1;
        }
    }

    // 高斯消去法
    mar = matrix_rref(mar, rc, n);
    for(int i=0; i<rc; i++){
        for(int j=0; j<n;j++){
            printf("%d ",mar[i][j]);
        }
        printf("\n");
    }

    FILE *outfp1;
    outfp1 = fopen("rowreducedform8000.txt","w");
    for (int i = 0; i < rc; i++) {
        for (int j = 0; j < n; j++) {
            if (mar[i][j] == 1) fprintf(outfp1,"%d ",j+1);
        }
        fprintf(outfp1,"\n");
    }
    fclose(outfp1);

    //H轉成G(沒排序)
    int **mar1 = (int**) malloc(A_col_index * sizeof(int*));
    for(int i=0; i<A_col_index; i++){
        mar1[i] = (int*) malloc(N * sizeof(int));
    }

    for(int i=0; i<A_col_index; i++){
        for(int j=0; j<N-A_col_index; j++){
            mar1[i][j] = mar[j][A_col[i]];
        }
    }
    printf("\n");
    for(int i=0; i<A_col_index; i++){
        for(int j=N-A_col_index; j<n; j++){
            if((j - (N - A_col_index)) == i) mar1[i][j] = 1;
            else mar1[i][j] = 0;
            printf("%d ",mar1[i][j]);
        }
        printf("\n");
    }

    //換col順序
    int **G = (int**) malloc(A_col_index * sizeof(int*));
    for(int i=0; i<A_col_index; i++){
        G[i] = (int*) malloc(N * sizeof(int));
    }

    int jump_index = 0;
    int I_index = N - A_col_index;
    int A_index = 0;
    for(int j=0; j<n; j++){
        if(j == A_col[jump_index]){
            for(int i=0; i<A_col_index; i++){
                G[i][j] = mar1[i][I_index];
            }
            I_index++;
            jump_index++;
        }
        else{
            for(int i=0; i<A_col_index; i++){
                G[i][j] = mar1[i][A_index];
            }
            A_index++;
        }
    }
    FILE *outfp;
    outfp = fopen("generator8000.txt","w");
    for (int i = 0; i < A_col_index; i++) {
        for (int j = 0; j < n; j++) {
            if (G[i][j] == 1) fprintf(outfp,"%d ",j+1);
        }
        fprintf(outfp,"\n");
    }
    fclose(outfp);
        

    int u_recursive[A_col_index];
    int codeword[N];
    printf("A_col_index = %d\n",A_col_index);
    while(error_times < ERROR_BLOCK){
        round = 0;
        //not_eq = 0;
        for(int j=0; j<N; j++) codeword[j] = 0;

        //設定codeword
        if(first_codeword == 1){
            u_recursive[0] = 1;
            u_recursive[1] = 0;
            u_recursive[2] = 0;
            u_recursive[3] = 0;
            u_recursive[4] = 0;
            u_recursive[5] = 0;
            for(int i=0; i<A_col_index-6; i++) u_recursive[i+6] = (u_recursive[i+1] + u_recursive[i]) % 2;
        }
        else{
            u_recursive[0] = (u_recursive[A_col_index-5] + u_recursive[A_col_index-6]) % 2;
            u_recursive[1] = (u_recursive[A_col_index-4] + u_recursive[A_col_index-5]) % 2;
            u_recursive[2] = (u_recursive[A_col_index-3] + u_recursive[A_col_index-4]) % 2;
            u_recursive[3] = (u_recursive[A_col_index-2] + u_recursive[A_col_index-3]) % 2;
            u_recursive[4] = (u_recursive[A_col_index-1] + u_recursive[A_col_index-2]) % 2;
            u_recursive[5] = (u_recursive[0] + u_recursive[A_col_index-1]) % 2;
            for(int i=0; i<A_col_index-6; i++) u_recursive[i+6] = (u_recursive[i+1] + u_recursive[i]) % 2;
        }

        for(int i=0; i<A_col_index; i++)
            if(u_recursive[i] == 1)
                for(int j=0; j<N; j++) codeword[j] = (codeword[j] + G[i][j]) % 2;

        //進入檢測
    }

    //善後
    fclose(outfile);
    fclose(fin);

    for (int i=0; i<RC; i++) free(mar[i]);
    free(mar);
    for (int i=0; i<RC; i++) free(mar1[i]);
    free(mar1);
    for (int i=0; i<RC; i++) free(G[i]);
    free(G);

    return 0;
}

void normal(double* n1,double* n2,double sigma)
{
    double x1, x2, s;
    do{
        x1 = Ranq1();
        x2 = Ranq1();
        x1 = 2*x1 - 1;
        x2 = 2*x2 - 1;
        s = x1*x1 + x2*x2;
    }while(s>=1.0);
    *n1 = sigma * x1 * sqrt(-2*log(s)/s);
    *n2 = sigma * x2 * sqrt(-2*log(s)/s);
}

double Ranq1()
{
    if(RANI == 0){
        RANV = SEED ^ 4101842887655102017LL;
        RANV ^= RANV >> 21;
        RANV ^= RANV << 35;
        RANV ^= RANV >> 4;
        RANV = RANV * 2685821657736338717LL;
        RANI++;
    }
    RANV ^= RANV >> 21;
    RANV ^= RANV << 35;
    RANV ^= RANV >> 4;
    return RANV * 2685821657736338717LL * 5.42101086242752217E-20;
}

int **matrix_rref(int **matrix, int rc, int n){
    int temp;

    int x = 0;
    int y = 0;
    int i, j;

    while(x<n && y<rc){
        // 如果列首為零，則找尋可以互換的列
        while(x<n && (matrix[y][x] == 0)){
            j = y + 1;
            while(j<rc && (matrix[j][x]==0)) j++;

            // 此行都為零，移至下一行
            if(j >= rc){
                A_col[A_col_index] = x;
                A_col_index++;
                x++;
                continue;
            }

            // 找到列首不為零的列，兩列互換
            for(i=0; i<n; i++){
                temp = matrix[j][i];
                matrix[j][i] = matrix[y][i];
                matrix[y][i] = temp;
            }

            // 互換結束跳出迴圈
            break;
        }
        if(x >= n) break;

        // 消去上下列
        for(j=0; j<rc; j++){
            if(matrix[j][x] == 1 && j != y){
                for(i=0; i<n; i++){
                    matrix[j][i] = (matrix[j][i] + matrix[y][i]) % 2;
                }
            }
        }

        x++;
        y++;
    }

    while(x<n && y == rc){
        A_col[A_col_index] = x;
        A_col_index++;
        x++;
    }

    return matrix;
}
