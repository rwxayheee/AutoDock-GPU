/*

AutoDock-GPU, an OpenCL implementation of AutoDock 4.2 running a Lamarckian Genetic Algorithm
Copyright (C) 2017 TU Darmstadt, Embedded Systems and Applications Group, Germany. All rights reserved.
For some of the code, Copyright (C) 2019 Computational Structural Biology Center, the Scripps Research Institute.

AutoDock is a Trade Mark of the Scripps Research Institute.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/


#ifndef GPUDATADOTH
#define GPUDATADOTH
#include <float.h>


static const int   TERMBITS         = 10;
static const float MAXTERM          = (float)(1 << (31 - TERMBITS - 8)); // 2^(31 - 10 - 8) = 2^13 = 8192
static const float TERMSCALE        = (float)(1 << TERMBITS);            // 2^10 = 1024
static const float ONEOVERTERMSCALE = 1.0f / TERMSCALE;                  // 1 / 1024 = 0.000977
static const float MAXREDUCE        = (float)(1 << (31 - TERMBITS - 4)); // 2^(31 - 10 - 4) = 2^17 = 131072

static const float MAXENERGY        = FLT_MAX / 100.0f; // Used to cap absurd energies so placeholder energy is always skipped in sorts
static const float MAXFORCE         = FLT_MAX / 100.0f; // Used to cap absurd gradients

#define RTERROR(status, s) \
	if (status != cudaSuccess) { \
		printf("%s %s\n", s, cudaGetErrorString(status)); \
		assert(0); \
		cudaDeviceReset(); \
		exit(-1); \
	}


#define SYNCHRONOUS
#ifdef SYNCHRONOUS
#define LAUNCHERROR(s) \
	{ \
		cudaError_t status = cudaGetLastError(); \
		if (status != cudaSuccess) { \
			printf("Error: %s launching kernel %s\n", cudaGetErrorString(status), s); \
			cudaDeviceReset(); \
			exit(-1); \
		} \
		status = cudaDeviceSynchronize(); \
		RTERROR(status, s); \
	}
#else
#define LAUNCHERROR(s) \
	{ \
		cudaError_t status = cudaGetLastError(); \
		if (status != cudaSuccess) { \
			printf("Error: %s launching kernel %s\n", cudaGetErrorString(status), s); \
			cudaDeviceReset(); \
			exit(-1); \
		} \
	}
#endif

struct GpuData {
	int                             devnum;
	int                             devid;
	int                             preallocated_gridsize;
	Dockparameters                  dockpars;
	
	// Consolidated constants and memory pointers to reduce kernel launch overhead
	kernelconstant_interintra*      pKerconst_interintra;
	kernelconstant_intracontrib*    pKerconst_intracontrib;
	kernelconstant_intra*           pKerconst_intra;
	kernelconstant_rotlist*         pKerconst_rotlist;
	kernelconstant_conform*         pKerconst_conform;
	kernelconstant_grads*           pKerconst_grads;
	float*                          pMem_fgrids;
	int*                            pMem_evals_of_new_entities;
	int*                            pMem_gpu_evals_of_runs;
	uint32_t*                       pMem_prng_states;
	int*                            pMem_rotbonds_const;
	int*                            pMem_rotbonds_atoms_const;
	int*                            pMem_num_rotating_atoms_per_rotbond_const;
	
	// CUDA-specific constants
	unsigned int                    warpmask;
	unsigned int                    warpbits;
};

struct GpuTempData {
	float*      pMem_fgrids;
	float*      pMem_conformations1;
	float*      pMem_conformations2;
	float*      pMem_energies1;
	float*      pMem_energies2;
	int*        pMem_evals_of_new_entities;
	int*        pMem_gpu_evals_of_runs;
	uint32_t*   pMem_prng_states;
	char*       device_name;
	bool        device_busy;
};
#endif

