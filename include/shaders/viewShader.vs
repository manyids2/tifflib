#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform vec2 windowSize;
uniform vec3 viewState;

out vec2 TexCoord;

void main()
{
   vec2 offset = aPos * viewState.z + viewState.xy;
   vec2 onScreen = ((offset / windowSize) * 2 ) - 1;
   gl_Position = vec4(onScreen.x, -onScreen.y, 0.0, 1.0);

   TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}

