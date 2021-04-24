#include "./headers/Application.hpp"

void App::StartApplication() {
    AdapterSFML::InitMainWindow(1200, 1000);
    MWManager = new ModalWManager;
    DrawManager *drawmgr = new DrawManager;
    
    LinkChild(MWManager);
    MWManager->LinkChild(drawmgr);
}

void App::Run() {
    while (AdapterSFML::IsMainWindowOpen()) {
        Event ev;

        while (AdapterSFML::PollEvent(ev)) {
            HandleEvent(ev);

            if (ev.EventType == EV_CLOSED) {
                // 
                AdapterSFML::CloseMainWindow();
                return;
            }
        }

        AdapterSFML::Clear();
        draw();
        AdapterSFML::Display();
    }
}
void App::Finish() {
    //
}
