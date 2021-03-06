#include <sustain/framework/net/patterns/survey/Participant.h>

#include <iostream>
#include <thread>

#include <nanomsg/survey.h>
#include "../nanomsg_helper.h"

namespace pfc {

struct nn_buffer : std::streambuf {
  nn_buffer(char* begin, char* end)
  {
    this->setg(begin, begin, end);
  }
};

struct Survey_Participant::Implementation {
  Implementation(URI&&);
  ~Implementation();

  Implementation(const Implementation&) = delete;
  Implementation(Implementation&&) = delete;
  Implementation& operator==(const Implementation&) = delete;
  Implementation& operator==(Implementation&&) = delete;

  URI uri;
  int socket;
  int rv;
  char* msg_buffer;
  bool running;

  void listen();

  std::thread pubsub_main_thread;
  ListenFunc handle_message_func;

  Error ec;
};
//-------------------------------------------------------------------------------
Survey_Participant::Implementation::Implementation(URI&& u)
  : uri(std::move(u))
  , socket(0)
  , rv(0)
  , msg_buffer(nullptr)
  , running(false)
{
  if ((socket = nn_socket(AF_SP, NN_RESPONDENT)) < 0) {
    ec = nano_to_Error(socket);
  }
  if (rv = nn_connect(socket, uri.c_str()) < 0) {
    ec = nano_to_Error(socket);
  }
}
    //-------------------------------------------------------------------------------
Survey_Participant::Implementation::~Implementation()
{
  if (rv && socket) {
    nn_shutdown(socket, rv);
  }

  if (socket) {
    nn_close(socket);
  }
  if (msg_buffer) {
    nn_freemsg(msg_buffer);
    msg_buffer = nullptr;
  }
}
//-----------------------------------------------------------------------------
void Survey_Participant::Implementation::listen()
{
  do {
    char* l_buffer = NULL;
    int bytes;
    if ((bytes = nn_recv(socket, &l_buffer, NN_MSG, 0)) < 0) {
      ec = nano_to_Error(bytes);
      continue;
    }
    std::vector<char> response = handle_message_func(l_buffer, bytes);
    nn_freemsg(l_buffer);
    if ((bytes = nn_send(socket, response.data(), response.size(), 0)) < 0) {
      ec = nano_to_Error(bytes);
    }
  } while (running);
}
//-----------------------------------------------------------------------------
Survey_Participant::Survey_Participant(URI uri)
  : _impl(std::make_unique<Implementation>(std::move(uri)))
{
}
//-----------------------------------------------------------------------------
Survey_Participant::~Survey_Participant()
{
  nn_close(_impl->socket);
  _impl = nullptr;
}
//-----------------------------------------------------------------------------
void Survey_Participant::listen(ListenFunc func)
{
  _impl->handle_message_func = func;
  _impl->running = false;
  _impl->listen();
}
//-----------------------------------------------------------------------------
void Survey_Participant::async_listen(ListenFunc func)
{
  _impl->handle_message_func = func;
  _impl->running = true;
  _impl->pubsub_main_thread = std::thread(&Implementation::listen, _impl.get());
}
//-----------------------------------------------------------------------------
void Survey_Participant::standup()
{
}
//-----------------------------------------------------------------------------
void Survey_Participant::shutdown()
{
  _impl->running = true;
  if (_impl->socket) {
    nn_close(_impl->socket);
    _impl->socket = 0;
  }
}
//-----------------------------------------------------------------------------
}