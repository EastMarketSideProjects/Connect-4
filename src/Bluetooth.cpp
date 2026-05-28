#include "Bluetooth.h"

static NimBLEServer* pServer;
static NimBLECharacteristic* pUpdateCharacteristic;

QueueHandle_t input_queue;
QueueHandle_t output_queue;
/** Server Callbacks **/
class ServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
    Serial.printf("Client connected, address: %s\n",
                  connInfo.getAddress().toString().c_str());
    pServer->updateConnParams(connInfo.getConnHandle(), 24, 48, 0, 180);
  }

  void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo,
                    int reason) override {
    Serial.printf("Client disconnected - start advertising\n");
    NimBLEDevice::startAdvertising();
  }
} serverCallbacks;

/** Characteristic Callbacks **/
class CharacteristicCallbacks : public NimBLECharacteristicCallbacks {
  void onSubscribe(NimBLECharacteristic* pCharacteristic,
                   NimBLEConnInfo& connInfo, uint16_t subValue) override {
    std::string str = "Client ID: ";
    str += connInfo.getConnHandle();
    str += " Address: ";
    str += connInfo.getAddress().toString();
    if (subValue == 0) {
      str += " Unsubscribed to ";
    } else if (subValue == 1) {
      str += " Subscribed to notifications for ";
    } else if (subValue == 2) {
      str += " Subscribed to indications for ";
    } else if (subValue == 3) {
      str += " Subscribed to notifications and indications for ";
    }
    str += std::string(pCharacteristic->getUUID());
    Serial.printf("%s\n", str.c_str());
  }

  void onRead(NimBLECharacteristic* pCharacteristic,
              NimBLEConnInfo& connInfo) override {
    Serial.printf("%s : onRead(), value: %s\n",
                  pCharacteristic->getUUID().toString().c_str(),
                  pCharacteristic->getValue().c_str());
  }
  void onWrite(NimBLECharacteristic* pCharacteristic,
               NimBLEConnInfo& connInfo) override {
    Serial.printf("%s : onWrite(), value: %s\n",
                  pCharacteristic->getUUID().toString().c_str(),
                  pCharacteristic->getValue().c_str());
  } /** * The value returned in code is the NimBLE host return code. */
  void onStatus(NimBLECharacteristic* pCharacteristic, int code) override {
    Serial.printf("Notification/Indication return code: %d, %s\n", code,
                  NimBLEUtils::returnCodeToString(code));
  } /** Peer subscribed to notifications/indications */
} chrCallbacks;

void bluetoothBegin() {
  NimBLEDevice::init("Connect 4");

  pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(&serverCallbacks);

  NimBLEService* pService = pServer->createService("C004");
  pUpdateCharacteristic = pService->createCharacteristic(
      "C004",
      NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
  NimBLEDescriptor* pDesc = pUpdateCharacteristic->createDescriptor("2901");
  pDesc->setValue("Connect 4");
  // pUpdateCharacteristic->createDescriptor("2902");

  pUpdateCharacteristic->setValue(0);
  pUpdateCharacteristic->setCallbacks(&chrCallbacks);

  pService->start();

  NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->setName("Connect 4");
  pAdvertising->addServiceUUID(pService->getUUID());
  pAdvertising->start();

  Serial.println("BLE Started. Waiting for a client connection to notify...");
}

void output_task(void* pvParameters) {
  bluetoothBegin();
  board_update_t update;
  int timeout = 0;

  while (1) {
    if (pServer->getConnectedCount() > 0) {
      if (xQueueReceive(output_queue, &update, timeout) == pdTRUE) {
        pUpdateCharacteristic->setValue((uint8_t*)&update, sizeof(update));
        pUpdateCharacteristic->notify();
      }
    }
    vTaskDelay(5 / portTICK_PERIOD_MS);
  }
}