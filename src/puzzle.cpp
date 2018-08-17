#include"Renderer.h"
#include<iostream>
#include"Shader_s.h"
#include"VertexBuffer.h"
#include"VertexBufferLayout.h"
#include"VertexArray.h"
#include"Texture.h"
#include"FrameBuffer.h"
#include<vector>
#include"imgui\imgui.h"
#include"imgui\imgui_impl_glfw_gl3.h"

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float lastTime = 0.0f, deltaTime = 0.0f;
bool firstTime = true;
const float _NEAR = 0.1f;
const float _FAR = 10.0f;
static int newMouseState = GLFW_RELEASE;
static int oldMouseState = GLFW_RELEASE;
static int aTex = -1, bTex = -1;
static int texIndex = -1;
std::vector<unsigned int> sqrTex(25);
std::vector<unsigned int> orgTex(25);
bool finish = false;
unsigned int StartTime = 0;
unsigned int TotalTime = 0;
unsigned int moves = 0;

void processInput(GLFWwindow*);
void framebuffer_size_callback(GLFWwindow*, int, int);
void mouse_button_callback(GLFWwindow*, int, int, int);
void mouse_callback(GLFWwindow*, double, double);

unsigned int getSeed()
{
	unsigned int seed = 100000 * glfwGetTime();
	seed = seed % 10;
	return seed;
}

std::string ConvertTime(unsigned int secTym)
{
	unsigned int hour = secTym / 3600;
	secTym = secTym % 3600;
	unsigned int minute = secTym / 60;
	unsigned int second = secTym % 60;
	std::string Time = std::to_string(second) + "s";
	if (minute != 0)
		Time = std::to_string(minute) + "m," + Time;
	if (hour != 0)
		Time = std::to_string(hour) + "h," + Time;
	return Time;
}

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar *message,
	const void *userParam)
{
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return; // ignore these non-significant error codes

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;
}

