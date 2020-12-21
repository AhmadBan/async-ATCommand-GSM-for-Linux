/*
 * fac.cpp
 *
 *  Created on: Dec 21, 2020
 *      Author: Ahmad
 */

#include "fac.h"

int factorial(int n){
	int result=1;
	for(int i=n;i>0;i--)
		result*=i;
	return result;
}
