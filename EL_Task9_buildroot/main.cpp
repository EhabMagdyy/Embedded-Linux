#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    // Boot the Qt engine and start the operating system handshake.
    // Initializes the Qt runtime
    // Sets up the event loop
    // Prepares windowing, input devices, rendering, etc.
    QGuiApplication app(argc, argv);

    /*
    Creates the QML engine object.
    This object:
        Loads QML files
        Instantiates your UI
        Owns all QML-created objects
    */
    QQmlApplicationEngine engine;
    QObject::connect(   // This connects a signal to a slot (callback), In human terms: “When X happens, run Y.”
        &engine,
        // This is the signal:If QML fails to load, objectCreationFailed is emitted.
        &QQmlApplicationEngine::objectCreationFailed,
        // This is what happens when loading fails:
        &app,
        []() { QCoreApplication::exit(-1); },
        // this avoids weird crashes during startup and threading issues.
        Qt::QueuedConnection);
    // This is where your UI is actually loaded
    /*
        "Task02_Calculator" → the QML module name
        "Main" → the QML file (Main.qml)
    */
    engine.loadFromModule("Task02_Calculator", "Main");
    // Start the event loop. This will not return until the application is exiting, 
    // it enters an infinite loop and processes events (like user input, windowing events, etc.) until the application is closed.
    return app.exec();  // without it will load the UI and immediately exit.
}
