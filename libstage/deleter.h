#if !defined(DELETER_H_YPBGAR0V)
#define DELETER_H_YPBGAR0V

template <class T>
struct deleter_c {
	deleter_c& operator()(T* a) {
		delete a;
		return *this;
	}
};

template <class T>
deleter_c<T> deleter() {
	return deleter_c<T>();
}

#endif /* end of include guard: DELETER_H_YPBGAR0V */
