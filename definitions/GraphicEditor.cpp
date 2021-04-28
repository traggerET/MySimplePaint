#include "./headers/GraphicEditor.hpp"

#include <random>
#include <time.h>

namespace HSVRGB {

Color HSVtoRGB(float H, float S,float V) {

    float s = S / 100;
    float v = V / 100;
    float C = s * v;
    float X = C * (1 - fabs(fmod(H / 60.0, 2) - 1));
    float m = v - C;
    float r, g, b;
    if (H >= 0 && H < 60){
        r = C, g = X, b = 0;
    }
    else if (H >= 60 && H < 120) {
        r = X, g = C, b = 0;
    }
    else if (H >= 120 && H < 180) {
        r = 0, g = C, b = X;
    }
    else if (H >= 180 && H < 240) {
        r = 0, g = X, b = C;
    }
    else if (H >= 240 && H < 300) {
        r = X, g = 0, b = C;
    }
    else {
        r = C, g = 0, b = X;
    }
    unsigned char R = (r + m) * 255;
    unsigned char G = (g + m) * 255;
    unsigned char B = (b + m) * 255;

    return {R, G, B};
}

HSV RGBtoHSV(unsigned char R_color, unsigned char  G_color, unsigned char B_color) {
    HSV hsv;
    float Eps = 1e-6;

    float max = std::max(std::max(R_color, G_color), B_color) / 255.0;
    float min = std::min(std::min(R_color, G_color), B_color) / 255.0;
    
    float R = R_color / 255.0;
    float G = G_color / 255.0;
    float B = B_color / 255.0;

    if (fabs(max - min) >= Eps) {
        if (fabs(max - R) < Eps && (fabs(G - B) < Eps || G > B)) {
            hsv.hue = 60 * (G - B) / (max - min);
        }
        else if (fabs(max - R) < Eps && G < B) {
            hsv.hue = 60 * (G - B) / (max - min) + 360;
        }
        else if (fabs(max - G) < Eps) {
            hsv.hue = 60 * (B - R) / (max - min) + 120;
        }
        else if (fabs(max - B) < Eps)
            hsv.hue = 60 * (R - G) / (max - min) + 240;
    }
    else hsv.hue = NAN;

    if (max == 0) hsv.sat = 0;
    else          hsv.sat = (1 - min / max) * 100;

    hsv.val = max * 100;
    return hsv;
}
}
namespace ByteCode {
    uint32_t ByteCodeFromColor(const Color &color) {
        uint8_t *bit = new uint8_t[4];

        bit[0] = color.red;
        bit[1] = color.green;
        bit[2] = color.blue;
        bit[3] = color.opacity;

        auto result = *(reinterpret_cast<uint32_t*>(bit));
        delete[] bit;
        return result;
    }
}

constexpr Color ColorPaletteManager::BkGColor;
constexpr Color ColorPaletteManager::InputBoxColor;

ColorPaletteManager::ColorPalette::ColorPalette() {
    mask.SetColorBcg(ColorPalette::maskcolor);
    mask.SetColorFrg(ColorPalette::maskcolor);
}
Color ColorPaletteManager::GetCurrColor() {
    return Palette->GetCurrentColor();
}
//
void ColorPaletteManager::ColorPalette::SetMaskOpacity(unsigned char opacity) {
    mask.GetColorFrg().opacity = opacity;
}
void ColorPaletteManager::ColorPalette::SetPosition(float x, float y) {
    ShapeRect::SetPosition(x, y);
    SetXPointerPosition(x);
    SetYPointerPosition(y);
    mask.SetPosition(x, y);
}
void ColorPaletteManager::ColorPalette::SetSize(float x, float y) {
    ShapeRect::SetSize(x, y);
    mask.SetSize(x, y);
}

void ColorPaletteManager::ColorPalette::SetPointerTexture(int64_t descriptor) {
    pointer_descriptor = descriptor;
}

void ColorPaletteManager::ColorPalette::SetXPointerPosition(float x) { 
    x_pointer = x;
}
void ColorPaletteManager::ColorPalette::SetYPointerPosition(float y) { 
    y_pointer = y;
}

void ColorPaletteManager::ColorPalette::draw() {
    AdapterSFML::DrawSprite(left, top, width, height, {255, 255, 255}, texture_descriptor);

    mask.draw();

    AdapterSFML::DrawSprite(x_pointer - pointer_width / 2, y_pointer - pointer_height / 2, pointer_width, pointer_height, {255, 255, 255}, pointer_descriptor);

}

