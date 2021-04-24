#include "GraphicEditor.hpp"

class App : public ContainerW {
    ModalWManager *MWManager;
    // DrawManager *drawmgr = nullptr;
    public:
    void StartApplication();
    void Run();
    void Finish();
    //~App() override {}
};