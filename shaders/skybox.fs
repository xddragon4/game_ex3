#version 330                                                                        
                                                                                    
in vec3 TexCoord0;                                                                  
                                                                                    
out vec4 FragColor;                                                                 
                                                                                    
uniform samplerCube gCubemapTexture;                                                
                                                                                    
void main()                                                                         
{                                                                                   
    FragColor = texture(gCubemapTexture, TexCoord0);
	//FragColor = vec4(1, 0, 0, 1);                              
}