void ColorPaletteManager::ColorPalette::PressImpact(const Event &event) {
    if (event.mouse.x >= left && event.mouse.x <= left + width)
        SetXPointerPosition(event.mouse.x);
    if (event.mouse.y >= top && event.mouse.y <= top + height)
        SetYPointerPosition(event.mouse.y);

    static_cast<ColorPaletteManager*>(parent)->UpdateInput();

}
void ColorPaletteManager::ColorPalette::MoveImpact(const Event &event) {
    if (event.mouse.x >= left && event.mouse.x <= left + width)
        SetXPointerPosition(event.mouse.x);
    if (event.mouse.y >= top && event.mouse.y <= top + height)
        SetYPointerPosition(event.mouse.y);

    static_cast<ColorPaletteManager*>(parent)->UpdateInput();
}


//modify all input boxes if palette has been modified
void ColorPaletteManager::UpdateInput() {

    HSV hsvcolor = {Palette->GetHue(), Palette->GetSaturation(), Palette->GetValue()};
    Color newcolor = HSVRGB::HSVtoRGB(hsvcolor.hue, hsvcolor.sat, hsvcolor.val);

    HSVFields[0]->GetText().SetMessage(std::to_string(static_cast<int>(hsvcolor.hue)));

    HSVFields[1]->GetText().SetMessage(std::to_string(static_cast<int>(hsvcolor.sat)));

    HSVFields[2]->GetText().SetMessage(std::to_string(static_cast<int>(hsvcolor.val)));

    RGBFields[0]->GetText().SetMessage(std::to_string(newcolor.red));

    RGBFields[1]->GetText().SetMessage(std::to_string(newcolor.green));

    RGBFields[2]->GetText().SetMessage(std::to_string(newcolor.blue));
}

void ColorPaletteManager::UpdatePalette() {

    HSV hsvcolor = GetHSV();
    Palette->SetXPointerPosition(static_cast<int>(Palette->GetWidth() * hsvcolor.hue / 360 + Palette->GetLeft()));
    Palette->SetYPointerPosition(static_cast<int>(Palette->GetHeight() + Palette->GetTop() - hsvcolor.sat * Palette->GetHeight() / 100));
    Value_Scrollbar->SetPositionRel((100 - hsvcolor.val) / 100 * length_Scrollbar);

}
const Color &ColorPaletteManager::ColorPalette::GetCurrentColor() const {
    return current_color;
}
Color &ColorPaletteManager::ColorPalette::GetCurrentColor() {
    return current_color;
}

float ColorPaletteManager::ColorPalette::GetHue() const {
    return (x_pointer - left) / width * 360;
}
float ColorPaletteManager::ColorPalette::GetValue() const {
    return (255 - static_cast<float>(mask.GetColorFrg().opacity)) / 255.0 * 100.0;
}
float ColorPaletteManager::ColorPalette::GetSaturation() const {
    return (1 - (y_pointer - top) / height) * 100;
}

void ColorPaletteManager::ColorPalette::PickColor() {
    auto Hue = ceil(GetHue());
    auto Saturation = ceil(GetSaturation());
    auto Value = ceil(GetValue());
    current_color = HSVRGB::HSVtoRGB(Hue, Saturation, Value);
}

//change RGB variables if HSV parameters has been modified
void ColorPaletteManager::UpdateHSV() {

    Color rgbcolor = GetRGB();

    HSV newhsv = HSVRGB::RGBtoHSV(rgbcolor.red, rgbcolor.green, rgbcolor.blue);

    if (!std::isnan(newhsv.hue)) {
        HSVFields[0]->GetText().SetMessage(std::to_string(static_cast<int>(newhsv.hue)));
    }

    HSVFields[1]->GetText().SetMessage(std::to_string(static_cast<int>(newhsv.sat)));
    HSVFields[2]->GetText().SetMessage(std::to_string(static_cast<int>(newhsv.val)));
}

//change HSV variables if RGB parameters has been modified
void ColorPaletteManager::UpdateRGB() {

    HSV hsvcolor = GetHSV();

    Color newcolor = HSVRGB::HSVtoRGB((hsvcolor.hue), (hsvcolor.sat), (hsvcolor.val));

    RGBFields[0]->GetText().SetMessage(std::to_string(newcolor.red));
    RGBFields[1]->GetText().SetMessage(std::to_string(newcolor.green));
    RGBFields[2]->GetText().SetMessage(std::to_string(newcolor.blue));
}

