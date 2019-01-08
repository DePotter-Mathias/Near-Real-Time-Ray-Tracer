//#define SDL_MAIN_HANDLED
#include "main.h"

int main(int argc, char *argv[])
{
	(void)argc, (void)argv;
	//_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc( 186 );

#pragma region INIT

	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF);

	std::vector< unsigned char > pixels;

	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);

	const double W = DM.w / 2, H = DM.w / 2 * (10 / 16.0), Scale = 1300;
	const unsigned smallestScreenAxis = std::min(W, H);

	Manager sceneManager(pixels, W, H, Scale, Scale, 21);
	pixels = std::vector< unsigned char >(sceneManager.m_PixelCount * 4, 0);

	sceneManager.m_TextPointSize = std::max(smallestScreenAxis*0.015, 6.0);

	SDL_Renderer* renderer;
	SDL_Window* window;
	SDL_CreateWindowAndRenderer(sceneManager.m_ScreenW, sceneManager.m_ScreenH, SDL_WINDOW_OPENGL, &window, &renderer);
	SDL_SetWindowTitle(window, "Near Real-Time Ray Tracer");

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

	SDL_Color textColor = { 255, 255, 255, 255 };
	SDL_Color textBgColor = { 20, 20, 30, 255 };
	TTF_Wrapper statManager(renderer, std::string("Font/Asimov.otf").c_str(), sceneManager.m_TextPointSize, sceneManager.m_TextPointSize*1.2, 0, 0, textColor, textBgColor);

	//creating the main texture
	SDL_Texture* texture = SDL_CreateTexture
	(renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		Scale, Scale* (H / W));

	//alpha used to blend next update to texture, per pixel
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
#pragma endregion

#pragma region Creating scenes

	//define and fill scene(s)
	//colors
	const Color White = Color(1, 1, 1);
	const Color Red = Color(0, 0, 1);
	const Color Green = Color(0, 1, 0);
	const Color Blue = Color(1, 0, 0);
	const Color Blue_Sky = (Blue * 18 + White * 20 + Green * 7).Normalized();

	//textures
	Texture FloorTilesTexture("\Textures/FloorTiles_512_diffuse.tiff", "\Textures/FloorTiles_512_normal.tiff", 5, 10);
	Texture CobblestoneFloorTexture("\Textures/CobblestoneFloor_512_diffuse.tiff", "\Textures/CobblestoneFloor_1024_normal.tiff", 6, 20);

#pragma region Scene 1
	Scene scene1(Blue_Sky, 3, 2, 4);

	scene1.AddEntity(new Sphere(Vec(-2, 1, 2), 1, Material(Blue, 0.7, 0, 5, 0.7, 0, false, Air)));

	scene1.AddEntity(new Sphere(Vec(0, 0.7, 2), 0.5, Material(Green, 0.7, 0, 10, 0.7, 0, false, Air)));
	scene1.AddEntity(new Sphere(Vec(-2, 1.5, 2.8), 0.2, Material(Red, 0.9, 0, 5, 0.5, 0, false, Air)));
	scene1.AddEntity(new Sphere(Vec(2, 1, 2), 1, Material(Red * 0, 0.5, 0, 130, 0.9, 0.5, false, Air)));
	scene1.AddEntity(new Sphere(Vec(2, 1, 6.1), 1, Material(Red * 0, 0, 0, 130, 0.9, 0.5, false, Air)));

	scene1.AddEntity(new Plane(Vec(0, 1, 0), 0, Material(FloorTilesTexture, 1, 0, 30, 0.3, 0, true)));

	scene1.AddLight(Light(Vec(-2, 6, 4), 1, White));

	scene1.AddCamera(Camera(Vec(0, 3, -2), Vec(1.5, 0, 2), sceneManager.m_PixelCountW, sceneManager.m_PixelCountH, 70), true);
	scene1.AddCamera(Camera(Vec(-4.6, 1.62, -0.3), Vec(0, 1, 0), sceneManager.m_PixelCountW, sceneManager.m_PixelCountH, 70));
	scene1.AddCamera(Camera(Vec(-4, 4, 4.6), Vec(2, 2, 2), sceneManager.m_PixelCountW, sceneManager.m_PixelCountH, 70));

	sceneManager.AddScene(&scene1, true);
#pragma endregion Scene 1

