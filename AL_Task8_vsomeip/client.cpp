#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <thread>

constexpr static auto APP_NAME = "CapsLockClient";

constexpr int SERVICE_ID = 0xEABA;
constexpr int INSTANCE_ID = 0x01;
constexpr int GREETINGS_METHOS_ID = 0x01;
constexpr int CAPSLOCK_METHOD_ID = 0x02;
constexpr int EXIT_SIGNAL_EVENT_ID = 0xF001;
constexpr int EVENT_GROUP_ID = 0x01;

void RequestService(vsomeip::state_type_e state){
    // This handler is called when the state of the application changes
    if(state == vsomeip::state_type_e::ST_REGISTERED){
        std::shared_ptr<vsomeip::application> app = vsomeip::runtime::get()->get_application(APP_NAME);
        app->request_service(SERVICE_ID, INSTANCE_ID);
        app->subscribe(SERVICE_ID, INSTANCE_ID, EVENT_GROUP_ID);
    }
}


void SendGreetingsRequest(vsomeip::service_t service, vsomeip::instance_t instance, bool is_available){
    // if (!is_available) return;
    std::shared_ptr<vsomeip::application> app = vsomeip::runtime::get()->get_application(APP_NAME);

    std::shared_ptr<vsomeip::message> request = vsomeip::runtime::get()->create_message();
    request->set_service(service);
    request->set_instance(instance);
    request->set_method(GREETINGS_METHOS_ID);
    request->set_message_type(vsomeip::message_type_e::MT_REQUEST);

    std::cout << "\nSending Greetings Request...\n";
    app->send(request);
}


void SendCapsLockRequest(std::shared_ptr<vsomeip::application> app){
    std::shared_ptr<vsomeip::message> request = vsomeip::runtime::get()->create_message();
    request->set_service(SERVICE_ID);
    request->set_instance(INSTANCE_ID);
    request->set_method(CAPSLOCK_METHOD_ID);
    request->set_message_type(vsomeip::message_type_e::MT_REQUEST);

    std::cout << "Sending CapsLock Request...\n";
    app->send(request);
}

void RespondeHandler(const std::shared_ptr<vsomeip::message>& response){
    std::cout << "> Response received!\n";
    std::cout << "Header: Service: 0x" << std::hex << response->get_service()
              << " - Instance: 0x" << response->get_instance()
              << " - Method: 0x" << response->get_method() << std::dec << "\n";

    auto payload = response->get_payload();
    if(payload){
        auto data = payload->get_data();
        std::size_t len = payload->get_length();
        std::cout << "Payload: " << std::string(reinterpret_cast<char*>(data), len) << "\n\n";
    }
}

void EventHandler(const std::shared_ptr<vsomeip::message>& event){
    std::cout << "> Event received!\n";
    std::cout << "Header: Service: 0x" << std::hex << event->get_service()
              << " - Instance: 0x" << event->get_instance()
              << " - Event: 0x" << event->get_method() << std::dec << "\n";

    auto payload = event->get_payload();
    if(payload){
        auto data = payload->get_data();
        std::size_t len = payload->get_length();
        std::cout << "Payload: " << std::string(reinterpret_cast<char*>(data), len) << "\n\n";
    }

    // Cleanup and Exit
    std::shared_ptr<vsomeip::application> app = vsomeip::runtime::get()->get_application(APP_NAME);
    // unregister the state handler
    app->unregister_state_handler();
    // unregister the message handler
    app->unregister_message_handler(SERVICE_ID, INSTANCE_ID, GREETINGS_METHOS_ID);
    app->unregister_message_handler(SERVICE_ID, INSTANCE_ID, CAPSLOCK_METHOD_ID);
    app->unregister_message_handler(SERVICE_ID, INSTANCE_ID, EXIT_SIGNAL_EVENT_ID);
    app->stop();
    std::cout << "Service stopped due to EXIT signal event\n";
    exit(0);
}

int main(){

    std::shared_ptr<vsomeip::application> app = vsomeip::runtime::get()->create_application(APP_NAME);
   
    app->init();

    // notified when the state changes & requesting service
    app->register_state_handler(RequestService);
    // Sending Request when the service is available
    app->register_availability_handler(SERVICE_ID, INSTANCE_ID, SendGreetingsRequest);
    // Request event
    std::set<vsomeip::eventgroup_t> groups = {EVENT_GROUP_ID};
    app->request_event(SERVICE_ID, INSTANCE_ID, EXIT_SIGNAL_EVENT_ID, groups,
                    vsomeip::event_type_e::ET_EVENT);
    // Registering handler for the EXIT_SIGNAL_EVENT_ID event
    app->register_message_handler(SERVICE_ID, INSTANCE_ID, EXIT_SIGNAL_EVENT_ID, EventHandler);

    // register response
    app->register_message_handler(SERVICE_ID, INSTANCE_ID, GREETINGS_METHOS_ID, RespondeHandler);
    app->register_message_handler(SERVICE_ID, INSTANCE_ID, CAPSLOCK_METHOD_ID, RespondeHandler);

    std::thread capslock_thread([app](){
        for(;;){
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
            SendCapsLockRequest(app);
        }
    });

    app->start();

    capslock_thread.join();

    return 0;
}