HSV ColorPaletteManager::GetHSV() const {
    auto Hue        = std::stof(HSVFields[0]->GetText().GetMessage());
    auto Saturation = std::stof(HSVFields[1]->GetText().GetMessage());
    auto Value      = std::stof(HSVFields[2]->GetText().GetMessage());
    return {Hue, Saturation, Value};
}

Color ColorPaletteManager::GetRGB() const {
    unsigned char Red   = std::stoul(RGBFields[0]->GetText().GetMessage());
    unsigned char Green = std::stoul(RGBFields[1]->GetText().GetMessage());
    unsigned char Blue  = std::stoul(RGBFields[2]->GetText().GetMessage());
    return {Red, Green, Blue};
}
void ColorPaletteManager::HandleEvent(const Event &event) {
    ContainerW::HandleEvent(event);

    if (Value_Scrollbar->GetState() == AbsB::STATE::PRESSED) {
        UpdateValues();
        UpdateRGB();
    }

    if (Palette->GetState() == AbsB::STATE::PRESSED) {
        UpdateInput();
    }
    Palette->PickColor();            
}
bool ColorPaletteManager::IsInputEmpty() const {
    for (auto i = 0; i < 3; i++) {
        if (HSVFields[i]->GetText().GetMessage() == "") {
            return true;
        }
    }
    for (auto i = 0; i < 3; i++) {
        if (RGBFields[i]->GetText().GetMessage() == "") {
            return true;
        }
    }
    return false;
}

ColorPaletteManager::ColorPaletteManager() {

    SetPosition(x, y);
    SetSize(width, height);
    SetColorFrg({223, 223, 223});
    SetThickness(0);

    Value_Scrollbar = new ScrollBar(horizontal, length_Scrollbar, width_Scrollbar);
    Palette = new ColorPalette();

    Palette->SetPosition(x_palette, y_palette);
    Palette->SetSize(palette_width, palette_height);
    Palette->SetTexture(AdapterSFML::LoadTexture(palette_path));
    Palette->SetPointerTexture(AdapterSFML::LoadTexture(pointer_path));
    
    Value_Scrollbar->SetSliderSize(width_Scrollbar, slider_len);
    Value_Scrollbar->SetPosition(x_scrollbar, y_scrollbar);
    Value_Scrollbar->SetSlider(x_scrollbar, y_scrollbar);
    Value_Scrollbar->SetSize(width_Scrollbar, length_Scrollbar);
    Value_Scrollbar->SetTexture(AdapterSFML::LoadTexture(slider_path));
    Value_Scrollbar->SetColorFrg({255, 255, 255});
    
    for (auto i = 0; i < 3; i++) {
        HSVFields[i] = new HSVInput;
        HSVFields[i]->SetPosition(x_HSVBoxStart, y_HSVBoxStart + i * HSVBox_space + 27);
        HSVFields[i]->SetSize(width_Box, height_Box);

        HSVFields[i]->GetText().SetSize(18);
        HSVFields[i]->GetText().SetMessage("240");
        HSVFields[i]->GetText().SetPosition(x_HSVBoxStart + 4, y_HSVBoxStart + i * HSVBox_space + 27);

        BoxNames[i].SetSize(14);
        BoxNames[i].SetMessage(FieldNames[i]);
        BoxNames[i].SetPosition(x_HSVBoxStart + 4, y_HSVBoxStart + i * HSVBox_space + 6);
        LinkChild(HSVFields[i]);
    }
    for (auto i = 3; i < 6; i++) {
        RGBFields[i - 3] = new RGBInput;
        //RGBFields[i]->SetColorFrg(InputBoxColor);
        RGBFields[i - 3]->SetPosition(x_HSVBoxStart, y_HSVBoxStart + i * HSVBox_space + 27);
        RGBFields[i - 3]->SetSize(width_Box, height_Box);

        RGBFields[i - 3]->GetText().SetSize(18);
        RGBFields[i - 3]->GetText().SetMessage("240");
        RGBFields[i - 3]->GetText().SetPosition(x_HSVBoxStart + 4, y_HSVBoxStart + i * HSVBox_space + 27);

        BoxNames[i].SetSize(14);
        BoxNames[i].SetMessage(FieldNames[i]);
        BoxNames[i].SetPosition(x_HSVBoxStart + 4, y_HSVBoxStart + i * HSVBox_space + 6);
        LinkChild(RGBFields[i - 3]);
    }
    UpdateInput();
    

    LinkChild(Value_Scrollbar);
    LinkChild(Palette);
}
void ColorPaletteManager::draw() {
    AdapterSFML::DrawRect(x, y, width, height, thickness, BkGColor, BkGColor);

    ContainerW::draw();

    for (auto i = 0; i < 6; i++) {
        BoxNames[i].draw();
    }
}
//changes V - Value in HSV
void ColorPaletteManager::UpdateValues() {
    float diff = Value_Scrollbar->GetOffsetRelScrollbarStart();
    float part = diff / Value_Scrollbar->GetLen();

    unsigned char opacity = static_cast<unsigned char>(part * 255);
    Palette->SetMaskOpacity(opacity);

    int Value = static_cast<int>((1 - opacity / 255.0) * 100);
    HSVFields[2]->GetText().SetMessage(std::to_string(Value));
}


