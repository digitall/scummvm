/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "backends/graphics/sdl/sdl-graphics.h"
#include "backends/platform/sdl/sdl-sys.h"
#include "backends/platform/sdl/sdl.h"
#include "backends/events/sdl/sdl-events.h"
#include "backends/keymapper/action.h"
#include "backends/keymapper/keymap.h"
#include "common/config-manager.h"
#include "common/fs.h"
#include "common/textconsole.h"
#include "common/translation.h"
#include "graphics/scaler/aspect.h"
#ifdef USE_OSD
#include "common/translation.h"
#endif

#ifdef EMSCRIPTEN
#include "backends/platform/sdl/emscripten/emscripten.h"
#endif

#if defined(USE_IMGUI) && SDL_VERSION_ATLEAST(3, 0, 0)
#include "backends/imgui/backends/imgui_impl_sdl3.h"
#ifdef USE_OPENGL
#include "backends/imgui/backends/imgui_impl_opengl3.h"
#endif
#ifdef USE_IMGUI_SDLRENDERER3
#include "backends/imgui/backends/imgui_impl_sdlrenderer3.h"
#endif
#elif defined(USE_IMGUI) && SDL_VERSION_ATLEAST(2, 0, 0)
#include "backends/imgui/backends/imgui_impl_sdl2.h"
#ifdef USE_OPENGL
#include "backends/imgui/backends/imgui_impl_opengl3.h"
#endif
#ifdef USE_IMGUI_SDLRENDERER2
#include "backends/imgui/backends/imgui_impl_sdlrenderer2.h"
#endif
#endif


static void getMouseState(int *x, int *y) {
#if SDL_VERSION_ATLEAST(3, 0, 0)
	float fx, fy;
	SDL_GetMouseState(&fx, &fy);
	*x = static_cast<int>(fx);
	*y = static_cast<int>(fy);
#else
	SDL_GetMouseState(x, y);
#endif
}

SdlGraphicsManager::SdlGraphicsManager(SdlEventSource *source, SdlWindow *window)
	: _eventSource(source), _window(window), _hwScreen(nullptr)
#if SDL_VERSION_ATLEAST(2, 0, 0)
	, _allowWindowSizeReset(false), _hintedWidth(0), _hintedHeight(0), _lastFlags(0)
#endif
{
	ConfMan.registerDefault("fullscreen_res", "desktop");

	getMouseState(&_cursorX, &_cursorY);
}

void SdlGraphicsManager::activateManager() {
	_eventSource->setGraphicsManager(this);

	// Register the graphics manager as a event observer
	g_system->getEventManager()->getEventDispatcher()->registerObserver(this, 10, false);
}

void SdlGraphicsManager::deactivateManager() {
	// Unregister the event observer
	if (g_system->getEventManager()->getEventDispatcher()) {
		g_system->getEventManager()->getEventDispatcher()->unregisterObserver(this);
	}

	_eventSource->setGraphicsManager(nullptr);
}

SdlGraphicsManager::State SdlGraphicsManager::getState() const {
	State state;

	state.screenWidth   = getWidth();
	state.screenHeight  = getHeight();
	state.aspectRatio   = getFeatureState(OSystem::kFeatureAspectRatioCorrection);
	state.fullscreen    = getFeatureState(OSystem::kFeatureFullscreenMode);
	state.cursorPalette = getFeatureState(OSystem::kFeatureCursorPalette);
	state.vsync         = getFeatureState(OSystem::kFeatureVSync);
	state.rotation      = _rotationMode;
#ifdef USE_RGB_COLOR
	state.pixelFormat   = getScreenFormat();
#endif
	return state;
}

