#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <ctime>
#include <gloox/client.h>
#include <gloox/message.h>
#include <gloox/messagehandler.h>
#include <gloox/connectionlistener.h>
#include <gloox/loghandler.h>
#include <gloox/logsink.h>
#include <gloox/disco.h>
#include <gloox/presence.h>
#include <gloox/rosterlistener.h>
#include <gloox/rosteritem.h>
#include <gloox/rostermanager.h>
#include <thread>
#include <memory>
#include <atomic>

using namespace gloox;
using namespace std;

// Abstract base class for message storage
class MessageStorage {
public:
    virtual void saveMessage(const string& sender, const string& content) = 0;
    virtual vector<pair<string, string>> getAllMessages() const = 0;
    virtual ~MessageStorage() = default;
};

// Concrete implementation of message storage
class FileMessageStorage : public MessageStorage {
private:
    const string messageFile;
    vector<pair<string, string>> messageHistory;
    string myUsername;

    void loadOldMessages() {
        ifstream file(messageFile);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                size_t pos = line.find(" | ");
                if (pos != string::npos) {
                    string sender = line.substr(0, pos);
                    string content = line.substr(pos + 3);
                    if (sender != myUsername) {
                        messageHistory.push_back({sender, content});
                    }
                }
            }
            file.close();
        }
    }

public:
    FileMessageStorage(const string& filename, const string& username) 
        : messageFile(filename), myUsername(username) {
        loadOldMessages();
    }

    void saveMessage(const string& sender, const string& content) override {
        if (sender != myUsername) {
            messageHistory.push_back({sender, content});
            ofstream file(messageFile, ios::app);
            if (file.is_open()) {
                file << sender << " | " << content << endl;
                file.close();
            }
        }
    }

    vector<pair<string, string>> getAllMessages() const override {
        return messageHistory;
    }

    void updateUsername(const string& username) {
        myUsername = username;
        messageHistory.clear();
        loadOldMessages();
    }
};

// Message class with operator overloading
class ChatMessage {
private:
    string sender;
    string content;
    chrono::system_clock::time_point sentTime;

public:
    ChatMessage(const string& from, const string& text) 
        : sender(from), content(text), sentTime(chrono::system_clock::now()) {}

    bool operator==(const ChatMessage& other) const {
        return sender == other.sender && content == other.content;
    }

    friend ostream& operator<<(ostream& os, const ChatMessage& msg) {
        auto time = chrono::system_clock::to_time_t(msg.sentTime);
        os << "[" << ctime(&time) << "] From: " << msg.sender << "\nMessage: " << msg.content;
        return os;
    }

    string getSender() const { return sender; }
    string getContent() const { return content; }
};

// Main XMPP chat client class
class XMPPChatClient : public Client, public MessageHandler, public ConnectionListener, public LogHandler, public RosterListener {
private:
    static int activeClients;
    atomic<bool> isOnline;
    thread eventHandler;
    unique_ptr<FileMessageStorage> messageStore;
    string myJID;
    RosterManager* contactList;
    bool contactsLoaded;

    void processIncomingMessage(const Message& msg) {
        if (msg.body() != "") {
            string sender = msg.from().bare();
            string content = msg.body();
            
            cout << "\n📨 New message from " << sender << ":" << endl;
            cout << content << endl;
            
            messageStore->saveMessage(sender, content);
        }
    }

    void startEventHandler() {
        eventHandler = thread([this]() {
            while (isOnline) {
                recv(1000);
            }
        });
    }

    void stopEventHandler() {
        isOnline = false;
        if (eventHandler.joinable()) {
            eventHandler.join();
        }
    }

public:
    XMPPChatClient(const string& jid, const string& password)
        : Client(jid, password), isOnline(false), myJID(jid),
          messageStore(make_unique<FileMessageStorage>("inbox.txt", jid)), contactsLoaded(false) {
        registerMessageHandler(this);
        registerConnectionListener(this);
        contactList = rosterManager();
        contactList->registerRosterListener(this);
        logInstance().registerLogHandler(LogLevelDebug, LogAreaAll, this);
        activeClients++;
    }

    static int getActiveClientCount() { return activeClients; }

    bool checkConnection() const { return isOnline; }

    void connectToServer() {
        if (Client::connect(false)) {
            isOnline = true;
            startEventHandler();
        } else {
            cout << "❌ Failed to connect to server" << endl;
        }
    }

    void onConnect() override {
        if (!isOnline) {
            cout << "✅ Connected to server" << endl;
            isOnline = true;
            
            Presence presence(Presence::Available, JID(), "", 0, "");
            send(presence);
            
            contactList->fill();
        }
    }

    void onDisconnect(ConnectionError e) override {
        if (isOnline) {
            cout << "❌ Disconnected from server" << endl;
            isOnline = false;
            contactsLoaded = false;
            stopEventHandler();
        }
    }