void Canvas::HandleEvent(const Event &event) {
    STATE ex_state = state;
    if (IsPressed(event)) {
        state = PRESSED;
        static_cast<DrawManager*>(parent)->StartApplicationTool(event.mouse.x - left, event.mouse.y - top);
    }
    else if (IsReleased(event) && state == PRESSED) {
        state = NORMAL;
        static_cast<DrawManager*>(parent)->EndApplicationTool();
    }
    if (IsHovered(event) && state == PRESSED) {
        static_cast<DrawManager*>(parent)->ApplyTool(*this, event.mouse.x - left, event.mouse.y - top);
        UpdateCanvas();
    }
}

AbsTool *Canvas::GetTool() {
    return tool;
}
void Canvas::SetTool(AbsTool* tool) {
    this->tool = tool;
}

Canvas::Canvas(int w, int h) {
    Bimage = new uint32_t[w * h];

    SetPosition(x, y);
    SetSize(w, h);

    auto Bcolor = ByteCode::ByteCodeFromColor(color);

    //fill canvas with white color
    for (auto i = 0; i < h * w; i++) {
        Bimage[i] = Bcolor;
    }
    

    texture_descriptor = AdapterSFML::PushEmptyTexture(w, h);

    AdapterSFML::UpdateTexture(Bimage, texture_descriptor);

}
Canvas::~Canvas() {
    //delete tool;
    delete[] Bimage;
}
uint32_t *Canvas::GetBimage() {
    return Bimage;
}

void Canvas::UpdateCanvas() {
    AdapterSFML::UpdateTexture(Bimage, texture_descriptor);
}
void Canvas::UpdateImage(const uint8_t *source, std::pair<int, int> size) {
    //auto scale = std::max(size.first / static_cast<double>(width), size.second / static_cast<double>(height));
    SetSize(size.first, size.second);
    delete[] Bimage;
    Bimage = new uint32_t[size.first * size.second];
    auto bits8 = reinterpret_cast<uint8_t*>(Bimage);
    for (uint32_t i = 0; i < size.first * size.second * 4; i++) {
        bits8[i] = source[i];
    }
}

void Canvas::draw() {
    AdapterSFML::DrawSprite(x, y, width, height, {255, 255, 255, 255}, texture_descriptor);
}


void AbsTool::SetToolColor(const Color &color) {
    current_color = color;
}
void AbsTool::StartApplication(Canvas *canvas, int x, int y, Color &color, std::vector<SettingU> settings) {
}
void AbsTool::endapplying() {}

SettingsCollection *AbsTool::GetSettings() {
    return settinger;
}

void AbsTool::SetActive(bool active) {
    this->active = active;
    if (active) SwitchToP();
    else        SwitchToN();

}
AbsTool::AbsTool() {
    settinger = nullptr;
}
void AbsTool::HandleEvent(const Event &event) {
    AbsB::HandleEvent(event);

    if (active) SwitchToP();
    else        SwitchToN();
}
void AbsTool::ClickImpact() {
    active = true;
    if (parent) {
        auto cparent = static_cast<ToolMenu*>(parent);
        cparent->Deactivate();
        auto csettings = cparent->SetNewActive(this);
        static_cast<DrawManager*>(cparent->GetParent())->SetSettingsCollection(csettings);
    }
}
void AbsTool::ClickOutsideImpact() {
}
void Brush::SetRadius(uint32_t radius) {
    this->radius = radius;
}

