// main.cpp
// Requires: GLAD, GLFW
// Compile (Linux):
// g++ main.cpp -o animated_name -lglfw -ldl -lGL -lpthread -lX11 -lXrandr -lXi
// macOS: add frameworks as required.

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <string>

void framebuffer_size_callback(GLFWwindow* window, int wdh, int hght);
void processInput(GLFWwindow *window);

// screen
const unsigned int SCR_W = 1000;
const unsigned int SCR_H = 600;

// ======= Vertex & Fragment shaders =======
// single shader used for both background shapes and text quads
const char *vertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec2 aPos;    // position (object space)
layout(location = 1) in vec3 aColor;  // base color
layout(location = 2) in vec2 aLocal;  // local uv/pos for text pixel (passed to frag)

out vec3 vColor;
out vec2 vLocal;
uniform vec2 uTranslate;
uniform float uAngle;
uniform float uScale;
uniform mat4 uOrtho;

void main()
{
    // 2D rotation/scale/translate (apply to aPos)
    float s = sin(uAngle), c = cos(uAngle);
    vec2 p = aPos * uScale;
    mat2 rot = mat2(c, -s, s, c);
    p = rot * p + uTranslate;
    gl_Position = uOrtho * vec4(p.xy, 0.0, 1.0);
    vColor = aColor;
    vLocal = aLocal;
}
)";

const char *fragmentShaderSrc = R"(
#version 330 core
in vec3 vColor;
in vec2 vLocal;
out vec4 FragColor;
uniform float uTime;

// soft circular mask for text pixels (makes them look nicer)
float circleMask(vec2 uv, float radius) {
    // uv in [-0.5, 0.5] range for single pixel quad
    float d = length(uv);
    float t = smoothstep(radius, radius - 0.04, d);
    return 1.0 - t;
}

void main()
{
    // dynamic hue shift using uTime and position to create gradient + shimmer
    float hueShift = sin(uTime * 1.2 + vLocal.x * 6.0 + vLocal.y * 4.0) * 0.5 + 0.5;
    vec3 dynamic = vec3(0.5 + 0.5*sin(uTime + hueShift*6.2831),
                        0.5 + 0.5*sin(uTime*0.7 + hueShift*4.0),
                        0.5 + 0.5*sin(uTime*1.4 + hueShift*2.0));
    vec3 col = vColor * dynamic;

    // if aLocal is (10,10) sentinel for non-text (we'll use vLocal out-of-range for bg shapes)
    // For text pixels: apply circular mask so pixels look like rounded dots
    if (vLocal.x <= 0.5) {
        // treat vLocal in [-0.5,0.5] space for masking
        vec2 uv = vLocal; // value already in [-0.5,0.5]
        float m = circleMask(uv, 0.45);
        FragColor = vec4(col * m, m); // alpha = mask -> soft edges
    } else {
        // background shapes: full opacity but slight vignette by distance from center
        float dist = length(gl_FragCoord.xy - vec2(%d, %d)) / max(%d.0, %d.0);
        float vign = smoothstep(0.9, 0.0, dist);
        FragColor = vec4(col * vign, 1.0);
    }
}
)";

// We'll fill the %d placeholders with SCR_W and SCR_H after building string below.


// 5x7 pixel font for characters we need. Each char is 5 columns (least significant bit = T row).
// We'll define uppercase letters and '.' and space. Pixel origin: T-L.
// For convenience, the bitmaps are stored as 7 rows of 5 bits but represented here as columns.
using Bitmap = std::array<int,5>;

// Helper to create bitmap from rows (5x7 rows as string lines)
Bitmap bm_from_rows(const char *rows[7]) {
    Bitmap b{};
    for (int col = 0; col < 5; ++col) b[col] = 0;
    for (int r = 0; r < 7; ++r) {
        for (int c = 0; c < 5; ++c) {
            if (rows[r][c] == 'X') {
                b[c] |= (1 << (6 - r)); // set bit (T row -> highest bit)
            }
        }
    }
    return b;
}

