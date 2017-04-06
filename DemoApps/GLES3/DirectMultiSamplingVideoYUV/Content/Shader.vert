  #version 300 es
  in vec4 my_Vertex;
  in  vec2 my_Texcoor;
  uniform   mat4 my_TransMatrix;
  out vec2 vTexcoor;
  
  void main()
  {
     vTexcoor = my_Texcoor;

     gl_Position = my_TransMatrix*my_Vertex;
  }