Brush::Brush() {
    current_color = Dcolor;
    texture_descriptor = AdapterSFML::LoadTexture("../assets/brush.jpg");
    SetOptionals *radia = new SetOptionals;

    SetColorFrg({180, 180, 180});
    SetNormalColor({240, 220, 220});
    SetHoveredColor({220, 200, 200});
    SetPressedColor({180, 180, 180});

    Optional *width20 = new Optional;
    width20->SetPressedColor({100, 100, 100});
    width20->SetNormalColor({140, 140, 140});
    width20->SetSize(40, 40);
    width20->SetValue(20);
    width20->SetPosition(40, 15);

    Optional *width10 = new Optional;
    width10->SetPressedColor({100, 100, 100});
    width10->SetNormalColor({140, 140, 140});
    width10->SetSize(25, 25);
    width10->SetValue(10);
    width10->SetPosition(45, 15 + 50);

    Optional *width5  = new Optional;
    width5->SetPressedColor({100, 100, 100});
    width5->SetNormalColor({140, 140, 140});
    width5->SetSize(15, 15);
    width5->SetValue(5);
    width5->SetPosition(50, 15 + 40 + 45);

    radia->AppendUoptionals(width10);
    radia->AppendUoptionals(width20);
    radia->AppendUoptionals(width5);

    radia->SetActive(width10);

    settinger = new SettingsCollection;
    settinger->AppendSetting(0, radia);
    LinkChild(settinger);
}  

void Brush::StartApplication(Canvas *canvas, int x, int y, Color &color, std::vector<SettingU> settings) {
    current_color = color;
    radius = settings[0].option;
}
void Brush::apply(Canvas &canvas, int x, int y) {
    fprintf(stderr, "In BRush applying");
    auto *Bimage = canvas.GetBimage(); 
    auto xt = x;
    auto yt = y;
    fprintf(stderr, "%d %d\n", xt, yt);

    int x_from = std::max(xt - static_cast<int>(radius), 0);
    int y_from = std::max(yt - static_cast<int>(radius), 0);

    int x_to = std::min(xt + radius, static_cast<uint32_t>(canvas.GetWidth()));
    int y_to = std::min(yt + radius, static_cast<uint32_t>(canvas.GetHeight()));

    if (x_to < x_from || y_to < y_from) {
        return;
    }

    uint32_t color = ByteCode::ByteCodeFromColor(current_color);

    for (uint32_t x_cur = x_from; x_cur < x_to; x_cur++) {
        for (uint32_t y_cur = y_from; y_cur < y_to; y_cur++) {
            if ((xt - x_cur) * (xt - x_cur) + (yt - y_cur) * (yt - y_cur) <= radius * radius) {
                Bimage[y_cur * static_cast<uint32_t>(canvas.GetWidth()) + x_cur] = color;
            }
        }
    }   
}

Eraser::Eraser() {
    current_color = EraseColor;
    texture_descriptor = AdapterSFML::LoadTexture("../assets/eraser.png");
    SetTexture(texture_descriptor);
    SetColorFrg({180, 180, 180});
    SetNormalColor({240, 220, 220});
    SetHoveredColor({220, 200, 200});
    SetPressedColor({180, 180, 180});

}
void Eraser::SetToolColor(const Color &color) {}
void Eraser::StartApplication(Canvas *canvas, int x, int y, Color &color, std::vector<SettingU> settings) {
    current_color = EraseColor;
    radius = settings[0].option;
}

void ToolMenu::Deactivate() {
    current_tool->SetActive(false);
}
void ToolMenu::draw() {
    ShapeRect::draw();
    ContainerW::draw();
}

SettingsCollection *ToolMenu::SetNewActive(AbsTool *tool) {
    current_tool = tool;
    current_tool->SetActive(true);
    return current_tool->GetSettings();
}
AbsTool *ToolMenu::GetActive() {
    return current_tool;
}

ToolMenu::ToolMenu(): current_tool(nullptr) {
    SetPosition(0, 0);
    SetColorFrg({255, 220, 220});
    SetSize(100, 400);
}

