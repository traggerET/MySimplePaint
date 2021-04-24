#include <vector>
#include <assert.h>
#include <random>
#include <time.h>
#include <cstdlib>
#include <assert.h>
#include <iostream>
#include <string>
#include <list>

#include "./headers/Adapter.hpp"

sf::Font                        AdapterSFML::ServiceFont;
sf::RenderWindow                AdapterSFML::MainWindow;
std::vector<sf::RenderTarget*>  AdapterSFML::Screens;
std::vector<sf::Texture>        AdapterSFML::Textures;
std::vector<sf::Vector2i>       AdapterSFML::GlobalOffsets;

int64_t AdapterSFML::PushEmptyTexture(int width, int height) {
    Textures.emplace_back();
    assert(Textures.back().create(width, height));
    return Textures.size() - 1;
}
void AdapterSFML::UpdateTexture(uint32_t *data, int64_t descriptor) {
    assert(descriptor >= 0);
    Textures[descriptor].update(reinterpret_cast<uint8_t*>(data));
}
void AdapterSFML::UpdateTexture(uint32_t *data, int64_t descriptor, unsigned x, unsigned y, unsigned width, unsigned height) {
    assert(descriptor >= 0);
    Textures[descriptor].update(reinterpret_cast<uint8_t*>(data), width, height, x, y);
}
int64_t AdapterSFML::LoadTexture(const char *texture_path) {
    Textures.emplace_back();
    assert(Textures.back().loadFromFile(texture_path));
    return Textures.size() - 1;
}
bool AdapterSFML::SaveToImage(uint32_t descriptor, const std::string &fname) {
    auto texture = Textures[descriptor];
    auto image = texture.copyToImage();
    std::string fullpath = "../works/" + fname;
    return image.saveToFile(fullpath);
}
//returns adrray of pixels and its size(width, height)
std::pair<uint8_t*, std::pair<int,int>> AdapterSFML::LoadFromImage(uint32_t descr, const std::string &fname) {
    std::string fullpath = "../works/" + fname;
    sf::Image image;
    if (!image.loadFromFile(fullpath)) {
        return {nullptr, {0, 0}};
    };
    
    sf::Texture texture;
    if (!texture.loadFromImage(image)) {//
        return {nullptr, {0, 0}};
    };
    Textures[descr] = texture;

    auto size = image.getSize();
    uint8_t *data = new uint8_t[size.x * size.y * 4];
    auto tmp = image.getPixelsPtr();

    for (auto i = 0; i < size.x * size.y * 4; i++) {
        data[i] = tmp[i];
    }
    return {data, {size.x, size.y}};
}

void AdapterSFML::InitMainWindow(size_t width, size_t height) {
    MainWindow.create(sf::VideoMode(width, height), "SFML Works!");
    GlobalOffsets.push_back({0, 0});
    Screens.push_back(&MainWindow);
    assert(ServiceFont.loadFromFile("../assets/fonts/open-sans/OpenSans-Bold.ttf"));
}
void AdapterSFML::CloseMainWindow() {
    MainWindow.close();
}
bool AdapterSFML::IsMainWindowOpen() {
    return MainWindow.isOpen();
}
void AdapterSFML::Display() {
    static_cast<sf::RenderWindow*>(Screens.back())->display();
}
void AdapterSFML::Clear() {
    Screens.back()->clear();
}
void AdapterSFML::CreateOffscreen(int width, int heigth) {
    sf::RenderTexture *texture = new sf::RenderTexture;
    texture->create(width, heigth); 
    Screens.push_back(texture);
}

void AdapterSFML::PushGlobalOffset(int x, int y) {
    GlobalOffsets.push_back({x, y}); 
}
void AdapterSFML::PopGlobalOffset() {
    GlobalOffsets.pop_back(); 
}

void AdapterSFML::DrawOffscreen(int posx, int posy) {
    sf::RenderTexture *texture = static_cast<sf::RenderTexture*>(Screens.back());
    texture->display();

    sf::Sprite offscreen;
    offscreen.setPosition(posx - GlobalOffsets.back().x, posy - GlobalOffsets.back().y);
    offscreen.setTexture(texture->getTexture());
    Screens.pop_back();
    Screens.back()->draw(offscreen);

    delete texture;
}

/*void AdapterSFML::HandleMainWindow() {
       
}*/
bool AdapterSFML::PollEvent(Event &ev) {
    sf::Event event;
    while (MainWindow.pollEvent(event)) {
        EventHandle(event, ev);
        return true;
    }
    return false;
}
void AdapterSFML::EventHandle(sf::Event &event, Event &ev) {
    switch (event.type) {

        case (sf::Event::Closed):
            ev.EventType = EV_CLOSED;
            break;

        case (sf::Event::MouseMoved):
            ev.EventType = EV_MOUSE_MOVED;
            ev.mouse.x = event.mouseMove.x;
            ev.mouse.y = event.mouseMove.y;
            break;

        case (sf::Event::MouseButtonPressed):
            ev.EventType = EV_MOUSE_PRESSED;
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                ev.mouse.button = Event::Button::LEFT;
            
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
                ev.mouse.button = Event::Button::RIGHT;
            
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
                ev.mouse.button = Event::Button::MIDDLE;

            ev.mouse.x = event.mouseButton.x;
            ev.mouse.y = event.mouseButton.y;
            break;

        case (sf::Event::MouseButtonReleased):
            ev.EventType = EV_MOUSE_RELEASED;
            ev.mouse.button = Event::Button::NONE;
            ev.mouse.x = event.mouseButton.x;
            ev.mouse.y = event.mouseButton.y;
            break;

        case (sf::Event::TextEntered):
            ev.EventType = EV_TEXT;
            ev.key.character = event.text.unicode;
            break;

        default:
            ev.EventType = 0b0;
            break;
        
    }
}
void AdapterSFML::DrawRect(float x, float y, float width, float height, float thickness, Color colorfrg, Color colorbkg) {
    sf::RectangleShape rect({width, height});
    //fprintf(stderr, "%d\n", GlobalOffsets.back().y);
    rect.setPosition({x - GlobalOffsets.back().x, y - GlobalOffsets.back().y});
    rect.setFillColor({colorfrg.red, colorfrg.green, colorfrg.blue, colorfrg.opacity});
    rect.setOutlineColor({colorbkg.red, colorbkg.green, colorbkg.blue, colorbkg.opacity});
    rect.setOutlineThickness(thickness);

    Screens.back()->draw(rect);
}
void AdapterSFML::DrawSprite(float x, float y, float width, float height, Color color, int64_t descriptor) {
    sf::Sprite sprite(Textures[descriptor]);
    sf::Vector2u size = Textures[descriptor].getSize();

    sprite.setPosition({x - GlobalOffsets.back().x, y - GlobalOffsets.back().y});
    sprite.setScale(width/size.x, height/size.y);
    sprite.setColor({color.red, color.green, color.blue, color.opacity});

    Screens.back()->draw(sprite);
}
void AdapterSFML::DrawText(const std::string &content, float pos_x, float pos_y, size_t font_size) {
    sf::Text text(content, ServiceFont, font_size);

    text.setPosition(pos_x - GlobalOffsets.back().x, pos_y - GlobalOffsets.back().y);
    text.setFillColor(sf::Color::Black);

    Screens.back()->draw(text);
    
}
