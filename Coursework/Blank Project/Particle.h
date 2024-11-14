#pragma once
#include "../nclgl/Vector2.h"
#include "../nclgl/Vector4.h"
#include "../nclgl/SceneNode.h"

#include<vector>

struct Particle
{
    Vector3 position;
    Vector3 velocity;
    Vector4 colour;

    float life;

    Particle() {
        position = Vector3(0, 0, 0);
        velocity = Vector3(0, 0, 0);
        colour = Vector4(0, 0, 0, 0);
        life = 0.0f;
    }
};


class ParticleManager
{
    public:

        const std::vector<Particle>& GetParticles() const { return particles; }

        ParticleManager(void) {}

        ~ParticleManager(void) 
        {
            delete shader;
            glDeleteTextures(1, texture);
        }

        ParticleManager(Vector3 offset, GLuint* texture, Shader* shader) {
            this->offset = offset;
            this->mesh = Mesh::GenerateTriangle();
            this->texture = texture;
            this->shader = shader;
            this->particles = std::vector<Particle>();

            for (unsigned int i = 0; i < count; ++i) {
                particles.push_back(Particle());
            }
        }
        
        void UpdateParticles(float dt) {

            unsigned int newcount = 2;
            for (unsigned int i = 0; i < newcount; ++i)
            {
                int unusedParticle = FirstUnusedParticle();
                RespawnParticle(particles[unusedParticle], this->offset);
            }

            for (unsigned int i = 0; i < count; ++i)
            {
                Particle& p = particles[i];
                p.life -= dt;
                if (p.life > 0.0f)
                    p.position = p.position - Vector3(p.velocity.x * dt, p.velocity.y * dt, p.velocity.z * dt);
                p.colour.w -= dt * 2.5f;
            }
        }

        void Draw() {
            mesh->Draw();
        }

        std::vector<Particle> particles;
        Shader* GetShader() { return shader; }
        GLuint* GetTexture() { return texture; }


  protected:    
        Vector3 offset;   
        Mesh* mesh;
        GLuint* texture;
        Shader* shader;

        unsigned int count = 500;
        unsigned int lastUsedParticle = 0;

        unsigned int FirstUnusedParticle()
        {
            for (unsigned int i = lastUsedParticle; i < count; ++i) {
                if (particles[i].life <= 0.0f) {
                    lastUsedParticle = i;
                    return i;
                }
            }
            for (unsigned int i = 0; i < lastUsedParticle; ++i) {
                if (particles[i].life <= 0.0f) {
                    lastUsedParticle = i;
                    return i;
                }
            }
            lastUsedParticle = 0;
            return 0;
        }

        void RespawnParticle(Particle& particle, Vector3 offset)
        {
            float random = ((rand() % 100) - 50) / 10.0f;
            float rColor = 0.5f + ((rand() % 100) / 100.0f);

            particle.position = Vector3(random,random, random) + offset;

            particle.colour = Vector4(rColor, rColor, rColor, 1.0f);
            particle.life = 1.0f;
            particle.velocity = Vector3(0.1f, 0.1f, 0.1f);
        }

};


