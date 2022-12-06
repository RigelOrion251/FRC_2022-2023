// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpigui.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

#include <GLFW/glfw3.h>
#include <IconsFontAwesome6.h>
#include <imgui.h>
#include <imgui_FontAwesomeSolid.h>
#include <imgui_ProggyDotted.h>
#include <imgui_impl_glfw.h>
#include <imgui_internal.h>
#include <implot.h>
#include <stb_image.h>

#include "wpigui_internal.h"

using namespace wpi::gui;

namespace wpi {

Context* gui::gContext;

static void ErrorCallback(int error, const char* description) {
  std::fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static void WindowSizeCallback(GLFWwindow* window, int width, int height) {
  if (!gContext->maximized) {
    gContext->width = width;
    gContext->height = height;
  }
  if (!gContext->isPlatformRendering) {
    PlatformRenderFrame();
  }
}

static void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
  PlatformFramebufferSizeChanged(width, height);
}

static void WindowMaximizeCallback(GLFWwindow* window, int maximized) {
  gContext->maximized = maximized;
}

static void WindowPosCallback(GLFWwindow* window, int xpos, int ypos) {
  if (!gContext->maximized) {
    gContext->xPos = xpos;
    gContext->yPos = ypos;
  }
}

static void* IniReadOpen(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                         const char* name) {
  if (std::strcmp(name, "GLOBAL") != 0) {
    return nullptr;
  }
  return static_cast<SavedSettings*>(gContext);
}

static void IniReadLine(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                        void* entry, const char* lineStr) {
  auto impl = static_cast<SavedSettings*>(entry);
  const char* value = std::strchr(lineStr, '=');
  if (!value) {
    return;
  }
  ++value;
  int num = std::atoi(value);
  if (std::strncmp(lineStr, "width=", 6) == 0) {
    impl->width = num;
    impl->loadedWidthHeight = true;
  } else if (std::strncmp(lineStr, "height=", 7) == 0) {
    impl->height = num;
    impl->loadedWidthHeight = true;
  } else if (std::strncmp(lineStr, "maximized=", 10) == 0) {
    impl->maximized = num;
  } else if (std::strncmp(lineStr, "xpos=", 5) == 0) {
    impl->xPos = num;
  } else if (std::strncmp(lineStr, "ypos=", 5) == 0) {
    impl->yPos = num;
  } else if (std::strncmp(lineStr, "userScale=", 10) == 0) {
    impl->userScale = num;
  } else if (std::strncmp(lineStr, "style=", 6) == 0) {
    impl->style = num;
  }
}

static void IniWriteAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                        ImGuiTextBuffer* out_buf) {
  if (!gContext) {
    return;
  }
  out_buf->appendf(
      "[MainWindow][GLOBAL]\nwidth=%d\nheight=%d\nmaximized=%d\n"
      "xpos=%d\nypos=%d\nuserScale=%d\nstyle=%d\n\n",
      gContext->width, gContext->height, gContext->maximized ? 1 : 0,
      gContext->xPos, gContext->yPos, gContext->userScale, gContext->style);
}

void gui::CreateContext() {
  gContext = new Context;
  AddFont("ProggyDotted", [](ImGuiIO& io, float size, const ImFontConfig* cfg) {
    auto font = ImGui::AddFontProggyDotted(io, size, cfg);
    static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
    ImFontConfig icons_cfg;
    icons_cfg.MergeMode = true;
    icons_cfg.PixelSnapH = true;
    ImGui::AddFontFontAwesomeSolid(io, size, &icons_cfg, icons_ranges);
    return font;
  });
  PlatformCreateContext();
}

void gui::DestroyContext() {
  PlatformDestroyContext();
  delete gContext;
  gContext = nullptr;
}

static void UpdateFontScale() {
  // Scale based on OS window content scaling
  float windowScale = 1.0;
#ifndef __APPLE__
  glfwGetWindowContentScale(gContext->window, &windowScale, nullptr);
#endif
  // map to closest font size: 0 = 0.5x, 1 = 0.75x, 2 = 1.0x, 3 = 1.25x,
  // 4 = 1.5x, 5 = 1.75x, 6 = 2x
  int fontScale =
      gContext->userScale + static_cast<int>((windowScale - 1.0) * 4);
  if (fontScale < 0) {
    fontScale = 0;
  }
  if (gContext->fontScale != fontScale) {
    gContext->reloadFonts = true;
    gContext->fontScale = fontScale;
  }
}

