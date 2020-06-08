#ifndef CARGOBUTTON_INL_
#define CARGOBUTTON_INL_

#include "ToggleButton.inl"

struct TextRenderer;

/**
 * @brief Toggle of cargo setting component
 *  It has Toggle properties but also contains extra fields
 * to have possibility to change text which represents cargo data
 */
struct CargoButton : public ToggleButton
{
public:
    CargoButton() = default;
    ~CargoButton() = default;

    ///@brief text renderer component which contains name of cargo
    TextRenderer* nameText;
    ///@brief text renderer component which contains weight of cargo
    TextRenderer* weightText;
    ///@brief text renderer component which contains income of cargo
    TextRenderer* incomeText;
};

#endif /* !CARGOBUTTON_INL_ */
