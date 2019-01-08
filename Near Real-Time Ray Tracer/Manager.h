#pragma once
#include <vector>
#include "Scene.h"
#include <future>
#include <functional>

class Manager
{
public:
	unsigned m_ScreenW = 100,
		m_ScreenH = 100,
		m_PixelCountW = 100,
		m_PixelCountH = 100,
		m_PixelCount = 1000,
		m_TextPointSize = 20;
	static unsigned m_CurrentScene;
	double deltaTime = 0;

	//vector holds future of async, no return value but keeps the asyncRenderLambdas alive
	std::vector<std::future<void>> future_vector;

	std::function<void(unsigned, unsigned, unsigned)> asyncRenderLambda;

	static std::vector<Scene*> m_Scenes;

	Manager(std::vector<unsigned char> &pixelBuffer,
		unsigned m_ScreenW = 100,
		unsigned m_ScreenH = 100,
		unsigned m_PixelCountW = 100,
		unsigned m_PixelCountH = 100,
		unsigned m_TextPointSize = 20);

	~Manager()
	{
		//m_Scenes go out of scape in main
	}

	void AddScene(Scene* scene, bool setThisAsCurrentScene = false);
	void SetScene(const unsigned indexOfScene);
	void NextScene();
	void PrevScene();
	void RenderScene();

	static Scene* GetCurrentScene();

private:
	bool offsetPixelRaster = false;
};
