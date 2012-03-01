#version 150

in vec4 vPosition;
in vec4 vColor;
uniform mat4 model_view;
uniform mat4 projection;
out vec4 color;

void
main()
{
    gl_Position = projection*model_view*vPosition;
	color = vColor;
}