#pragma once
#include "Camera.h"
#include "Light.h"
#include "Entity.h"

struct SceneSettings
{
	Color skyColor;
	unsigned	shadowSamples = 3;
	unsigned	dofSamples = 2;
	unsigned	bounces = 5;
};

class Scene
{

	private:
	unsigned m_CurrentCamera = 0;

	std::vector<Camera> m_SceneCameras;
	std::vector<Light> m_SceneLights;
	std::vector<Entity*> m_SceneEntities;

	public:
	SceneSettings settings;

	Scene( Color m_SkyColor, unsigned m_ShadowSamples = 3,
		unsigned	m_DOFSamples = 2, unsigned m_Bounces = 5 )
	{
		settings.skyColor = m_SkyColor; settings.skyColor.Normalize();
		m_CurrentCamera = 0;
		settings.shadowSamples = m_ShadowSamples;
		settings.dofSamples = m_DOFSamples;
		settings.bounces = m_Bounces;
	}

	Scene( SceneSettings settings ) :settings( settings )
	{
	};

	void AddLight( const Light & light );
	void AddCamera( const Camera & camera, const bool setThisAsCurrentCam = false );
	void AddEntity( Entity* entity );

	const std::vector<Light>& GetLights() const;
	const std::vector<Entity*>& GetEntities() const;

	void NextCamera();
	void PrevCamera();

	const Color& GetSkyColor()const;
	Camera* GetCurrentCamera();
	unsigned GetCurrentCameraIndex();
	unsigned GetShadowSamples()const;
	unsigned GetDOFSamples()const;
	unsigned GetBounces()const;
	bool GetDoFEnabled()const;

	~Scene()
	{
		for (auto a : m_SceneEntities)
		{
			delete a;
			a = nullptr;
		}
		m_SceneEntities.clear();
	}
};