#pragma region Scene 2
	Scene scene2(White * 5 + Blue, 13, 1, 5);

	scene2.AddEntity(new Sphere(Vec(0, 1, 2), 1, Material(Blue, 1, 0, 5, 0.7)));
	scene2.AddEntity(new Sphere(Vec(2, 1, 0), 1, Material(Color() + 1, 0.08, 0.5, 100, 1, 0.3, false, Glass)));
	scene2.AddEntity(new Sphere(Vec(0, 3, 0), 0.5, Material(Red, 0.9, 0, 5, 0.5, 0, false, Air)));
	scene2.AddEntity(new Sphere(Vec(-3, 0.1, 2), 2, Material(Blue * 3 + Red + White, 0.3, 0.0, 20, 30, 0.7, false, Glass)));

	scene2.AddEntity(new Plane(Vec(0, 1, 0), 0, Material(CobblestoneFloorTexture, 1, 0, 20, 0.4, 0, true, MaterialsType::Water)));

	scene2.AddLight(Light(Vec(-2, 10, 4), 2, Blue));
	scene2.AddLight(Light(Vec(1, 5, -5), 2, Red));
	scene2.AddLight(Light(Vec(3, 10, -4), 2, Green));

	scene2.AddCamera(Camera(Vec(0, 3.15, -4.85), Vec(1, 0, 2), sceneManager.m_PixelCountW, sceneManager.m_PixelCountH, 80));
	scene2.AddCamera(Camera(Vec(-6.33, 1.62, -2.43), Vec(0.5, 5, 2).Normalized(), sceneManager.m_PixelCountW, sceneManager.m_PixelCountH, 70), true);

	sceneManager.AddScene(&scene2);
#pragma endregion Scene 2