// the range is based on 13px being the "nominal" 100% size and going from
// ~0.5x (7px) to ~2.0x (25px)
static void ReloadFonts() {
  auto& io = ImGui::GetIO();
  io.Fonts->Clear();
  gContext->fonts.clear();
  float size = 7.0f + gContext->fontScale * 3.0f;
  bool first = true;
  for (auto&& makeFont : gContext->makeFonts) {
    if (makeFont.second) {
      ImFontConfig cfg;
      std::snprintf(cfg.Name, sizeof(cfg.Name), "%s", makeFont.first);
      ImFont* font = makeFont.second(io, size, &cfg);
      if (first) {
        ImGui::GetIO().FontDefault = font;
        first = false;
      }
      gContext->fonts.emplace_back(font);
    }
  }
}

bool gui::Initialize(const char* title, int width, int height,
                     ImGuiConfigFlags configFlags) {
  gContext->title = title;
  gContext->width = width;
  gContext->height = height;
  gContext->defaultWidth = width;
  gContext->defaultHeight = height;

  // Setup window
  glfwSetErrorCallback(ErrorCallback);
  glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE);
  glfwInitHint(GLFW_COCOA_CHDIR_RESOURCES, GLFW_FALSE);
  PlatformGlfwInitHints();
  if (!glfwInit()) {
    return false;
  }

  PlatformGlfwWindowHints();

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= configFlags;

  // Hook ini handler to save settings
  ImGuiSettingsHandler iniHandler;
  iniHandler.TypeName = "MainWindow";
  iniHandler.TypeHash = ImHashStr(iniHandler.TypeName);
  iniHandler.ReadOpenFn = IniReadOpen;
  iniHandler.ReadLineFn = IniReadLine;
  iniHandler.WriteAllFn = IniWriteAll;
  ImGui::GetCurrentContext()->SettingsHandlers.push_back(iniHandler);

  if (gContext->loadSettings) {
    gContext->loadSettings();
    io.IniFilename = nullptr;
  } else {
    io.IniFilename = gContext->iniPath.c_str();
  }

  for (auto&& initialize : gContext->initializers) {
    if (initialize) {
      initialize();
    }
  }

  // Load INI file
  if (gContext->loadIniSettings) {
    gContext->loadIniSettings();
  } else if (io.IniFilename) {
    ImGui::LoadIniSettingsFromDisk(io.IniFilename);
  }

  // Set initial window settings
  glfwWindowHint(GLFW_MAXIMIZED, gContext->maximized ? GLFW_TRUE : GLFW_FALSE);

  if (gContext->width == 0 || gContext->height == 0) {
    gContext->width = gContext->defaultWidth;
    gContext->height = gContext->defaultHeight;
    gContext->loadedWidthHeight = false;
  }

  float windowScale = 1.0;
  if (!gContext->loadedWidthHeight) {
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    // get the primary monitor work area to see if we have a reasonable initial
    // window size; if not, maximize, and default scaling to smaller
    if (GLFWmonitor* primary = glfwGetPrimaryMonitor()) {
      int monWidth, monHeight;
      glfwGetMonitorWorkarea(primary, nullptr, nullptr, &monWidth, &monHeight);
      if (monWidth < gContext->width || monHeight < gContext->height) {
        glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
        windowScale = (std::min)(monWidth * 1.0 / gContext->width,
                                 monHeight * 1.0 / gContext->height);
      }
    }
  }
  if (gContext->xPos != -1 && gContext->yPos != -1) {
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  }

  // Create window with graphics context
  gContext->window =
      glfwCreateWindow(gContext->width, gContext->height,
                       gContext->title.c_str(), nullptr, nullptr);
  if (!gContext->window) {
    return false;
  }

  if (!gContext->loadedWidthHeight) {
#ifndef __APPLE__
    if (windowScale == 1.0) {
      glfwGetWindowContentScale(gContext->window, &windowScale, nullptr);
    }
#endif
    // force user scale if window scale is smaller
    if (windowScale <= 0.5) {
      gContext->userScale = 0;
    } else if (windowScale <= 0.75) {
      gContext->userScale = 1;
    }
    if (windowScale != 1.0) {
      for (auto&& func : gContext->windowScalers) {
        func(windowScale);
      }
    }
  }

  // Update window settings
  if (gContext->xPos != -1 && gContext->yPos != -1) {
    // check to make sure the position isn't off-screen
    bool found = false;
    int monCount;
    GLFWmonitor** monitors = glfwGetMonitors(&monCount);
    for (int i = 0; i < monCount; ++i) {
      int monXPos, monYPos, monWidth, monHeight;
      glfwGetMonitorWorkarea(monitors[i], &monXPos, &monYPos, &monWidth,
                             &monHeight);
      if (gContext->xPos >= monXPos && gContext->xPos < (monXPos + monWidth) &&
          gContext->yPos >= monYPos && gContext->yPos < (monYPos + monHeight)) {
        found = true;
        break;
      }
    }
    if (found) {
      glfwSetWindowPos(gContext->window, gContext->xPos, gContext->yPos);
    }
    glfwShowWindow(gContext->window);
  }

  // Set window callbacks
  glfwGetWindowSize(gContext->window, &gContext->width, &gContext->height);
  glfwSetWindowSizeCallback(gContext->window, WindowSizeCallback);
  glfwSetFramebufferSizeCallback(gContext->window, FramebufferSizeCallback);
  glfwSetWindowMaximizeCallback(gContext->window, WindowMaximizeCallback);
  glfwSetWindowPosCallback(gContext->window, WindowPosCallback);

  // Set icons
  if (!gContext->icons.empty()) {
    glfwSetWindowIcon(gContext->window, gContext->icons.size(),
                      gContext->icons.data());
    for (auto&& icon : gContext->icons) {
      stbi_image_free(icon.pixels);
    }
    gContext->icons.clear();
  }

  // Setup Dear ImGui style
  SetStyle(static_cast<Style>(gContext->style));

  // Load Fonts
  UpdateFontScale();
  ReloadFonts();
  gContext->reloadFonts = false;  // init renderer will do this

  if (!PlatformInitRenderer()) {
    return false;
  }

  return true;
}

