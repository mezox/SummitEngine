//#pragma once
//
//#include <cstdint>
//
//namespace Renderer
//{
//	class ISwapChain
//	{
//	public:
//		virtual ~ISwapChain() = default;
//
//		virtual void Initialize(const uint32_t width, const uint32_t height) = 0;
//		virtual void Destroy() = 0;
//
//		virtual const uint8_t GetImageCount() const = 0;
//
//		virtual uint32_t SwapBuffers() = 0;
//		virtual void Present() = 0;
//	};
//}