int main()
{
	if (!glfwInit())
	{
		std::cout << "glfw can't be initialized" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "AlooMatar", NULL, NULL);
	if (!window)
	{
		std::cout << "Window creation failed" << std::endl;
		std::cin.get();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, NULL);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSwapInterval(1);
	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;

	GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // makes sure errors are displayed synchronously
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
	glDisable(GL_DEPTH_TEST);
	{
		float vertices[] = {
			//pos		  //tex
			1.0f,1.0f,	   0.2f,0.2f,
			1.0f,-1.0f,   0.2f,0.0f, 
			-1.0f,-1.0f,   0.0f,0.0f,
			-1.0f,-1.0f,   0.0f,0.0f,
			-1.0f,1.0f,    0.0f,0.2f,
			1.0f,1.0f,     0.2f,0.2f  
		};

		VertexArray va;
		VertexBuffer vb(vertices,sizeof(vertices),false);
		{
			VertexBufferLayout vbl;
			vbl.push<float>(2);
			vbl.push<float>(2);
			va.AddBuffNLay(vb, vbl);
			vb.Unbind();
			va.Unbind();
		}

		const unsigned int SQR_WIDTH = SCR_WIDTH / 5;
		const unsigned int SQR_HEIGHT = SCR_HEIGHT / 5;

		unsigned int sem1;
		GLCall(glGenSamplers(1, &sem1));
		GLCall(glSamplerParameteri(sem1, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCall(glSamplerParameteri(sem1, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLCall(glBindSampler(0, sem1));
		
		Texture T1("extras/textures/puzzle/simpson.jpg",GL_REPEAT);

		Shader quadShader;
		quadShader.push(GL_VERTEX_SHADER, "extras/shader/puzzle/quad.vs");
		quadShader.push(GL_FRAGMENT_SHADER, "extras/shader/puzzle/quad.fs");
		quadShader.buildShader();

		Shader sQShader;
		sQShader.push(GL_VERTEX_SHADER, "extras/shader/puzzle/sQyad.vs");
		sQShader.push(GL_FRAGMENT_SHADER, "extras/shader/puzzle/sQyad.fs");
		sQShader.buildShader();

		quadShader.use();
		quadShader.setInt("Texture1", 0);
		sQShader.use();
		sQShader.setInt("Texture1", 1);
		sQShader.setFloat("finish", 0.0f);

		Renderer render;
		glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 projection = glm::ortho(1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

		unsigned int sqrFB[25];
		GLCall(glGenFramebuffers(25, sqrFB));

		glGenTextures(25, &sqrTex[0]);

		for (unsigned int i = 0; i < sqrTex.size(); ++i)
		{
			GLCall(glBindFramebuffer(GL_FRAMEBUFFER, sqrFB[i]));
			glActiveTexture(GL_TEXTURE1);
			GLCall(glBindTexture(GL_TEXTURE_2D, sqrTex[i]));
			GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL));
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

			GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sqrTex[i], 0));

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "ERROR::FRAMBUFFER - Framebuffer is not complete." << std::endl;
			GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
			GLCall(glBindTexture(GL_TEXTURE_2D, 0));
		}
		orgTex = sqrTex;
		for(unsigned int i=0;i<5;++i)
		{
			for (unsigned int j = 0; j < 5; ++j)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, sqrFB[5 * i + j]);

				glViewport(0.0f, 0.0f, SQR_WIDTH, SQR_HEIGHT);
				glClearColor(0.5f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);

				quadShader.use();
				quadShader.setMat4("view", glm::value_ptr(view));
				quadShader.setMat4("projection", glm::value_ptr(projection));
				quadShader.setInt("indexX", j);
				quadShader.setInt("indexY", i);
				T1.Bind(0, GL_TEXTURE_2D);

				render.Draw(GL_TRIANGLES, va, quadShader, 6, 0);

				glfwSwapBuffers(window);
				glfwPollEvents();
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		const float waitTime = 5.0f;
		while (glfwGetTime()<waitTime)
		{
			glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			for (unsigned int i = 0; i < 5; ++i)
			{
				for (unsigned int j = 0; j < 5; ++j)
				{
					glViewport(j*SQR_WIDTH, i*SQR_HEIGHT, SQR_WIDTH, SQR_HEIGHT);

					sQShader.use();
					sQShader.setMat4("view", glm::value_ptr(view));
					sQShader.setMat4("projection", glm::value_ptr(projection));
					sQShader.setFloat("aBloom", 0.0f);

					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, sqrTex[5 * i + j]);

					render.Draw(GL_TRIANGLES, va, sQShader, 6, 0);
				}
			}
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
		std::srand(getSeed());
		for (unsigned int k = 0; k < 2; ++k)
		{
			for (unsigned int i = 0; i < 5; ++i)
			{
				for (unsigned int j = 0; j < 4; ++j)
				{
					int rand = std::rand();
					rand = rand % 10;
					rand = rand % (4 - j) + 1 + j;
					if(k==0)
						std::swap(sqrTex[5 * i + j], sqrTex[5 * i + rand]);
					else
						std::swap(sqrTex[5 * j + i], sqrTex[5 * j + rand]);
				}
			}
		}
		std::srand(getSeed());
		int max = sqrTex.size();
		for (unsigned int i = 0; i < max; ++i)
		{
			int rand = std::rand();
			rand = rand % 100;
			rand = rand % max;
			std::swap(sqrTex[i], sqrTex[rand]);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		ImGui::StyleColorsDark();
		ImGuiIO& io = ImGui::GetIO();
		ImFont* pFont = io.Fonts->AddFontFromFileTTF("E:/c++/glCont/glCont/extras/textures/Ruda-Bold.ttf", 50);
		
		glfwSetMouseButtonCallback(window, mouse_button_callback);
		StartTime = glfwGetTime();
		while (!glfwWindowShouldClose(window))
		{
			float currentFrame = float(glfwGetTime());
			deltaTime = currentFrame - lastTime;
			lastTime = currentFrame;
			
			ImGui_ImplGlfwGL3_NewFrame();

			processInput(window);
			glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			for (unsigned int i = 0; i < 5; ++i)
			{
				for (unsigned int j = 0; j < 5; ++j)
				{
					glViewport(j*SQR_WIDTH, i*SQR_HEIGHT, SQR_WIDTH, SQR_HEIGHT);

					sQShader.use();
					sQShader.setMat4("view", glm::value_ptr(view));
					sQShader.setMat4("projection", glm::value_ptr(projection));
					if (texIndex > -1 && (5 * i + j) == texIndex)
						sQShader.setFloat("aBloom", 1.0f);
					else
						sQShader.setFloat("aBloom", 0.0f);

					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, sqrTex[5 * i + j]);

					render.Draw(GL_TRIANGLES, va, sQShader, 6, 0);
				}
			}

			if (finish)
			{
				bool bOpened(true);
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
				ImGui::Begin("BCKGND", NULL, ImGui::GetIO().DisplaySize, 0.0f, ImGuiWindowFlags_NoTitleBar |
							ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
							ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings |
							ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
				ImGui::SetWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
				ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);
				ImGui::SetWindowCollapsed(false);
				ImGui::GetWindowDrawList()->AddText(pFont, 50.0f, ImVec2(750, 400), ImColor(1.0f, 1.0f, 1.0f, 1.0f), "CONGRATULATIONS");
				std::string TimeSTR = "Time - " + ConvertTime(TotalTime);
				ImGui::GetWindowDrawList()->AddText(pFont, 50.0f, ImVec2(750, 500), ImColor(1.0f, 1.0f, 1.0f, 1.0f), TimeSTR.c_str());
				std::string MoveSTR = "Moves - " + std::to_string(moves);
				sQShader.use();
				sQShader.setFloat("finish", 1.0f);
				ImGui::GetWindowDrawList()->AddText(pFont, 50.0f, ImVec2(750, 600), ImColor(1.0f, 1.0f, 1.0f, 1.0f), MoveSTR.c_str());
				ImGui::End();
				ImGui::PopStyleColor();
			}

			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);
	//if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	//	sqrTex = orgTex;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	GLCall(glViewport(0, 0, width, height));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	newMouseState = glfwGetMouseButton(window, button);
	if (button == GLFW_MOUSE_BUTTON_LEFT && newMouseState == GLFW_RELEASE && oldMouseState == GLFW_PRESS)
	{
		aTex = (int)lastX / 384;
		bTex = 4 - (int)lastY / 216;
		texIndex = 5 * bTex + aTex;	
	}
	if (texIndex!=-1 && button == GLFW_MOUSE_BUTTON_RIGHT && newMouseState == GLFW_RELEASE && oldMouseState == GLFW_PRESS)
	{
		int sTex = (int)lastX / 384;
		int tTex = 4 - (int)lastY / 216;
		
		int as = aTex - sTex;
		int bt = bTex - tTex;
		if (as == 0)
		{
			if (bt == 1)
			{
				std::swap(sqrTex[texIndex], sqrTex[texIndex - 5]);
				texIndex = -1;
				++moves;
			}
			else if (bt == -1)
			{
				std::swap(sqrTex[texIndex], sqrTex[texIndex + 5]);
				texIndex = -1;
				++moves;
			}
		}
		else if (bt == 0)
		{
			if (as == 1)
			{
				std::swap(sqrTex[texIndex], sqrTex[texIndex - 1]);
				texIndex = -1;
				++moves;
			}
			else if (as == -1)
			{
				std::swap(sqrTex[texIndex], sqrTex[texIndex + 1]);
				texIndex = -1;
				++moves;
			}
		}
		if (sqrTex == orgTex)
		{
			glfwSetMouseButtonCallback(window,NULL);
			TotalTime = (int)glfwGetTime() - StartTime;
			finish = true;
		}
	}
	oldMouseState = newMouseState;
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstTime)
	{
		lastX = xPos;
		lastY = yPos;
		firstTime = false;
	}
	lastX = xPos;
	lastY = yPos;
}
