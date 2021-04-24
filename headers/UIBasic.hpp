
#include "Adapter.hpp"
#include <list>
class Text {
protected:
    std::string message = "";
    float size = 20;

    float left = 100;
    float top = 100;

public:
    void SetMessage(const std::string &message);
    void SetSize(float size);
    void SetPosition(float x, float y);
    void draw();
    const std::string &GetMessage() const;
    std::string &GetMessage();

};


class ShapeRect {
protected:
    float left = 0;
    float top = 0;
    
    float width = 100;
    float height = 100;

    float thickness = 0;

    Color colorfrg = {255, 0, 0};
    Color colorbkg = {255, 0, 0};
public:

    void draw();
    bool Contains(float x, float y) const;

    virtual void SetPosition(float x, float y);
    void SetColorFrg(const Color &color);
    void SetColorBcg(const Color &color);
    Color &GetColorBcg();
    Color &GetColorFrg();
    const Color &GetColorBcg() const;
    const Color &GetColorFrg() const;
    void SetThickness(float thickness);
    virtual void SetSize(float x, float y);

    float GetLeft() const;
    float GetTop() const;
    float GetWidth() const;
    float GetHeight() const;
    virtual ~ShapeRect() = default;
};

class ModalW;

class AbsW {
protected:
    AbsW *parent = nullptr;

    int ThisMask; // includes propagation mask also
    int PropagationMask;

    void UpdateThisMask(int update);
    void UpdatePropMask(int update);

public:
    AbsW *GetParent();
    void LinkParent(AbsW *parent);
    void DelinkParent();

    AbsW();

    virtual void InvokeModalWindow(ModalW *mw);
    virtual void HandleEvent(const Event &event) = 0;
    virtual void draw() = 0;
    virtual ~AbsW() = default;
};

class ContainerW : public AbsW {
protected:
    std::list<AbsW*> children;
public:
    void HandleEvent(const Event &event) override;
    void LinkChild(AbsW *child);
    void DelinkChild(AbsW *child);
    void draw() override;
    ~ContainerW() override;
    std::list<AbsW *> &GetChildren();
};

//AbsButton shouldnt inherit any shape but as all its subclasses have the same shape, i've simplified it
class AbsB : public ContainerW, public ShapeRect {
protected:

    //There are some objects which have field "active". It would be better to
    //put it here and then just redefine the way subclasses treat this field.

    int64_t texture_descriptor;

    virtual void SwitchToN();
    virtual void SwitchToH();
    virtual void SwitchToP();

public:
    void HandleEvent(const Event &event) override;
    virtual void PressImpact(const Event &event);
    virtual void MoveImpact (const Event &event);
    virtual void ClickImpact();
    virtual void ClickOutsideImpact();

    void ResetTexture();
    void SetTexture(int64_t descriptor);
    uint32_t GetDescr() const;

    void draw() override;

    enum STATE {NORMAL, PRESSED, HOVERED};
    STATE state;

    bool IsHovered       (const Event &event) const;
    bool IsPressed       (const Event &event) const;
    bool IsReleased      (const Event &event) const;
    bool IsMoved         (const Event &event) const;
    bool IsPressedOutside(const Event &event) const;

    STATE GetState() const ;
    AbsB();
};

class InputBox : public AbsB {
protected:
    Text text;
    bool active;

    Color colorN = {255, 255, 255};
    Color colorP = {200, 200, 200};
    Color colorH = {255, 255, 255};

public:
    InputBox();
    Text &GetText();
    const Text &GetText() const;
    void SetPosition(float x, float y) override;
    void SetSize(float x, float y) override;
    void draw() override;
    void HandleEvent(const Event &event) override;
    void ClickImpact() override;
    void ClickOutsideImpact() override;

    bool IsActive() const;

    void SwitchToN() override;
    void SwitchToH() override;
    void SwitchToP() override;
};

class Slider : public AbsB {
protected:


    Color colorN = {255, 0, 0};
    Color colorH = {255, 0, 0};
    Color colorP = {255, 0, 0};

    float MousePositionBeforeSliding;
    float SliderPositionBeforeSliding;

    bool horizontal;
public:
    float lefttopborder = 0;
    float rightdownborder = 100;
    friend class Scrollbar;

    //Slider();
    //sets slider in position where mouse is situated
    void SetPositionAlong(const Event &event);
    //sets slider position relatibely its lefttop border
    void SetPositionRel(float coordinate);

    void SetPressedColor(const Color &color);
    void SetNormalColor (const Color &color);
    void SetHoveredColor(const Color &color);
    void SetHorizontal  (bool horizontal);

    void MoveImpact (const Event &event) override;
    void PressImpact(const Event &event) override;

    void draw() override;

    void CorrectBorders();
};


class ScrollBar : public AbsB {
protected:
    friend Slider;
    
    float length;
    bool horizontal;
    Slider slider;
public:

    float GetLen() const;
    //const Slider &GetSlider() const;
    //Slider &GetSlider();
    void SetSliderSize(float x, float y);
    ScrollBar(bool horizontal, float length, float width);
    void PressImpact(const Event &event) override;
    void MoveImpact (const Event &event) override;
    bool IsOnSlider (const Event &event);

    //set left top position of the whole scrollbar and align its slider
    void SetSlider(float x, float y);
    const Slider &GetSlider() const;
    void draw() override;

    float GetOffsetRelScrollbarStart() const;

    void SetSliderPressedColor(const Color &color);
    void SetSliderNormalColor (const Color &color);
    void SetSliderHoveredColor(const Color &color);

    void SetPositionRel(float coordinate);
};

class View : public ContainerW {  
protected:
    int left;
    int top;

    int width;
    int height;

public:
    void SetPosition(int left, int top);
    void SetSize(int width, int height);


    void draw() override;
};


class ModalW : public ContainerW, public ShapeRect {

    bool invoked;

public:
    ModalW();
    virtual void Action() {} 
    void InvokeMW();
    void DeinvokeMW();
    void draw() override;
    void HandleEvent(const Event &event) override;
    bool IsInvoked() {
        return invoked;
    }
};

class ModalWManager : public ContainerW {

    ModalW *modal = nullptr;
    public:
    void SetModalW(ModalW *mw) {
        modal = mw;
    }
    void InvokeModalWindow(ModalW *mw) override;
    void HandleEvent(const Event &ev) override;
    void draw() override;
};


class UsualB : public AbsB {
protected:
    Text title;

    Color colorN = {255, 0, 0};
    Color colorH = {255, 0, 0};
    Color colorP = {255, 0, 0};

public:
    UsualB();
    void SetPressedColor(const Color &color);
    void SetNormalColor (const Color &color);
    void SetHoveredColor(const Color &color);

    void SwitchToN() override;
    void SwitchToH() override;
    void SwitchToP() override;

    Text &GetTitle();
    const Text &GetTitle() const;

    void draw() override;
};