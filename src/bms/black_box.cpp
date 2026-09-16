#include <NimBLEDevice.h>
#include "bms.h"
#include "queue.h"
extern "C" {
#include <gnl_timer.h>
#include "esp_bt.h"
}

// 4h
#define BMS_RECONNECT_TIMER 14400000
// 3m
#define BMS_SOC_TIMER 180000
#define BMS_SKIP_SOC_VALUE 12
#define BMS_MAC_ADDRESS "A4:C1:38:01:34:1C"

NimBLEAddress bmsAddress(BMS_MAC_ADDRESS, BLE_ADDR_PUBLIC);
NimBLEUUID serviceUUID("0000FFE0-0000-1000-8000-00805F9B34FB");
NimBLEUUID charUUID_1("0000FFE1-0000-1000-8000-00805F9B34FB");
NimBLERemoteCharacteristic* remoteChar_1;
NimBLEClient* pClient;
gnl_timer_t soc_timer;
gnl_timer_t recon_timer;

Queue queue;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

static uint8_t usefullPacket[] = {0x55, 0xAA, 0xEB, 0x90};
static uint8_t badPacket1[] = {0x41, 0x54, 0x0D, 0x0A};
static uint8_t badPacket2[] = {0xAA, 0x55, 0x90, 0xEB};
uint8_t skipPacketsCounter = 0;
uint8_t skipSocValueCounter = 0;

enum ConnectionStatus {
    S_0,  // connection
    S_2,  // send first request
    S_3,  // recive data
    S_4   // disconnected
};

int8_t SOC = -1;

static ConnectionStatus connectionStatus;

class ClientCallbacks : public NimBLEClientCallbacks {
    void onConnect(NimBLEClient* pClient) override {
        Serial.println(F("BMS. Connected"));
        connectionStatus = ConnectionStatus::S_2;
    }

    void onDisconnect(NimBLEClient* pClient, int reason) override {
        Serial.print(F("BMS. Disconnected, reason: "));
        Serial.println(reason);
        connectionStatus = ConnectionStatus::S_4;
    }
};

ClientCallbacks clientCallbacks;

enum FilterResult { SKIP, HEADER_WITH_DATA, DATA };

/*
 * 
 * 
 * */
FilterResult filter(uint8_t* pData) {
    if (skipPacketsCounter < 32) {
        skipPacketsCounter++;
        return FilterResult::SKIP;
    }

    if (pData[0] == badPacket1[0] && pData[1] == badPacket1[1] && pData[2] == badPacket1[2] && pData[3] == badPacket1[3])
        return FilterResult::SKIP;

    if (pData[0] == badPacket2[0] && pData[1] == badPacket2[1] && pData[2] == badPacket2[2] && pData[3] == badPacket2[3])
        return FilterResult::SKIP;

    if (pData[0] == usefullPacket[0] && pData[1] == usefullPacket[1] && pData[2] == usefullPacket[2] && pData[3] == usefullPacket[3])
        return FilterResult::HEADER_WITH_DATA;

    return FilterResult::DATA;
}

void notifyCallback(NimBLERemoteCharacteristic* pChar, uint8_t* pData, uint16_t length, bool isNotify) {
    FilterResult result = filter(pData);
    if (result == FilterResult::SKIP)
        return;

    portENTER_CRITICAL_ISR(&mux);
    queue.push(pData, length, result == FilterResult::HEADER_WITH_DATA);
    portEXIT_CRITICAL_ISR(&mux);
}

bool connectToBMS() {
    pClient = NimBLEDevice::createClient();
    pClient->setClientCallbacks(&clientCallbacks, false);
    Serial.println(F("BMS. Connecting"));
    if (!pClient->connect(bmsAddress)) {
        Serial.println(F("BMS. Failed to connect!"));
        return false;
    }

    NimBLERemoteService* service = pClient->getService(serviceUUID);
    if (service == nullptr) {
        Serial.println(F("BMS. BLE Service not found!"));
        return false;
    }

    remoteChar_1 = service->getCharacteristic(charUUID_1);
    if (remoteChar_1 == nullptr) {
        Serial.println(F("BMS. Service Characteristic 1 not found!"));
        return false;
    }

    if (remoteChar_1->canNotify()) {
        remoteChar_1->subscribe(true, notifyCallback);
        delay(500);
    }

    return true;
}

