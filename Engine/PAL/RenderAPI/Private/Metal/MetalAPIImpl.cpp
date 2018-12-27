#include <Metal/MetalAPIImpl.h>
#include <Logging/LoggingService.h>

#import <Metal/MTLDevice.h>

#ifdef LOG_MODULE_ID
#undef LOG_MODULE_ID
#endif

#define LOG_MODULE_ID LOG_MODULE_4BYTE('M','T','L','R')

namespace PAL::RenderAPI
{
    class MetalDevice: public IDevice
    {
    public:
        
    public:
        id<MTLDevice> mDevice;
    };
    
    class MetalRenderAPI::MetalImpl
    {
    public:
        MetalImpl()
        {
            externalGPUs = [[NSMutableArray alloc] init];
            integratedGPUs = [[NSMutableArray alloc] init];
            discreteGPUs = [[NSMutableArray alloc] init];
        }
        
        ~MetalImpl()
        {
        }
        
        void Initialize()
        {
#if 0
            id <NSObject> deviceObserver = nil;
            NSArray<id<MTLDevice>> *deviceList = MTLCopyAllDevicesWithObserver(&deviceObserver,
                                                       ^(id<MTLDevice> device, MTLDeviceNotificationName name) {
                                                           /*[self handleExternalGPUEventsForDevice:device notification:name];*/
                                                       });
            
            for (id <MTLDevice> device in deviceList)
            {
                if (device.removable)
                    [externalGPUs addObject:device];
                else if (device.lowPower)
                    [integratedGPUs addObject:device];
                else
                    [discreteGPUs addObject:device];
            }
            
            LOG(Information) << "Found " << [externalGPUs count] << " external GPUs:";
            for(id <MTLDevice> device in externalGPUs)
            {
                LOG(Information) << "Device name: " << [[device name] UTF8String];
            }
            
            LOG(Information) << "Found " << [discreteGPUs count] << " discrete GPUs:";
            for(id <MTLDevice> device in discreteGPUs)
            {
                LOG(Information) << "Device name: " << [[device name] UTF8String];
            }
            
            LOG(Information) << "Found " << [integratedGPUs count] << " integrated GPUs:";
            for(id <MTLDevice> device in integratedGPUs)
            {
                LOG(Information) << "Device name: " << [[device name] UTF8String];
            }
#endif
        }
        
        std::shared_ptr<IDevice> CreateDevice(DeviceType type)
        {
            if(type == DeviceType::Discrete) return nullptr;
            else if(type == DeviceType::External) return nullptr;
            else if(type == DeviceType::Integrated)
            {
                auto device = std::make_shared<MetalDevice>();
                device->mDevice = [integratedGPUs objectAtIndex:0];
                return device;
            }
            
            return nullptr;
        }
        
    private:
        NSMutableArray<id<MTLDevice>> *externalGPUs;
        NSMutableArray<id<MTLDevice>> *integratedGPUs;
        NSMutableArray<id<MTLDevice>> *discreteGPUs;
    };
    
    MetalRenderAPI::MetalRenderAPI()
        : mImpl(std::make_unique<MetalImpl>())
    {}
    
    MetalRenderAPI::~MetalRenderAPI()
    {}
    
    void MetalRenderAPI::Initialize()
    {
        mImpl->Initialize();
    }
    
    void MetalRenderAPI::DeInitialize()
    {
        mImpl.release();
    }
    
    std::shared_ptr<IDevice> MetalRenderAPI::CreateDevice(DeviceType type)
    {
        return mImpl->CreateDevice(type);
    }
}
