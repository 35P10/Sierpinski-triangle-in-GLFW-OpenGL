class sierpinski_triangle {
private:
    sierpinski_triangle *childs[3];
    bool            isFull = false;
    float           vertices[30];
    float           thickness = 0.03f;
    unsigned int    indices[24] = {
        0, 1, 2,
        3, 4, 5, 
        0, 3, 5,
        2, 5, 0,
        1, 4, 2,
        2, 5, 4,
        0, 3, 1,
        1, 4, 3
    };

    unsigned int    VBO, VAO, EBO;
    unsigned int    shaderProgram = glCreateProgram();
    std::deque<glm::vec3> childs_colors;
    int             color_speed_transition = 0;
    std::deque<glm::vec3> generate_color(int n);
    //glm::mat4       transform = glm::mat4(1.0f);
    glm::mat4 transform = glm::rotate(glm::mat4(1.0f), glm::radians(0.1f), glm::vec3(0.0f, 1.0f, 0.0f));
    bool compileShader();
    void setVertices(float new_vertices[30]);
    void setRotation(float rot_grades, glm::vec3 rot_vect);

public:
    sierpinski_triangle(float new_vertices[30]);
    ~sierpinski_triangle();
    void divide();
    void render(glm::vec3 color, glm::mat4 model, glm::mat4 view, glm::mat4 projection);
    void input(GLFWwindow*);
};

sierpinski_triangle::sierpinski_triangle(float new_vertices[30]){
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

        //Color de triangulo central
        //color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); 
        
        childs_colors = generate_color(3);

        float verticess_1[30] = {
            vertices[0],vertices[1] , thickness, 0.0f, 0.0f, // left original
            _1_xf      , _1_yf      , thickness, 0.0f, 0.0f, // right = (1)
            _2_xf      , _2_yf      , thickness, 0.0f, 0.0f, // top = (2)
            vertices[0],vertices[1] ,-thickness, 0.0f, 0.0f, // left original
            _1_xf      , _1_yf      ,-thickness, 0.0f, 0.0f, // right = (1)
            _2_xf      , _2_yf      ,-thickness, 0.0f, 0.0f  // top = (2)    
        };
        float verticess_2[30] = {
            _1_xf      , _1_yf       , thickness, 0.0f, 0.0f, // left = (1)
            vertices[5],vertices[6]  , thickness, 0.0f, 0.0f, // right original
            _3_xf      , _3_yf       , thickness, 0.0f, 0.0f, // top = (3)
            _1_xf      , _1_yf       ,-thickness, 0.0f, 0.0f, // left = (1)
            vertices[5],vertices[6]  ,-thickness, 0.0f, 0.0f, // right original
            _3_xf      , _3_yf       ,-thickness, 0.0f, 0.0f  // top = (3)
        }; 
        float verticess_3[30] = {
            _2_xf      , _2_yf       , thickness, 0.0f, 0.0f, // left = (2)
            _3_xf      , _3_yf       , thickness, 0.0f, 0.0f, // right  = (3)
            vertices[10],vertices[11], thickness, 0.0f, 0.0f, // top original
            _2_xf      , _2_yf       ,-thickness, 0.0f, 0.0f, // left = (2)
            _3_xf      , _3_yf       ,-thickness, 0.0f, 0.0f, // right  = (3)
            vertices[10],vertices[11],-thickness, 0.0f, 0.0f  // top original
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
    "uniform mat4 transform; \n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * transform * vec4(aPos, 1.0);\n"
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

void sierpinski_triangle::render(glm::vec3 _color,glm::mat4 model, glm::mat4 view, glm::mat4 projection){
    if(isFull){
        for(int i=0;i<3;i++)
            childs[i]->render(childs_colors[i],model,view,projection);

        //velocidad de gradiente
        // ------
        if (color_speed_transition == 200) {
            childs_colors.push_back(childs_colors.front());
            childs_colors.pop_front();
            color_speed_transition = 0;
        }
        color_speed_transition++;
    }
    else {     
        glPolygonOffset(1,1);
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
        glEnable(GL_POLYGON_OFFSET_FILL);

        glUseProgram(shaderProgram);
        //transform
        unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        //color
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        glUniform4f(vertexColorLocation, _color[0], _color[1], _color[2], 1.0f);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);

        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        glDisable(GL_POLYGON_OFFSET_FILL);

        //color
        glUniform4f(vertexColorLocation, 0.0f, 0.0f, 0.0f, 1.0f);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
    
    }
}


void sierpinski_triangle::input(GLFWwindow* window){
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        this->divide();
    //if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
     //   this->setRotation(-0.01, glm::vec3(0.0f, 1.0f, 0.0f));
}

void sierpinski_triangle::setRotation(float rot_grades, glm::vec3 rot_vect) {
    if(this->isFull){
        for(int i=0;i<3;i++)
            childs[i]->setRotation(rot_grades, rot_vect);
    }
    else {
        transform = glm::rotate(transform, glm::radians(rot_grades), rot_vect);
    }
}


//creacion de gradiente
std::deque<glm::vec3> sierpinski_triangle::generate_color(int n_triangles = 2) {
    std::deque <glm::vec3> colors;

    float start_color_r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float start_color_g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float start_color_b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

    float end_color_r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float end_color_g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float end_color_b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

    float distance_r = (end_color_r- start_color_r)  / n_triangles;
    float distance_g = (end_color_g- start_color_g) / n_triangles;
    float distance_b = (end_color_b- start_color_b) / n_triangles;

    for (int i = 0; i < n_triangles; i++) {
        colors.push_back(glm::vec3(start_color_r, start_color_g, start_color_b));
        start_color_r += distance_r;
        start_color_g += distance_g;
        start_color_b += distance_b;
    }
    return colors;
}