void ToolMenu::AppendTool(AbsTool *tool) {
    tool->SetSize(80, 70);
    tool->SetPressedColor({200, 200, 200});
    tool->SetPosition(left + 10, top + 10 + 80 * children.size());
    LinkChild(tool);//
}
void SetOptionals::SetActive(Optional *option) {
    if (active) {
        active->Deactivate();
    }
    option->Activate();
    active = option;
}
void SetOptionals::AppendUoptionals(Optional *option) {
    optionals.push_back(option);
    LinkChild(option);
}
SettingU SetOptionals::getSettingValue() {
    SettingU result;
    result.option = active->GetValue();
    return result;
}
std::vector<Optional *> &SetOptionals::GetAllOptions() {
    return optionals;
}
void SettingsCollection::draw() {
    AdapterSFML::PushGlobalOffset(-left, -top);
    ContainerW::draw();
    AdapterSFML::PopGlobalOffset();
}
std::map<int, Settings*> SettingsCollection::GetSettings() {
    return settings;
}
void SettingsCollection::AppendSetting(int key, Settings *setting) {
    setting->SetPosition(0, GeneralHeight);
    GeneralHeight += setting->GetHeight();

    LinkChild(setting);
    settings[key] = setting;
}

void CheckBox::HandleEvent(const Event &event) {
    UsualB::HandleEvent(event);

    if (active) SwitchToP();
    else        SwitchToN();
}
void CheckBox::Deactivate() {
    active = false;
    SwitchToN();
}
void CheckBox::Activate() {
    active = true;
    SwitchToP();
}

void Optional::ClickImpact() {
    static_cast<SetOptionals*>(parent)->SetActive(this);
}
int Optional::GetValue() const {
    fprintf(stderr, "%d - value in GetVAlue\n", value);
    return value;
}
void Optional::SetValue(int val) {
    value = val;
    fprintf(stderr, "%d - value after constructing\n", value);
}


SettingU Settings::getSettingValue() {
    fprintf(stderr, "just helpless function\n");
    SettingU result;
    return result;
}
std::vector<SettingU> SettingsManager::GetCurrSettings() {
    auto settings = current_settings->GetSettings();

    std::vector<SettingU> SettingsCode;
    int i = 0;
    for (auto setting : settings) {
        fprintf(stderr, "idx i in GetCurrSettings: %d", i);
        SettingU code = (setting.second)->getSettingValue();
        SettingsCode.push_back(code);
        i++;
    }

    return SettingsCode;
}
SettingsManager::SettingsManager() {
    SetPosition(700, 0);
    SetSize(120, 600);
    SetColorFrg({125, 125, 125});
    current_settings = nullptr;
}
void SettingsManager::SetCurrentSettings(SettingsCollection *settings) {
    if (current_settings) {
        DelinkChild(current_settings);
    }
    LinkChild(settings);
    fprintf(stderr, "linked\n");
    current_settings = settings;
}
void SettingsManager::HandleEvent(const Event &event) {
    auto ev = event;
    ev.mouse.x -= left;
    ev.mouse.y -= top;
    ContainerW::HandleEvent(ev);
}
void SettingsManager::draw() {
    if (current_settings) {
        current_settings->SetPosition(left, top);
    }
    AdapterSFML::DrawRect(left, top, width, height, thickness, colorfrg, colorbkg);
    ContainerW::draw();
}
SettingsManager::~SettingsManager() {
    DelinkChild(current_settings);
}

DrawManager::DrawManager() {
    canvas       = new Canvas(600, 400);
    toolmenu     = new ToolMenu;
    settingsmenu = new SettingsManager;
    palettemenu  = new ColorPaletteManager;

    LinkChild(canvas);
    LinkChild(toolmenu);
    LinkChild(settingsmenu);
    LinkChild(palettemenu);

    Brush *brush = new Brush;
    Eraser *eraser = new Eraser;
    toolmenu->AppendTool(brush);
    toolmenu->AppendTool(eraser);
    toolmenu->SetNewActive(brush);
    auto settings = toolmenu->SetNewActive(brush);
    settingsmenu->SetCurrentSettings(settings);

    ButtonModalW *save = new ButtonModalW;

    save->SetTexture(AdapterSFML::LoadTexture("../assets/savepic.png"));
    save->SetPosition(330, 400);
    save->SetColorFrg({220, 220, 220});
    save->SetNormalColor({220, 220, 220});
    save->SetHoveredColor({220, 200, 200});
    save->SetPressedColor({220, 180, 180});
    save->SetSize(70, 70);

    LinkChild(save);

    SavePicMW *saver = new SavePicMW;
    saver->SetSavingDescr(canvas->GetDescr());
    save->SetMW(saver);



    ButtonModalW *load = new ButtonModalW;

    load->SetTexture(AdapterSFML::LoadTexture("../assets/loadpic.jpeg"));
    load->SetPosition(260, 400);
    load->SetColorFrg({220, 220, 220});
    load->SetNormalColor({220, 220, 220});
    load->SetHoveredColor({220, 200, 200});
    load->SetPressedColor({220, 180, 180});
    load->SetSize(70, 70);

    LinkChild(load);

    LoadPicMW *loader = new LoadPicMW;
    loader->SetSavingDescr(canvas->GetDescr());
    loader->SetModifiableCanvas(canvas);
    load->SetMW(loader);

}