    bool onTLSConnect(const CertInfo& info) override {
        return true;
    }

    void handleMessage(const Message& msg, MessageSession* session) override {
        if (msg.body() != "") {
            string sender = msg.from().bare();
            string content = msg.body();
            
            cout << "\n📨 New message from " << sender << ":" << endl;
            cout << content << endl;
            
            messageStore->saveMessage(sender, content);
        }
    }

    void handleRoster(const Roster& roster) override {
        if (!contactsLoaded) {
            cout << "✅ Ready to receive messages" << endl;
            contactsLoaded = true;
        }
    }

    void handleRosterPresence(const RosterItem& item, const string& resource, 
                            Presence::PresenceType presence, const string& msg) override {}

    void handleRosterError(const IQ& iq) override {
        cout << "❌ Contact list error occurred" << endl;
    }

    void handleItemAdded(const JID& jid) override {}
    void handleItemSubscribed(const JID& jid) override {}
    void handleItemRemoved(const JID& jid) override {}
    void handleItemUpdated(const JID& jid) override {}
    void handleItemUnsubscribed(const JID& jid) override {}
    void handleSelfPresence(const RosterItem& item, const string& resource,
                          Presence::PresenceType presence, const string& msg) override {}
    bool handleSubscriptionRequest(const JID& jid, const string& msg) override { return true; }
    bool handleUnsubscriptionRequest(const JID& jid, const string& msg) override { return true; }
    void handleNonrosterPresence(const Presence& presence) override {}

    void handleLog(LogLevel level, LogArea area, const string& message) override {
        if (level == LogLevelError) {
            cout << "❌ Error: " << message << endl;
        }
    }

    void sendMessage(const string& recipient, const string& content) {
        if (!isOnline) {
            cout << "❌ Not connected to server" << endl;
            return;
        }

        if (recipient.empty() || content.empty()) {
            cout << "❌ Recipient and message cannot be empty" << endl;
            return;
        }

        try {
            Message msg(Message::Chat, JID(recipient), content);
            send(msg);
            cout << "✅ Message sent to " << recipient << endl;
        } catch (const exception& e) {
            cout << "❌ Error sending message: " << e.what() << endl;
        }
    }

    void showInbox() {
        auto messages = messageStore->getAllMessages();
        if (messages.empty()) {
            cout << "📨 Inbox is empty." << endl;
            return;
        }

        cout << "\n=== Your Inbox ===" << endl;
        cout << "Messages received by " << myJID << ":" << endl;
        for (size_t i = 0; i < messages.size(); ++i) {
            cout << "[" << (i + 1) << "] From: " << messages[i].first << endl;
            cout << "    Message: " << messages[i].second << endl;
            cout << "----------------------------------------" << endl;
        }
    }

    ~XMPPChatClient() {
        stopEventHandler();
        disconnect();
        activeClients--;
    }
};

int XMPPChatClient::activeClients = 0;

int main() {
    try {
        cout << "=== XMPP Chat Client ===" << endl;
        cout << "Enter your credentials to connect:" << endl;
        
        string jid, password;
        cout << "JID (e.g. user@13.88.189.147): ";
        getline(cin, jid);
        cout << "Password: ";
        getline(cin, password);

        XMPPChatClient client(jid, password);
        client.connectToServer();

        cout << "\n=== Chat Commands ===" << endl;
        cout << "Type 'help' to see available commands" << endl;
        cout << "Type 'quit' to exit" << endl;

        while (true) {
            string command;
            cout << "\nEnter command: ";
            getline(cin, command);

            if (command == "quit") {
                cout << "Disconnecting and exiting..." << endl;
                break;
            }
            else if (command == "help") {
                cout << "\nAvailable Commands:" << endl;
                cout << "send    - Send a message to another user" << endl;
                cout << "inbox   - View your message inbox" << endl;
                cout << "status  - Check your connection status" << endl;
                cout << "help    - Show this help message" << endl;
                cout << "quit    - Exit the program" << endl;
            }
            else if (command == "send") {
                string recipient, message;
                cout << "Recipient JID: ";
                getline(cin, recipient);
                cout << "Message: ";
                getline(cin, message);
                
                if (!message.empty()) {
                    client.sendMessage(recipient, message);
                }
            }
            else if (command == "inbox") {
                client.showInbox();
            }
            else if (command == "status") {
                cout << "Connection Status: " << (client.checkConnection() ? "✅ Connected" : "❌ Disconnected") << endl;
                cout << "Active Clients: " << XMPPChatClient::getActiveClientCount() << endl;
            }
            else {
                cout << "❌ Unknown command. Type 'help' to see available commands." << endl;
            }
        }
    } catch (const exception& e) {
        cout << "❌ Error: " << e.what() << endl;
        return 1;
    }

    return 0;
} 
