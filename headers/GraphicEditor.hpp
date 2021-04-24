
#include "UIBasic.hpp"

class ColorPaletteManager : public ContainerW, public ShapeRect {
    //just variables to initialize start positions and make creation process clearer
    constexpr const static char *palette_path = "../assets/colorpalette.png";
    constexpr const static char *slider_path = "../assets/slidertexture.jpg";
    constexpr const static char *pointer_path = "../assets/pointer.png";

    constexpr static float x = 400;
    constexpr static float y = 400;
    constexpr static float width = 600;
    constexpr static float height = 400;
    constexpr static Color BkGColor = {170, 170, 170};


    constexpr static int   length_Scrollbar = 360;
    constexpr static int   width_Scrollbar = 20;
    constexpr static bool  horizontal = false;
    constexpr static float x_scrollbar = x + 30;
    constexpr static float y_scrollbar = y + 20;
    constexpr static float slider_len = 5;

    constexpr static float x_palette = x + 70;
    constexpr static float y_palette = y + 20;
    constexpr static float palette_width = 360;
    constexpr static float palette_height = 360;

    constexpr static float x_HSVBoxStart = x_palette + palette_width + 20;
    constexpr static float y_HSVBoxStart = y + 20;
    constexpr static float HSVBox_space = 60;

    constexpr static float width_Box = 100;
    constexpr static float height_Box = 30;

    constexpr static Color InputBoxColor = {255, 255, 255};

    class ColorPalette : public AbsB {
        //keeps descriptor of a pointer texture
        int64_t pointer_descriptor = -1;

        //mask that will darken and lighten the palette
        ShapeRect mask;
        const Color maskcolor = {0, 0, 0, 0}; //opacity = 255 by default

        //coordinates of a chosen color to depict pointer
        float x_pointer = left;
        float y_pointer = top;

        float pointer_width = 40;
        float pointer_height = 40;

        Color current_color = {200, 200, 200};


        public:
        ColorPalette();
        void SetMaskOpacity(unsigned char opacity);
        void SetPosition(float x, float y) override;
        void SetSize(float x, float y) override;

        void SetPointerTexture(int64_t descriptor);

        void SetXPointerPosition(float x);
        void SetYPointerPosition(float y);

        void draw() override;

        void PressImpact(const Event &event) override;
        void MoveImpact(const Event &event) override;

        void PickColor();

        const Color &GetCurrentColor() const;
        Color &GetCurrentColor();

        float GetHue() const;
        float GetValue() const;
        float GetSaturation() const;
    };

    //exists just to influence RGBInputs and palette in ColorPalette
    //quite small classes so i decided to define them right here
    class HSVInput : public InputBox {
        public:
        void HandleEvent(const Event &event) {
            InputBox::HandleEvent(event);
            if (active && !static_cast<ColorPaletteManager*>(parent)->IsInputEmpty()) {
                static_cast<ColorPaletteManager*>(parent)->HSVInfluencesOnRGB();
                static_cast<ColorPaletteManager*>(parent)->PaletteChange();
                static_cast<ColorPaletteManager*>(parent)->ValueChanger();
            }
        }
    };
    //exists just to influence RGBInputs and palette in ColorPalette
    class RGBInput : public InputBox {
        public:
        void HandleEvent(const Event &event) {
            InputBox::HandleEvent(event);
            if (active && !static_cast<ColorPaletteManager*>(parent)->IsInputEmpty()) {
                static_cast<ColorPaletteManager*>(parent)->RGBInfluencesOnHSV();
                static_cast<ColorPaletteManager*>(parent)->PaletteChange();
                static_cast<ColorPaletteManager*>(parent)->ValueChanger();
            }
        }
    };

    ScrollBar *Value_Scrollbar = nullptr;
    ColorPalette *Palette = nullptr;


    HSVInput *HSVFields[3];
    RGBInput *RGBFields[3];

    std::string FieldNames[6] = {"Hue", "Saturation", "Value", "Red", "Green", "Blue"};
    Text BoxNames[6];
    void PaletteInfluencesOnInput();
    void PaletteChange();

    unsigned char Mask_opacity = 255;

    void RGBInfluencesOnHSV();
    void HSVInfluencesOnRGB();

public:
    void ValueChanger();
    ColorPaletteManager();
    void draw() override;
    void HandleEvent(const Event &event) override;
    HSV GetHSV() const;
    Color GetCurrColor();
    Color GetRGB() const;
    bool IsInputEmpty() const;

};

class AbsTool;

class Canvas : public AbsB {
    protected:
    const static int x = 100;
    const static int y = 0;
    const Color color = {255, 255, 255};

    uint32_t *Bimage = nullptr; 

    AbsTool *tool = nullptr;
    void HandleEvent(const Event &event) override;

public:
    AbsTool *GetTool();
    void SetTool(AbsTool* tool);

