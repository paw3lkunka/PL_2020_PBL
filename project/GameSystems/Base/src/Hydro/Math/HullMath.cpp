#include "Hydro/Math/HullMath.hpp"

#include "Hydro/Data/HydroTriangle.hpp"
#include "Hydro/Data/HullTriangles.hpp"

HullTriangles hullTriangles;

glm::vec3 H;
glm::vec3 M;
glm::vec3 L;

float hH;
float hM;
float hL;

HullTriangles HullMath::cutHull(std::vector<HydroTriangle> triangles)
{
    hullTriangles.abovewater.clear();
    hullTriangles.underwater.clear();
    
    for(auto triangle : triangles)
    {
        processTriangle(triangle);
    }

    return hullTriangles;
}

void HullMath::processTriangle(HydroTriangle triangle)
{
    std::pair<glm::vec3, float> vertices[] =
    {
        std::pair(triangle.A, triangle.hA),
        std::pair(triangle.B, triangle.hB),
        std::pair(triangle.C, triangle.hC)
    };

    std::sort
    (
        vertices[0], 
        vertices[2], 
        [](const std::pair<glm::vec3, float>& v1, const std::pair<glm::vec3, float>& v2)
        {
            return v1.second > v2.second;
        }
    );

    H = vertices[0].first;
    M = vertices[1].first;
    L = vertices[2].first;

    hH = vertices[0].second;
    hM = vertices[1].second;
    hL = vertices[2].second;

    if(hH <= 0.0f && hM <= 0.0f && hL <= 0.0f)
    {
        hullTriangles.underwater.push_back(triangle);
    }
    else if(hH > 0.0f && hM < 0.0f && hL < 0.0f)
    {
        oneVertexAboveSurface();
    }
    else if(hH > 0.0f && hM > 0.0f && hL < 0.0f)
    {
        twoVerticesAboveSurface();
    }
    else
    {
        hullTriangles.abovewater.push_back(triangle);
    }
    
}

void HullMath::oneVertexAboveSurface()
{
    float tM = -hM / (hH - hM);
    float tL = -hL / (hH - hL);
    glm::vec3 MH = H - M;
    glm::vec3 LH = H - L;
    glm::vec3 cutMH = M + MH * tM;
    glm::vec3 cutLH = L + LH * tL;

    hullTriangles.underwater.push_back(HydroTriangle(M, cutMH, L));
    hullTriangles.underwater.push_back(HydroTriangle(L, cutLH, cutMH));
    hullTriangles.abovewater.push_back(HydroTriangle(H, cutMH, cutLH));
}

void HullMath::twoVerticesAboveSurface()
{
    float tM = -hL / (hM - hL);
    float tH = -hL / (hH - hL);
    glm:vec3 LM = M - L;
    glm:vec3 LH = H - L;
    glm:vec3 cutLM = L + LM * tM;
    glm:vec3 cutLH = L + LH * tH;

    hullTriangles.underwater.push_back(HydroTriangle(L, cutLM, cutLH));
    hullTriangles.abovewater.push_back(HydroTriangle(H, cutLH, cutLM));
    hullTriangles.abovewater.push_back(HydroTriangle(H, M, cutLM));
}