bool SdlGraphicsManager::setState(const State &state) {
	beginGFXTransaction();
#ifdef USE_RGB_COLOR
		// When switching between the SDL and OpenGL graphics manager, the list
		// of supported format changes. This means that the pixel format in the
		// state may not be supported. In that case use the preferred supported
		// pixel format instead.
		Graphics::PixelFormat format = state.pixelFormat;
		Common::List<Graphics::PixelFormat> supportedFormats = getSupportedFormats();
		if (!supportedFormats.empty() && Common::find(supportedFormats.begin(), supportedFormats.end(), format) == supportedFormats.end())
			format = supportedFormats.front();
		initSize(state.screenWidth, state.screenHeight, &format);
#else
		initSize(state.screenWidth, state.screenHeight, nullptr);
#endif
		setFeatureState(OSystem::kFeatureAspectRatioCorrection, state.aspectRatio);
		setFeatureState(OSystem::kFeatureFullscreenMode, state.fullscreen);
		setFeatureState(OSystem::kFeatureCursorPalette, state.cursorPalette);
		setFeatureState(OSystem::kFeatureVSync, state.vsync);
		setRotationMode(state.rotation);

	if (endGFXTransaction() != OSystem::kTransactionSuccess) {
		return false;
	} else {
		return true;
	}
}

Common::Rect SdlGraphicsManager::getPreferredFullscreenResolution() {
	// Default to the desktop resolution, unless the user has set a
	// resolution in the configuration file
	const Common::String &fsres = ConfMan.get("fullscreen_res");
	if (fsres != "desktop") {
		uint newW, newH;
		int converted = sscanf(fsres.c_str(), "%ux%u", &newW, &newH);
		if (converted == 2) {
			return Common::Rect(newW, newH);
		} else {
			warning("Could not parse 'fullscreen_res' option: expected WWWxHHH, got %s", fsres.c_str());
		}
	}

	return _window->getDesktopResolution();
}

bool SdlGraphicsManager::defaultGraphicsModeConfig() const {
	const Common::ConfigManager::Domain *transientDomain = ConfMan.getDomain(Common::ConfigManager::kTransientDomain);
	if (transientDomain && transientDomain->contains("scaler")) {
		const Common::String &mode = transientDomain->getVal("scaler");
		if (!mode.equalsIgnoreCase("default")) {
			return false;
		}
	}

	const Common::ConfigManager::Domain *gameDomain = ConfMan.getActiveDomain();
	if (gameDomain && gameDomain->contains("scaler")) {
		const Common::String &mode = gameDomain->getVal("scaler");
		if (!mode.equalsIgnoreCase("default")) {
			return false;
		}
	}

	return true;
}

void SdlGraphicsManager::initSizeHint(const Graphics::ModeList &modes) {
#if SDL_VERSION_ATLEAST(2, 0, 0)
	const bool useDefault = defaultGraphicsModeConfig();

	// This gets called from engine before they call initGraphics(), which means we cannot use getScaleFactor()
	// because the scale factor in the backend has not yet been updated to use the game settings. So directly
	// read the game settings. This may be -1, which means we want to use the default. Fortunately runGame()
	// in main.cpp sets the gaphics mode (OpenGL or SurfaceSDL) before starting the engine. So we already have
	// the correct graphics manager and we can call getDefaultScaleFactor() here.
	int scale = ConfMan.getInt("scale_factor");
	if (scale == -1) {
		scale = getDefaultScaleFactor();
	}

	int16 bestWidth = 0, bestHeight = 0;
	const Graphics::ModeList::const_iterator end = modes.end();
	for (Graphics::ModeList::const_iterator it = modes.begin(); it != end; ++it) {
		int16 width = it->width, height = it->height;

		// TODO: Normalize AR correction by passing a PAR in the mode list
		// instead of checking the dimensions here like this, since not all
		// 320x200/640x400 uses are with non-square pixels (e.g. DreamWeb).
		if (ConfMan.getBool("aspect_ratio")) {
			if ((width == 320 && height == 200) || (width == 640 && height == 400)) {
				height = real2Aspect(height);
			}
		}

		if (!useDefault || width <= 320) {
			width *= scale;
			height *= scale;
		}

		if (bestWidth < width) {
			bestWidth = width;
		}

		if (bestHeight < height) {
			bestHeight = height;
		}
	}

	_hintedWidth = bestWidth;
	_hintedHeight = bestHeight;
#endif
}

