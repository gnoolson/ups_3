#include "queue.h"

Queue::Queue()
{
  this->pArray = new Packet*[QUEUE__ARRAY_BUFFER_LENGTH];
  for (int i = 0; i < QUEUE__ARRAY_BUFFER_LENGTH; i++)
  {
    this->pArray[i] = new Packet;
  }
}

void Queue::push(uint8_t *pData, size_t length, bool header)
{
  if (length > QUEUE__PACKET_BUFFER_LENGTH)
  {
    return;
  }

  Packet *pPacket = this->pArray[writeIndex];
  if (pPacket->status == Status::FULL)
  {
    return;
  }

  memcpy(pPacket->data, pData, length);
  pPacket->length = length;
  pPacket->header = header;
  pPacket->status = Status::FULL;

  this->writeIndex = this->writeIndex + 1;
  this->incomingPacketsCounter = this->incomingPacketsCounter + 1;

  if (this->writeIndex == QUEUE__ARRAY_BUFFER_LENGTH)
    this->writeIndex = 0;
}


bool Queue::pull(uint8_t *pData, size_t &length, bool &header)
{
  if (incomingPacketsCounter == 0)
    return false;

  Packet* pPacke = this->pArray[readIndex];
  if (pPacke->status == Status::EMPTY)
  {
    return false;
  }
  
  pPacke->status = EMPTY;
  memcpy(pData, pPacke->data, pPacke->length);
  length = pPacke->length;
  header = pPacke->header;
  incomingPacketsCounter = incomingPacketsCounter - 1;

  this->readIndex = this->readIndex + 1;
  if (readIndex == QUEUE__ARRAY_BUFFER_LENGTH)
  {
    readIndex = 0;
  }

  return true;
}
