//from Angel book section 8.4.1

#include <GL/Angel.h>

class matrix_stack{
public:
	static const int MAX = 50;
	matrix_stack() {index = 0;}
	void push(const mat4& matrix);
	mat4 pop();
private:
	mat4 matrices[MAX];
	int index;
};