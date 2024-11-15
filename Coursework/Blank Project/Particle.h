#pragma once

#include "../nclgl/Vector2.h"
#include "../nclgl/Vector4.h"
#include "../nclgl/Matrix4.h"
#include "../nclgl/Vector3.h"
#include "../nclgl/Mesh.h"
#include "../nclgl/Material.h"
#include <vector>

#include<vector>


struct Particle
{
    Vector3 position;
    Vector3 velocity;
    Vector4 colour;

    float life;

    float CalcLifeSpan() {
        return 30.0f * (((rand() % 100)) / 100.0f);
    }

    Particle() {
        position = Vector3(0,0,0);
        velocity = Vector3(0.1, 0.1, 0.1);
        colour = Vector4(0, 0, 0, 0);
        life = CalcLifeSpan();
    }
};



class ParticleManager
{
    public:

        const std::vector<Particle>& GetParticles() const { return particles; }

        ParticleManager(void) {}

        ParticleManager(Vector3 offset, GLuint* texture, Shader* shader) {
            this->offset = offset;
            this->mesh = Mesh::GenerateQuad();
            this->texture = texture;
            this->shader = shader;
            this->particles = std::vector<Particle>();

            this->count = 500;
            aliveCount = count;
            positions = new Matrix4[count];

            for (unsigned int i = 0; i < count; ++i)
            {
                particles.push_back(Particle());
                positions[i] = Matrix4::Translation(Vector3(0,0,0)) * Matrix4::Scale(Vector3(3, 3, 3));

            }
        }

        int GetCount() { return aliveCount; }
        
        void UpdateParticles(float dt, Vector3 offset)
        {
            this->offset = offset;

            for (int i = 0; i < count; ++i)
            {
                Particle& p = particles[i];
                p.life -= dt * 4;

                if (p.life <= 0.0f) {

                    RespawnParticle(p, this->offset);
                }
                else {
                    p.position.y -= 2 * p.velocity.y;
                    p.position.x += p.velocity.x * 3;
                    p.position.z += p.velocity.z * 3;
                }

                Vector3 toCamera = (offset - p.position);
                toCamera.Normalise();

                Vector3 up = Vector3::Cross(toCamera, Vector3(1, 0, 0));

                if (up.Length() < 1e-6) {
                    up = Vector3::Cross(toCamera, Vector3(0, 1, 0));
                }
                up.Normalise();

                Vector3 right = Vector3::Cross(up, toCamera);
                Matrix4 rot = Matrix4::FromAxes(right, up, toCamera);


                positions[i] = Matrix4::Translation(p.position) * rot * Matrix4::Scale(Vector3(3,3,3));
            }
        }

        void Draw(const OGLRenderer& r)
        {
            glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, *texture);
            glUniform3fv(glGetUniformLocation(shader->GetProgram(), "position"), 1, (float*)&Vector3(0,0,0));


            int binding = 5;
            GLuint someBuffer;
            glGenBuffers(1, &someBuffer);
            glBindBuffer(GL_UNIFORM_BUFFER, someBuffer);
            glBufferData(GL_UNIFORM_BUFFER, 8192, positions, GL_DYNAMIC_DRAW);
            glBindBufferBase(GL_UNIFORM_BUFFER, binding, someBuffer);
            int ubo = glGetUniformBlockIndex(shader->GetProgram(), "ObjectMatrices");
            glUniformBlockBinding(shader->GetProgram(), ubo, binding);

            mesh->InstancedDraw(count);
        }

        std::vector<Particle> particles;
        Shader* GetShader() { return shader; }
        GLuint* GetTexture() { return texture; }


  protected:    
        Vector3 offset;   
        Mesh* mesh;
        GLuint* texture;
        Shader* shader;
        Matrix4* positions;
        int count = 500;
        int aliveCount;
        int lastUsedParticle = 0;


        void RespawnParticle(Particle& particle, Vector3 offset)
        {
            float rColor = 0.5f + ((rand() % 100) / 100.0f);

            float mult = 30;
            float xPos = ((rand() % 200) - 100) / 100.0f;
            float zPos = ((rand() % 200) - 100) / 100.0f;
            xPos *= mult;
            zPos *= mult;
            particle.position = Vector3(xPos, 200, zPos) + offset;
            particle.colour = Vector4(rColor, rColor, rColor, 1.0f);
            particle.life = particle.CalcLifeSpan();
            particle.velocity = Vector3(((rand() % 200) - 100) / 100.0f,
                (((rand() % 100)) / 100.0f) + 0.2f,
                ((rand() % 200) - 100) / 100.0f);
        }


};


