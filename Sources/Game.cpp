//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include "PerlinNoise.hpp"
#include "Engine/Shader.h"
#include "Engine/Buffers.h"
#include "Engine/VertexLayout.h"
#include "Engine/Texture.h"
#include "Engine/DefaultResources.h"
#include "Minicraft/World.h"
#include "Minicraft/Player.h"
#include "Minicraft/Utils.h"
#include "Engine/Light.h"
#include "Minicraft/Skybox.h"
#include "string"
#include "iostream"


extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

// Global stuff
DefaultResources gpuResources;
Shader basicShader(L"Basic");
Shader blockShader(L"Block");
Shader skyboxShader(L"Skybox");
VertexBuffer<VertexLayout_PositionColor> crosshairLine;

Texture texture(L"terrain");
Texture textureSky(L"skybox");
World world;
Player player(&world, Vector3(16, 32, 16));
OrthographicCamera hudCamera(400, 600);

Light light;
Skybox skybox;

bool showGUI = true;
int seed = 786768768876;
float treeThreshold = 0.4f;
char filenameBuf[50] = "Coast";
std::vector<const char*> maps = {"Coast","River","Mountain","Delta", "Islands","Channel","Extreme" };

/// <summary>
/// Creates a new game
/// </summary>
Game::Game() noexcept(false) {
	m_deviceResources = std::make_unique<DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM_SRGB, DXGI_FORMAT_D32_FLOAT, 2);
	m_deviceResources->RegisterDeviceNotify(this);
}

/// <summary>
/// Destroys the game
/// </summary>
Game::~Game() {
	g_inputLayouts.clear();
}

/// <summary>
/// Initialize the game
/// </summary>
/// <param name="window">The game'window</param>
/// <param name="width">The window's width</param>
/// <param name="height">The window's height</param>
void Game::Initialize(HWND window, int width, int height) {
	// Create input devices
	m_gamePad = std::make_unique<GamePad>();
	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(window);
	m_mouse->SetMode(Mouse::MODE_ABSOLUTE);

	// Initialize the Direct3D resources
	m_deviceResources->SetWindow(window, width, height);
	m_deviceResources->CreateDeviceResources();
	m_deviceResources->CreateWindowSizeDependentResources();

	basicShader.Create(m_deviceResources.get());
	blockShader.Create(m_deviceResources.get());
	skyboxShader.Create(m_deviceResources.get());
	GenerateInputLayout<VertexLayout_PositionColor>(m_deviceResources.get(), &basicShader);
	//GenerateInputLayout<VertexLayout_PositionNormalUV>(m_deviceResources.get(), &blockShader);
	GenerateInputLayout<VertexLayout_PositionNormalUVInstanced>(m_deviceResources.get(), &blockShader);
	
	// Initialize textures
	texture.Create(m_deviceResources.get());
	textureSky.Create(m_deviceResources.get());

	// Initialize GPU resources
	gpuResources.Create(m_deviceResources.get());

	// Initialize player & cameras
	player.GenerateGPUResources(m_deviceResources.get());
	player.GetCamera()->UpdateAspectRatio((float)width / (float)height);
	hudCamera.UpdateSize((float)width, (float)height);

	// Initialize world
	light.Generate(m_deviceResources.get());
	//world.Generate(m_deviceResources.get(),786768768876,treeThreshold);
	world.GenerateFromFile(m_deviceResources.get(), L"Coast", treeThreshold);
	skybox.Generate(m_deviceResources.get());

	// Initialize crossahir for the GUI
	crosshairLine.PushVertex({ {-7, 0, 1, 1}, {1, 1, 1, 1} });
	crosshairLine.PushVertex({ {6, 0, 1, 1}, {1, 1, 1, 1} });
	crosshairLine.PushVertex({ {0, -6, 1, 1}, {1, 1, 1, 1} });
	crosshairLine.PushVertex({ {0, 7, 1, 1}, {1, 1, 1, 1} });

	crosshairLine.Create(m_deviceResources.get());


	// Initialize ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(m_deviceResources.get()->GetD3DDevice(), m_deviceResources.get()->GetD3DDeviceContext());
}

/// <summary>
/// Updates the game, then renders it
/// </summary>
void Game::Tick() {
	// DX::StepTimer will compute the elapsed time and call Update() for us
	// We pass Update as a callback to Tick() because StepTimer can be set to a "fixed time" step mode, allowing us to call Update multiple time in a row if the framerate is too low (useful for physics stuffs)
	m_timer.Tick([&]() { Update(m_timer); });

	Render(m_timer);
}

/// <summary>
/// Updates the game's
/// </summary>
/// <param name="timer">The game's timer</param>
void Game::Update(DX::StepTimer const& timer) {

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	auto const kb = m_keyboard->GetState();
	auto const ms = m_mouse->GetState();
	m_mouse->ResetScrollWheelValue();
	
	player.Update(timer.GetElapsedSeconds(), kb, ms);

	if (kb.Escape)
		ExitGame();

	Im(timer);

	auto const pad = m_gamePad->GetState(0);
}

