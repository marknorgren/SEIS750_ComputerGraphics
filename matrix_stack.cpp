//From Angel book Section 8.4.1
#include "matrix_stack.h"
#include <assert.h>

void matrix_stack::push(const mat4& matrix){
	assert(index <= MAX);
	matrices[index] = matrix;
	index++;
}

mat4 matrix_stack::pop(){
	assert(index > 0);
	index--;
	return matrices[index];
}