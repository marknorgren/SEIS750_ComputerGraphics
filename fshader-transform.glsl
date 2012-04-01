#version 150
in vec4 color;
in vec4 ambient;

out vec4  fColor;

uniform mat4 model_view;
uniform mat4 projection;
uniform vec4 light_position;
uniform vec4 light_color;
uniform vec4 ambient_light;

void main()
{
    //fColor = color;
	fColor = color * ambient;
}