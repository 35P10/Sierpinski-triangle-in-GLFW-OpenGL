class sierpinski_triangle {
private:
    sierpinski_triangle *childs[3];
    bool            isFull = false;
    float           vertices[30];
    glm::vec4       color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    unsigned int indices[6] = {
        0, 1, 2, // first triangle
        3, 4, 5  // second triangle
    };
    unsigned int    VBO, VAO, EBO;;
    unsigned int    shaderProgram = glCreateProgram();
    
    bool compileShader();
    void setVertices(float new_vertices[30]); 
public:
    sierpinski_triangle(float new_vertices[30]);
    sierpinski_triangle(float new_vertices[30],glm::vec4 new_color);
    ~sierpinski_triangle();
    void divide();
    void render();
    void input(GLFWwindow*);
};

sierpinski_triangle::sierpinski_triangle(float new_vertices[30]){
    compileShader();
    setVertices(new_vertices);
}

sierpinski_triangle::sierpinski_triangle(float new_vertices[30],glm::vec4 new_color){
    color = new_color;
    compileShader();
    setVertices(new_vertices);
}

void sierpinski_triangle::divide(){
    if(isFull){
        for(int i=0;i<3;i++)
            childs[i]->divide();
    }
    else {
        
        //float vertices[9] = {left_original, right_original, top_original}
        //(1) middle 0f left original and right original
        float _1_xf = (vertices[0]+vertices[5])/2;
        float _1_yf = (vertices[1]+vertices[6])/2;
        //(2) middle of top original and left original 
        float _2_xf = (vertices[10]+vertices[0])/2;
        float _2_yf = (vertices[11]+vertices[1])/2;
        //(3) middle 0f top original and right original
        float _3_xf = (vertices[10]+vertices[5])/2;
        float _3_yf = (vertices[11]+vertices[6])/2;

        color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); 
        float verticess_1[30] = {
            vertices[0],vertices[1] , 0.25f, 0.0f, 0.0f, // left original
            _1_xf      , _1_yf      , 0.25f, 0.0f, 0.0f, // right = (1)
            _2_xf      , _2_yf      , 0.25f, 0.0f, 0.0f, // top = (2)
            vertices[0],vertices[1] ,-0.25f, 0.0f, 0.0f, // left original
            _1_xf      , _1_yf      ,-0.25f, 0.0f, 0.0f, // right = (1)
            _2_xf      , _2_yf      ,-0.25f, 0.0f, 0.0f  // top = (2)    
        };
        float verticess_2[30] = {
            _1_xf      , _1_yf       , 0.25f, 0.0f, 0.0f, // left = (1)
            vertices[5],vertices[6]  , 0.25f, 0.0f, 0.0f, // right original
            _3_xf      , _3_yf       , 0.25f, 0.0f, 0.0f, // top = (3)
            _1_xf      , _1_yf       ,-0.25f, 0.0f, 0.0f, // left = (1)
            vertices[5],vertices[6]  ,-0.25f, 0.0f, 0.0f, // right original
            _3_xf      , _3_yf       ,-0.25f, 0.0f, 0.0f  // top = (3)
        }; 
        float verticess_3[30] = {
            _2_xf      , _2_yf       , 0.25f, 0.0f, 0.0f, // left = (2)
            _3_xf      , _3_yf       , 0.25f, 0.0f, 0.0f, // right  = (3)
            vertices[10],vertices[11], 0.25f, 0.0f, 0.0f, // top original
            _2_xf      , _2_yf       ,-0.25f, 0.0f, 0.0f, // left = (2)
            _3_xf      , _3_yf       ,-0.25f, 0.0f, 0.0f, // right  = (3)
            vertices[10],vertices[11],-0.25f, 0.0f, 0.0f  // top original
        }; 

        childs[0]=new sierpinski_triangle(verticess_1); //left
        childs[1]=new sierpinski_triangle(verticess_2); //right
        childs[2]=new sierpinski_triangle(verticess_3); //top
        isFull=true;
    }
}

sierpinski_triangle::~sierpinski_triangle(){
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}

void sierpinski_triangle::setVertices(float new_vertices[30]){
    for (int i = 0; i < 30; i++)
        vertices[i] = new_vertices[i];

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) <35P10/>;
}

bool sierpinski_triangle::compileShader() {
    const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

    const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = ourColor;\n"
    "}\n\0";

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors <35P10/>
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

void sierpinski_triangle::render(){
    if(isFull){
        for(int i=0;i<3;i++)
            childs[i]->render();
    }
    else {
        glUseProgram(shaderProgram);

        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        glUniform4f(vertexColorLocation, color[0], color[1], color[2], 1.0f);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}


void sierpinski_triangle::input(GLFWwindow* window){
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        this->divide();
}