bool SdlGraphicsManager::showMouse(bool visible) {
	if (visible == _cursorVisible) {
		return visible;
	}

	bool showCursor = false;
	if (visible) {
		// _cursorX and _cursorY are currently always clipped to the active
		// area, so we need to ask SDL where the system's mouse cursor is
		// instead
		int x, y;
		getMouseState(&x, &y);
		if (!_activeArea.drawRect.contains(Common::Point(x, y))) {
			showCursor = true;
		}
	}
	showSystemMouseCursor(showCursor);

	return WindowedGraphicsManager::showMouse(visible);
}

bool SdlGraphicsManager::lockMouse(bool lock) {
	return _window->lockMouse(lock);
}

bool SdlGraphicsManager::notifyMousePosition(Common::Point &mouse) {
	bool showCursor = false;
	// Currently on macOS we need to scale the events for HiDPI screen, but on
	// Windows we do not. We can find out if we need to do it by querying the
	// SDL window size vs the SDL drawable size.
	float dpiScale = _window->getSdlDpiScalingFactor();
	mouse.x = (int)(mouse.x * dpiScale + 0.5f);
	mouse.y = (int)(mouse.y * dpiScale + 0.5f);
	bool valid = true;
	if (_activeArea.drawRect.contains(mouse)) {
		_cursorLastInActiveArea = true;
	} else {
		// The right/bottom edges are not part of the drawRect. As the clipping
		// is done in drawable area coordinates, but the mouse position is set
		// in window coordinates, we need to subtract as many pixels from the
		// edges as corresponds to one pixel in the window coordinates.
		mouse.x = CLIP<int>(mouse.x, _activeArea.drawRect.left,
							_activeArea.drawRect.right - (int)(1 * dpiScale + 0.5f));
		mouse.y = CLIP<int>(mouse.y, _activeArea.drawRect.top,
							_activeArea.drawRect.bottom - (int)(1 * dpiScale + 0.5f));

		if (_window->mouseIsGrabbed() ||
			// Keep the mouse inside the game area during dragging to prevent an
			// event mismatch where the mouseup event gets lost because it is
			// performed outside of the game area
			(_cursorLastInActiveArea && SDL_GetMouseState(nullptr, nullptr) != 0)) {
			setSystemMousePosition(mouse.x, mouse.y);
		} else {
			// Allow the in-game mouse to get a final movement event to the edge
			// of the window if the mouse was moved out of the game area
			if (_cursorLastInActiveArea) {
				_cursorLastInActiveArea = false;
			} else if (_cursorVisible) {
				// Keep sending events to the game if the cursor is invisible,
				// since otherwise if a game lets you skip a cutscene by
				// clicking and the user moved the mouse outside the active
				// area, the clicks wouldn't do anything, which would be
				// confusing
				valid = false;
			}

			if (_cursorVisible) {
				showCursor = true;
			}
		}
	}

	showSystemMouseCursor(showCursor);
	if (valid) {
		setMousePosition(mouse.x, mouse.y);
		mouse = convertWindowToVirtual(mouse.x, mouse.y);
	}
	return valid;
}

void SdlGraphicsManager::showSystemMouseCursor(bool visible) {
#if SDL_VERSION_ATLEAST(3, 0, 0)
	if (visible) {
		SDL_ShowCursor();
	} else {
		SDL_HideCursor();
	}
#else
	SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE);
#endif
}

void SdlGraphicsManager::setSystemMousePosition(const int x, const int y) {
	assert(_window);
	if (!_window->warpMouseInWindow(x, y)) {
		const Common::Point mouse = convertWindowToVirtual(x, y);
		_eventSource->fakeWarpMouse(mouse.x, mouse.y);
	}
}