void DrawManager::SetSettingsCollection(SettingsCollection *settings) {
    settingsmenu->SetCurrentSettings(settings);
}
void DrawManager::ApplyTool(Canvas &canvas, int x, int y) {
    toolmenu->GetActive()->apply(canvas, x, y);
}
void DrawManager::StartApplicationTool(int x, int y) {
    auto color = palettemenu->GetCurrColor();
    auto tool = toolmenu->GetActive();
    auto settings = settingsmenu->GetCurrSettings();
    
    tool->StartApplication(canvas, x, y, color, settings);

}
void DrawManager::EndApplicationTool() {
    toolmenu->GetActive()->endapplying();
}

void ButtonModalW::ClickImpact() {
    if (mwindow) {
        InvokeModalWindow(mwindow);
    }
}

void ButtonModalW::SetMW(ModalW *mw) {
    mwindow = mw;
    LinkChild(mwindow);
}

LoadPicMW::LoadPicMW() {
    SetPosition(200, 300);
    SetSize(500, 200);
    SetColorFrg({220, 220, 220});
    path = new InputBox;
    path->SetPosition(250, 370);
    path->SetSize(400, 30);
    path->SetColorFrg({244, 244, 244});
    path->GetText().SetMessage("default.png");
    LinkChild(path);

    ActionButton *button = new ActionButton;
    button->SetPosition(370, 420);
    button->SetColorFrg({125, 220, 212});
    button->SetNormalColor({125, 220, 212, 120});
    button->SetHoveredColor({125, 210, 200});
    button->SetPressedColor({125, 220, 212});
    button->SetSize(100, 50);
    button->GetTitle().SetMessage("Load!");
    button->GetTitle().SetSize(18);
    button->GetTitle().SetPosition(390, 430);
    LinkChild(button);
}
void LoadPicMW::SetModifiableCanvas(Canvas *canv) {
    canvas = canv;
}
void LoadPicMW::SetSavingDescr(uint32_t descr) {
    saving_descriptor = descr;
}
void LoadPicMW::Action() {
    auto loadpath = path->GetText().GetMessage();
    auto data = AdapterSFML::LoadFromImage(saving_descriptor, loadpath);

    canvas->UpdateImage(data.first, data.second);
    delete[] data.first;
    DeinvokeMW();
}
void LoadPicMW::draw() {
    ShapeRect::draw();
    ContainerW::draw();
}

SavePicMW::SavePicMW() {
    SetPosition(200, 300);
    SetSize(500, 200);
    SetColorFrg({220, 220, 220});
    path = new InputBox;
    path->SetPosition(250, 370);
    path->SetSize(400, 30);
    path->SetColorFrg({244, 244, 244});
    path->GetText().SetMessage("default.png");
    LinkChild(path);

    ActionButton *button = new ActionButton;
    button->SetPosition(370, 420);
    button->SetColorFrg({125, 220, 212});
    button->SetNormalColor({125, 220, 212, 120});
    button->SetHoveredColor({125, 210, 200});
    button->SetPressedColor({125, 220, 212});
    button->SetSize(100, 50);
    button->GetTitle().SetMessage("Save!");
    button->GetTitle().SetSize(18);
    button->GetTitle().SetPosition(390, 430);
    LinkChild(button);
}
void SavePicMW::SetSavingDescr(uint32_t descr) {
    saving_descriptor = descr;
}
void SavePicMW::Action() {
    auto savepath = path->GetText().GetMessage();
    AdapterSFML::SaveToImage(saving_descriptor, savepath);
    DeinvokeMW();
}
void SavePicMW::draw() {
    ShapeRect::draw();
    ContainerW::draw();
}

void ActionButton::ClickImpact() {
    static_cast<ModalW*>(parent)->Action();
}