    void UpdateCanvas();
    //used when loading image from file
    void UpdateImage(const uint8_t *source, std::pair<int, int> size);

    Canvas(int w, int h);
    ~Canvas() override;

    uint32_t *GetBimage();
    void draw() override;

};

union SettingU {
    //holds some value when some option is chosen
    int option;
    bool checkbox;
    double slider_pos;
    char *str;
};

class SettingsCollection;

class AbsTool : public UsualB {
    protected:
    bool active = false;
    Color current_color = {0, 0, 0};

    void ClickImpact() override;
    void ClickOutsideImpact();
    void HandleEvent(const Event &event) override;

    SettingsCollection *settinger;
public:
    virtual void StartApplication(Canvas *canvas, int x, int y, Color &color, std::vector<SettingU> settings);
    AbsTool();
    virtual void apply(Canvas &canvas, int x, int y) = 0;
    virtual void endapplying();
    void SetActive(bool active);
    virtual void SetToolColor(const Color &color);
    SettingsCollection *GetSettings();    
};

class Brush : public AbsTool {
    //def Brush parameters
    const Color Dcolor = {255, 0, 0};
protected:
    uint32_t radius = 10;

public:
    Brush();
    void StartApplication(Canvas *canvas, int x, int y, Color &color, std::vector<SettingU> settings) override;
    void SetRadius(uint32_t radius);
    void apply(Canvas &canvas, int x, int y);
};

class Eraser : public Brush {
    const Color EraseColor = {255, 255, 255};
    public:
    void SetToolColor(const Color &color) override;
    void StartApplication(Canvas *canvas, int x, int y, Color &color, std::vector<SettingU> settings) override;
    Eraser();
};

class SettingsCollection;

class ToolMenu : public ContainerW, public ShapeRect {
    AbsTool *current_tool = nullptr;

    public:
    SettingsCollection *SetNewActive(AbsTool *tool);
    void Deactivate();
    AbsTool *GetActive();
    ToolMenu();
    void draw() override;
    void AppendTool(AbsTool *tool);
};

class CheckBox : public UsualB {
    protected:
    bool active = false;

public:
    void HandleEvent(const Event &event) override;
    void Deactivate();
    void Activate();
};

//holds int value, and, when active, may return it
class Optional : public CheckBox {
    protected:
    int value = 0;
    public:
    int GetValue() const;
    void SetValue(int val);
    void ClickImpact() override;
};

class Settings : public ContainerW, public ShapeRect {
    public:
    virtual SettingU getSettingValue();
};

class SetOptionals : public Settings {
    protected:
    std::vector<Optional *>optionals = {};
    Optional *active = nullptr;
    public:
    void SetActive(Optional *option);
    void AppendUoptionals(Optional *option);
    SettingU getSettingValue() override;
    //making shell is better
    std::vector<Optional *> &GetAllOptions();
};

class SettingsCollection : public ContainerW, public ShapeRect {
    protected:
    std::map<int, Settings*> settings;
    int GeneralHeight = 0;
    public:
    void AppendSetting(int key, Settings *setting);
    std::map<int, Settings*> GetSettings();
    void draw() override;
};

class SettingsManager : public ShapeRect, public ContainerW {
    SettingsCollection *current_settings = nullptr;

    public:
    SettingsManager();
    std::vector<SettingU> GetCurrSettings();
    void SetCurrentSettings(SettingsCollection *settings);
    void draw() override;
    ~SettingsManager() override;
    void HandleEvent(const Event &event) override;
};

//button for modal windows (or not only them?!)
//which launches some positive action in modal window
//it's like "OK" button, or "Next" button
class ActionButton : public UsualB {
    void ClickImpact() override;
};

class SavePicMW : public ModalW {

    InputBox *path = nullptr;
    uint32_t saving_descriptor = 0;
    public:
    SavePicMW();
    void SetSavingDescr(uint32_t descr);
    void Action() override;
    void draw() override;
};
class LoadPicMW : public ModalW {
    Canvas *canvas; 
    InputBox *path = nullptr;
    uint32_t saving_descriptor = 0;
    public:
    LoadPicMW();
    void SetModifiableCanvas(Canvas *canv);
    void SetSavingDescr(uint32_t descr);
    void Action() override;
    void draw() override;
};

class ButtonModalW : public UsualB {
    protected:
    ModalW *mwindow = nullptr;
    void ClickImpact() override;
    
public:
    void SetMW(ModalW *mw);
};

class DrawManager : public ContainerW {
    Canvas              *canvas = nullptr;
    ToolMenu            *toolmenu = nullptr;
    SettingsManager     *settingsmenu = nullptr;
    ColorPaletteManager *palettemenu = nullptr;

public:
    DrawManager();

    void SetSettingsCollection(SettingsCollection *settings);
    void ApplyTool(Canvas &canvas, int x, int y);
    void StartApplicationTool(int x, int y);
    void EndApplicationTool();
    //~DrawManager() = default;
};
