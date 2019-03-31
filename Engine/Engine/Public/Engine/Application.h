#pragma once

#include <Engine/EngineBase.h>
#include <Event/Signal.h>

namespace Summit
{
    class FrameData;
    
    /*!
     * @brief Base application interface.
     */
    class ENGINE_API SummitApplication
    {
    public:
        /*!
         * @brief Base application constructor.
         */
        SummitApplication() = default;
        
        /*!
         * @brief Base application destructor.
         */
        virtual ~SummitApplication() = default;
        
        SummitApplication(const SummitApplication& other) = delete;
        SummitApplication(SummitApplication&& other) = delete;
        SummitApplication& operator=(const SummitApplication& other) = delete;
        SummitApplication& operator=(SummitApplication&& other) = default;
        
        /*!
         @brief Early frame update handler function.
         @param data Per frame data.
         */
        virtual void OnEarlyUpdate(const FrameData& data) = 0;
        
        /*!
         @brief Update frame update handler function.
         @param data Per frame data.
         */
        virtual void OnUpdate(const FrameData& data) = 0;
        
        /*!
         @brief Late frame update handler function.
         @param data Per frame data.
         */
        virtual void OnLateUpdate(const FrameData& data) = 0;
        
        /*!
         @brief Frame render handler function.
         @param data Per frame data.
         */
        virtual void OnRender(const FrameData& data) = 0;
        
        /*!
         @brief Application UI render handler function.
         @param data Per frame data.
         */
        virtual void OnUIRender(const FrameData& data) = 0;
        
    protected:
        /*!
         @brief Early update connection object.
         */
        sigslot::connection mEarlyUpdateConnection;
        
        /*!
         @brief Update connection object.
         */
        sigslot::connection mUpdateConnection;
        
        /*!
         @brief Late update connection object.
         */
        sigslot::connection mLateUpdateConnection;
        
        /*!
         @brief Render connection object.
         */
        sigslot::connection mRenderConnection;
        
        /*!
         @brief UI render connection object.
         */
        sigslot::connection mUIRenderConnection;
    };
}
