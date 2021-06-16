#version 450 core

layout (location=1) in vec2 vTexCoord;
layout (location=0) in vec3 vInterpColor;
layout (location=0) out vec4 fFragColor;

uniform int uTileSize;
uniform int uShaderMode;
uniform bool uModulate;

uniform sampler2D uTex2d;

void main () 
{
    if (uShaderMode == 0) {
        fFragColor = vec4(vInterpColor, 1.0);
    }
    else if (uShaderMode == 1) {
        bool isEven = mod (  floor(gl_FragCoord.x/32) + floor(gl_FragCoord.y/32)  ,  2.0f )== 0.0 ;
        vec4 col1 = vec4 (0.0,0.68,0.94,1.0);
        vec4 col2 = vec4 (0.0,1.0,1.0,1.0);
	    fFragColor = ((isEven)? col1:col2);
    }
    else if (uShaderMode == 2) {
        bool isEven = mod (  floor(gl_FragCoord.x/uTileSize) + floor(gl_FragCoord.y/uTileSize)  ,  2.0f )== 0.0 ;
        vec4 col1 = vec4 (0.0,0.68,0.94,1.0);
        vec4 col2 = vec4 (0.0,1.0,1.0,1.0);
	    fFragColor = ((isEven)? col1:col2);
    }
    else if (uShaderMode == 3) {
        fFragColor = texture(uTex2d, vTexCoord);
    }
    else if (uShaderMode == 4) {
        fFragColor = texture(uTex2d, vTexCoord*5);
    }
    else if (uShaderMode == 5) {
        fFragColor = texture(uTex2d, vTexCoord*5);
    }
    else if (uShaderMode == 6) {
        fFragColor = texture(uTex2d, vTexCoord*5);
    }
    if (uModulate && uShaderMode != 0) {
        fFragColor = vec4 (vInterpColor.x * fFragColor.x ,vInterpColor.y * fFragColor.y,vInterpColor.z * fFragColor.z, fFragColor.a );
    }
}

