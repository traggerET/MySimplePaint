
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
    //for complex elements you can set your own Set's actions
    virtual void SetPosition(float x, float y);
    virtual void SetSize(float x, float y);
    void SetColorFrg(const Color &color);
    void SetColorBcg(const Color &color);
    void SetThickness(float thickness);
    const Color &GetColorBcg() const;
    const Color &GetColorFrg() const;
    Color &GetColorBcg();
    Color &GetColorFrg();
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
    AbsW(); 
    AbsW *GetParent();
    void LinkParent(AbsW *parent);
    void DelinkParent();
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
    std::list<AbsW *> &GetChildren();
    ~ContainerW() override;
};

//AbsButton shouldnt inherit any shape but as all its subclasses have the same shape, i've simplified it
class AbsB : public ContainerW, public ShapeRect {
public:
    enum STATE {NORMAL, PRESSED, HOVERED};

protected:
    int64_t texture_descriptor;
    virtual void SwitchToN();
    virtual void SwitchToH();
    virtual void SwitchToP();
    virtual void PressImpact(const Event &event);
    virtual void MoveImpact (const Event &event);
    virtual void ClickImpact();
    virtual void ClickOutsideImpact();
    STATE state;

public:
    AbsB();
    void HandleEvent(const Event &event) override;
    void ResetTexture();
    void SetTexture(int64_t descriptor);
    uint32_t GetDescr() const;
    void draw() override;
    bool IsHovered       (const Event &event) const;
    bool IsPressed       (const Event &event) const;
    bool IsReleased      (const Event &event) const;
    bool IsMoved         (const Event &event) const;
    bool IsPressedOutside(const Event &event) const;
    STATE GetState() const ;
    void SetState(STATE state) {
        this->state = state; 
    }
};

class InputBox : public AbsB {
protected:
    Text text;
    bool active;

    Color colorN = {255, 255, 255};
    Color colorP = {200, 200, 200};
    Color colorH = {255, 255, 255};
    void SwitchToN() override;
    void SwitchToH() override;
    void SwitchToP() override;
    void ClickImpact() override;
    void ClickOutsideImpact() override;

public:
    InputBox();
    Text &GetText();
    const Text &GetText() const;
    void SetPosition(float x, float y) override;
    void SetSize(float x, float y) override;
    void draw() override;
    void HandleEvent(const Event &event) override;
    bool IsActive() const;
};

class Slider : public AbsB {
protected:
    friend class ScrollBar;
    Color colorN = {255, 0, 0};
    Color colorH = {255, 0, 0};
    Color colorP = {255, 0, 0};
    float MousePositionBeforeSliding;
    float SliderPositionBeforeSliding;
    float lefttopborder = 0;
    float rightdownborder = 100;
    bool horizontal;
    void MoveImpact (const Event &event) override;
    void PressImpact(const Event &event) override;
    void CorrectBorders();

public:
    //sets slider in position where mouse is situated
    void SetPositionAlong(const Event &event);
    //sets slider position relatibely its lefttop border
    void SetPositionRel(float coordinate);
    void SetPressedColor(const Color &color);
    void SetNormalColor (const Color &color);
    void SetHoveredColor(const Color &color);
    void SetHorizontal  (bool horizontal);
    void draw() override;
};

class ScrollBar : public AbsB {
    friend Slider;

protected:
    float length;
    bool horizontal;
    Slider slider;
    void PressImpact(const Event &event) override;
    void MoveImpact (const Event &event) override;

public:
    ScrollBar(bool horizontal, float length, float width);
    bool IsOnSlider (const Event &event);
    //set left top position of the whole scrollbar and align its slider
    const Slider &GetSlider() const;
    void draw() override;
    void SetSlider(float x, float y);
    void SetSliderSize(float x, float y);
    void SetSliderPressedColor(const Color &color);
    void SetSliderNormalColor (const Color &color);
    void SetSliderHoveredColor(const Color &color);
    void SetPositionRel(float coordinate);
    float GetOffsetRelScrollbarStart() const;
    float GetLen() const;
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
protected:
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
protected:
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
    void SwitchToN() override;
    void SwitchToH() override;
    void SwitchToP() override;

public:
    UsualB();
    void SetPressedColor(const Color &color);
    void SetNormalColor (const Color &color);
    void SetHoveredColor(const Color &color);
    void draw() override;
    Text &GetTitle();
    const Text &GetTitle() const;
};