void SdlGraphicsManager::notifyActiveAreaChanged() {
	_window->setMouseRect(_activeArea.drawRect);
}

void SdlGraphicsManager::handleResizeImpl(const int width, const int height) {
	_forceRedraw = true;
}

#if SDL_VERSION_ATLEAST(2, 0, 0)
bool SdlGraphicsManager::createOrUpdateWindow(int width, int height, const Uint32 flags) {
	if (!_window) {
		return false;
	}

	// width *=3;
	// height *=3;

	// We only update the actual window when flags change (which usually means
	// fullscreen mode is entered/exited), when updates are forced so that we
	// do not reset the window size whenever a game makes a call to change the
	// size or pixel format of the internal game surface (since a user may have
	// resized the game window), or when the launcher is visible (since a user
	// may change the scaler, which should reset the window size)
	if (!_window->getSDLWindow() || _lastFlags != flags || _overlayVisible || _allowWindowSizeReset) {
#if SDL_VERSION_ATLEAST(3, 0, 0)
		const bool fullscreen = (flags & (SDL_WINDOW_FULLSCREEN)) != 0;
#else
		const bool fullscreen = (flags & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)) != 0;
#endif
		const bool maximized = (flags & SDL_WINDOW_MAXIMIZED);
		if (!fullscreen && !maximized) {
			if (_hintedWidth > width) {
				width = _hintedWidth;
			}
			if (_hintedHeight > height) {
				height = _hintedHeight;
			}
		}

		if (!_window->createOrUpdateWindow(width, height, flags)) {
			return false;
		}
#if SDL_VERSION_ATLEAST(3, 0, 0)
		if (fullscreen) {
			if (!SDL_SetWindowFullscreenMode(_window->getSDLWindow(), NULL))
				return false;
			if (!SDL_SyncWindow(_window->getSDLWindow()))
				return false;
		}
#endif

		_lastFlags = flags;
		_allowWindowSizeReset = false;
	}

	return true;
}
#endif

void SdlGraphicsManager::saveScreenshot() {
	Common::String filename;

	Common::Path screenshotsPath;
	OSystem_SDL *sdl_g_system = dynamic_cast<OSystem_SDL*>(g_system);
	if (sdl_g_system)
		screenshotsPath = sdl_g_system->getScreenshotsPath();

	// Use the name of the running target as a base for screenshot file names
	Common::String currentTarget = ConfMan.getActiveDomainName();

#ifdef USE_PNG
	const char *extension = "png";
#else
	const char *extension = "bmp";
#endif

	for (int n = 0;; n++) {
		filename = Common::String::format("scummvm%s%s-%05d.%s", currentTarget.empty() ? "" : "-",
		                                  currentTarget.c_str(), n, extension);

		Common::FSNode file = Common::FSNode(screenshotsPath.appendComponent(filename));
		if (!file.exists()) {
			break;
		}
	}

	if (saveScreenshot(screenshotsPath.appendComponent(filename))) {
		if (screenshotsPath.empty())
			debug("Saved screenshot '%s' in current directory", filename.c_str());
		else
			debug("Saved screenshot '%s' in directory '%s'", filename.c_str(),
					screenshotsPath.toString(Common::Path::kNativeSeparator).c_str());

#ifdef USE_OSD
		if (!ConfMan.getBool("disable_saved_screenshot_osd"))
			displayMessageOnOSD(Common::U32String::format(_("Saved screenshot '%s'"), filename.c_str()));
#endif

#ifdef EMSCRIPTEN
		// Users can't access the virtual emscripten filesystem in the browser, so we export the generated screenshot file via OSystem_Emscripten::exportFile.
		OSystem_Emscripten *emscripten_g_system = dynamic_cast<OSystem_Emscripten*>(g_system);
		emscripten_g_system->exportFile(screenshotsPath.appendComponent(filename));
#endif
	} else {
		if (screenshotsPath.empty())
			warning("Could not save screenshot in current directory");
		else
			warning("Could not save screenshot in directory '%s'", screenshotsPath.toString(Common::Path::kNativeSeparator).c_str());

#ifdef USE_OSD
		displayMessageOnOSD(_("Could not save screenshot"));
#endif
	}
}

