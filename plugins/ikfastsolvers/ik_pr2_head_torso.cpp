/// autogenerated analytical inverse kinematics code from ikfast program part of OpenRAVE
/// \author Rosen Diankov
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///     http://www.apache.org/licenses/LICENSE-2.0
/// 
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// ikfast version 39 generated on 2011-03-07 23:57:54.583129
/// To compile with gcc:
///     gcc -lstdc++ ik.cpp
/// To compile without any main function as a shared object:
///     gcc -fPIC -lstdc++ -DIKFAST_NO_MAIN -shared -Wl,-soname,ik.so -o ik.so ik.cpp
#include <cmath>
#include <vector>
#include <limits>
#include <algorithm>
#include <complex>

#ifdef IKFAST_HEADER
#include IKFAST_HEADER
#endif

#ifndef IKFAST_ASSERT
#include <stdexcept>
#include <sstream>

#ifdef _MSC_VER
#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCDNAME__
#endif
#endif

#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __func__
#endif

#define IKFAST_ASSERT(b) { if( !(b) ) { std::stringstream ss; ss << "ikfast exception: " << __FILE__ << ":" << __LINE__ << ": " <<__PRETTY_FUNCTION__ << ": Assertion '" << #b << "' failed"; throw std::runtime_error(ss.str()); } }

#endif

#if defined(_MSC_VER)
#define IKFAST_ALIGNED16(x) __declspec(align(16)) x
#else
#define IKFAST_ALIGNED16(x) x __attribute((aligned(16)))
#endif

#define IK2PI  6.28318530717959
#define IKPI  3.14159265358979
#define IKPI_2  1.57079632679490

#ifdef _MSC_VER
#ifndef isnan
#define isnan _isnan
#endif
#endif // _MSC_VER

// defined when creating a shared object/dll
#ifdef IKFAST_CLIBRARY
#ifdef _MSC_VER
#define IKFAST_API extern "C" __declspec(dllexport)
#else
#define IKFAST_API extern "C"
#endif
#else
#define IKFAST_API
#endif

// lapack routines
extern "C" {
  void dgetrf_ (const int* m, const int* n, double* a, const int* lda, int* ipiv, int* info);
  void zgetrf_ (const int* m, const int* n, std::complex<double>* a, const int* lda, int* ipiv, int* info);
  void dgetri_(const int* n, const double* a, const int* lda, int* ipiv, double* work, const int* lwork, int* info);
  void dgesv_ (const int* n, const int* nrhs, double* a, const int* lda, int* ipiv, double* b, const int* ldb, int* info);
  void dgetrs_(const char *trans, const int *n, const int *nrhs, double *a, const int *lda, int *ipiv, double *b, const int *ldb, int *info);
  void dgeev_(const char *jobvl, const char *jobvr, const int *n, double *a, const int *lda, double *wr, double *wi,double *vl, const int *ldvl, double *vr, const int *ldvr, double *work, const int *lwork, int *info);
}

#ifdef IKFAST_NAMESPACE
namespace IKFAST_NAMESPACE {
#endif

#ifdef IKFAST_REAL
typedef IKFAST_REAL IKReal;
#else
typedef double IKReal;
#endif

class IKSolution
{
public:
    /// Gets a solution given its free parameters
    /// \param pfree The free parameters required, range is in [-pi,pi]
    void GetSolution(IKReal* psolution, const IKReal* pfree) const {
        for(size_t i = 0; i < basesol.size(); ++i) {
            if( basesol[i].freeind < 0 )
                psolution[i] = basesol[i].foffset;
            else {
                IKFAST_ASSERT(pfree != NULL);
                psolution[i] = pfree[basesol[i].freeind]*basesol[i].fmul + basesol[i].foffset;
                if( psolution[i] > IKPI ) {
                    psolution[i] -= IK2PI;
                }
                else if( psolution[i] < -IKPI ) {
                    psolution[i] += IK2PI;
                }
            }
        }
    }

