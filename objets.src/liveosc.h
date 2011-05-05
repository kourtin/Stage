#if !defined(LIVEOSC_H_QKYGBEO)
#define LIVEOSC_H_QKYGBEO

#include <queue>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

#include <osc/OscReceivedElements.h>
#include <osc/OscPacketListener.h>
#include <osc/OscOutboundPacketStream.h>
#include <ip/UdpSocket.h>

struct liveosc_values {
	static int port_send;
	static int port_receive;
	static std::string addresse;
	enum { buffer_size = 1024 };
};

struct liveosc_send {
	typedef osc::OutboundPacketStream paquet_osc;
	liveosc_send();
	virtual ~liveosc_send();
	paquet_osc new_packet();
	void send(paquet_osc p);
	void send_dummy();
	static liveosc_send& instance() { return instance_; }
private:
	static liveosc_send instance_;
	UdpTransmitSocket socket_send_;
	char buffer_[liveosc_values::buffer_size];
};

struct liveosc_receive : public osc::OscPacketListener {
	liveosc_receive(unsigned int port = liveosc_values::port_receive);
	virtual ~liveosc_receive();
	boost::mutex& mutex();
	void join();
	bool pending();
	int size_pending();
	osc::ReceivedMessage next();
	osc::ReceivedMessage top();
protected:
	virtual void ProcessMessage(const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint);
private:
	struct thread_runner {
		thread_runner(liveosc_receive& l) : l_(l) {}
		void operator()() {
			l_.socket_receive_.RunUntilSigInt();
		}
	private:
		liveosc_receive& l_;
	};
	thread_runner r_;
	UdpListeningReceiveSocket socket_receive_;
	boost::mutex mutex_;
	boost::thread thread_;
	std::queue<osc::ReceivedMessage> pending_;
};

#endif /* end of include guard: LIVEOSC_H_QKYGBEO */
