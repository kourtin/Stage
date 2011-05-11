#if !defined(osc_H_QKYGBEO)
#define osc_H_QKYGBEO

#include <queue>
// #include <boost/thread/thread.hpp>
// #include <boost/thread/mutex.hpp>
// #include <boost/thread/locks.hpp>

#include <osc/OscReceivedElements.h>
#include <osc/OscPacketListener.h>
#include <osc/OscOutboundPacketStream.h>
#include <ip/UdpSocket.h>

struct osc_values {
	static int port_send;
	static int port_receive;
	static std::string addresse;
	enum { buffer_size = 1024 };
};

struct osc_send {
	typedef osc::OutboundPacketStream paquet_osc;
	osc_send();
	virtual ~osc_send();
	paquet_osc new_packet();
	void send(paquet_osc p);
	void send_dummy();
	static osc_send& instance() { return instance_; }
private:
	static osc_send instance_;
	UdpTransmitSocket socket_send_;
	char buffer_[osc_values::buffer_size];
};

// struct osc_receive : public osc::OscPacketListener {
// 	osc_receive(unsigned int port = osc_values::port_receive);
// 	virtual ~osc_receive();
// 	boost::mutex& mutex();
// 	void join();
// 	bool pending();
// 	int size_pending();
// 	osc::ReceivedMessage next();
// 	osc::ReceivedMessage top();
// 	void run_thread();
// protected:
// 	virtual void ProcessMessage(const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint);
// private:
// 	UdpListeningReceiveSocket socket_receive_;
// 	boost::mutex mutex_;
// 	boost::thread thread_;
// 	std::queue<osc::ReceivedMessage> pending_;
// };

#endif /* end of include guard: osc_H_QKYGBEO */
