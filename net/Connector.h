#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <type_traits>
#include <tr1/memory>
#include <functional>
//#include <memory>
#include "InetAddress.h"

namespace kimgbo
{
	namespace net
	{
		class Channel;
		class EventLoop;

		class Connector: public std::tr1::enable_shared_from_this<Connector>
		{
 		public:
  			typedef std::function<void (int sockfd)> NewConnectionCallback;

  			Connector(EventLoop* loop, const InetAddress& serverAddr);
  			~Connector();

  			void setNewConnectionCallback(const NewConnectionCallback& cb)
  			{ m_newConnectionCallback = cb; }

  			void start();  // can be called in any thread
  			void restart();  // must be called in loop thread
  			void stop();  // can be called in any thread

  			const InetAddress& serverAddress() const { return m_serverAddr; }

 			private:
  			enum States { kDisconnected, kConnecting, kConnected };
  			static const int kMaxRetryDelayMs = 30*1000;
  			static const int kInitRetryDelayMs = 500;

  			void setState(States s) { m_state = s; }
  			void startInLoop();
  			void stopInLoop();
  			void connect();
  			void connecting(int sockfd);
 			  void handleWrite();
  			void handleError();
  			void retry(int sockfd);
  			int removeAndResetChannel();
  			void resetChannel();

  			EventLoop* m_loop;
  			InetAddress m_serverAddr;
  			bool m_connect; // atomic
  			States m_state;  // FIXME: use atomic variable
 	 			std::unique_ptr<Channel> m_channel;
  			NewConnectionCallback m_newConnectionCallback;
  			int m_retryDelayMs;
		};
	}
}

#endif