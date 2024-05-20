// подключение всех библиотек

//  ********    !!!ВООБЩЕ НЕ ТРОГАТЬ!!!   *********
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector> //для массива вектор
#include <imgui.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
//  ********    !!!ВООБЩЕ НЕ ТРОГАТЬ!!!   *********

//обще известные переменные
bool drawMode = true; // режим рисования вкл/выкл
bool drawPolygon = false;
bool addNewPoints = true;
std::vector<std::pair<float, float>> points; // массив точек
float initialPointX = 0.0f, initialPointY = 0.0f;
float currentArea = 0.0f;

//выход по кнопке ESC
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) // функция выхода от кнопки ESC
{
    // Когда пользователь нажимает ESC, мы устанавливаем свойство WindowShouldClose в true,
    // и приложение после этого закроется
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

//реакция на мышку
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
        drawMode = !drawMode;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && addNewPoints and drawMode == true) {
        // Добавление точки при нажатии левой кнопки мыши
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float x = static_cast<float>(xpos) / width * 2 - 1;
        float y = static_cast<float>(height - ypos) / height * 2 - 1;
        points.push_back(std::make_pair(x, y));
        if (points.size() == 1) {
            initialPointX = x;
            initialPointY = y;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        // Удаление последней точки при нажатии правой кнопки мыши
        if (!points.empty()) {
            points.pop_back();
            // Дополнительные действия при удалении точки, если нужно
        }
    }
}


//*********     Не трогать! Математические колдунсва!   *********
// счётчик площади
float calculateArea(const std::vector<std::pair<float, float>>& polygon) {
    float area = 0.0f;
    int n = polygon.size();

    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        area += polygon[i].first * polygon[j].second - polygon[j].first * polygon[i].second;
    }

    return std::abs(area) / 2.0f;
}

// счётчик площади в пикселях
float calculateAreaInPixels(const std::vector<std::pair<float, float>>& polygon, int width, int height) {
    float area = calculateArea(polygon);
    float worldArea = 4.0f;
    float pixelArea = (area / worldArea) * (width * height);
    return pixelArea;
}
//*********     Не трогать! Математические колдунсва!   *********



void renderInterface() {
    ImGui::Begin("Information");
    switch (drawMode)
    {
    case true :
        ImGui::TextColored(ImVec4(0.1f, 0.7f, 0.1f,1), "You can DRAW!");
        break;

    case false :

        ImGui::TextColored(ImVec4(1.0f,0.0f,0.0f,1), "You can NOT draw!");
        break;
    }
    currentArea = calculateArea(points);
    ImGui::InputFloat("Area of figure", &currentArea, ImGuiInputTextFlags_ReadOnly);
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f,1),"Control:");

    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f,1), "left mouse button - draw a point");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f,1), "right mouse button - delete the last drawn point");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f,1), "Middle mouse button - switch mod of drawing (on/off)");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f,1), "ESC - close program");

    ImGui::End();
}

int main() {
    if (!glfwInit()) {
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(1024, 768, "2D Area calculator", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback); // если нажали ESC - прога завершаеться
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Oh no! GLAD is De@d! (Again)" << std::endl;
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::StyleColorsDark();

    while (!glfwWindowShouldClose(window)) {
        // цвет фона
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // отрисовка если точек уже 3
        if (drawPolygon && points.size() >= 3) {
            float area = calculateArea(points);
            ImGui::Text("Area: %.2f", area);
        }


        // отрисовка если точек только 2
        if (points.size() >= 2) {
            glLineWidth(2.0f);
            glBegin(GL_LINES);
            glColor3f(0.3f, 0.8f, 0.3f);                                        // ЦВЕТ ЛИНИЙ (тёмно зелёный)
            for (size_t i = 0; i < points.size() - 1; ++i) {
                glVertex2f(points[i].first, points[i].second);
                glVertex2f(points[i + 1].first, points[i + 1].second);
            }
            glVertex2f(points.back().first, points.back().second);
            glVertex2f(points.front().first, points.front().second);
            glEnd();
        }

        //если точек вообще нет
        if (!points.empty()) {
            glPointSize(8.0f);
            glBegin(GL_POINTS);
            glColor3f(0.1f, 1.0f, 0.1f);                                        // цвет точек (кислотно зелёный)
            for (const auto& point : points) {
                glVertex2f(point.first, point.second);
            }
            glEnd();
        }

        //  ********    это база! не трогать!   *********
        renderInterface();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
        //  ********    это база! не трогать!   *********
    }

    //  ********    это база! не трогать!   *********
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    //  ********    это база! не трогать!   *********

    return 0;
}
