#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <fstream>
#include <csignal>

// Make the signal sends event to the client to also terminate

constexpr static auto CAPSLOCK_PATH = "/sys/class/leds/input3::capslock/brightness";
constexpr static auto APP_NAME = "CapsLockService";

constexpr int SERVICE_ID = 0xEABA;
constexpr int INSTANCE_ID = 0x01;
constexpr int GREETINGS_METHOS_ID = 0x01;
constexpr int CAPSLOCK_METHOD_ID = 0x02;
constexpr int EXIT_SIGNAL_EVENT_ID = 0xF001;
constexpr int EVENT_GROUP_ID = 0x01;

// Cleanup before termination
void SignalHandler(int signal){
    std::shared_ptr<vsomeip::application> app = vsomeip::runtime::get()->get_application(APP_NAME);
    // Send EXIT Signal event to all subscribed clients
    std::shared_ptr<vsomeip::payload> payload = vsomeip::runtime::get()->create_payload();
    std::vector<vsomeip::byte_t> data = {'E', 'X', 'I', 'T'};
    payload->set_data(data);
    std::cout << "\nSending EXIT signal event to clients...\n";
    app->notify(SERVICE_ID, INSTANCE_ID, EVENT_GROUP_ID, payload);

    app->stop_offer_service(SERVICE_ID, INSTANCE_ID);
    // unregister the state handler
    app->unregister_state_handler();
    // unregister the message handler
    app->unregister_message_handler(SERVICE_ID, INSTANCE_ID, GREETINGS_METHOS_ID);
    app->unregister_message_handler(SERVICE_ID, INSTANCE_ID, CAPSLOCK_METHOD_ID);
    app->stop();
    std::cout << "Service stopped due to -> Signal " << signal << std::endl;
}

void GreetingsHandler(const std::shared_ptr<vsomeip::message>& request){
    std::cout << "\nReceived Greetings Request\n";

    // Create the response message
    std::shared_ptr<vsomeip::message> response = vsomeip::runtime::get()->create_message();
    // Service & instance & method must be the same as the request
    response->set_service(request->get_service());
    response->set_instance(request->get_instance());
    response->set_method(GREETINGS_METHOS_ID);
    response->set_client(request->get_client());
    response->set_message_type(vsomeip::message_type_e::MT_RESPONSE);

    // Create payload for the response
    std::shared_ptr<vsomeip::payload> payload = vsomeip::runtime::get()->create_payload();
    std::vector<vsomeip::byte_t> data = {'H', 'I', ' ', 'C', 'l', 'i', 'e', 'n', 't', '!'};
    payload->set_data(data);
    response->set_payload(payload);

    // Send the response
    std::cout << "Sending Greetings Response...\n";
    vsomeip::runtime::get()->get_application(APP_NAME)->send(response);
}

void CapsLockMessageHandler(const std::shared_ptr<vsomeip::message>& request){
    static int count = 0;
    std::cout << "CapsLock Message Received\n";

    // Create the response message
    std::shared_ptr<vsomeip::message> response = vsomeip::runtime::get()->create_message();
    response->set_service(request->get_service());
    response->set_instance(request->get_instance());
    response->set_method(CAPSLOCK_METHOD_ID);
    response->set_client(request->get_client());
    response->set_message_type(vsomeip::message_type_e::MT_RESPONSE);

    // Create payload for the response
    std::shared_ptr<vsomeip::payload> payload = vsomeip::runtime::get()->create_payload();
    std::string message;

    // Toggle Caps Lock LED
    int brightness = (count % 2 == 0) ? 1 : 0;
    std::ofstream capslock_file(CAPSLOCK_PATH);
    if(capslock_file.is_open()){
        std::string str = (brightness ? "ON" : "OFF");
        capslock_file << brightness;
        capslock_file.close();
        std::cout << "Caps Lock LED set to " << str << std::endl;
        message = "Capslock: " + str;
    }
    else{
        std::cerr << "Failed to open Caps Lock LED file." << std::endl;
        message = "Capslock: ERROR\n";
    }

    std::vector<vsomeip::byte_t> data(message.begin(), message.end());
    payload->set_data(data);
    response->set_payload(payload);

    // Send the response
    std::cout << "Sending CapsLock Response...\n\n";
    vsomeip::runtime::get()->get_application(APP_NAME)->send(response);
    count++;
}



int main(){
    // signal handling
    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);
    // Create application
    std::shared_ptr<vsomeip::application> app = vsomeip::runtime::get()->create_application(APP_NAME);
    // Initialize VSOME/IP
    app->init();
    // Register a handler for this SERVICE_ID + INSTANCE_ID + METHOD_ID -> callback
    app->register_message_handler(SERVICE_ID, INSTANCE_ID,GREETINGS_METHOS_ID, GreetingsHandler);
    app->register_message_handler(SERVICE_ID, INSTANCE_ID, CAPSLOCK_METHOD_ID, CapsLockMessageHandler);
    // Enable event notification for this service
    std::set<vsomeip::eventgroup_t> groups = {EVENT_GROUP_ID};
    app->offer_event(SERVICE_ID, INSTANCE_ID, EXIT_SIGNAL_EVENT_ID, groups, 
                    vsomeip::event_type_e::ET_EVENT, std::chrono::milliseconds(0));
    // Offer the service for the client to discover
    app->offer_service(SERVICE_ID, INSTANCE_ID);
    // Start the app
    app->start();

    return 0;
}