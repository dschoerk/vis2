#version 330

in vec3 gNormal;
in vec2 gUV;
in float gDepth;

//out vec3 outColor;
//out float depth;

void main()
{
	if(length(gUV) > 1)
		discard;
	//outColor = abs(gNormal);
	//depth = gDepth;
	//gl_FragDepth = gDepth;
}