int main()
{
    // Patch the fragment shader with screen sizes for vignette computation
    char fragSrcBuffer[10000];
    {
        // format fragment shader source with constants
        snprintf(fragSrcBuffer, sizeof(fragSrcBuffer), fragmentShaderSrc,
                 SCR_W, SCR_H, SCR_W, SCR_H);
    }

    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_W, SCR_H, "S.M. Sajjad Hossain JIM — Animated", NULL, NULL);
    if (!window) { std::cout << "Failed to create GLFW window\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { std::cout << "Failed to init GLAD\n"; return -1; }

    // Build shaders
    auto compileShader = [&](GLenum type, const char* src)->GLuint {
        GLuint s = glCreateShader(type);
        glShaderSource(s, 1, &src, NULL);
        glCompileShader(s);
        int ok; char log[1024];
        glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if (!ok) { glGetShaderInfoLog(s, 1024, NULL, log); std::cout << "Shader compile error:\n" << log << std::endl; }
        return s;
    };

    GLuint vert = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragSrcBuffer);
    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);
    { int ok; char log[1024]; glGetProgramiv(program, GL_LINK_STATUS, &ok); if (!ok) { glGetProgramInfoLog(program, 1024, NULL, log); std::cout << "Link error:\n" << log << std::endl; } }
    glDeleteShader(vert); glDeleteShader(frag);

    // Enable blending for smooth rounded pixels
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ===== Build background shapes: many tiny triangles arranged in rings =====
    std::vector<float> bgVertices; // each vertex: x,y, r,g,b, localX,localY (local unused for bg: set to 999)
    int rings = 6;
    for (int ring = 0; ring < rings; ++ring) {
        float radius = 0.2f + ring * 0.12f;
        int tris = 24 + ring * 6;
        for (int t = 0; t < tris; ++t) {
            float ang = (t / (float)tris) * 2.0f * M_PI;
            // triangle center
            float cx = cos(ang) * radius;
            float cy = sin(ang) * radius;
            // small equilateral triangle vertices around center
            float triSize = 0.05f + 0.01f * ring;
            for (int v = 0; v < 3; ++v) {
                float a = ang + v * 2.0f * M_PI / 3.0f;
                float x = cx + cos(a) * triSize;
                float y = cy + sin(a) * triSize;
                // color per ring
                float rr = 0.4f + 0.12f * ring + 0.2f * sin(ang * 3.0f);
                float gg = 0.4f + 0.15f * ring + 0.2f * cos(ang * 2.2f);
                float bb = 0.5f - 0.08f * ring + 0.15f * sin(ang * 1.4f + 1.0f);
                bgVertices.push_back(x);
                bgVertices.push_back(y);
                bgVertices.push_back(rr);
                bgVertices.push_back(gg);
                bgVertices.push_back(bb);
                // sentinel local for bg (set > 0.5 to signal frag)
                bgVertices.push_back(100.0f);
                bgVertices.push_back(100.0f);
            }
        }
    }

    // VAO/VBO for background
    GLuint bgVAO, bgVBO;
    glGenVertexArrays(1, &bgVAO);
    glGenBuffers(1, &bgVBO);
    glBindVertexArray(bgVAO);
    glBindBuffer(GL_ARRAY_BUFFER, bgVBO);
    glBufferData(GL_ARRAY_BUFFER, bgVertices.size() * sizeof(float), bgVertices.data(), GL_STATIC_DRAW);
    // layout: pos(2), color(3), local(2)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // ===== Pixel font definitions for needed characters =====
    std::map<char, Bitmap> font;
    // Helper lambda to add by rows
    auto addChar = [&](char ch, const char *r0, const char *r1, const char *r2, const char *r3, const char *r4, const char *r5, const char *r6) {
        const char* rows[7] = { r0, r1, r2, r3, r4, r5, r6 };
        font[ch] = bm_from_rows(rows);
    };

    // '.' (dot)
    addChar('.', ".....",".....",".....",".....",".....","..X..","..X..");
    // SPACE
    addChar(' ', ".....",".....",".....",".....",".....",".....",".....");

    // A
    addChar('A', ".XXX.", "X...X", "X...X", "X...X", "XXXXX", "X...X", "X...X");
    // D
    addChar('D', "XXXX.", "X...X", "X...X", "X...X", "X...X", "X...X", "XXXX.");
    // H
    addChar('H', "X...X", "X...X", "X...X", "XXXXX", "X...X", "X...X", "X...X");
    // I
    addChar('I', "XXXXX", "..X..", "..X..", "..X..", "..X..", "..X..", "XXXXX");
    // J
    addChar('J', "..XXX", "...X.", "...X.", "...X.", "X..X.", "X..X.", ".XX..");
    // M
    addChar('M', "X...X", "XX.XX", "X.X.X", "X...X", "X...X", "X...X", "X...X");
    // N
    addChar('N', "X...X", "XX..X", "X.X.X", "X..XX", "X...X", "X...X", "X...X");
    // O
    addChar('O', ".XXX.", "X...X", "X...X", "X...X", "X...X", "X...X", ".XXX.");
    // S
    addChar('S', ".XXX.", "X...X", "X....", ".XXX.", "....X", "X...X", ".XXX.");

    // For any undefined char, use full blank
    for (char c : std::string("ABCDEFGHIJKLMNOPQRSTUVWXYZ. ")) {
        if (font.find(c) == font.end()) {
            addChar(c, ".....",".....",".....",".....",".....",".....",".....");
        }
    }

    // The name to render (use mixture as requested; we'll uppercase where mapping exists)
    std::string name = "S.M. Sajjad Hossain JIM";

    // Precompute quads for every "ON" pixel in name
    // We'll treat each pixel as a small rounded square (quad). Each quad = 6 vertices (2 tris).
    std::vector<float> textVerts; // each vertex: x,y, r,g,b, localX, localY
    float pixelSize = 0.035f; // base pixel size
    float cursorX = -0.85f;   // starting x (L)
    float cursorY = 0.18f;    // baseline y
    float xAdvance = 0.07f;   // spacing between characters
    float yAdvance = -0.09f;  // downward step if needed (we only use one line)

    for (size_t idx = 0; idx < name.size(); ++idx) {
        char raw = name[idx];
        // normalize to uppercase for mapping except keep dot and space
        char ch = raw;
        if (ch >= 'a' && ch <= 'z') ch = char(ch - 'a' + 'A');

        if (font.find(ch) == font.end()) ch = ' '; // fallback

        Bitmap bm = font[ch];

        // bm[c] has bits for rows: T->B in bits 6..0
        // Draw columns L to R, rows T to B
        for (int col = 0; col < 5; ++col) {
            for (int row = 0; row < 7; ++row) {
                bool on = (bm[col] & (1 << (6 - row))) != 0;
                if (!on) continue;
                float px = cursorX + col * (pixelSize + 0.004f);
                float py = cursorY - row * (pixelSize + 0.006f);
                // create quad centered at (px,py) with size pixelSize
                float half = pixelSize * 0.5f;
                // local coords for mask: pass uv in [-0.5,0.5]
                std::array<std::pair<float,float>,6> quad = {{
                    {px-half, py-half}, {px+half, py-half}, {px+half, py+half},
                    {px-half, py-half}, {px+half, py+half}, {px-half, py+half}
                }};
                for (auto &v : quad) {
                    textVerts.push_back(v.first);
                    textVerts.push_back(v.second);
                    // base color white (text color will be tinted by shader)
                    textVerts.push_back(1.0f);
                    textVerts.push_back(1.0f);
                    textVerts.push_back(1.0f);
                    // local uv in [-0.5,0.5]
                    float lx = (v.first - px) / (pixelSize);
                    float ly = (v.second - py) / (pixelSize);
                    // clamp to [-0.5,0.5]
                    if (lx < -0.5f) lx = -0.5f; if (lx > 0.5f) lx = 0.5f;
                    if (ly < -0.5f) ly = -0.5f; if (ly > 0.5f) ly = 0.5f;
                    textVerts.push_back(lx);
                    textVerts.push_back(ly);
                }
            }
        }

        // advance cursor
        cursorX += xAdvance;
        // small extra spacing if dot
        if (ch == '.') cursorX += 0.02f;
    }

    // Center the text horizontally by computing its bounds and shifting (optional)
    // (Simpler: skip; placement chosen above looks centered-ish.)

    // VAO/VBO for text
    GLuint textVAO, textVBO;
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, textVerts.size() * sizeof(float), textVerts.data(), GL_STATIC_DRAW);
    // layout same: pos(2), color(3), local(2)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // Precompute orthographic projection
    float L = -1.0f, R = 1.0f, B = -1.0f, T = 1.0f;
    float ortho[16] = {
        2.0f/(R-L), 0, 0, 0,
        0, 2.0f/(T-B), 0, 0,
        0, 0, -1, 0,
        -(R+L)/(R-L), -(T+B)/(T-B), 0, 1
    };

    // shader uniforms locations
    glUseProgram(program);
    GLint locTranslate = glGetUniformLocation(program, "uTranslate");
    GLint locAngle = glGetUniformLocation(program, "uAngle");
    GLint locScale = glGetUniformLocation(program, "uScale");
    GLint locTime = glGetUniformLocation(program, "uTime");
    GLint locOrtho = glGetUniformLocation(program, "uOrtho");
    glUniformMatrix4fv(locOrtho, 1, GL_FALSE, ortho);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        float t = (float)glfwGetTime();

        // background clear - we use dark transparent clear and let fragment do vignette
        glClearColor(0.03f + 0.02f*sin(t*0.3f), 0.02f + 0.01f*sin(t*0.2f+1.0f), 0.06f + 0.02f*sin(t*0.25f+2.0f), 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        // animate global transforms
        // Draw background shapes:
        glUniform2f(locTranslate, 0.0f, 0.0f);
        glUniform1f(locAngle, t * 0.18f); // slow rotation applied globally (each ring already positioned)
        glUniform1f(locScale, 1.0f);
        glUniform1f(locTime, t);

        // draw background triangles
        glBindVertexArray(bgVAO);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(bgVertices.size()/7));
        glBindVertexArray(0);

        // draw text: give slight bob/rotation & scale pulse
        float bobX = 0.02f * sin(t * 1.1f);
        float bobY = 0.025f * cos(t * 0.9f);
        float angle = 0.6f * sin(t * 0.6f) * 0.3f;
        float scale = 1.0f + 0.06f * sin(t * 2.2f);

        glUniform2f(locTranslate, bobX, bobY - 0.1f);
        glUniform1f(locAngle, angle);
        glUniform1f(locScale, scale * 0.9f);
        glUniform1f(locTime, t * 1.3f);

        glBindVertexArray(textVAO);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(textVerts.size()/7));
        glBindVertexArray(0);

        // small glowing duplicate layer for stronger neon effect (draw offset with additive blending)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glUniform1f(locTime, t * 1.7f + 3.0f);
        glUniform1f(locScale, scale * 1.02f);
        glBindVertexArray(textVAO);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(textVerts.size()/7));
        glBindVertexArray(0);
        // restore blend
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // cleanup
    glDeleteVertexArrays(1, &bgVAO);
    glDeleteBuffers(1, &bgVBO);
    glDeleteVertexArrays(1, &textVAO);
    glDeleteBuffers(1, &textVBO);
    glDeleteProgram(program);

    glfwTerminate();
    return 0;
}

// process input
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}

// on window resize
void framebuffer_size_callback(GLFWwindow* window, int wdh, int hght)
{
    glViewport(0, 0, wdh, hght);
}

