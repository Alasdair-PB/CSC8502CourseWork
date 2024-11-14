#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include <unordered_map>
#include <iostream>
#include <variant>
#include <vector>
#include "../nclgl/MeshAnimation.h"


class Material
{
public:
    Material(Shader* shader) : shader(shader) {
        properties = std::vector<std::unordered_map<std::string, std::unordered_map<std::string, PropertyValue>>>();
    }

    enum WorldValue {
        CameraPosition, DeltaTimeSeason, DeltaTime, Temperature, CubeMap,
        TessQuad, TessTri, LightRender, FarPlane, DualFace, DepthTexture, BatchSize50, BatchSize100, BatchSize300, ShadowMap, ProjMatrix, ViewMatrix, Dimensions
    };
    using PropertyValue = std::variant<Vector4, GLuint*, Vector3, Matrix4, Matrix4*, int, std::string, MeshAnimation*, WorldValue, float, std::vector<GLuint*>>;

    void SetShader(Shader* shader) { this->shader = shader; }
    Shader* GetShader() const { return shader; }


    std::string getOuterKey(const PropertyValue& value) {
        return std::visit([](auto&& val) -> std::string {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, GLuint*>) return "guint";
            else if constexpr (std::is_same_v<T, Vector4>) return "vector4";
            else if constexpr (std::is_same_v<T, Vector3>) return "vector3";
            else if constexpr (std::is_same_v<T, Matrix4>) return "matrix4";
            else if constexpr (std::is_same_v<T, int>) return "int";
            else if constexpr (std::is_same_v<T, float>) return "float";
            else if constexpr (std::is_same_v<T, std::string>) return "string";
            else if constexpr (std::is_same_v<T, Material::WorldValue>) return "worldValue";
            else if constexpr (std::is_same_v<T, std::vector<GLuint*>>) return "textureArray";
            else if constexpr (std::is_same_v<T, Matrix4*>) return "matrixArray";
            else if constexpr (std::is_same_v<T, MeshAnimation*>) return "meshAnimation";

            else return "unknown";
            }, value);
    }

    void AddProperty(const std::string& innerKey, const PropertyValue& value) {
        std::string outerKey = getOuterKey(value);
        if (properties.empty() || properties.back().empty() || properties.back().find(outerKey) == properties.back().end()) {
            properties.emplace_back(std::unordered_map<std::string, std::unordered_map<std::string, PropertyValue>>());
        }
        auto& propertyMap = properties.back();
        if (propertyMap.find(outerKey) == propertyMap.end()) {
            propertyMap[outerKey] = std::unordered_map<std::string, PropertyValue>();
        }
        propertyMap[outerKey][innerKey] = value;
    }

    const std::vector<std::unordered_map<std::string, std::unordered_map<std::string, PropertyValue>>>& GetProperties() const {
        return properties;
    }

protected:
    Shader* shader;
    std::vector<std::unordered_map<std::string, std::unordered_map<std::string, PropertyValue>>> properties;
};
