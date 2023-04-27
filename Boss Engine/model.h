#ifndef MODEL
#define MODEL

class Model
{
    protected:
        unsigned int VAO;
        unsigned int VBO;
        int dbCsucs;
        unsigned int textureID;

        int colorvan;

        float teszt[1000][10];
        int tesztdb;

    public:
        Model(char *modelHely, char *textureHely)
        {
            VAO = 0; VBO = 0; dbCsucs = 0;
            colorvan = 0;

            tesztdb = 0;

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            beolvas(modelHely);
            betoltTexture(textureHely);
        }

        Model(char *modelHely, char *right, char *left, char *top, char *bottom, char *front1, char *back1)
        {
            VAO = 0; VBO = 0; dbCsucs = 0;

            tesztdb = 0;

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            beolvas(modelHely);
            loadCubemap(right,left,top,bottom,front1,back1);
        }

    protected:
        void beolvas(char *hely)
        {

            float *vertex =(float*) malloc(200000000);
            unsigned int sorrend[9];

            int v=0, vt=0,vn=0;
            char a[1001];
            char mtl[100];
            char mtluse[100];
            char cmpmtl[100];

            float R,G,B,A;

            float *vertex1 = (float*) malloc(10000000);
            float *texture = (float*) malloc(10000000);
            float *normal = (float*) malloc(10000000);

            int faceszama = 0;

            int mtldb = 1;

            int plusz=0;

            FILE *f=fopen(hely,"r");
            if(f==0) {
                printf("Nem lehet megnyitni a file-t!\n%s\n\n",hely);
            }else {

            while(fgets(a,1000,f) != NULL){

                if(sscanf(a,"mtllib %s\n",&mtl))
                    printf("Van\n");

                if(sscanf(a,"usemtl %s\n",&mtluse)){
                    sprintf(a,"model/%s",mtl);
                    FILE *be = fopen(a,"r");

                    mtldb = 0;

                    if(be == 0)
                        printf("Hiba van!\n");
                    else
                    {
                        while(fgets(a,1000,be) != NULL && mtldb != 3)
                        {
                            if(sscanf(a,"newmtl %s\n",cmpmtl))
                                if(strcmp(cmpmtl,mtluse) == 0)
                                    mtldb = 1;

                            if(mtldb == 1)
                                if(sscanf(a,"Kd %f %f %f\n",&R,&G,&B))
                                {
                                    mtldb = 2;
                                    colorvan = 1;
                                }

                            if(mtldb == 2)
                                if(sscanf(a,"d %f\n",&A))
                                {
                                    mtldb = 3;

                                    int volt = 0;
                                    for(int i=0; i<tesztdb; i++)
                                    {
                                        if(teszt[i][0] == R && teszt[i][1] == G && teszt[i][2] == B && teszt[i][3] == A)
                                        {
                                            volt = 1; printf("\ndb szam a: (%d)\n",i); i = tesztdb+1;
                                        }
                                    }
                                    if(volt == 0)
                                    {
                                        teszt[tesztdb][0] = R; teszt[tesztdb][1] = G; teszt[tesztdb][2] = B; teszt[tesztdb][3] = A;
                                        printf("\ndb szam a: (%d)\n",tesztdb);
                                        tesztdb++;
                                    }
                                }
                        }
                    }
                    fclose(be);
                    }
                if(sscanf(a,"v %f %f %f",&vertex1[v],&vertex1[v+1],&vertex1[v+2])) {
                    v+=3; faceszama=1;
                }
                else if(sscanf(a,"vt %f %f",&texture[vt],&texture[vt+1])) {
                    if(faceszama==1) faceszama=2;
                    vt+=2;
                }
                else if(sscanf(a,"vn %f %f %f",&normal[vn],&normal[vn+1],&normal[vn+2])) {
                    if(faceszama==2) faceszama=3;
                    vn+=3;
                }
                else if(a[0]=='f' && a[1]==' ') {
                    if(faceszama==3){
                        sscanf(a,"f %d/%d/%d %d/%d/%d %d/%d/%d\n",&sorrend[0],&sorrend[1],&sorrend[2],&sorrend[3],&sorrend[4],&sorrend[5],&sorrend[6],&sorrend[7],&sorrend[8]);
                        sorrend[0]-=1; sorrend[1]-=1; sorrend[2]-=1;
                        sorrend[3]-=1; sorrend[4]-=1; sorrend[5]-=1;
                        sorrend[6]-=1; sorrend[7]-=1; sorrend[8]-=1;

                        for(int i=0; i<9; i+=3)
                        {
                            vertex[plusz+0]=vertex1[sorrend[i]*3];
                            vertex[plusz+1]=vertex1[sorrend[i]*3+1];
                            vertex[plusz+2]=vertex1[sorrend[i]*3+2];
                            vertex[plusz+3]=texture[sorrend[i+1]*2];
                            vertex[plusz+4]=texture[sorrend[i+1]*2+1];
                            vertex[plusz+5]=normal[sorrend[i+2]*3];
                            vertex[plusz+6]=normal[sorrend[i+2]*3+1];
                            vertex[plusz+7]=normal[sorrend[i+2]*3+2];

                            if(mtldb == 3)
                            {
                                vertex[plusz+8] = R;
                                vertex[plusz+9]= G;
                                vertex[plusz+10]= B;
                                vertex[plusz+11]= A;
                            }
                            else
                            {
                                vertex[plusz+8] = 1.0;
                                vertex[plusz+9]= 1.0;
                                vertex[plusz+10]= 1.0;
                                vertex[plusz+11]= 1.0;
                            }

                            plusz+=12;
                        }

                        dbCsucs+=3;
                    }
                    else if(faceszama==2){
                        sscanf(a,"f %d/%d %d/%d %d/%d\n",&sorrend[0],&sorrend[1],&sorrend[2],&sorrend[3],&sorrend[4],&sorrend[5]);
                        sorrend[0]-=1; sorrend[1]-=1; sorrend[2]-=1;
                        sorrend[3]-=1; sorrend[4]-=1; sorrend[5]-=1;

                        for(int i=0; i<6; i+=2){
                            vertex[plusz]=vertex1[sorrend[i]*3];
                            vertex[plusz+1]=vertex1[sorrend[i]*3+1];
                            vertex[plusz+2]=vertex1[sorrend[i]*3+2];
                            vertex[plusz+3]=texture[sorrend[i+1]*2];
                            vertex[plusz+4]=texture[sorrend[i+1]*2+1];

                            if(mtldb == 2)
                            {
                                vertex[plusz+5] = R;
                                vertex[plusz+6]= G;
                                vertex[plusz+7]= B;
                            }
                            else
                            {
                                vertex[plusz+5] = 1.0;
                                vertex[plusz+6]= 1.0;
                                vertex[plusz+7]= 1.0;
                            }

                            plusz+=8;
                        }

                        dbCsucs+=3;
                    }
                }

            }

            fclose(f);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, plusz * sizeof(float), vertex, GL_STATIC_DRAW);

            if(faceszama==2){
                     // position attribute
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
                    glEnableVertexAttribArray(0);
                    // texture coord attribute
                    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
                    glEnableVertexAttribArray(1);
                    ///color
                    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
                    glEnableVertexAttribArray(2);

            }else if(faceszama==3){
                    // position attribute
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
                    glEnableVertexAttribArray(0);
                    // texture coord attribute
                    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(3 * sizeof(float)));
                    glEnableVertexAttribArray(2);
                    // normal attribute
                    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(5 * sizeof(float)));
                    glEnableVertexAttribArray(1);
                    ///color
                    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(8 * sizeof(float)));
                    glEnableVertexAttribArray(3);
            }
            glBindVertexArray(0);

            free(vertex);
            free(vertex1);
            free(texture);
            free(normal);
            }
        }

    public:
        draw(Shader shader, float eltolX = 0.0, float eltolY = 0.0, float eltolZ = 0.0, float meretez = 1.0)
        {

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureID);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(eltolX, eltolY, eltolZ));
            model = glm::scale(model, glm::vec3(meretez));
            shader.setMat4("model", model);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, dbCsucs);
            glBindVertexArray(0);
        }

        draw(Shader shader, Camera camera, glm::mat4 projection)
        {
            glDisable(GL_CULL_FACE);
            // draw skybox as last
            glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
            shader.use();
            //glm::mat4 view = glm::mat4(glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up)); // remove translation from the view matrix
            glm::mat4 view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up)));
            shader.setMat4("view", view);
            shader.setMat4("projection", projection);
            // skybox cube
            glBindVertexArray(VAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
            glDrawArrays(GL_TRIANGLES, 0, dbCsucs);
            glBindVertexArray(0);
            glDepthFunc(GL_LESS); // set depth function back to default
        }

    protected:
        void betoltTexture(char const * path)
        {
            glGenTextures(1, &textureID);

            int width, height, nrComponents;
            unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
            if (data)
            {
                GLenum format;
                if (nrComponents == 1)
                    format = GL_RED;
                else if (nrComponents == 3)
                    format = GL_RGB;
                else if (nrComponents == 4)
                    format = GL_RGBA;

                glBindTexture(GL_TEXTURE_2D, textureID);
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                stbi_image_free(data);
            }
            else
            {
                printf("Texture failed to load at path: %s",path);
                stbi_image_free(data);
            }

        }

        void loadCubemap(char *right, char *left, char *top, char *bottom, char *front1, char *back1)
        {
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

            char faces[6][100];

            sprintf(faces[0],right);
            sprintf(faces[1],left);
            sprintf(faces[2],top);
            sprintf(faces[3],bottom);
            sprintf(faces[4],front1);
            sprintf(faces[5],back1);


            int width, height, nrChannels;
            for (int i = 0; i < 6; i++)
            {
                unsigned char *data = stbi_load(faces[i], &width, &height, &nrChannels, 0);
                if (data)
                {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
                    );
                    stbi_image_free(data);
                }
                else
                {
                    printf("Cubemap texture failed to load at path: %s",faces[i]);
                    stbi_image_free(data);
                }
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        }


};



#endif
