/*
 * vebarray.h
 *
 *  Created on: 2015-04-24
 *      Author: morin
 */

#ifndef VEBARRAY_H_
#define VEBARRAY_H_

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>

using std::cout;
using std::endl;

template<class T, class I>
class veb_array {
protected:
	T *a;    // the data
	I n;     // the length of a
	I h;     // the height of the tree

	static const unsigned MAX_H = 32;

	typedef unsigned char h_type;
	struct dumdum {
		h_type h0, h1, dummy[2];
		I m0, m1;
	};
	dumdum s[MAX_H];

	static void sequencer(I h, dumdum *s, unsigned d);

	I copy_data(T *a0, I *rtl, I i, I path, unsigned d);

public:
	veb_array(T *a0, I n0);
	~veb_array();
	I search(const T &x);

	const T& get_data(const I &i) {
		if (i < 0 || i >= n) {
			std::ostringstream ss;
			ss << "index " << i << " is out of bounds ({0,...," << n-1 << "})";
			throw std::out_of_range(ss.str());
		}
		return a[i];

	}
};

template<class T, class I>
void veb_array<T,I>::sequencer(I h, dumdum *s, unsigned d) {
	if (h == 0) return;
	I h0 = h/2;
	I h1 = h-h0-1;
	sequencer(h0, s, d);
	s[d+h0].h0 = h0;
	s[d+h0].m0 = (((I)2)<<h0)-1;
	s[d+h0].h1 = h1;
	s[d+h0].m1 = (((I)2)<<h1)-1;
	sequencer(h1, s, d+h0+1);
}

template<class T, class I>
I veb_array<T,I>::copy_data(T *a0, I *rtl, I i, I path, unsigned d) {

	if (d > h || rtl[d] >= n || i >= n) return i;

	// visit left child
	path <<= 1;
	rtl[d+1] = rtl[d-s[d].h0] + s[d].m0 + (path&s[d].m0)*(s[d].m1);
	i = copy_data(a0, rtl, i, path, d+1);

	a[rtl[d]] = a0[i++];

	// visit right child
	path += 1;
	rtl[d+1] = rtl[d-s[d].h0] + s[d].m0 + (path&s[d].m0)*(s[d].m1);
	i = copy_data(a0, rtl, i, path, d+1);

	return i;
}


template<class T, class I>
veb_array<T,I>::veb_array(T *a0, I n0) {
	n = n0;

	// find smallest h such that sum_i=0^h 2^h >= n
	int m = 1;
	for (h = 0; m < n; h++, m += 1<<h);

	// build our magic sequence
	dumdum q = {(h_type)h, 0, {0, 0}, ((I)2<<h)-1, 1};
	std::fill_n(s, MAX_H+1, q);
	sequencer(h, s, 0);

	// allocate new array and copy data into it
	a = new T[n];
	I rtl[MAX_H+1];
	rtl[0] = 0;
	copy_data(a0, rtl, 0, 0, 0);
}

template<class T, class I>
veb_array<T,I>::~veb_array() {
	delete[] a;
}

template<class T, class I>
I veb_array<T,I>::search(const T &x) {
	I rtl[MAX_H+1];
	I j = n;
	I i = 0;
	I p = 0;
	for (int d = 0; i < n; d++) {
		rtl[d] = i;
		if (x < a[i]) {
			p <<= 1;
			j = i;
		} else if (x > a[i]) {
			p = (p << 1) + 1;
		} else {
			return i;
		}
		i = rtl[d-s[d].h0] + s[d].m0 + (p&s[d].m0)*(s[d].m1);
	}
	return j;
}



#endif /* VEBARRAY_H_ */