void processPacket(const uint8_t* data, size_t len, bool header) {
    if (header)
        return;

	if(skipSocValueCounter < BMS_SKIP_SOC_VALUE){
		skipSocValueCounter++;
		return;
	}

    uint8_t soc = data[23];	
    SOC = soc;

    gnl_timer_start(&soc_timer);
}

void close_connection() {
    if (pClient != nullptr && pClient->isConnected()) {
        pClient->disconnect();
    }
}

/*
 *
 *
 * */
int8_t bms_get_soc(void) {
    return SOC;
}

void bms_begin(void) {
    if (esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_IDLE) {
        esp_bt_controller_disable();
        esp_bt_controller_deinit();
    }

    NimBLEDevice::init("ESP32");
    NimBLEDevice::setMTU(247);

    connectionStatus = ConnectionStatus::S_0;
    gnl_timer_setup(&soc_timer, BMS_SOC_TIMER, false);  
    gnl_timer_setup(&recon_timer, BMS_RECONNECT_TIMER, true);  
	gnl_timer_start(&recon_timer);
}

void bms_force_close(void) {
    close_connection();
    connectionStatus = ConnectionStatus::S_4;
}

void bms_update(void) {
    if (connectionStatus == ConnectionStatus::S_0) {
		skipSocValueCounter = 0;
        if (connectToBMS())
            connectionStatus = ConnectionStatus::S_2;
        else
            connectionStatus = ConnectionStatus::S_4;
    } else if (connectionStatus == ConnectionStatus::S_2) {
        uint8_t request_1[] = {0xaa, 0x55, 0x90, 0xeb, 0x97, 0x00, 0x14, 0x24, 0xd0, 0x39, 0x7a, 0x0d, 0x1a, 0x4b, 0x59, 0x3e, 0xd2, 0x76, 0xb3, 0xd0};
        remoteChar_1->writeValue(request_1, sizeof(request_1), false);
        delay(500);  // TODO

        uint8_t request_2[] = {0xaa, 0x55, 0x90, 0xeb, 0x96, 0x00, 0x0c, 0xe5, 0xfa, 0x7a, 0xdc, 0x51, 0x61, 0x40, 0x1a, 0xd8, 0xaf, 0x2e, 0x37, 0x49};
        remoteChar_1->writeValue(request_2, sizeof(request_2), false);
        delay(500);  // TODO

        uint8_t request_3[] = {0xaa, 0x55, 0x90, 0xeb, 0x6c, 0x04, 0x1e, 0xea, 0xb1, 0x0b, 0x57, 0xf4, 0xe4, 0x82, 0x94, 0x78, 0xa6, 0x65, 0x31, 0xa7};
        remoteChar_1->writeValue(request_3, sizeof(request_3), false);
        delay(500);  // TODO

        connectionStatus = ConnectionStatus::S_3;
    } else if (connectionStatus == ConnectionStatus::S_3) {
        uint8_t data[150];
        size_t length = 0;
        bool header = false;
        bool hasPacket = false;

        portENTER_CRITICAL(&mux);
        if (queue.pull(data, length, header)) {
            hasPacket = true;
        }
        portEXIT_CRITICAL(&mux);

        if (hasPacket) {
            processPacket(data, length, header);
        }
    } else if (connectionStatus == ConnectionStatus::S_4) {
        NimBLEDevice::deleteClient(pClient);
        connectionStatus = ConnectionStatus::S_0;
    }

    if (gnl_timer_check(&soc_timer)) {
        SOC = -1;
    } 
	
	if (gnl_timer_check(&recon_timer)) {
        bms_force_close();
    }
}