#pragma once

#include "Base/Application.h"
#include "Base/UIFontManager.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>
#include <string>

static void Log(const char *log)
{
	std::cout << log << std::endl;
};

static void InitGlad()
{
	if (!gladLoadGL())
	{
		Log("Failed to Initialize GLAD!");
		exit(-1);
	}
}

static void InitImGui(std::string &configPath)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	io.IniFilename = configPath.c_str();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#ifdef TERR3D_WIN32 // Multiviewport is not supported stable on linux
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
#endif
	io.ConfigViewportsNoTaskBarIcon = false;
	io.ConfigViewportsNoAutoMerge = true;
	ImGui::StyleColorsDark();
	ImGuiStyle &style = ImGui::GetStyle();

	float uiScale = 1.0f;
	if (const char* envScale = std::getenv("TF3D_UI_SCALE"))
	{
		uiScale = std::strtof(envScale, nullptr);
	}
	else
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get()->GetWindow()->GetNativeWindow());
		float xscale = 1.0f;
		float yscale = 1.0f;
		glfwGetWindowContentScale(window, &xscale, &yscale);
		uiScale = (xscale > yscale) ? xscale : yscale;
	}
	if (uiScale < 1.0f) uiScale = 1.0f;
	style.ScaleAllSizes(uiScale);
	SetUIFontScale(uiScale);

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow *>(Application::Get()->GetWindow()->GetNativeWindow()), true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

static void ImGuiShutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

Application *Application::s_App;



Application::Application()
{
	isActive = false;
	m_Window = nullptr;
	previousTime = 0.0f;
}


void Application::SetWindowConfigPath(std::string path)
{
	windowConfigPath = path;
}

void Application::SetLogsDir(std::string ld)
{
	logsDir = ld;
}

void Application::SetTitle(std::string title)
{
	m_WindowTitle = title;
}

void Application::Init()
{
	m_Window = new Window(m_WindowTitle);
	m_Window->SetVSync(false);
	isActive = true;
	s_App = this;
	InitGlad();
	InitImGui(windowConfigPath);
	m_Window->SetVisible(true);
}

void Application::Render()
{
}

void Application::ImGuiRenderBegin()
{
	ImGuiIO &io = ImGui::GetIO();
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();
}

void Application::ImGuiRenderEnd()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	ImGuiIO &io = ImGui::GetIO();

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow *backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

bool Application::IsActive()
{
	return isActive;
}

void Application::RenderImGui()
{
	glEnable(GL_BLEND);
	ImGuiRenderBegin();
	OnImGuiRender();
	ImGuiRenderEnd();
	glDisable(GL_BLEND);
}

void Application::Run(std::string loadFile)
{

	float oneSecCounter = 0;

	while (isActive)
	{
		float currentTime = (float)glfwGetTime();
		float deltaTime = currentTime - previousTime;
		previousTime = currentTime;
		oneSecCounter += deltaTime;
		Log("DEBUG_RUN: Before OnUpdate()");
		OnUpdate(deltaTime);
		Log("DEBUG_RUN: After OnUpdate()");

		if (oneSecCounter >= 1)
		{
			OnOneSecondTick();
			oneSecCounter = 0;
		}

		Log("DEBUG_RUN: Before Render()");
		Render();
		Log("DEBUG_RUN: After Render()");
		Log("DEBUG_RUN: Before m_Window->Update()");
		m_Window->Update();
		Log("DEBUG_RUN: After m_Window->Update()");
	}

	OnEnd();
	ImGuiShutdown();
}


Application::~Application()
{
	std::cout << "Shutting down Application" << std::endl;
	delete m_Window;
}
