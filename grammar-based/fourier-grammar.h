#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct node {
	int size;
	int weight;
	int argmin;
	int minVal;
	struct node *left;
	struct node *right;
} node;


node *create();
node *duplicate(node *x);
node *add(int i, int j, int d, node *x);
void printSums(int i, int j, node *x, int sum);