void gui::Main() {
  // Main loop
  while (!glfwWindowShouldClose(gContext->window) && !gContext->exit) {
    // Poll and handle events (inputs, window resize, etc.)
    glfwPollEvents();
    gContext->isPlatformRendering = true;
    UpdateFontScale();
    if (gContext->reloadFonts) {
      ReloadFonts();
      // PlatformRenderFrame() will clear reloadFonts flag
    }
    PlatformRenderFrame();
    gContext->isPlatformRendering = false;

    auto& io = ImGui::GetIO();

    // custom saving
    if (gContext->saveSettings) {
      if (io.WantSaveIniSettings) {
        gContext->saveSettings(false);
        io.WantSaveIniSettings = false;  // reset flag
      }
    }
  }

  // Save (if custom save)
  if (gContext->saveSettings) {
    gContext->saveSettings(true);
  }

  // Cleanup
  PlatformShutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();

  // Delete the save file if requested.
  if (!gContext->saveSettings && gContext->resetOnExit) {
    std::remove(gContext->iniPath.c_str());
  }

  glfwDestroyWindow(gContext->window);
  glfwTerminate();
}

void gui::CommonRenderFrame() {
  ImGui_ImplGlfw_NewFrame();

  // Start the Dear ImGui frame
  ImGui::NewFrame();

  for (size_t i = 0; i < gContext->earlyExecutors.size(); ++i) {
    auto& execute = gContext->earlyExecutors[i];
    if (execute) {
      execute();
    }
  }

  for (size_t i = 0; i < gContext->lateExecutors.size(); ++i) {
    auto& execute = gContext->lateExecutors[i];
    if (execute) {
      execute();
    }
  }

  // Rendering
  ImGui::Render();
}

void gui::Exit() {
  if (!gContext) {
    return;
  }
  gContext->exit = true;
}

void gui::AddInit(std::function<void()> initialize) {
  if (initialize) {
    gContext->initializers.emplace_back(std::move(initialize));
  }
}

void gui::AddWindowScaler(std::function<void(float scale)> windowScaler) {
  if (windowScaler) {
    gContext->windowScalers.emplace_back(std::move(windowScaler));
  }
}

void gui::AddEarlyExecute(std::function<void()> execute) {
  if (execute) {
    gContext->earlyExecutors.emplace_back(std::move(execute));
  }
}

void gui::AddLateExecute(std::function<void()> execute) {
  if (execute) {
    gContext->lateExecutors.emplace_back(std::move(execute));
  }
}

void gui::ConfigureCustomSaveSettings(std::function<void()> load,
                                      std::function<void()> loadIni,
                                      std::function<void(bool)> save) {
  gContext->loadSettings = load;
  gContext->loadIniSettings = loadIni;
  gContext->saveSettings = save;
}

GLFWwindow* gui::GetSystemWindow() {
  return gContext->window;
}

bool gui::AddIcon(const unsigned char* data, int len) {
  // Load from memory
  GLFWimage image;
  image.pixels =
      stbi_load_from_memory(data, len, &image.width, &image.height, nullptr, 4);
  if (!data) {
    return false;
  }
  gContext->icons.emplace_back(std::move(image));
  return true;
}

int gui::AddFont(
    const char* name,
    std::function<ImFont*(ImGuiIO& io, float size, const ImFontConfig* cfg)>
        makeFont) {
  if (makeFont) {
    gContext->makeFonts.emplace_back(name, std::move(makeFont));
  }
  return gContext->makeFonts.size() - 1;
}

