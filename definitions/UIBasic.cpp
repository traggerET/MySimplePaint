#include "./headers/UIBasic.hpp"


AbsW *AbsW::GetParent() {
    return parent;
}
void AbsW::InvokeModalWindow(ModalW *mw) {
    parent->InvokeModalWindow(mw);
}
void AbsW::LinkParent(AbsW *parent) {
    if (parent == nullptr) 
        fprintf(stderr, "Wooops, no parent ;(");

    this->parent = parent;
    parent->UpdatePropMask(ThisMask);
}
void AbsW::DelinkParent() {
    if (!parent) return;

    static_cast<ContainerW*>(parent)->DelinkChild(this);
    this->parent = nullptr;
}
AbsW::AbsW(): parent(nullptr), ThisMask(0), PropagationMask(0) {
}
void AbsW::UpdateThisMask(int update) {
    ThisMask |= update;
    if (parent)
        parent->UpdatePropMask(update);
}
void AbsW::UpdatePropMask(int update) {
    PropagationMask |= update;
    if (parent)
        parent->UpdatePropMask(update);
}

float ShapeRect::GetLeft() const {
    return left;
}
float ShapeRect::GetTop() const {
    return top;
}
float ShapeRect::GetWidth() const {
    return width;
}
float ShapeRect::GetHeight() const {
    return height;
}

Color &ShapeRect::GetColorBcg() {
    return colorbkg;
}
Color &ShapeRect::GetColorFrg() {
    return colorfrg;
}
const Color &ShapeRect::GetColorBcg() const {
    return colorbkg;
}
const Color &ShapeRect::GetColorFrg() const {
    return colorfrg;
}

void ShapeRect::SetPosition(float x, float y) {
    left = x;
    top = y;
}
void ShapeRect::SetColorFrg(const Color &color) {
    colorfrg = color;
}
void ShapeRect::SetColorBcg(const Color &color) {
    colorbkg = color;
}
void ShapeRect::SetThickness(float thickness) {
    this->thickness = thickness;
}
void ShapeRect::SetSize(float x, float y) { 
    width = x;
    height = y;
}
void ShapeRect::draw() {
    AdapterSFML::DrawRect(left, top, width, height, thickness, colorfrg, colorbkg);
}
bool ShapeRect::Contains(float x, float y) const {
    return x >= left && x <= left + width && y >= top && y <= top + height;
}



const std::string &Text::GetMessage() const {
    return message;
}
std::string &Text::GetMessage() {
    return message;
}
void Text::SetMessage(const std::string &message) {
    this->message = message;
}
void Text::SetSize(float size) {
    this->size = size;
}
void Text::draw() {
    AdapterSFML::DrawText(message, left, top, size);
}
void Text::SetPosition(float x, float y) {
    left = x;
    top = y;
}

ContainerW::~ContainerW() {
    for (auto child : children) {
        delete child;
    }
}

std::list<AbsW *> &ContainerW::GetChildren() {
    return children;
}

void ContainerW::draw() {
    for (auto child : children) {
        child->draw();
    }
}

void ContainerW::LinkChild(AbsW *child) {
    children.push_back(child);
    child->LinkParent(this);
}
void ContainerW::DelinkChild(AbsW *child) {
    children.remove(child);
}

void ContainerW::HandleEvent(const Event &event) {
    for (auto child : children) {
        child->HandleEvent(event);
    }
}

void ModalWManager::InvokeModalWindow(ModalW *mw) {
    if (modal) {
        modal->DeinvokeMW();
    }
    modal = mw;
    modal->InvokeMW();
}

void ModalWManager::HandleEvent(const Event &ev) {
    if (!modal || !modal->IsInvoked()) {
        ContainerW::HandleEvent(ev);
    }
    else {
        modal->HandleEvent(ev);
    }
}
void ModalWManager::draw() {
    ContainerW::draw();
    if (modal && modal->IsInvoked()) {
        modal->draw();
    }
}

void ModalW::HandleEvent(const Event &event) {
    if (invoked) 
        ContainerW::HandleEvent(event);
}

void ModalW::draw() {
    if (invoked) 
        ContainerW::draw();
}

void ModalW::DeinvokeMW() {
    invoked = false;
}

void ModalW::InvokeMW() {
    invoked = true;
}

ModalW::ModalW() {
    invoked = false;
}


void View::SetPosition(int left, int top) {
    this->left = left; 
    this->top = top;
}
void View::SetSize(int width, int height) {
    this->width = width;
    this->height = height;
}


void View::draw() {
    AdapterSFML::CreateOffscreen(width, height);
    AdapterSFML::PushGlobalOffset(left, top); 
    ContainerW ::draw();
    AdapterSFML::PopGlobalOffset(); 
    AdapterSFML::DrawOffscreen(left, top);
}


