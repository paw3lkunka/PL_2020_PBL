#ifndef _UIMODULE_HPP
#define _UIMODULE_HPP

#include "IModule.inl"
#include "RectTransform.inl"

#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H 

class Message;

class UiModule : public IModule
{
public:
    UiModule() = default;
    virtual ~UiModule() = default;

    /**
     * @brief Message reciever, checks if the window was resized
     */
    virtual void receiveMessage(Message msg);

    /**
     * @brief Initialize the UI module
     */
    void init();

    /**
     * @brief Traverse the graphs of rect transforms
     */
    void updateRectTransforms(bool windowResized = false);

    /**
     * @brief Vector of rect transform roots
     */
    std::vector<RectTransform*> rootNodes;

private:
    void process(RectTransform* transform, RectTransform* parent, bool dirty);
};

#endif // _UIMODULE_HPP
