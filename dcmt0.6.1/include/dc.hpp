/*
 * dc.hpp
 *
 *  Created on: 27 oct. 2010
 *      Author: ppudlo
 */

#ifndef DC_HPP_
#define DC_HPP_
#include <inttypes.h> /* C99 compiler */

extern "C" typedef struct mt_struct;

/* old interface */
extern "C" void init_dc(uint32_t seed);
extern "C" mt_struct *get_mt_parameter(int w, int p);
extern "C" mt_struct *get_mt_parameter_id(int w, int p, int id);
extern "C" mt_struct **get_mt_parameters(int w, int p, int max_id, int *count);

/* new interface */
extern "C" mt_struct *get_mt_parameter_st(int w, int p, uint32_t seed);
extern "C" mt_struct *get_mt_parameter_id_st(int w, int p, int id, uint32_t seed);
extern "C" mt_struct **get_mt_parameters_st(int w, int p, int start_id, int max_id,
				 uint32_t seed, int *count);
/* common */
extern "C" void free_mt_struct(mt_struct *mts);
extern "C" void free_mt_struct_array(mt_struct **mtss, int count);
extern "C" void sgenrand_mt(uint32_t seed, mt_struct *mts);
extern "C" uint32_t genrand_mt(mt_struct *mts);


#endif /* DC_HPP_ */
