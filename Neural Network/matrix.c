#include <stdio.h>

//do not forget to free pointers after usage
double ** transpose(double **matrix, int rows, int cols){
    double **transposed = (double**)malloc(sizeof(double*)*cols);
    for(int i=0; i < rows; i++){
        transposed[i] = (double*)malloc(sizeof(double)*rows);
    }
    for(int i=0; i< rows; i++){
        for(int j=0; j < cols; j++){
            transposed[j][i] = matrix[i][j];
        }
    }
    return transposed;
}
//do not forget to free pointers after usage
double ** multiply(double **matrix, double **matrix2, int m,int n, int k, int l){
    double **result =(double**)malloc(sizeof(double*)*m);
    for(int i=0; i < m; i++){
        result[i] = (double*)malloc(sizeof(double)*l);
    }

    for(int row=0; row < m; row++){
        double sum = 0;
        for(int j=0; j < l; j++, sum = 0){
            for(int i=0; i < n; i++){
                sum+=matrix[row][i]*matrix2[j][i];
            }
            result[row][j] = sum;
        }
    }
    return result;
}

