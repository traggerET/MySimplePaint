#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <utility>

#ifndef ADAPTER_SFML
#define ADAPTER_SFML

#define EV_MOUSE_MOVED          0b1
#define EV_MOUSE_PRESSED       0b10
#define EV_MOUSE_RELEASED     0b100
#define EV_TEXT              0b1000
#define EV_CLOSED           0b10000

struct Event {

    enum Button { // NONE?
        LEFT,
        RIGHT,
        MIDDLE,
        NONE
    };

    struct Mouse {
        Button button;
        int x;
        int y;
    };

    enum KeyButton {
        PG_UP,
        PG_DN,
        OTHER // here are only crucial buttons implemented
    };

    struct Key {
        KeyButton keybutton;
        unsigned char character;
    };

    uint64_t EventType;
    union {
        Mouse mouse;
        Key key;
    };

};

struct Color {
    unsigned char red;
    unsigned char green;
    unsigned char blue;

    unsigned char opacity = 255;
};

struct HSV {
    float hue;
    float sat;
    float val;
};

class AdapterSFML {
private:
    static sf::Font                         ServiceFont;
    static sf::RenderWindow                 MainWindow;
    static std::vector<sf::Texture>         Textures;
    static std::vector<sf::Vector2i>        GlobalOffsets;
    static std::vector<sf::RenderTarget*>   Screens;


    AdapterSFML();

public:
    static bool SaveToImage(uint32_t descriptor, const std::string &fname); 
    static std::pair<uint8_t*, std::pair<int,int>> LoadFromImage(uint32_t descr, const std::string &fname);
    static int64_t PushEmptyTexture(int width, int height);
    static void UpdateTexture(uint32_t *data, int64_t descriptor);
    static void UpdateTexture(uint32_t *data, int64_t descriptor, unsigned x, unsigned y, unsigned width, unsigned height);
    static void PopGlobalOffset();
    static void PushGlobalOffset(int x, int y);
    static void EventHandle(sf::Event &event, Event &ev);
    static void InitMainWindow(size_t width, size_t height);
    static bool PollEvent(Event &ev);
    static void DrawSprite(float x, float y, float width, float height, Color color, int64_t descriptor);
    static void DrawText(const std::string &content, float pos_x, float pos_y, size_t font_size);
    static void DrawRect(float x, float y, float width, float height, float thickness, Color colorfrg, Color colorbkg);
    static int64_t LoadTexture(const char *texture_path);
    static void InitData();
    static bool IsMainWindowOpen();
    static void CloseMainWindow();
    static void Display();
    static void Clear();
    static void DrawOffscreen(int offsetx, int offsety);
    static void CreateOffscreen(int width, int heigth);
};

#endif //ADAPTER_SFML