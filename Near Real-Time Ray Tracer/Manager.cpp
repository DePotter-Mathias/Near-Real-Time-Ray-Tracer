#define NOMINMAX
#include <Windows.h>
#include <iostream>
#include <sstream>

#include "Manager.h"
#include "ShaderFunctions.h"

//defined in main.h
extern const unsigned int Cores;

std::vector<Scene*> Manager::m_Scenes = std::vector<Scene*>();
unsigned Manager::m_CurrentScene = 0;

Manager::Manager(
	std::vector<unsigned char>& pixelBuffer,
	unsigned m_ScreenW,
	unsigned m_ScreenH,
	unsigned m_PixelCountW,
	unsigned m_PixelCountH,
	unsigned m_TextPointSize) :
	m_ScreenW(m_ScreenW),
	m_ScreenH(m_ScreenH),
	m_PixelCountW(m_PixelCountW),
	m_PixelCountH(m_PixelCountH),
	m_PixelCount(m_PixelCountW*m_PixelCountH),
	m_TextPointSize(m_TextPointSize)
{
	const auto PixelCount = m_PixelCount;
	const auto Ratio = m_ScreenH / (double)m_ScreenW;

	asyncRenderLambda = [&pixelBuffer, Ratio, PixelCount](unsigned offset, bool offsetPixelRaster, unsigned cores) {

		Scene* scene = GetCurrentScene();
		Camera* cam = scene->GetCurrentCamera();

		for (size_t index(offset); index < PixelCount * Ratio + offsetPixelRaster; index += cores)
		{
			if (cam->enableDof)
			{
				if (fastrand() < halfInt*1.5) continue;
			}
			else
			{
				if (fastrand() < halfInt/2) continue;
			}

			size_t offsetIndex = index;
			//size_t offsetIndex = fmin(PixelCount - 1, index * 2 + oneMore);


			Vec ray = Rotate((*Camera::GetScreenRays())[offsetIndex], cam->rotMatrix);

			Color color;
			if (cam->enableDof)
			{
				auto DOFSamples = scene->GetDOFSamples();
				for (size_t i = 0; i < DOFSamples; ++i)
				{
					//jitter camera rays for Dof
					auto rvec = randVecSigned.ComponentMultiply(cam->dir + 1)*-0.02;

					color += RecursiveShading(cam->pos + rvec, (ray - rvec / cam->focalPoint).Normalized(), *scene, scene->GetBounces(), scene->GetShadowSamples());
				}
				color /= DOFSamples;
			}
			else
			{
				color = RecursiveShading(cam->pos, ray, *scene, 2, 3);
			}

			const double gamma = 1.3;
			color.x = fastPow(color.x, gamma);
			color.y = fastPow(color.y, gamma);
			color.z = fastPow(color.z, gamma);
			color = color.SaturateTop();

			offsetIndex *= 4;
			pixelBuffer[offsetIndex + 0] = (unsigned char)(color.x * 255);//r
			pixelBuffer[offsetIndex + 1] = (unsigned char)(color.y * 255);//g
			pixelBuffer[offsetIndex + 2] = (unsigned char)(color.z * 255);//b
			pixelBuffer[offsetIndex + 3] = (unsigned char)(cam->enableDof ? 63 / std::max(fastPow(cam->timeNotMoved*0.7, 2.0), 1.0) + 12 : 255-12);//alpha
		}
	};
}

void Manager::AddScene(Scene* scene, bool setThisAsCurrentScene)
{
	m_Scenes.emplace_back(scene);
	if (setThisAsCurrentScene)
	{
		m_CurrentScene = m_Scenes.size() - 1;
	}
}

void Manager::SetScene(const unsigned indexOfScene)
{
	m_CurrentScene = Clamp(indexOfScene, m_Scenes.size() - 1, 0u);
}

void Manager::NextScene()
{
	++m_CurrentScene;
	m_CurrentScene %= m_Scenes.size();
}

void Manager::PrevScene()
{
	if (m_CurrentScene == 0)
		m_CurrentScene = m_Scenes.size() - 1;
	else
		--m_CurrentScene;
}

void Manager::RenderScene()
{
	unsigned index = Cores;
	const unsigned coresIndex = Cores;

	//while stops when index hits zero
	while (index)
		//lambda functions passed with parameters and passed as args to async
		future_vector.emplace_back(
			std::async(asyncRenderLambda, --index, offsetPixelRaster, coresIndex)
		);

	//for (const auto& t : future_vector) t.wait();
	offsetPixelRaster = !offsetPixelRaster;

	future_vector.clear();
}

Scene* Manager::GetCurrentScene()
{
	return m_Scenes[m_CurrentScene];
}