#pragma endregion
	Camera * pCurrentCam = sceneManager.GetCurrentScene()->GetCurrentCamera();

	SDL_Event * event = new SDL_Event();
	//int *keyCountPressed = new int(0);
	const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);

	Uint64 NOW = 0;
	Uint64 LAST = 0;

	Vec camVel;
	int mouseOldX = 0, mouseOldY = 0;

	LAST = NOW = SDL_GetPerformanceCounter();

	double elapsedTime = 0;
	double accumulation = 0;
	double& deltaTime = sceneManager.deltaTime;

	bool mouseDownLastFrame = false;
	bool KeyDownLastFrame = false;
	bool hideInfo = false;
	bool running = true;

	while (running)
	{
		NOW = SDL_GetPerformanceCounter();

		deltaTime = (NOW - LAST) / (double)SDL_GetPerformanceFrequency();
		elapsedTime += deltaTime;
		accumulation += deltaTime;
		pCurrentCam->timeNotMoved += deltaTime;
		LAST = NOW;

		//lock fps for input and "physics"
		if (accumulation > 1 / 45.0)
		{
			while (SDL_PollEvent(event))
			{
				if (event->type == SDL_QUIT)
				{
					running = false;
					break;
				}
				if (event->type == SDL_KEYDOWN)
				{
					pCurrentCam->Moved();
				}
				if (event->type == SDL_MOUSEWHEEL)
				{
					pCurrentCam->focalPoint += event->wheel.y * 0.5;
					pCurrentCam->focalPoint = std::max(pCurrentCam->focalPoint, 1.0);
					pCurrentCam->Moved();
				}
			}

			//if not visible to user stop inputs and updating; 
			//note rendering is still happening
			if (SDL_WINDOW_SHOWN == SDL_GetWindowFlags(window))
				continue;

			//SDL_PumpEvents();//not needed SDL_PollEvent internal call

			int  x, y;
			if (SDL_GetMouseState(&y, &x) & SDL_BUTTON(1))
			{
				pCurrentCam->Moved();

				if (mouseDownLastFrame)
				{
					int diffX = x - mouseOldX, diffY = y - mouseOldY;
					if (mouseDownLastFrame && (diffX != 0 && diffY != 0))
					{
						double d = 7e-4;
						pCurrentCam->Rotate(diffX * d, diffY * d);
					}
				}
				else
				{
					mouseOldX = x;
					mouseOldY = y;
					mouseDownLastFrame = true;
				}
			}
			else
			{
				mouseDownLastFrame = false;
			}

			//change scene
			if (keyboardState[SDL_SCANCODE_KP_PLUS] || keyboardState[SDL_SCANCODE_EQUALS])
			{
				sceneManager.NextScene();
				pCurrentCam = sceneManager.GetCurrentScene()->GetCurrentCamera();
			}
			if (keyboardState[SDL_SCANCODE_KP_MINUS] || keyboardState[SDL_SCANCODE_MINUS])
			{
				sceneManager.PrevScene();
				pCurrentCam = sceneManager.GetCurrentScene()->GetCurrentCamera();
			}

			//change cam
			if (keyboardState[SDL_SCANCODE_COMMA])
			{
				sceneManager.GetCurrentScene()->NextCamera();
				pCurrentCam = sceneManager.GetCurrentScene()->GetCurrentCamera();
			}
			if (keyboardState[SDL_SCANCODE_PERIOD])
			{
				sceneManager.GetCurrentScene()->PrevCamera();
				pCurrentCam = sceneManager.GetCurrentScene()->GetCurrentCamera();
			}

			//clear screen
			if (keyboardState[SDL_SCANCODE_F5])
			{
				//memcpy( &pixels.begin(), 0, pixels.size() );
				for (size_t i = 0; i < pixels.size(); i++)
					pixels[i] = 0;

				SDL_UpdateTexture(texture, NULL, &pixels[0], sceneManager.m_PixelCountW);
				SDL_RenderCopy(renderer, texture, NULL, NULL);
				SDL_RenderPresent(renderer);
				continue;
			}

			if (keyboardState[SDL_SCANCODE_Z])
			{
				pCurrentCam->enableDof = true;
			}
			if (keyboardState[SDL_SCANCODE_C])
			{
				pCurrentCam->enableDof = false;
			}

			if (keyboardState[SDL_SCANCODE_TAB])
			{
				hideInfo = !hideInfo;
			}

			//reset camera
			if (keyboardState[SDL_SCANCODE_ESCAPE])
			{
				camVel = { 0,0,0 };
				pCurrentCam->pos = { 0, 0, 0 };
				pCurrentCam->focalPoint = 4;
				pCurrentCam->dir = Vec(0, 0, 1);

				pCurrentCam->ResetRotationMatrix();
				pCurrentCam->Rotate(0, 0);
				pCurrentCam->dir = Vec(0, 0, 1);
				continue;
			}

			//Camera Movement
			Vec dir = pCurrentCam->dir;

			if (keyboardState[SDL_SCANCODE_W] || keyboardState[SDL_SCANCODE_UP])
			{
				camVel += dir;
			}
			if (keyboardState[SDL_SCANCODE_S] || keyboardState[SDL_SCANCODE_DOWN])
			{
				camVel -= dir;
			}
			if (keyboardState[SDL_SCANCODE_D] || keyboardState[SDL_SCANCODE_RIGHT])
			{
				camVel += Vec(dir.z, dir.y, -dir.x);
			}
			if (keyboardState[SDL_SCANCODE_A] || keyboardState[SDL_SCANCODE_LEFT])
			{
				camVel += Vec(-dir.z, dir.y, dir.x);
			}
			if (keyboardState[SDL_SCANCODE_E])
			{
				camVel += Vec(0, 1, 0);
			}
			if (keyboardState[SDL_SCANCODE_Q])
			{
				camVel += Vec(0, -1, 0);
			}

			pCurrentCam->pos += camVel * 0.075;
			camVel *= 0.5;

			//drawing pixel buffer
			SDL_UpdateTexture(texture, NULL, &pixels[0], sceneManager.m_PixelCountW * 4);
			SDL_RenderCopy(renderer, texture, NULL, NULL);

			//last chance for screen cap before text info is written
			if (keyboardState[SDL_SCANCODE_F1])//screenshot
			{
				//code taken from https://forums.libsdl.org/viewtopic.php?p=45496 posted by Naith
				SDL_Surface* pScreenShot = SDL_CreateRGBSurface(0, sceneManager.m_ScreenW, sceneManager.m_ScreenH, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
				if (pScreenShot)
				{
					SDL_RenderReadPixels(renderer, NULL, SDL_GetWindowPixelFormat(window), pScreenShot->pixels, pScreenShot->pitch);
					SDL_SaveBMP(pScreenShot, "Screenshot.bmp");
					SDL_FreeSurface(pScreenShot);
				}
			}

			//drawing text
			if (!hideInfo)
			{
				statManager.DrawInfo("FPS: " + std::to_string((int)std::min(1 / deltaTime, 120.0)));
				statManager.DrawInfo("");

				statManager.DrawInfo("Elapsed Time: " + std::to_string((int)elapsedTime));
				statManager.DrawInfo("");

				statManager.DrawInfo("Camera XYZ: " +
					std::to_string(pCurrentCam->pos.x) + ", " +
					std::to_string(pCurrentCam->pos.y) + ", " +
					std::to_string(pCurrentCam->pos.z));
				statManager.DrawInfo("");

				statManager.DrawInfo("Camera Rotation: " +
					std::to_string(pCurrentCam->dir.x) + ", " +
					std::to_string(pCurrentCam->dir.y) + ", " +
					std::to_string(pCurrentCam->dir.z));
				statManager.DrawInfo("");

				statManager.DrawInfo(std::string("| Q: down          E: up"));
				statManager.DrawInfo(std::string("| A: left              D: right"));
				statManager.DrawInfo(std::string("| W: forward    S: back"));
				statManager.DrawInfo("");

				statManager.DrawInfo(std::string("(Z or C)  Depth of Field: ") + (pCurrentCam->enableDof ? "On" : "Off"));
				statManager.DrawInfo(std::string("(Scroll)  Depth of Field Focal Point: ") + std::to_string(pCurrentCam->focalPoint));
				statManager.DrawInfo("");

				statManager.DrawInfo(std::string("(- or +)  Change Scene, Current Scene: ")
					+ std::to_string(sceneManager.m_CurrentScene));
				statManager.DrawInfo("");

				statManager.DrawInfo(std::string("(< or >)  Change Camera, Current Camera: "
					+ std::to_string(sceneManager.GetCurrentScene()->GetCurrentCameraIndex())));
				statManager.DrawInfo("");

				statManager.DrawInfo("");
				statManager.DrawInfo("");
				statManager.DrawInfo("");
				statManager.DrawInfo("");
				statManager.DrawInfo("");
				statManager.DrawInfo("");
				statManager.DrawInfo(std::string("<Tab> To hide Info "));
				statManager.DrawInfo("");

				statManager.BackToTop();
			}

			SDL_RenderPresent(renderer);
			sceneManager.RenderScene();
			accumulation = 0;
		}
	}
	sceneManager.future_vector.clear();
	delete event;
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

