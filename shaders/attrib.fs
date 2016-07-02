#version 330

in vec3 gNormal;
in vec2 gUV;
in vec3 gCol;
in float gDepth;
in vec2 gAxis;

uniform float filterRadius;
uniform sampler2D texDepth;
uniform int useLength;

out vec4 outColor;
out vec4 outNormal;
out float outWeight;

void main()
{
	//union of lowpass and reconstruction filter
	
	float weight;
	if(useLength == 1)
		weight = min(pow(length(gUV * gAxis)/filterRadius, 2), gUV.x*gUV.x + gUV.y*gUV.y);
	else
		weight = min(pow(length(gUV)/filterRadius, 2), gUV.x*gUV.x + gUV.y*gUV.y);
	
	//float off = gUV.x*gUV.x + gUV.y*gUV.y;
	if(weight > 1)
		discard;

	weight = sqrt(weight);		
	weight = exp(-weight*3);

	
	float d = texelFetch(texDepth, ivec2(gl_FragCoord.xy), 0).x;
	if(gl_FragCoord.z > d + 0.002)
		discard;

	//outColor = vec4(, weight);
	outColor = vec4(gCol*weight, weight);
	outNormal = vec4(gNormal*weight, weight);
	//outNormal = vec4(gl_FragCoord.z,gl_FragCoord.z,gl_FragCoord.z, weight);
	outWeight = weight;
}