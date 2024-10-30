#include "fourier-grammar.hpp"


node *create() {

	node *x = (node *) malloc(sizeof(node));
	x -> size = 1;
	x -> weight = 0;
	x -> argmin = 0;
	x -> minVal = 0;
	x -> left = NULL;
	x -> right = NULL;

	return(x);
}


node *duplicate(node *x) {

	node *y = (node *) malloc(sizeof(node));
	y -> size = 2 * (x -> size);
	y -> weight = 0;
	y -> argmin = x -> argmin;
	y -> minVal = x -> minVal;
	y -> left = x;
	y -> right = x;

	return(y);
}


node *add(int i, int j, int d, node *x) {

	node *y = (node *) malloc(sizeof(node));
	memcpy(y, x, sizeof(node));

	if (j - i + 1 == y -> size) {
		y -> weight += d;
	} else if (j < (y -> size) / 2) {
		y -> left = add(i, j, d, y -> left);
	} else if (i >= (y -> size) / 2) {
		y -> right = add(i - (y -> size) / 2, j - (y -> size) / 2, d, y -> right);
	} else {
		y -> left = add(i, (y -> size) / 2 - 1, d, y -> left);
		y -> right = add(0, j - (y -> size) / 2, d, y -> right);
	}

	if (y -> size == 1) {
		y -> argmin = 0;
		y -> minVal = y -> weight;
	} else if ((y -> left) -> minVal <= (y -> right) -> minVal) {
		y -> argmin = (y -> left) -> argmin;
		y -> minVal = ((y -> left) -> minVal) + (y -> weight);
	} else {
		y -> argmin = ((y -> right) -> argmin) + (y -> size) / 2;
		y -> minVal = ((y -> right) -> minVal) + (y -> weight);
	}

	return(y);
}


void printSums(int i, int j, node *x, int sum) {

	if (x -> size == 1) {
		printf("%i ", sum + (x -> weight));
		putchar('\n');
	} else if (j < (x -> size) / 2) {
		printSums(i, j, x -> left, sum + (x -> weight));
	} else if (i >= (x -> size) / 2) {
		printSums(i - (x -> size) / 2, j - (x -> size) / 2, x -> right, sum + (x -> weight));
	} else {
		printSums(i, (x -> size) / 2 - 1, x -> left, sum + (x -> weight));
		printSums(0, j - (x -> size) / 2, x -> right, sum + (x -> weight));
	}

	return;
}

int getSum(int i, node *x, int sum) {

	if (x -> size == 1) {
		return sum + (x -> weight);
	} else if (i < (x -> size) / 2) {
		return getSum(i, x -> left, sum + (x -> weight));
	} else if (i >= (x -> size) / 2) {
		return getSum(i - (x -> size) / 2, x -> right, sum + (x -> weight));
	}

	return -1;
}