void AbsB::SwitchToN() {}
void AbsB::SwitchToH() {}
void AbsB::SwitchToP() {}

uint32_t AbsB::GetDescr() const {
    return texture_descriptor;
}

AbsB::STATE AbsB::GetState() const {
    return state;
}

void AbsB::ResetTexture() {
    texture_descriptor = -1;
}
void AbsB::SetTexture(int64_t descriptor) {
    texture_descriptor = descriptor;
    fprintf(stderr, "texture descr %ld\n", descriptor);
}

void AbsB::HandleEvent(const Event &event) {
    STATE ex_state = state;
    if (event.EventType == EV_MOUSE_MOVED || event.EventType == EV_MOUSE_PRESSED || event.EventType == EV_MOUSE_RELEASED) {
        if (IsPressed(event)) {
            state = PRESSED;
            
            if (ex_state != PRESSED) 
                SwitchToP();

            PressImpact(event);
        }
        else if (IsMoved(event) && state == PRESSED) {
            MoveImpact(event);
        }
        else if (IsReleased(event) && state == PRESSED) {
            state = NORMAL;

            if (ex_state != NORMAL) 
                SwitchToN();

            ClickImpact();
        }
        else if (IsHovered(event) && state != PRESSED) {
            state = HOVERED;
            
            if (ex_state != HOVERED) 
                SwitchToH();
        }
        else {
            state = NORMAL;

            if (ex_state != NORMAL) 
                SwitchToN();

            if (IsPressedOutside(event)) 
                ClickOutsideImpact();
        }
    }
}

AbsB::AbsB() {
    fprintf(stderr, "constructing again\n");
    state = NORMAL;
    UpdateThisMask(EV_MOUSE_MOVED | EV_MOUSE_PRESSED | EV_MOUSE_RELEASED);
}

bool AbsB::IsHovered (const Event &event) const {
    return ShapeRect::Contains(event.mouse.x, event.mouse.y);
}
bool AbsB::IsPressed (const Event &event) const {
    return IsHovered(event) && event.EventType == EV_MOUSE_PRESSED;
}
bool AbsB::IsReleased(const Event &event) const {
    return IsHovered(event) && event.EventType == EV_MOUSE_RELEASED;
}
bool AbsB::IsMoved   (const Event &event) const {
    return event.EventType == EV_MOUSE_MOVED;
}
bool AbsB::IsPressedOutside(const Event &event) const {
    return !IsHovered(event) && event.EventType == EV_MOUSE_PRESSED;
}


void AbsB::MoveImpact (const Event &event) {}
void AbsB::PressImpact(const Event &event) {}
void AbsB::ClickImpact()                   {}
void AbsB::ClickOutsideImpact()            {}

void AbsB::draw() {}


UsualB::UsualB() {
    texture_descriptor = -1;
    colorfrg = {244, 0, 0};
    colorbkg = {40, 40, 40};
    thickness = 2;
    colorN = colorH = colorP = colorfrg;
}


void UsualB::SetPressedColor(const Color &color) {
    colorP = color;
}
void UsualB::SetNormalColor (const Color &color) {
    colorN = color;
}
void UsualB::SetHoveredColor(const Color &color) {
    colorH = color;
}

void UsualB::SwitchToN() {
    SetColorFrg(colorN);
}
void UsualB::SwitchToH() {
    SetColorFrg(colorH);
}
void UsualB::SwitchToP() {
    SetColorFrg(colorP);
}

Text &UsualB::GetTitle() {
    return title;
}
const Text &UsualB::GetTitle() const {
    return title;
}

void UsualB::draw() {
    if (texture_descriptor >= 0) {
        AdapterSFML::DrawSprite(left, top, width, height, colorfrg, texture_descriptor);
    }
    else {
        ShapeRect::draw();
    }
    title.draw();
}



const Slider &ScrollBar::GetSlider() const {
    return slider;
}

void ScrollBar::SetPositionRel(float coordinate) {
    slider.SetPositionRel(coordinate);
}


ScrollBar::ScrollBar(bool horizontal, float length, float width) {
    colorbkg = {50, 50, 50};
    colorfrg = {70, 70, 70};
    thickness = 0;
    slider.SetColorBcg({100, 100, 100});
    slider.SetColorFrg({115, 115, 115});
    slider.SetThickness(0);

    this->horizontal = horizontal;
    slider.SetHorizontal(horizontal);

    if (horizontal) {
        this->width = length;
        height = width;
    }
    else {
        height = length;
        this->width = width;  
    }
    this->length = length;
}

void ScrollBar::SetSliderSize(float x, float y) {
    slider.SetSize(x, y);
}

float ScrollBar::GetOffsetRelScrollbarStart() const {
    if (horizontal) {
        return slider.GetLeft() - left;
    }
    else {
        return slider.GetTop() - top;
    }
}

