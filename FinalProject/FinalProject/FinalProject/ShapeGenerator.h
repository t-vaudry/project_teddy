#pragma once
#include <vector>
#include <Shape.h>
#include <objloader.h>

using namespace std;

class ShapeGenerator
{
private:
    static float mWallHeight;
    static float mWallWidth;
    static float mWallDepth;
    static vector<glm::mat4> mTopWindowMatrices;
    static vector<glm::mat4> mBottomWindowMatrices;
    static vector<glm::mat4> mWindowMatrices;
public:
    static Shape GenerateCube(glm::vec3 color, glm::vec3 scale = glm::vec3(1.0f), glm::vec3 rotate = glm::vec3(0.0f), glm::vec3 translate = glm::vec3(0.0f));
    static Shape GenerateQuad(glm::vec3 color, float width, float height, float depth, glm::vec3 rotate = glm::vec3(0.0f), glm::vec3 translate = glm::vec3(0.0f));
    static Shape GenerateAABB(vector<glm::vec3>& points);
    static Shape GenerateWall();
    static Shape GenerateWindowWall();
    static Shape GenerateWindow();
    static Shape GenerateLine(glm::vec3 first, glm::vec3 second, glm::vec3 color);
    static Shape GenerateOBJ(const char* path, glm::vec3 color, glm::vec3 scale = glm::vec3(1.0f), glm::vec3 rotate = glm::vec3(0.0f), glm::vec3 translate = glm::vec3(0.0f));
    static Shape GeneratePoint(glm::vec2 position, glm::vec3 color);
    static Shape GenerateTerrain(glm::vec3 color, float height, bool inverse);
    static Shape GenerateCrosshair();
    static Shape GenerateMenu();

    static void SetWallDimensions(float w, float h, float d);
    static vector<glm::mat4> GenerateWallModelMatrices();
    static vector<glm::mat4> GetTopWindowMatrices();
    static vector<glm::mat4> GetBottomWindowMatrices();
    static vector<glm::mat4> GetWindowMatrices();
    static vector<glm::mat4> GeneratePlaneModelMatrices(Shape* shape);
};

