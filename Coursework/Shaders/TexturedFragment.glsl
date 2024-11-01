#version 330 core
uniform sampler2D diffuseTex ;

in Vertex {
vec2 texCoord ;
} IN ;

out vec4 fragColour ;

void main ( void ) 
{
	fragColour = texture (diffuseTex, IN.texCoord);
	//fragColour = texture ( diffuseTex , IN.texCoord ).rgba ; // Allowed
	//fragColour = texture ( diffuseTex , IN.texCoord ).xyzw ; // Allowed
	//fragColour = texture ( diffuseTex , IN.texCoord ).rgzw ; // DISallowed !
	//fragColour = texture ( diffuseTex , IN.texCoord ).bgra ; // Swizzling
	//fragColour = texture ( diffuseTex , IN.texCoord ).xxxw ; // Swizzling 
}