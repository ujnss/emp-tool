#ifndef PLAIN_GC_H__
#define PLAIN_GC_H__
#include "block.h"
#include "utils.h"
#include "garble_circuit.h"
#include <iostream>
#include <fstream>

bool plain_gc_is_public(GarbleCircuit* gc, const block & b, int party);

block plain_gc_public_label(GarbleCircuit* gc, bool b);

block plain_gc_and(GarbleCircuit* gc, const block&a, const block&b);

block plain_gc_xor(GarbleCircuit*gc, const block&a, const block&b);

block plain_gc_not(GarbleCircuit*gc, const block&a);

class PlainGC:public GarbleCircuit{ public:
	const static unsigned long long P1 = 1;
	const static unsigned long long P0 = 2;
	const static unsigned long long S0 = 3;
	const static unsigned long long S1 = 4;
	bool print = false;
	block public_one, public_zero;
	uint64_t gates = 0;
	std::ofstream &fout;
	PlainGC(bool print, std::ofstream & fout):fout(fout) {
		is_public_ptr = &plain_gc_is_public;
		public_label_ptr = &plain_gc_public_label;
		gc_and_ptr = &plain_gc_and;
		gc_xor_ptr = &plain_gc_xor;
		gc_not_ptr = &plain_gc_not;
		public_one = zero_block();
		public_zero = zero_block();
		this->print = print;
		uint64_t *arr = (uint64_t*) &public_one;
		arr[0] = P1;
		arr = (uint64_t*) &public_zero;
		arr[0] = P0;
		gid = 0;
	}
	bool is_public_impl(const block & b, int party) {
		uint64_t *arr = (uint64_t*) &b;
		return arr[0] == P0 or arr[0] == P1;
	}
	block public_label_impl(bool b) {
		return b? public_one : public_zero;
	}
	uint64_t compute_and(uint64_t a, uint64_t b) {
		if(a == S0 or b == S0)
			return S0;
		else return S1;
	}
	uint64_t compute_xor(uint64_t a, uint64_t b) {
		if (a == b)
			return S0;
		else return S1;
	}
	block gen_and(const block& a, const block& b) {
		uint64_t *arr_a = (uint64_t*) &a;
		uint64_t *arr_b = (uint64_t*) &b;
		if (arr_a[0] == P1) {
			return b;
		} else if (arr_b[0] == P1) {
			return a;
		} else if(arr_a[0] == P0 or arr_b[0] == P0) {
			return public_zero;
		} else {
			block res = zero_block();
			uint64_t *arr = (uint64_t*) &res;
			arr[0] = compute_and(arr_a[0], arr_b[0]);
			arr[1] = gid;
			if(print)
				fout <<"2 1 "<<arr_a[1] <<" "<<arr_b[1]<<" "<<gid<<" AND"<<std::endl;
			gid++;
			gates++;
			return res;
		}
	}
	block gen_xor(const block&a, const block& b) {
		uint64_t *arr_a = (uint64_t*) &a;
		uint64_t *arr_b = (uint64_t*) &b;
		if (arr_a[0] == P1) {
			return gen_not(b);
		} else if (arr_b[0] == P1) {
			return gen_not(a);
		} else if(arr_a[0] == P0) {
			return b;
		} else if(arr_b[0] == P0){
			return a;
		} else {
			block res = zero_block();
			uint64_t *arr = (uint64_t*) &res;
			arr[0] = compute_xor(arr_a[0], arr_b[0]);
			arr[1] = gid;
			if(print)
				fout <<"2 1 "<<arr_a[1] <<" "<<arr_b[1]<<" "<<gid<<" XOR"<<std::endl;
			gates++;
			gid++;
			return res;
		}
	}
	block private_label(bool b) {
		block res = zero_block();
		uint64_t *arr = (uint64_t*) &res;
		arr[0] = b ? S1 : S0;
		arr[1] = gid;
		gid++;
		return res;
	}
	bool get_value(block a) {
		uint64_t *arr = (uint64_t*) &a;
		//		if(print)
		//			fout <<"1 0 "<<arr[1] <<" OUT"<<endl;
		if (arr[0] == S0 or arr[0] == P0)
			return false;
		else return true;
	}
	block gen_not(const block&a) {
		uint64_t *arr_a = (uint64_t*) &a;
		if (arr_a[0] == P1) {
			return public_zero;
		} else if (arr_a[0] == P0) {
			return public_one;
		} else {
			block res = zero_block();
			uint64_t *arr = (uint64_t*) &res;
			if(arr_a[0] == S0) arr[0] = S1;
			else arr[0] = S0;
			arr[1] = gid;
			if(print)
				fout <<"1 1 "<<arr_a[1] <<" "<<gid<<" INV"<<std::endl;
			gid++;
			gates++;
			return res;
		}
	}
};
bool plain_gc_is_public(GarbleCircuit* gc, const block & b, int party) {
	return ((PlainGC*)gc)->is_public_impl(b, party);
}
block plain_gc_public_label(GarbleCircuit* gc, bool b) {
	return ((PlainGC*)gc)->public_label_impl(b);
}
block plain_gc_and(GarbleCircuit* gc, const block&a, const block&b) {
	return ((PlainGC*)gc)->gen_and(a, b);
}
block plain_gc_xor(GarbleCircuit*gc, const block&a, const block&b) {
	return ((PlainGC*)gc)->gen_xor(a, b);
}
block plain_gc_not(GarbleCircuit*gc, const block&a) {
	return ((PlainGC*)gc)->gen_not(a);
}

#endif
