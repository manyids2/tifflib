#version 330 core
out vec4 FragColor;

// texture samplers
uniform sampler2D thumbnail;

in vec2 TexCoord;

void main()
{
	// Sample from texture
	FragColor = texture(thumbnail, TexCoord);
}
