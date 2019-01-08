#include "Scene.h"

Camera* Scene::GetCurrentCamera()
{
	return &m_SceneCameras[m_CurrentCamera];
}

unsigned Scene::GetCurrentCameraIndex()
{
	return m_CurrentCamera;
}


void Scene::AddLight( const Light & light )
{
	m_SceneLights.push_back( light );
}

void Scene::AddCamera( const Camera & camera, const bool setThisAsCurrentCam )
{
	m_SceneCameras.push_back( camera );

	if (setThisAsCurrentCam)
	{
		m_CurrentCamera = m_SceneCameras.size() - 1;
	}
	m_SceneCameras.back().Rotate( 0, 0 );
}

void Scene::NextCamera()
{
	++m_CurrentCamera;
	m_CurrentCamera %= m_SceneCameras.size();
}

void Scene::PrevCamera()
{
	if (m_CurrentCamera == 0)
	{
		m_CurrentCamera = m_SceneCameras.size() - 1;
		return;
	}
	--m_CurrentCamera;
}

void Scene::AddEntity( Entity * entity )
{
	m_SceneEntities.push_back( entity );
}

const std::vector<Light>& Scene::GetLights()const
{
	return m_SceneLights;
}

const std::vector<Entity*>& Scene::GetEntities() const
{
	return m_SceneEntities;
}

const Color & Scene::GetSkyColor()const
{
	return settings.skyColor;
}

unsigned Scene::GetShadowSamples() const
{
	return settings.shadowSamples;
}

unsigned Scene::GetDOFSamples() const
{
	return settings.dofSamples;
}

unsigned Scene::GetBounces() const
{
	return settings.bounces;
}

bool Scene::GetDoFEnabled() const
{
	return m_SceneCameras[m_CurrentCamera].enableDof;
}