    /// Gets the free parameters the solution requires to be set before a full solution can be returned
    /// \return vector of indices indicating the free parameters
    const std::vector<int>& GetFree() const { return vfree; }

    struct VARIABLE
    {
        VARIABLE() : freeind(-1), fmul(0), foffset(0) {}
        VARIABLE(int freeind, IKReal fmul, IKReal foffset) : freeind(freeind), fmul(fmul), foffset(foffset) {}
        int freeind;
        IKReal fmul, foffset; ///< joint value is fmul*sol[freeind]+foffset
    };

    std::vector<VARIABLE> basesol;       ///< solution and their offsets if joints are mimiced
    std::vector<int> vfree;
};

inline float IKabs(float f) { return fabsf(f); }
inline double IKabs(double f) { return fabs(f); }

inline float IKlog(float f) { return logf(f); }
inline double IKlog(double f) { return log(f); }

inline float IKasin(float f)
{
IKFAST_ASSERT( f > -1.001f && f < 1.001f ); // any more error implies something is wrong with the solver
if( f <= -1 ) return -IKPI_2;
else if( f >= 1 ) return IKPI_2;
return asinf(f);
}
inline double IKasin(double f)
{
IKFAST_ASSERT( f > -1.001 && f < 1.001 ); // any more error implies something is wrong with the solver
if( f <= -1 ) return -IKPI_2;
else if( f >= 1 ) return IKPI_2;
return asin(f);
}

// return positive value in [0,y)
inline float IKfmod(float x, float y)
{
    while(x < 0) {
        x += y;
    }
    return fmodf(x,y);
}

// return positive value in [0,y)
inline float IKfmod(double x, double y)
{
    while(x < 0) {
        x += y;
    }
    return fmod(x,y);
}

inline float IKacos(float f)
{
IKFAST_ASSERT( f > -1.001f && f < 1.001f ); // any more error implies something is wrong with the solver
if( f <= -1 ) return IKPI;
else if( f >= 1 ) return 0.0f;
return acosf(f);
}
inline double IKacos(double f)
{
IKFAST_ASSERT( f > -1.001 && f < 1.001 ); // any more error implies something is wrong with the solver
if( f <= -1 ) return IKPI;
else if( f >= 1 ) return 0.0;
return acos(f);
}
inline float IKsin(float f) { return sinf(f); }
inline double IKsin(double f) { return sin(f); }
inline float IKcos(float f) { return cosf(f); }
inline double IKcos(double f) { return cos(f); }
inline float IKtan(float f) { return tanf(f); }
inline double IKtan(double f) { return tan(f); }
inline float IKsqrt(float f) { if( f <= 0.0f ) return 0.0f; return sqrtf(f); }
inline double IKsqrt(double f) { if( f <= 0.0 ) return 0.0; return sqrt(f); }
inline float IKatan2(float fy, float fx) {
    if( isnan(fy) ) {
        IKFAST_ASSERT(!isnan(fx)); // if both are nan, probably wrong value will be returned
        return IKPI_2;
    }
    else if( isnan(fx) )
        return 0;
    return atan2f(fy,fx);
}
inline double IKatan2(double fy, double fx) {
    if( isnan(fy) ) {
        IKFAST_ASSERT(!isnan(fx)); // if both are nan, probably wrong value will be returned
        return IKPI_2;
    }
    else if( isnan(fx) )
        return 0;
    return atan2(fy,fx);
}

IKFAST_API int getNumFreeParameters() { return 1; }
IKFAST_API int* getFreeParameters() { static int freeparams[] = {0}; return freeparams; }
IKFAST_API int getNumJoints() { return 3; }

IKFAST_API int getIKRealSize() { return sizeof(IKReal); }

IKFAST_API int getIKType() { return 0x23000006; }

/// solves the inverse kinematics equations.
/// \param pfree is an array specifying the free joints of the chain.
IKFAST_API void fk(const IKReal* j, IKReal* eetrans, IKReal* eerot) {
IKReal x0,x1,x2,x3;
x0=IKcos(j[1]);
x1=IKcos(j[2]);
x2=IKsin(j[1]);
x3=IKsin(j[2]);
eetrans[0]=((-0.0670700000000000)+(((0.0232000000000000)*(x0)*(x1)))+(((-0.0300000000000000)*(x2)))+(((0.0680000000000000)*(x0)))+(((0.0980000000000000)*(x0)*(x3))));
eetrans[1]=((((0.0980000000000000)*(x2)*(x3)))+(((0.0300000000000000)*(x0)))+(((0.0680000000000000)*(x2)))+(((0.0232000000000000)*(x1)*(x2))));
eetrans[2]=((1.12112500000000)+(((0.0980000000000000)*(x1)))+(((-0.0232000000000000)*(x3)))+(j[0]));
eerot[0]=((x0)*(x1));
eerot[1]=((x1)*(x2));
eerot[2]=((-1.00000000000000)*(x3));
}

class IKSolver {
public:
IKReal j13, cj13, sj13, htj13,
j14, cj14, sj14, htj14,
j12, cj12, sj12, htj12,
new_r00, r00, rxp0_0,
new_r01, r01, rxp0_1,
new_r02, r02, rxp0_2,
new_r10, r10, rxp1_0,
new_r11, r11, rxp1_1,
new_r12, r12, rxp1_2,
new_r20, r20, rxp2_0,
new_r21, r21, rxp2_1,
new_r22, r22, rxp2_2,
new_px, new_py, new_pz, px, py, pz, npx, npy, npz, pp;

bool ik(const IKReal* eetrans, const IKReal* eerot, const IKReal* pfree, std::vector<IKSolution>& vsolutions) {
for(int dummyiter = 0; dummyiter < 1; ++dummyiter) {
vsolutions.resize(0); vsolutions.reserve(8);
px = eetrans[0]; py = eetrans[1]; pz = eetrans[2];

j12=pfree[0]; cj12=cos(pfree[0]); sj12=sin(pfree[0]);
new_px=((0.0670700000000000)+(px));
new_py=py;
new_pz=((-1.12112500000000)+(((-1.00000000000000)*(j12)))+(pz));
px = new_px; py = new_py; pz = new_pz;
pp=(((px)*(px))+((py)*(py))+((pz)*(pz)));
{
IKReal dummyeval[1];
dummyeval[0]=((-1.00000000000000)+(((-33.3333333333333)*(py))));
if( IKabs(dummyeval[0]) < 0.0000001000000000  )
{
continue;

} else
{
IKReal op[2+1], zeror[2];
int numroots;
IKReal x0=((2.00000000000000)*(py));
op[0]=((-0.0600000000000000)+(((-1.00000000000000)*(x0))));
op[1]=((-4.00000000000000)*(px));
op[2]=((-0.0600000000000000)+(x0));
polyroots2(op,zeror,numroots);
IKReal j13array[2], cj13array[2], sj13array[2], tempj13array[1];
int numsolutions = 0;
for(int ij13 = 0; ij13 < numroots; ++ij13)
{
IKReal htj13 = zeror[ij13];
tempj13array[0]=((2.00000000000000)*(atan(htj13)));
for(int kj13 = 0; kj13 < 1; ++kj13)
{
j13array[numsolutions] = tempj13array[kj13];
if( j13array[numsolutions] > IKPI )
{
    j13array[numsolutions]-=IK2PI;
}
else if( j13array[numsolutions] < -IKPI )
{
    j13array[numsolutions]+=IK2PI;
}
sj13array[numsolutions] = IKsin(j13array[numsolutions]);
cj13array[numsolutions] = IKcos(j13array[numsolutions]);
bool valid = true;
for( int kj13 = 0; kj13 < numsolutions; ++kj13)
{
    if( IKabs(cj13array[kj13]-cj13array[numsolutions]) < 0.0001 && IKabs(sj13array[kj13]-sj13array[numsolutions]) < 0.0001 )
    {
        valid=false; break;
    }
}
if( valid ) { numsolutions++; }
}
}
for(int ij13 = 0; ij13 < numsolutions; ++ij13)
    {
    j13 = j13array[ij13]; cj13 = cj13array[ij13]; sj13 = sj13array[ij13];

{
IKReal j14array[2], cj14array[2], sj14array[2];
bool j14valid[2]={false};
IKReal x1=((cj13)*(px));
IKReal x2=((py)*(sj13));
IKReal x3=((x2)+(x1));
IKReal x4=((0.0680000000000000)+(((-1.00000000000000)*(x3))));
IKReal x5=(pz)*(pz);
IKReal x6=(x4)*(x4);
IKReal x7=((x6)+(x5));
if( (x7) < (IKReal)-0.00001 )
    continue;
IKReal x8=IKsqrt(x7);
IKReal x9=IKabs(x8);
IKReal x10=((IKabs(x9) != 0)?((IKReal)1/(x9)):(IKReal)1.0e30);
IKReal x11=((0.0980000000000000)*(x10));
if( (x11) < -1.0001 || (x11) > 1.0001 )
    continue;
IKReal x12=IKasin(x11);
IKReal x13=((-1.00000000000000)*(pz));
IKReal x14=IKatan2(x13, x4);
j14array[0]=((((-1.00000000000000)*(x14)))+(((-1.00000000000000)*(x12))));
sj14array[0]=IKsin(j14array[0]);
cj14array[0]=IKcos(j14array[0]);
j14array[1]=((3.14159265358979)+(((-1.00000000000000)*(x14)))+(x12));
sj14array[1]=IKsin(j14array[1]);
cj14array[1]=IKcos(j14array[1]);
if( j14array[0] > IKPI )
{
    j14array[0]-=IK2PI;
}
else if( j14array[0] < -IKPI )
{    j14array[0]+=IK2PI;
}
j14valid[0] = true;
if( j14array[1] > IKPI )
{
    j14array[1]-=IK2PI;
}
else if( j14array[1] < -IKPI )
{    j14array[1]+=IK2PI;
}
j14valid[1] = true;
if( j14valid[0] && j14valid[1] && IKabs(cj14array[0]-cj14array[1]) < 0.0001 && IKabs(sj14array[0]-sj14array[1]) < 0.0001 )
{
    j14valid[1]=false;
}
for(int ij14 = 0; ij14 < 2; ++ij14)
{
if( !j14valid[ij14] )
{
    continue;
}
j14 = j14array[ij14]; cj14 = cj14array[ij14]; sj14 = sj14array[ij14];

IKReal soleval[1];
soleval[0]=((-0.0232000000000000)+(((-1.00000000000000)*(cj14)*(((0.0680000000000000)+(((-1.00000000000000)*(cj13)*(px)))+(((-1.00000000000000)*(py)*(sj13)))))))+(((-1.00000000000000)*(pz)*(sj14))));
if( soleval[0] > 0.0000000000000000  )
{
vsolutions.push_back(IKSolution()); IKSolution& solution = vsolutions.back();
solution.basesol.resize(3);
solution.basesol[0].foffset = j12;
solution.basesol[1].foffset = j13;
solution.basesol[2].foffset = j14;
solution.vfree.resize(0);
}
}
}
    }

}

}
}
return vsolutions.size()>0;
}

static inline void polyroots2(IKReal rawcoeffs[2+1], IKReal rawroots[2], int& numroots) {
    IKReal det = rawcoeffs[1]*rawcoeffs[1]-4*rawcoeffs[0]*rawcoeffs[2];
    if( det < 0 ) {
        numroots=0;
    }
    else if( det == 0 ) {
        rawroots[0] = -0.5*rawcoeffs[1]/rawcoeffs[0];
        numroots = 1;
    }
    else {
        det = IKsqrt(det);
        rawroots[0] = (-rawcoeffs[1]+det)/(2*rawcoeffs[0]);
        rawroots[1] = (-rawcoeffs[1]-det)/(2*rawcoeffs[0]);//rawcoeffs[2]/(rawcoeffs[0]*rawroots[0]);
        numroots = 2;
    }
}
};


/// solves the inverse kinematics equations.
/// \param pfree is an array specifying the free joints of the chain.
IKFAST_API bool ik(const IKReal* eetrans, const IKReal* eerot, const IKReal* pfree, std::vector<IKSolution>& vsolutions) {
IKSolver solver;
return solver.ik(eetrans,eerot,pfree,vsolutions);
}

IKFAST_API const char* getKinematicsHash() { return "2640ae411e0c87b03f56bf289296f9d8"; }

#ifdef IKFAST_NAMESPACE
} // end namespace
#endif

