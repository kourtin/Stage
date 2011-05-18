#if !defined(COMPORTEMENT_TOOL_H_LOWN79ZG)
#define COMPORTEMENT_TOOL_H_LOWN79ZG

#include "comportements/comportement.h"

struct comportement_tool : public comportement {
	comportement_tool(objet& o);
	virtual ~comportement_tool();
	virtual void operator()() = 0;
	static std::string titre() { return "comportement_tool"; }
	virtual type what() { return type_tool; }
};

#endif /* end of include guard: COMPORTEMENT_TOOL_H_LOWN79ZG */