void gui::SetStyle(Style style) {
  gContext->style = static_cast<int>(style);
  switch (style) {
    case kStyleClassic:
      ImGui::StyleColorsClassic();
      break;
    case kStyleDark:
      ImGui::StyleColorsDark();
      break;
    case kStyleLight:
      ImGui::StyleColorsLight();
      break;
  }
}

void gui::SetClearColor(ImVec4 color) {
  gContext->clearColor = color;
}

std::string gui::GetPlatformSaveFileDir() {
#if defined(_MSC_VER)
  const char* env = std::getenv("APPDATA");
  if (env) {
    return env + std::string("/");
  }
#elif defined(__APPLE__)
  const char* env = std::getenv("HOME");
  if (env) {
    return env + std::string("/Library/Preferences/");
  }
#else
  const char* xdg = std::getenv("XDG_CONFIG_HOME");
  const char* env = std::getenv("HOME");
  if (xdg) {
    return xdg + std::string("/");
  } else if (env) {
    return env + std::string("/.config/");
  }
#endif
  return "";
}

void gui::ConfigurePlatformSaveFile(const std::string& name) {
  gContext->iniPath = GetPlatformSaveFileDir() + name;
}

void gui::EmitViewMenu() {
  if (ImGui::BeginMenu("View")) {
    if (ImGui::BeginMenu("Style")) {
      bool selected;
      selected = gContext->style == kStyleClassic;
      if (ImGui::MenuItem("Classic", nullptr, &selected, true)) {
        SetStyle(kStyleClassic);
      }
      selected = gContext->style == kStyleDark;
      if (ImGui::MenuItem("Dark", nullptr, &selected, true)) {
        SetStyle(kStyleDark);
      }
      selected = gContext->style == kStyleLight;
      if (ImGui::MenuItem("Light", nullptr, &selected, true)) {
        SetStyle(kStyleLight);
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Zoom")) {
      for (int i = 0; i < kFontScaledLevels && (25 * (i + 2)) <= 200; ++i) {
        char label[20];
        std::snprintf(label, sizeof(label), "%d%%", 25 * (i + 2));
        bool selected = gContext->userScale == i;
        if (ImGui::MenuItem(label, nullptr, &selected)) {
          gContext->userScale = i;
        }
      }
      ImGui::EndMenu();
    }

    if (!gContext->saveSettings) {
      ImGui::MenuItem("Reset UI on Exit?", nullptr, &gContext->resetOnExit);
    }
    ImGui::EndMenu();
  }
}

bool gui::UpdateTextureFromImage(ImTextureID* texture, int width, int height,
                                 const unsigned char* data, int len) {
  // Load from memory
  int width2 = 0;
  int height2 = 0;
  unsigned char* imgData =
      stbi_load_from_memory(data, len, &width2, &height2, nullptr, 4);
  if (!data) {
    return false;
  }

  if (width2 == width && height2 == height) {
    UpdateTexture(texture, kPixelRGBA, width2, height2, imgData);
  } else {
    *texture = CreateTexture(kPixelRGBA, width2, height2, imgData);
  }

  stbi_image_free(imgData);

  return true;
}

bool gui::CreateTextureFromFile(const char* filename, ImTextureID* out_texture,
                                int* out_width, int* out_height) {
  // Load from file
  int width = 0;
  int height = 0;
  unsigned char* data = stbi_load(filename, &width, &height, nullptr, 4);
  if (!data) {
    return false;
  }

  *out_texture = CreateTexture(kPixelRGBA, width, height, data);
  if (out_width) {
    *out_width = width;
  }
  if (out_height) {
    *out_height = height;
  }

  stbi_image_free(data);

  return true;
}

bool gui::CreateTextureFromImage(const unsigned char* data, int len,
                                 ImTextureID* out_texture, int* out_width,
                                 int* out_height) {
  // Load from memory
  int width = 0;
  int height = 0;
  unsigned char* imgData =
      stbi_load_from_memory(data, len, &width, &height, nullptr, 4);
  if (!imgData) {
    return false;
  }

  *out_texture = CreateTexture(kPixelRGBA, width, height, imgData);
  if (out_width) {
    *out_width = width;
  }
  if (out_height) {
    *out_height = height;
  }

  stbi_image_free(imgData);

  return true;
}

void gui::MaxFit(ImVec2* min, ImVec2* max, float width, float height) {
  float destWidth = max->x - min->x;
  float destHeight = max->y - min->y;
  if (width == 0 || height == 0) {
    return;
  }
  if (destWidth * height > destHeight * width) {
    float outputWidth = width * destHeight / height;
    min->x += (destWidth - outputWidth) / 2;
    max->x -= (destWidth - outputWidth) / 2;
  } else {
    float outputHeight = height * destWidth / width;
    min->y += (destHeight - outputHeight) / 2;
    max->y -= (destHeight - outputHeight) / 2;
  }
}

}  // namespace wpi