#ifndef IKFAST_NO_MAIN
#include <stdio.h>
#include <stdlib.h>
#ifdef IKFAST_NAMESPACE
using namespace IKFAST_NAMESPACE;
#endif
int main(int argc, char** argv)
{
    if( argc != 12+getNumFreeParameters()+1 ) {
        printf("\nUsage: ./ik r00 r01 r02 t0 r10 r11 r12 t1 r20 r21 r22 t2 free0 ...\n\n"
               "Returns the ik solutions given the transformation of the end effector specified by\n"
               "a 3x3 rotation R (rXX), and a 3x1 translation (tX).\n"
               "There are %d free parameters that have to be specified.\n\n",getNumFreeParameters());
        return 1;
    }

    std::vector<IKSolution> vsolutions;
    std::vector<IKReal> vfree(getNumFreeParameters());
    IKReal eerot[9],eetrans[3];
    eerot[0] = atof(argv[1]); eerot[1] = atof(argv[2]); eerot[2] = atof(argv[3]); eetrans[0] = atof(argv[4]);
    eerot[3] = atof(argv[5]); eerot[4] = atof(argv[6]); eerot[5] = atof(argv[7]); eetrans[1] = atof(argv[8]);
    eerot[6] = atof(argv[9]); eerot[7] = atof(argv[10]); eerot[8] = atof(argv[11]); eetrans[2] = atof(argv[12]);
    for(size_t i = 0; i < vfree.size(); ++i)
        vfree[i] = atof(argv[13+i]);
    bool bSuccess = ik(eetrans, eerot, vfree.size() > 0 ? &vfree[0] : NULL, vsolutions);

    if( !bSuccess ) {
        fprintf(stderr,"Failed to get ik solution\n");
        return -1;
    }

    printf("Found %d ik solutions:\n", (int)vsolutions.size());
    std::vector<IKReal> sol(getNumJoints());
    for(size_t i = 0; i < vsolutions.size(); ++i) {
        printf("sol%d (free=%d): ", (int)i, (int)vsolutions[i].GetFree().size());
        std::vector<IKReal> vsolfree(vsolutions[i].GetFree().size());
        vsolutions[i].GetSolution(&sol[0],vsolfree.size()>0?&vsolfree[0]:NULL);
        for( size_t j = 0; j < sol.size(); ++j)
            printf("%.15f, ", sol[j]);
        printf("\n");
    }
    return 0;
}

#endif

#if defined(IKFAST_HEADER) && defined(IKFAST_NAMESPACE)
#include "ikbase.h"
namespace IKFAST_NAMESPACE {
#ifdef RAVE_REGISTER_BOOST
#include BOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()
BOOST_TYPEOF_REGISTER_TYPE(IKSolution)
#endif
IkSolverBasePtr CreateIkSolver(EnvironmentBasePtr penv, const std::vector<dReal>& vfreeinc) {
    std::vector<int> vfree(getNumFreeParameters());
    for(size_t i = 0; i < vfree.size(); ++i) {
        vfree[i] = getFreeParameters()[i];
    }
    return IkSolverBasePtr(new IkFastSolver<IKReal,IKSolution>(ik,vfree,vfreeinc,getNumJoints(),(IkParameterization::Type)getIKType(), boost::shared_ptr<void>(), getKinematicsHash(), penv));
}
} // end namespace
#endif
