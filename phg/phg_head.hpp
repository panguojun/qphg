#pragma once

#define real        float
#define PRINT(msg)//			{std::stringstream ss; ss << "\n> " << msg; printf(ss.str().c_str());}
#define ASSERT(x)		{if(!(x)) throw;}

#define var			ELEMENT
#define INVALIDVAR	ELEMENT(0)

//#undef PHGPRINT
#define PHGPRINT	GROUP::_PHGPRINT

#define PHG_VAR(name, defaultval) (GROUP::gvarmapstack.stack.empty() || GROUP::gvarmapstack.stack.front().find(#name) == GROUP::gvarmapstack.stack.front().end() ? defaultval : GROUP::gvarmapstack.stack.front()[#name])
#define PHG_PARAM(index)	cd.valstack.get(args - index)
#define PHG_POP_PARAMS(n) { for (int i = 0; i < n; i++) cd.valstack.pop(); }

#define DEFAULT_ELEMENT	\
	ELEMENT() {}\
	ELEMENT(int _val) {}\
	operator const int& (){return 1;}\
	bool operator == (const ELEMENT& v) const{return false;}
