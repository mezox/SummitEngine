//
//  AppDelegate.m
//  SummitEngine
//
//  Created by Mes on 20/11/2018.
//  Copyright © 2018 Kubovcik. All rights reserved.
//

#import "AppDelegate.h"
#import <AppKit/Appkit.h>

#include <Logging/Logger.h>
#include <Logging/LoggingService.h>

#include <Engine/Engine.h>
#include <PAL/FileSystem/File.h>
#include <Engine/Window.h>
#include <Engine/Renderer.h>


#ifdef LOG_MODULE_ID
#undef LOG_MODULE_ID
#endif

#ifdef LOGGER_ID
#undef LOGGER_ID
#define LOGGER_ID "Application"
#endif

#define LOG_MODULE_ID LOG_MODULE_4BYTE(' ','A','P','P')

#include <Engine/VertexBuffer.h>

using namespace Renderer;

@interface AppDelegate ()
{
    Application::Window* window;
    std::unique_ptr<Logging::Logger> logger;
}
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    
    Engine::EngineServiceLocator::Provide(Engine::CreateEngineService());
    Engine::EngineServiceLocator::Service()->Initialize();
    
    logger = std::make_unique<Logging::Logger>(LOGGER_ID);
    Logging::LoggingServiceLocator::Service().AddLogger(std::move(logger));
    LOG(Information) << "SummitApp did finish launching!";
    
    window = Engine::EngineServiceLocator::Service()->CreateWindow("SummitEngine", 1280, 720);
    Engine::EngineServiceLocator::Service()->RegisterWindow(window);
    
    Object3D triangle;
    auto& positionData = triangle.mVertexBuffer.GetPositionDataStream().GetData();
    auto& colorData = triangle.mVertexBuffer.GetColorDataStream().GetData();
    auto& indexData = triangle.mVertexBuffer.GetIndexDataStream().GetData();
    
    positionData.push_back({ -0.5f, -0.5f });
    positionData.push_back({ 0.5f, -0.5f });
    positionData.push_back({ 0.5f, 0.5f });
    positionData.push_back({ -0.5f, 0.5f });
    
    colorData.push_back({ 1.0f, 0.0f, 0.0f });
    colorData.push_back({ 0.0f, 1.0f, 0.0f });
    colorData.push_back({ 0.0f, 0.0f, 1.0f });
    colorData.push_back({ 1.0f, 1.0f, 1.0f });
    
    indexData = { 0, 1, 2, 2, 3, 0 };
    
    auto& renderer = Engine::EngineServiceLocator::Service()->GetRenderer();
    
    BufferDesc descriptorPosition;
    descriptorPosition.usage = BufferUsage::VertexBuffer;
    descriptorPosition.sharingMode = SharingMode::Exclusive;
    descriptorPosition.memoryUsage = MemoryType::DeviceLocal;
    descriptorPosition.bufferSize = positionData.size() * sizeof(positionData[0]);
    descriptorPosition.data = positionData.data();
    
    renderer.CreateBuffer(descriptorPosition, triangle.mVertexBuffer.GetPositionDataStream().GetDeviceResource());
    
    BufferDesc descriptorColor;
    descriptorColor.usage = BufferUsage::VertexBuffer;
    descriptorColor.sharingMode = SharingMode::Exclusive;
    descriptorColor.memoryUsage = MemoryType::DeviceLocal;
    descriptorColor.bufferSize = colorData.size() * sizeof(colorData[0]);
    descriptorColor.data = colorData.data();
    
    renderer.CreateBuffer(descriptorColor, triangle.mVertexBuffer.GetColorDataStream().GetDeviceResource());
    
    BufferDesc descriptorIndexBuffer;
    descriptorIndexBuffer.usage = BufferUsage::IndexBuffer;
    descriptorIndexBuffer.sharingMode = SharingMode::Exclusive;
    descriptorIndexBuffer.memoryUsage = MemoryType::DeviceLocal;
    descriptorIndexBuffer.bufferSize = indexData.size() * sizeof(indexData[0]);
    descriptorIndexBuffer.data = indexData.data();
    
    renderer.CreateBuffer(descriptorIndexBuffer, triangle.mVertexBuffer.GetIndexDataStream().GetDeviceResource());
    
    Pipeline pipeline;
    pipeline.vertexShaderFile = "/Users/tomaskubovcik/Dev/SummitEngine/vert.spv";
    pipeline.fragmentShaderFile = "/Users/tomaskubovcik/Dev/SummitEngine/frag.spv";
    pipeline.Create();
    
    renderer.CreateCommandBuffers(pipeline, triangle);
    
    Engine::EngineServiceLocator::Service()->Run();
    
    window->SetTitle("SummitEngineApplication");
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
    
    NSLog(@"Application will terminate!");
}


@end
