#include <Arduino.h>

#define QUEUE__ARRAY_BUFFER_LENGTH 4 
#define QUEUE__PACKET_BUFFER_LENGTH 150

enum Status
{
  EMPTY,
  FULL
};

struct Packet
{
  uint8_t data[QUEUE__PACKET_BUFFER_LENGTH] = {0};
  size_t length = 0;
  bool header = false;
  Status status = Status::EMPTY;
};

class Queue
{
  private:
    Packet **pArray = NULL;
    volatile uint8_t writeIndex = 0;
    volatile uint8_t readIndex = 0;
    volatile uint8_t incomingPacketsCounter = 0;

  public:
    Queue();
    void push(uint8_t *pData, size_t length, bool header);
    bool pull(uint8_t *pData, size_t &length, bool &header);

};