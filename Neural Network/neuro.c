#include <stdio.h>
#include <stdlib.h>
#include "headers/matrix.h"

typedef enum
{
    CLASSIFICATION,
    REGRESSION
} TaskType;

typedef enum
{
    VALUE,
    BINARY_ARRAY,
    ARRAY_OF_VALUES,
    ARRAY_OF_PROBABILITIES
} AnswerType;

typedef struct NeuralNetwork
{
    TaskType taskType;
    AnswerType answerType;
} NeuralNetwork;

typedef struct Layer
{
    struct Layer *next; //8 bytes
    struct Layer *prev; //8 bytes
    double **weights;   //8 bytes
    double *values;     //8 bytes // values at neurons after calculating linear combination and applying activationFunction;
    //double (*activationFunction)(double, double *);
    double (*activationFunction)(double);
    char **description; //8 bytes
    int m;              //4 bytes
    int n;              //4 bytes
    short switched : 8; // 1 byte
} Layer;

Layer *createLayer(int m, int n, short switched)
{
    Layer *new = (Layer *)malloc(sizeof(Layer));
    new->weights = (double **)malloc(sizeof(double *) * m);
    for (int i = 0; i < m; i++)
    {
        new->weights[i] = (double *)malloc(sizeof(double) * n);
        for (int j = 0; j < n; j++)
        {
            new->weights[i][j] = -1;
        }
    }
    new->m = m;
    new->n = n;
    new->switched = switched;
    return new;
}

void appendLayer(Layer **head, int m, int n, short switched)
{
    Layer *new = createLayer(m, n, switched);
    if (head == NULL)
    {
        *head = new;
        return;
    }
    Layer *cur = *head;
    while (cur->next != NULL)
    {
        cur = cur->next;
    }
    cur->next = new;
    new->prev = cur;
}

void insertLayer(Layer **head, int pos, int m, int n, short switched)
{
    Layer *new = createLayer(m, n, switched);
    if (pos == 0)
    {
        if (head == NULL)
        {
            *head = new;
            return;
        }
        Layer *temp = *head;
        (*head)->prev = new;
        new->next = *head;
        *head = new;
    }
    return;
}

double ***backPropagation(Layer **head, double **xTrain, double *yTrain)
{
}

void *predict(NeuralNetwork *network, Layer **head, double *objectFeatures)
{
    if ((*head)->values == NULL)
    {
        (*head)->values = (double *)malloc(sizeof(double) * (*head)->m);
    }
    for (int i = 0; i < (*head)->m; i++)
    {
        (*head)->values[i] = objectFeatures[i];
    };
    //?
    Layer *cur = (*head)->next, *last = cur;
    while (cur->next != NULL)
    {
        if (cur->values == NULL)
        {
            cur->values = (double *)malloc(sizeof(double) * cur->m);
        }
        for (int i = 0; i < cur->m; i++)
        {
            double sum = 0;
            for (int j = 0; j < cur->prev->m; j++)
            {
                sum += cur->prev->values[j] * cur->prev->weights[j][i];
            }
            cur->values[i] = cur->activationFunction(sum);
        }
        cur = cur->next;
    }
    switch (network->answerType)
    {
    case VALUE:
        return (void *)(&cur->values[0]);
    case ARRAY_OF_VALUES:
    case BINARY_ARRAY:
    case ARRAY_OF_PROBABILITIES:
        return (void *)cur->values;
    default:
        return (void *)cur->values;
    }
}

void train(Layer **head, double **xTrain, double *yTrain, double *params, double ***(*loss)(double **, double *, double *))
{
    loss(xTrain, yTrain, params);
}

void test(double **xTest, double *yTest, double *yTrue, double ***weightForLayers)
{
}

void freeDoublePointerMemory(void **location, int size)
{
    for (int i = 0; i < size; i++)
    {
        free(location[i]);
    }
    free(location);
}

int main(int argc, char *argv[])
{
    FILE *fa;
    if ((fa = fopen("data", "r")) == NULL)
    {
        perror("Couldn't open file");
        return -1;
    };
    int m, n, k, l;
    fscanf(fa, "%d %d", &m, &n);
    double **matrix = (double **)malloc(sizeof(double *) * m);
    for (int i = 0; i < m; i++)
    {
        matrix[i] = (double *)malloc(sizeof(double) * n);
        for (int j = 0; j < n; j++)
        {
            fscanf(fa, "%d", &matrix[i][j]);
        }
    }
    fscanf(fa, "%d %d", &k, &l);
    double **matrix2 = (double **)malloc(sizeof(double *) * k);
    for (int i = 0; i < k; i++)
    {
        matrix[i] = (double *)malloc(sizeof(double) * l);
        for (int j = 0; j < l; j++)
        {
            fscanf(fa, "%d", &matrix2[i][j]);
        }
    }
    fclose(fa);
    double **result = multiply(matrix, matrix2, m, n, k, l);
    freeDoublePointerMemory(result, m);
    return 0;
}