/// <summary>
/// Draws the scene
/// </summary>
/// <param name="timer">The game's timer</param>
void Game::Render(DX::StepTimer const& timer) {
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
		return;

	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();
	auto const viewport = m_deviceResources->GetScreenViewport();

	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->RSSetViewports(1, &viewport);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);
	
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	//ApplyInputLayout<VertexLayout_PositionNormalUV>(m_deviceResources.get());
	ApplyInputLayout<VertexLayout_PositionNormalUVInstanced>(m_deviceResources.get());

	// Draw Skybox

	skyboxShader.Apply(m_deviceResources.get());

	player.GetCamera()->ApplyCamera(m_deviceResources.get());
	textureSky.Apply(m_deviceResources.get());

	skybox.Draw(m_deviceResources.get());
	
	// Draw World

	player.GetCamera()->ApplyCamera(m_deviceResources.get());
	light.Apply(m_deviceResources.get());

	blockShader.Apply(m_deviceResources.get());
	texture.Apply(m_deviceResources.get());
	world.Draw(player.GetCamera(), m_deviceResources.get());
	player.Draw(m_deviceResources.get());
	
	// Draw buildings

	ApplyInputLayout<VertexLayout_PositionNormalUVInstanced>(m_deviceResources.get());
	world.DrawBuildings(player.GetCamera(), m_deviceResources.get());

	// Draw UI

	//ApplyInputLayout<VertexLayout_PositionNormalUV>(m_deviceResources.get());

	hudCamera.ApplyCamera(m_deviceResources.get());
	ApplyInputLayout<VertexLayout_PositionColor>(m_deviceResources.get());
	basicShader.Apply(m_deviceResources.get());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	crosshairLine.Apply(m_deviceResources.get());
	context->Draw(4, 0);

	// Render All
	m_deviceResources->Present();
	
	// Render ImGui
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
}


#pragma region Message Handlers
void Game::OnActivated() {}

void Game::OnDeactivated() {}

void Game::OnSuspending() {}

void Game::OnResuming() {
	m_timer.ResetElapsedTime();
}

void Game::OnWindowMoved() {
	auto const r = m_deviceResources->GetOutputSize();
	m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange() {
	m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height) {
	if (!m_deviceResources->WindowSizeChanged(width, height))
		return;

	// The windows size has changed:
	// We can realloc here any resources that depends on the target resolution (post processing etc)
	player.GetCamera()->UpdateAspectRatio((float)width / (float)height);
	hudCamera.UpdateSize((float)width, (float)height);
}

void Game::OnDeviceLost() {
	// We have lost the graphics card, we should reset resources [TODO]
}

void Game::OnDeviceRestored() {
	// We have a new graphics card context, we should realloc resources [TODO]
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept {
	width = 800;
	height = 600;
}

#pragma endregion


/// <summary>
/// Represents the ImGui UI
/// </summary>
/// <param name="timer"></param>
void Game::Im(DX::StepTimer const& timer)
{
	bool open = true;
	ImGui::Begin("GUI", &open);
	ImGui::SetWindowSize(ImVec2(500, 600));

	if (ImGui::CollapsingHeader("General")) {
		ImGui::Text("FPS : ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(timer.GetFramesPerSecond()).c_str());

		ImGui::Text("Tree threshold : ");
		ImGui::SameLine();

		ImGui::InputFloat("   ", &treeThreshold);

		ImGui::Text("Seed : ");
		ImGui::SameLine();

		ImGui::InputInt(" ", &seed);
		if (ImGui::Button("Generate from seed")) {
			world.Generate(m_deviceResources.get(), seed, treeThreshold);
			player.Reset();
		}

		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("Filename : ");
		ImGui::SameLine();
		ImGui::InputText("  ", filenameBuf, 50);
		if (ImGui::Button("Generate from file")) {
			const size_t cSize = strlen(filenameBuf) + 1;
			wchar_t* wc = new wchar_t[cSize];
			mbstowcs(wc, filenameBuf, cSize);

			world.GenerateFromFile(m_deviceResources.get(), wc, treeThreshold);
			player.Reset();
		}

		ImGui::Spacing();
		ImGui::Spacing();

		for (int i = 0; i < maps.size(); i++) {
			ImGui::PushID(i);
			if (ImGui::Button(maps.at(i))) {
				const size_t cSize = strlen(maps.at(i)) + 1;
				wchar_t* wc = new wchar_t[cSize];
				mbstowcs(wc, maps.at(i), cSize);

				world.GenerateFromFile(m_deviceResources.get(), wc, treeThreshold);
				player.Reset();
			}
			ImGui::PopID();
			if (i != maps.size() - 1) {
				ImGui::SameLine();
			}
		}


	}
	if (ImGui::CollapsingHeader("Controls")) {
		ImGui::Text("Left Click : Build (If you have enough money)");
		ImGui::Text("Build your city.");
		ImGui::Text("Money is earned with taxes");
		ImGui::Text("Taxes are divided by 2 if there is not enough energy and water.");
		ImGui::Text("Buildings give taxes only when near a road.");
		ImGui::Text("Water plant must be built near a water source.");
	}

	player.Im(timer);
	ImGui::End();
}