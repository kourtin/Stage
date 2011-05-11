#if !defined(LINE_IN_H_ELHO95SX)
#define LINE_IN_H_ELHO95SX

#include "son/source.h"

struct line_in : public source {
	line_in(unsigned int n = 1);
	virtual void creer();
	virtual std::string classe() { return "line_in"; }
private:
	unsigned int channel_;
};

#endif /* end of include guard: LINE_IN_H_ELHO95SX */
