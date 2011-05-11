#include "son/opensoundcontrol.h"

#include <iostream>

int osc_values::port_send(9005);
int osc_values::port_receive(9001);
std::string osc_values::addresse("127.0.0.1");

osc_send osc_send::instance_;

osc_send::osc_send() : socket_send_(IpEndpointName(osc_values::addresse.c_str(), osc_values::port_send)) {}

osc_send::~osc_send() {}

osc_send::paquet_osc osc_send::new_packet() { 
	return osc::OutboundPacketStream(buffer_, osc_values::buffer_size) << osc::BeginBundleImmediate;
}

void osc_send::send(paquet_osc p) { 
	p << osc::EndBundle; socket_send_.Send(p.Data(), p.Size());
}

void osc_send::send_dummy() { 
	send(new_packet() << osc::BeginMessage("/test") << 1 << osc::EndMessage);
}

// osc_receive::osc_receive(unsigned int port) : socket_receive_(IpEndpointName(IpEndpointName::ANY_ADDRESS, port), this), thread_(boost::bind(&osc_receive::run_thread, *this)) {}
// 
// osc_receive::~osc_receive() {
// 	socket_receive_.AsynchronousBreak();
// }
// 
// void osc_receive::run_thread() {
// 	l_.socket_receive_.RunUntilSigInt();
// }
// 
// boost::mutex& osc_receive::mutex() {
// 	return mutex_;
// }
// 
// void osc_receive::join() {
// 	thread_.join();
// }
// 
// bool osc_receive::pending() {
// 	boost::lock_guard<boost::mutex> g(mutex_); 
// 	return !pending_.empty();
// }
// 
// int osc_receive::size_pending() {
// 	boost::lock_guard<boost::mutex> g(mutex_);
// 	return pending_.size();
// }
// 
// osc::ReceivedMessage osc_receive::next() {
// 	boost::lock_guard<boost::mutex> g(mutex_);
// 	osc::ReceivedMessage m = pending_.front();
// 	pending_.pop();
// 	return m;
// }
// osc::ReceivedMessage osc_receive::top() {
// 	boost::lock_guard<boost::mutex> g(mutex_);
// 	return pending_.front();
// }
// 
// void osc_receive::ProcessMessage(const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint) {
// 	boost::lock_guard<boost::mutex> g(mutex_); 
// 	pending_.push(m);
// 	std::cout << pending_.size() << std::endl;
// 	// try{
// 	// 	if(strcmp(m.AddressPattern(), "/test1") == 0) {
// 	// 		// example #1 -- argument stream interface
// 	// 		osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
// 	// 		bool a1;
// 	// 		osc::int32 a2;
// 	// 		float a3;
// 	// 		const char *a4;
// 	// 		args >> a1 >> a2 >> a3 >> a4 >> osc::EndMessage;
// 	// 		std::cout << "received '/test1' message with arguments: " << a1 << " " << a2 << " " << a3 << " " << a4 << "\n";
// 	// 	} else if(strcmp(m.AddressPattern(), "/test2") == 0) {
// 	// 		// example #2 -- argument iterator interface, supports
// 	// 		// reflection for overloaded messages (eg you can call 
// 	// 		// (*arg)->IsBool() to check if a bool was passed etc).
// 	// 		osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
// 	// 		bool a1 = (arg++)->AsBool();
// 	// 		int a2 = (arg++)->AsInt32();
// 	// 		float a3 = (arg++)->AsFloat();
// 	// 		const char *a4 = (arg++)->AsString();
// 	// 		if(arg != m.ArgumentsEnd())
// 	// 			throw osc::ExcessArgumentException();
// 	// 		std::cout << "received '/test2' message with arguments: " << a1 << " " << a2 << " " << a3 << " " << a4 << "\n";
// 	// 	}
// 	// } catch(osc::Exception& e) {
// 	// 		std::cout << "error while parsing message: " << m.AddressPattern() << ": " << e.what() << "\n";
// 	// }
// }