void ScrollBar::SetSlider(float x, float y) {
    SetPosition(x, y);
    slider.SetPosition(x, y);
    if (horizontal) {
        slider.lefttopborder = left;
        slider.rightdownborder = left + width;
    }
    else {
        slider.lefttopborder = top;
        slider.rightdownborder = top + height;
    }
}

void ScrollBar::draw() {
    if (texture_descriptor > -1) {
        AdapterSFML::DrawSprite(left, top, width, height, colorfrg, texture_descriptor);
    }
    else 
        AdapterSFML::DrawRect(left, top, width, height, thickness, colorfrg, colorbkg);

    slider.draw();
}

float ScrollBar::GetLen() const {
    return length;
}

void ScrollBar::PressImpact(const Event &event) {
    if (!slider.IsHovered(event) && IsHovered(event)) {
        slider.SetPositionAlong(event);
        slider.CorrectBorders();
    }
        slider.PressImpact(event);
}
void ScrollBar::MoveImpact(const Event &event) {
        slider.MoveImpact(event);
}

void Slider::SetPositionAlong(const Event &event) {
    if (horizontal) 
        left = event.mouse.x;
    else 
        top = event.mouse.y;
}

void Slider::SetPositionRel(float coordinate) {
    if (horizontal) 
        left = lefttopborder + coordinate;
    else 
        top = lefttopborder + coordinate;
}

void Slider::draw() {
    AdapterSFML::DrawRect(left, top, width, height, thickness, colorfrg, colorbkg);
}

void Slider::PressImpact(const Event &event) {

    if (horizontal) {
        MousePositionBeforeSliding = event.mouse.x;
        SliderPositionBeforeSliding = left;
    }
    else {
        MousePositionBeforeSliding = event.mouse.y;
        SliderPositionBeforeSliding = top;
    }
}
void Slider::MoveImpact (const Event &event) {
    fprintf(stderr, "moving slider\n");
    if (horizontal) 
        left = SliderPositionBeforeSliding + event.mouse.x - MousePositionBeforeSliding;

    else 
        top  = SliderPositionBeforeSliding + event.mouse.y - MousePositionBeforeSliding;
    CorrectBorders();
}
void Slider::CorrectBorders() {
    if (horizontal) {
        if (left + width > rightdownborder) 
            left = rightdownborder - width;
        else if (left < lefttopborder)
            left = lefttopborder;
    }
    else {
        if (top + height > rightdownborder) 
            top = rightdownborder - height;
        else if (top < lefttopborder)
            top = lefttopborder;
    }
}

void ScrollBar::SetSliderPressedColor(const Color &color) {
    slider.SetPressedColor(color);
}
void ScrollBar::SetSliderNormalColor (const Color &color) {
    slider.SetNormalColor(color);
}
void ScrollBar::SetSliderHoveredColor(const Color &color) {
    slider.SetHoveredColor(color);
}



void Slider::SetPressedColor(const Color &color) {
    colorP = color;
}
void Slider::SetNormalColor (const Color &color) {
    colorN = color;
}
void Slider::SetHoveredColor(const Color &color) {
    colorH = color;
}

void Slider::SetHorizontal(bool horizontal) {
    this->horizontal = horizontal;
}


bool InputBox::IsActive() const {
    return active;
}

void InputBox::SwitchToN() {
    SetColorFrg(colorN);
}
void InputBox::SwitchToH() {
}
void InputBox::SwitchToP() {
    SetColorFrg(colorP);
}
void InputBox::ClickOutsideImpact() {
    active = false;
}

void InputBox::ClickImpact() {
    active = true;
}

void InputBox::HandleEvent(const Event &event) {
    if (event.EventType == EV_TEXT) {
        if (active) {
            if (event.key.character == '\b') {
                if (text.GetMessage().length() > 0)
                    text.GetMessage().pop_back();
            }

            else if (event.key.character != '\n') {
                text.GetMessage().push_back(event.key.character);
            }
        }
    }
    else {
        AbsB::HandleEvent(event);
    }
    //while inputbox is active it has a specific color
    if (active) 
        SwitchToP();
    else 
        SwitchToN();
}

void InputBox::draw() {
    AdapterSFML::DrawRect(left, top, width, height, thickness, colorfrg, colorbkg);
    text.draw();
}

void InputBox::SetPosition(float x, float y) {
    ShapeRect::SetPosition(x, y);
    text.SetPosition(x + 2, y);
}
void InputBox::SetSize(float x, float y) {
    ShapeRect::SetSize(x, y);
    text.SetSize(y - 4);
}

InputBox::InputBox() {
    UpdateThisMask(EV_TEXT);
    active = false;
    // ThisMask |= EV_TEXT;
}

Text &InputBox::GetText() {
    return text;
}
const Text &InputBox::GetText() const {
    return text;
}