bool SdlGraphicsManager::notifyEvent(const Common::Event &event) {
	if (event.type != Common::EVENT_CUSTOM_BACKEND_ACTION_START) {
		return false;
	}

	switch ((CustomEventAction) event.customType) {
	case kActionToggleMouseCapture:
		getWindow()->grabMouse(!getWindow()->mouseIsGrabbed());
		return true;

	case kActionToggleResizableWindow:
		getWindow()->setResizable(!getWindow()->resizable());
		return true;

	case kActionToggleFullscreen:
		toggleFullScreen();
		return true;

	case kActionSaveScreenshot:
		saveScreenshot();
		return true;

	default:
		return false;
	}
}

void SdlGraphicsManager::toggleFullScreen() {
	if (!g_system->hasFeature(OSystem::kFeatureFullscreenMode) ||
	   !canSwitchFullscreen()) {
		return;
	}

	beginGFXTransaction();
	setFeatureState(OSystem::kFeatureFullscreenMode, !getFeatureState(OSystem::kFeatureFullscreenMode));
	endGFXTransaction();
#ifdef USE_OSD
	if (getFeatureState(OSystem::kFeatureFullscreenMode))
		displayMessageOnOSD(_("Fullscreen mode"));
	else
		displayMessageOnOSD(_("Windowed mode"));
#endif
}

Common::Keymap *SdlGraphicsManager::getKeymap() {
	using namespace Common;

	Keymap *keymap = new Keymap(Keymap::kKeymapTypeGlobal, "sdl-graphics", _("Graphics"));
	Action *act;

	if (g_system->hasFeature(OSystem::kFeatureFullscreenMode)) {
		act = new Action("FULS", _("Toggle fullscreen"));
		act->addDefaultInputMapping("A+RETURN");
		act->addDefaultInputMapping("A+KP_ENTER");
		act->setCustomBackendActionEvent(kActionToggleFullscreen);
		keymap->addAction(act);
	}

	act = new Action("CAPT", _("Toggle mouse capture"));
	act->addDefaultInputMapping("C+m");
	act->setCustomBackendActionEvent(kActionToggleMouseCapture);
	keymap->addAction(act);

	act = new Action("RSZW", _("Toggle resizable window"));
	act->addDefaultInputMapping("C+r");
	act->setCustomBackendActionEvent(kActionToggleResizableWindow);
	keymap->addAction(act);

	act = new Action("SCRS", _("Save screenshot"));
	act->addDefaultInputMapping("A+s");
	act->setCustomBackendActionEvent(kActionSaveScreenshot);
	keymap->addAction(act);

	if (hasFeature(OSystem::kFeatureAspectRatioCorrection)) {
		act = new Action("ASPT", _("Toggle aspect ratio correction"));
		act->addDefaultInputMapping("C+A+a");
		act->setCustomBackendActionEvent(kActionToggleAspectRatioCorrection);
		keymap->addAction(act);
	}

	if (hasFeature(OSystem::kFeatureFilteringMode)) {
		act = new Action("FILT", _("Toggle linear filtered scaling"));
		act->addDefaultInputMapping("C+A+f");
		act->setCustomBackendActionEvent(kActionToggleFilteredScaling);
		keymap->addAction(act);
	}

	if (hasFeature(OSystem::kFeatureStretchMode)) {
		act = new Action("STCH", _("Cycle through stretch modes"));
		act->addDefaultInputMapping("C+A+s");
		act->setCustomBackendActionEvent(kActionCycleStretchMode);
		keymap->addAction(act);
	}

	act = new Action("SCL+", _("Increase the scale factor"));
	act->addDefaultInputMapping("C+A+PLUS");
	act->addDefaultInputMapping("C+A+KP_PLUS");
	act->setCustomBackendActionEvent(kActionIncreaseScaleFactor);
	keymap->addAction(act);

	act = new Action("SCL-", _("Decrease the scale factor"));
	act->addDefaultInputMapping("C+A+MINUS");
	act->addDefaultInputMapping("C+A+KP_MINUS");
	act->setCustomBackendActionEvent(kActionDecreaseScaleFactor);
	keymap->addAction(act);

	if (hasFeature(OSystem::kFeatureScalers)) {
		act = new Action("FLTN", _("Switch to the next scaler"));
		act->addDefaultInputMapping("C+A+0");
		act->setCustomBackendActionEvent(kActionNextScaleFilter);
		keymap->addAction(act);

		act = new Action("FLTP", _("Switch to the previous scaler"));
		act->addDefaultInputMapping("C+A+9");
		act->setCustomBackendActionEvent(kActionPreviousScaleFilter);
		keymap->addAction(act);
	}

	return keymap;
}

