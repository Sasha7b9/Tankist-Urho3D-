#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define VIEW_MASK_TERRAIN 1 << 10       ///< Маска видимости для ландшафта


enum TypeApplication
{
    Type_None,      ///< Перед инициализацией
    Type_Server,    ///< Тип приложения - сервер
    Type_Client     ///< Тип приложения - клиент
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Tank;


class Tankist : public Application
{
    URHO3D_OBJECT(Tankist, Application);

    friend class Client;

public:
    Tankist(Context* context);
    
    /// Функция вызывается движком перед во время инициализации
    virtual void Setup();
    /// Функция вызывается движком
    virtual void Start();
    /// Функция вызывается движком
    virtual void Stop();

    void Exit();
    
private:

    /// Функция вызывается классом Client для оповещения приложения о текущем пинге
    void SetPing(float pingMS);
    /// Функция вызывается классом Client для оповещения приложения о загрузке ЦПУ
    void SetLoadCPU(float loadCPU);
    /// Функция вызывается классом Client для оповещения приложения о количестве подключенных к серверу клиентов
    void SetNumClients(int numClients);
    /// Функция вызывается классом Client для оповещения приложения о скорости входящего соединения
    void SetBytesInPerSec(float bytesInPerSec);
    /// Функция вызывается классом Client для оповещения приложения о скорости исходящего соединения
    void SetBytesOutPerSec(float bytesOutPerSec);
    /// Функция вызывается классом Client для оповещения приложения о суммарной входящей скорости сервера
    void SetBytesInPerSecServer(float bytes);
    /// Функция вызывается классом Client для оповещения приложения о суммарной исходящей скорости сервера
    void SetBytesOutPerSecServer(float bytes);
    /// Создание сцены
    void CreateScene();

    void CreateInstructions();

    void CreateUI();
    /// Функция вызывается каждый кадр
    void UpdateCamera();

    void SetWindowTitleAndIcon();

    void CreateConsoleAndDebugHud();
    /// Функция разбора параметров командной строки
    bool ParseArguments(Vector<String> &arguments, TypeApplication &type, String &address, unsigned short &port);

    bool GetNumPort(String &str, unsigned short &port);

    void HandleKeyDown(StringHash, VariantMap&);

    void HandleKeyUp(StringHash, VariantMap&);

    void HandlePostUpdate(StringHash, VariantMap&);

    void HandleLanguageChanged(StringHash, VariantMap&);

    void HandlePostRenderUpdate(StringHash, VariantMap&);

    void SubscribeToEvents();

    Tankist(Tankist const&) : Application(nullptr) {};
    Tankist operator=(Tankist const &) {};

    bool exit = false;

    SharedPtr<Text> statisticsWindow;

    float ping = 0.0f;

    float loadCPU = 0.0f;

    int numClients = 0;

    float bytesInPerSec = 0.0f;

    float bytesOutPerSec = 0.0f;

    float bytesInPerSecServer = 0.0f;

    float bytesOutPerSecServer = 0.0f;

    void UpdateStatisticWindow();
};