#if defined(USE_IMGUI) && SDL_VERSION_ATLEAST(2, 0, 0)
void SdlGraphicsManager::setImGuiCallbacks(const ImGuiCallbacks &callbacks) {
	if (_imGuiInited) {
		if (_imGuiCallbacks.cleanup) {
			_imGuiCallbacks.cleanup();
		}
		_imGuiInited = false;
	}

	_imGuiCallbacks = callbacks;

	if (!_imGuiReady) {
		return;
	}

	if (_imGuiCallbacks.init) {
		_imGuiCallbacks.init();
	}
	_imGuiInited = true;
}

void SdlGraphicsManager::initImGui(SDL_Renderer *renderer, void *glContext) {
	assert(!_imGuiReady);
	_imGuiInited = false;

	IMGUI_CHECKVERSION();
	if (!ImGui::CreateContext()) {
		return;
	}
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	ImGui::StyleColorsDark();
	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0.0f;
	style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	io.IniFilename = nullptr;

	_imGuiSDLRenderer = nullptr;
#ifdef USE_OPENGL
	if (!_imGuiReady && glContext) {
		// Only OpenGL and GLES2 are supported, not GLES
		if ((OpenGLContext.type != OpenGL::kContextGL) &&
			(OpenGLContext.type != OpenGL::kContextGLES2)) {
			ImGui::DestroyContext();
			return;
		}

		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

#if SDL_VERSION_ATLEAST(3, 0, 0)
		if (!ImGui_ImplSDL3_InitForOpenGL(_window->getSDLWindow(), glContext)) {
#else
		if (!ImGui_ImplSDL2_InitForOpenGL(_window->getSDLWindow(), glContext)) {
#endif
			ImGui::DestroyContext();
			return;
		}

		const char *glslVersion;
		if (OpenGLContext.type == OpenGL::kContextGLES2) {
			glslVersion = "#version 100";
		} else {
			glslVersion = "#version 110";
		}
		if (!ImGui_ImplOpenGL3_Init(glslVersion)) {
#if SDL_VERSION_ATLEAST(3, 0, 0)
			ImGui_ImplSDL3_Shutdown();
#else
			ImGui_ImplSDL2_Shutdown();
#endif
			ImGui::DestroyContext();
			return;
		}

		_imGuiReady = true;
	}
#endif
#ifdef USE_IMGUI_SDLRENDERER3
	if (!_imGuiReady && renderer) {
		if (!ImGui_ImplSDL3_InitForSDLRenderer(_window->getSDLWindow(), renderer)) {
			ImGui::DestroyContext();
			return;
		}

		if (!ImGui_ImplSDLRenderer3_Init(renderer)) {
			ImGui_ImplSDL3_Shutdown();
			ImGui::DestroyContext();
			return;
		}

		_imGuiReady = true;
		_imGuiSDLRenderer = renderer;
	}
#elif defined(USE_IMGUI_SDLRENDERER2)
	if (!_imGuiReady && renderer) {
		if (!ImGui_ImplSDL2_InitForSDLRenderer(_window->getSDLWindow(), renderer)) {
			ImGui::DestroyContext();
			return;
		}

		if (!ImGui_ImplSDLRenderer2_Init(renderer)) {
			ImGui_ImplSDL2_Shutdown();
			ImGui::DestroyContext();
			return;
		}

		_imGuiReady = true;
		_imGuiSDLRenderer = renderer;
	}
#endif
	if (!_imGuiReady) {
		warning("No ImGui renderer has been found");
		ImGui::DestroyContext();
		return;
	}

	if (_imGuiCallbacks.init) {
		_imGuiCallbacks.init();
	}
	_imGuiInited = true;
}

void SdlGraphicsManager::renderImGui() {
	if (!_imGuiReady || !_imGuiCallbacks.render) {
		return;
	}

	if (!_imGuiInited) {
		if (_imGuiCallbacks.init) {
			_imGuiCallbacks.init();
		}
		_imGuiInited = true;
	}

#ifdef USE_IMGUI_SDLRENDERER3
	if (_imGuiSDLRenderer) {
		ImGui_ImplSDLRenderer3_NewFrame();
	} else {
#elif defined(USE_IMGUI_SDLRENDERER2)
	if (_imGuiSDLRenderer) {
		ImGui_ImplSDLRenderer2_NewFrame();
	} else {
#endif
#ifdef USE_OPENGL
		ImGui_ImplOpenGL3_NewFrame();
#endif
#if defined(USE_IMGUI_SDLRENDERER2) || defined(USE_IMGUI_SDLRENDERER3)
	}
#endif
#if SDL_VERSION_ATLEAST(3, 0, 0)
	ImGui_ImplSDL3_NewFrame();
#else
	ImGui_ImplSDL2_NewFrame();
#endif

	ImGui::NewFrame();
	_imGuiCallbacks.render();
	ImGui::Render();
#ifdef USE_IMGUI_SDLRENDERER3
	if (_imGuiSDLRenderer) {
		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), _imGuiSDLRenderer);
	} else {
#elif defined(USE_IMGUI_SDLRENDERER2)
	if (_imGuiSDLRenderer) {
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), _imGuiSDLRenderer);
	} else {
#endif
#ifdef USE_OPENGL
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
#endif
#if defined(USE_IMGUI_SDLRENDERER2) || defined(USE_IMGUI_SDLRENDERER3)
	}
#endif
}

void SdlGraphicsManager::destroyImGui() {
	if (!_imGuiReady) {
		return;
	}

	if (_imGuiCallbacks.cleanup) {
		_imGuiCallbacks.cleanup();
	}

	_imGuiInited = false;
	_imGuiReady = false;

#ifdef USE_IMGUI_SDLRENDERER3
	if (_imGuiSDLRenderer) {
		ImGui_ImplSDLRenderer3_Shutdown();
	} else {
#elif defined(USE_IMGUI_SDLRENDERER2)
	if (_imGuiSDLRenderer) {
		ImGui_ImplSDLRenderer2_Shutdown();
	} else {
#endif
#ifdef USE_OPENGL
		ImGui_ImplOpenGL3_Shutdown();
#endif
#if defined(USE_IMGUI_SDLRENDERER2) || defined(USE_IMGUI_SDLRENDERER3)
	}
#endif
#if SDL_VERSION_ATLEAST(3, 0, 0)
	ImGui_ImplSDL3_Shutdown();
#else
	ImGui_ImplSDL2_Shutdown();
#endif
	ImGui::DestroyContext();
}
#endif
